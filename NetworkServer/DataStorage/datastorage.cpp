#include "datastorage.h"

#include <cassert>

#include <QSqlError>
#include <QSqlQuery>

using namespace std;

using namespace NetworkProtocol;
using namespace DTO;
using namespace Types;



void AbstractDataSession::setLogger(shared_ptr<AbstractLogger> logger)
{
    if(!logger)
    {
        throw invalid_argument("logger cannot be a nullptr.");
    }
    _logger.swap(logger);
}






PooledDataStorageFactory::PooledDataStorageFactory(shared_ptr<AbstractLoggerFactory> loggerFactory,
                                                   shared_ptr<AbstractDataStorageFactory> sessionFactory,
                                                   size_t poolSize)
    : _loggerFactory(loggerFactory), _sessionFactory(sessionFactory)
{
    if(!_loggerFactory)
    {
        throw invalid_argument("loggerFactory cannot be a nullptr.");
    }

    auto logger = _loggerFactory->createLoggingSession();

    if(!_sessionFactory)
    {
        LOG_DEBUG(logger, "sessionFactory == nullptr.");
        throw invalid_argument("sessionFactory cannot be a nullptr.");
    }

    if(poolSize == 0)
    {
        LOG_DEBUG(logger, "pool size == 0");
        throw invalid_argument("pool size must be at least 1.");
    }
    assert(_pool.empty());

    LOG_TRACE(logger, QString("Generating %1 postgre sessions.").arg(poolSize));

    _pool_access_lock.lock();

    for(size_t i = 0; i < poolSize; i++)
    {
        _pool.append(move(_sessionFactory->openSession()));
    }

    _pool_access_lock.unlock();

    assert(_pool.size() == poolSize);

    LOG_TRACE(logger, QString("Created pooled postgres data storage with "\
                              "%1 sessions.").arg(_pool.size()));
}

void PooledDataStorageFactory::addToPool(shared_ptr<AbstractDataSession> session)
{
    _pool_access_lock.lock();

    _pool.enqueue(move(session));
    _pool_not_empty.wakeOne();

    _pool_access_lock.unlock();
}

shared_ptr<AbstractDataSession> PooledDataStorageFactory::openSession()
{
    _pool_access_lock.lock();

    if(_pool.empty())
    {
        _pool_not_empty.wait(&_pool_access_lock);
    }

    auto session = _pool.dequeue();

    _pool_access_lock.unlock();

    auto logger = _loggerFactory->createLoggingSession();
    // That is sooo wrong. Well... yolo!
    auto owner = this;
    auto pooled_session = shared_ptr<AbstractDataSession>(
                new PooledDataSession(move(logger), move(session), owner));

    return pooled_session;
}










/*                              SESSIONS                            */



PooledDataSession::PooledDataSession(shared_ptr<AbstractLogger> logger,
                                     shared_ptr<AbstractDataSession> session,
                                     PooledDataStorageFactory* owner)
    : _session(move(session)), _owner(owner)
{
    if(!logger)
    {
        throw invalid_argument("logger cannot be nullptr.");
    }
    _logger.swap(logger);

    LOG_TRACE(_logger, "Logger set.");

    if(!_session)
    {
        LOG_DEBUG(_logger, "session == nullptr");
        throw invalid_argument("session cannot be nullptr");
    }
    _session->setLogger(_logger);
}

void PooledDataSession::loginUser(UserIdType userId, const QString& password,
                                  Failure::FailureType* fail)
{
    _session->loginUser(userId, password, fail);
}

UserIdType PooledDataSession::registerAccount(const QString &password,
                                              Failure::FailureType *fail)
{
    return _session->registerAccount(password, fail);
}

OrderIdType PooledDataSession::sellStock(UserIdType userId, StockIdType stockId,
                                         AmountType amount,PriceType price,
                                         Failure::FailureType* status)
{
    return _session->sellStock(userId, stockId, amount, price, status);
}

OrderIdType PooledDataSession::buyStock(UserIdType userId, StockIdType stockId,
                                 AmountType amount,PriceType price,
                                 Failure::FailureType* status)
{
    return _session->buyStock(userId, stockId, amount, price, status);
}
UserStocks PooledDataSession::getUserStocks(UserIdType userId,
                                                         Failure::FailureType* status)
{
    return _session->getUserStocks(userId, status);
}
std::vector<std::shared_ptr<NetworkProtocol::DTO::Order > >
                            PooledDataSession::getUserOrders(UserIdType userId,
                                                            Failure::FailureType* status)
{
    return _session->getUserOrders(userId, status);
}

void PooledDataSession::cancelOrder(UserIdType userId, OrderIdType orderId,
                                    Failure::FailureType* status)
{
    _session->cancelOrder(userId, orderId, status);
}

void PooledDataSession::startSession(Failure::FailureType* status)
{
    _session->startSession(status);
}

void PooledDataSession::stopSession(Failure::FailureType* status)
{
    _session->stopSession(status);
}

std::vector<NetworkProtocol::DTO::Types::StockIdType> PooledDataSession::getAvailableStocks()
{
    return _session->getAvailableStocks();
}

PooledDataSession::~PooledDataSession()
{
    if(_owner != nullptr)
    {
        LOG_TRACE(_logger, "session object is being returned to session pool.");
        _owner->addToPool(_session);
    }
}
