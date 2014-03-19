#include "datastorage.h"

#include <cassert>

#include <QSqlError>

using namespace std;

void AbstractDataSession::setLogger(shared_ptr<AbstractLogger> logger)
{
    if(!logger)
    {
        throw invalid_argument("logger cannot be a nullptr.");
    }
    _logger.swap(logger);
}

PostgresDataStorageFactory::PostgresDataStorageFactory(shared_ptr<AbstractLoggerFactory> loggerFactory,
                                                       const QHash<QString, QString> &config)
    : _loggerFactory(move(loggerFactory))
{
    if(!_loggerFactory)
    {
        throw invalid_argument("loggerFactory cannot be nullptr.");
    }
    auto logger = _loggerFactory->createLoggingSession();

    if(!config.contains("username"))
    {
        LOG_TRACE(logger, "key: username not found in config.");
        throw invalid_argument("key: username not found in config.");
    }
    if(!config.contains("password"))
    {
        LOG_TRACE(logger, "key: password not found in config.");
        throw invalid_argument("key: password not found in config.");
    }
    if(!config.contains("host"))
    {
        LOG_TRACE(logger, "key: host not found in config.");
        throw invalid_argument("key: host not found in config.");
    }
    if(!config.contains("port"))
    {
        LOG_TRACE(logger, "key: port not found in config.");
        throw invalid_argument("key: port not found in config.");
    }
    if(!config.contains("name"))
    {
        LOG_TRACE(logger, "key: name not found in config.");
        throw invalid_argument("key: name not found in config.");
    }


    _config["username"] = config["username"];
    _config["password"] = config["password"];
    _config["host"] = config["host"];
    _config["port"] = config["port"];

    bool port_to_int;
    _config["port"].toInt(&port_to_int);

    if(!port_to_int)
    {
        LOG_TRACE(logger, QString("Cannot convert port to int. config["\
                  "\"port\"] = ").arg(_config["port"]));
        throw invalid_argument("Error while converting port to int.");
    }
    _config["name"] = config["name"];
}

std::shared_ptr<AbstractDataSession> PostgresDataStorageFactory::openSession()
{
    auto logger = _loggerFactory->createLoggingSession();

    int suffix = sessions_created.fetchAndAddOrdered(1);
    unique_ptr<QSqlDatabase> _handle = unique_ptr<QSqlDatabase>(new QSqlDatabase());
    _handle->addDatabase("QPSQL", QString("Rynki finansowe %1")
                                  .arg(suffix).toLatin1());

    _handle->setHostName(_config["host"]);
    _handle->setDatabaseName(_config["name"]);
    _handle->setUserName(_config["username"]);
    _handle->setPassword(_config["password"]);
    _handle->setPort( _config["port"].toInt());

    LOG_TRACE(logger, QString("Opening postgres connection to %1:%2.")
              .arg(_config["host"]).arg(_config["port"]));

    if(!_handle->open())
    {
        LOG_ERROR(logger, QString("Error while opening connection to database. "\
                  "Error: %1").arg(_handle->lastError().text()));
                throw DatastoreError();
    }
    return shared_ptr<AbstractDataSession>(new PostgreDataSession(move(logger),
                                                                  move(_handle)));
}





PooledDataStorageFactory::PooledDataStorageFactory(shared_ptr<AbstractLoggerFactory> loggerFactory,
                                                   shared_ptr<AbstractDataStorageFactory> sessionFactory,
                                                   size_t poolSize)
    : _loggerFactory(move(loggerFactory)), _sessionFactory(move(sessionFactory))
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

    _pool_not_empty.wait(&_pool_access_lock);
    auto session = move(_pool.dequeue());

    _pool_access_lock.unlock();

    auto logger = _loggerFactory->createLoggingSession();
    // That is sooo wrong. Well... yolo!
    auto owner = shared_ptr<PooledDataStorageFactory>(this);
    auto pooled_session = shared_ptr<AbstractDataSession>(
                new PooledDataSession(move(logger), move(session), move(owner)));

    return move(pooled_session);
}









/*                              SESSIONS                            */

PostgreDataSession::PostgreDataSession(shared_ptr<AbstractLogger> logger,
                                       unique_ptr<QSqlDatabase> handle)
    : _handle(move(handle))
{
    setLogger(move(logger));

    if(!_handle)
    {
        LOG_DEBUG(_logger, "Database handle == nullptr.");
        throw invalid_argument("Database handle cannot be a nullptr.");
    }
}

PostgreDataSession::~PostgreDataSession()
{
    _handle->close();
}


PooledDataSession::PooledDataSession(shared_ptr<AbstractLogger> logger,
                                     shared_ptr<AbstractDataSession> session,
                                     std::shared_ptr<PooledDataStorageFactory> owner)
    : _session(move(session)), _owner(owner)
{
    setLogger(logger);

    if(!_session)
    {
        LOG_DEBUG(logger, "session == nullptr");
        throw invalid_argument("session cannot be nullptr");
    }
    session->setLogger(move(logger));
}


PooledDataSession::~PooledDataSession()
{
    if(_owner)
    {
        LOG_TRACE(_logger, "session object being returned to session pool.");
        _owner->addToPool(move(_session));
    }
}