#include "tradingserver.h"

#include "Utils/utils.h"

#include <stdexcept>


#include <../NetworkProtocol/Responses/failuremsg.h>
#include <../NetworkProtocol/Responses/okmsg.h>

#include <../NetworkProtocol/Responses/ordercompletedmsg.h>
#include <../NetworkProtocol/Responses/orderchangemsg.h>
#include <../NetworkProtocol/Responses/showbestordermsg.h>
#include <../NetworkProtocol/Responses/lasttransactionmsg.h>
#include <../NetworkProtocol/Responses/orderacceptedmsg.h>
#include <../NetworkProtocol/Responses/listofordersmsg.h>
#include <../NetworkProtocol/Responses/listofstocksmsg.h>


#include <../NetworkProtocol/networkprotocol_utilities.h>


using namespace NetworkProtocol;
using namespace DTO;
using namespace Types;

using namespace std;



TradingServer::TradingServer(std::shared_ptr<AbstractLoggerFactory> loggerFactory,
                             std::shared_ptr<AbstractDataStorageFactory> datastorageFactory,
                             std::shared_ptr<SharedSet<UserIdType> > online_users)
    : _loggerFactory(loggerFactory), _dataStorageFactory(datastorageFactory),
      _online_users(online_users)
{
    moveToThread(this);

    if(!_loggerFactory)
    {
        throw invalid_argument("loggerFactory cannot be nullptr.");
    }

    auto logger = _loggerFactory->createLoggingSession();

    if(!_dataStorageFactory)
    {
        LOG_TRACE(logger, "dataStorageFactory cannot be nullptr.");
        throw invalid_argument("dataStorageFactory cannot be nullptr.");
    }
}

void TradingServer::run()
{
    LOG_INFO(_loggerFactory->createLoggingSession(),
             "Starting new Trading Server...");
    QThread::run();
}

void TradingServer::addUserConnection(UserConnection *user_connection)
{
    auto logger = _loggerFactory->createLoggingSession();
    auto user_id = user_connection->getUserId();

    user_connection->configureConnections();

    if(!_userConnections.contains(user_id))
    {
        _userConnections.insert(user_id, user_connection);
        connect(user_connection,  SIGNAL(disconnected(NetworkProtocol::DTO::Types::UserIdType)),
                this,              SLOT(removeConnection(NetworkProtocol::DTO::Types::UserIdType)));
        connect(user_connection,  SIGNAL(readyRead(NetworkProtocol::DTO::Types::UserIdType)),
                this,             SLOT(processMessageFrom(NetworkProtocol::DTO::Types::UserIdType)));

        LOG_INFO(logger, QString("Added user(%1)").arg(user_id.value));
        LOG_INFO(logger, QString("Users online: %1").arg(_userConnections.size()));

        Responses::Ok confirm_login;
        user_connection->send(&confirm_login);

        processMessageFrom(user_id);
    }
    else
    {
        _online_users->remove(user_id);
        LOG_WARNING(logger, QString("User(%1) is already on logged on this"\
                                    " trading server. This shouldn't happen.")
                            .arg(user_id.value));
        user_connection->disconnected();
        user_connection->deleteLater();
    }
}

void TradingServer::processMessageFrom(UserIdType userId)
{
    auto logger = _loggerFactory->createLoggingSession();

    LOG_TRACE(logger, QString("Reading from user(%1).").arg(userId.value));

    assert( _userConnections.contains(userId));

    auto source = _userConnections[userId];
    auto socket = source->getSocket();

    QDataStream stream(socket);
    try
    {
        while(_userConnections.contains(userId))
        {
            try
            {
            // Log transaction: request -> data access -> response
            auto logger = _loggerFactory->createLoggingSession();
            auto request = Requests::fromStream(logger, stream);

            switch(request->type())
            {
                case Message::REQUEST_SELL_STOCK_ORDER:
                {
                    handleRequest(logger, static_cast<Requests::SellStock*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_BUY_STOCK_ORDER:
                {
                    handleRequest(logger, static_cast<Requests::BuyStock*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_CANCEL_ORDER:
                {
                    handleRequest(logger, static_cast<Requests::CancelOrder*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_GET_MY_STOCKS:
                {
                    handleRequest(logger, static_cast<Requests::GetMyStocks*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_GET_MY_ORDERS:
                {
                    handleRequest(logger, static_cast<Requests::GetMyOrders*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_SUBSCRIBE_STOCK:
                {
                    handleRequest(logger, static_cast<Requests::SubscribeStock*>(request.get()),
                                  userId);
                }
                break;
                case Message::REQUEST_UNSUBSCRIBE_STOCK:
                {
                    handleRequest(logger, static_cast<Requests::UnsubscribeStock*>(request.get()),
                                  userId);
                }
                break;
                default:
                    handleRequest(logger, request.get(), userId);
                break;
            }
            }
            catch(Requests::MalformedRequest& e)
            {
                LOG_TRACE(logger, QString("Malformed request: %1").arg(e.what()));

                auto response = Responses::Failure(Failure::MALFORMED_MESSAGE);
                source->send(&response);
            }
            catch(Requests::InvalidRequestType& e)
            {
                LOG_TRACE(logger, QString("Invalid request type: %1").arg(e.what()));

                auto response = Responses::Failure(Failure::UNRECOGNIZED_MESSAGE);
                source->send(&response);
            }
            catch(Requests::InvalidRequestBody& e)
            {
                LOG_TRACE(logger, QString("Invalid request body: %1").arg(e.what()));

                auto response = Responses::Failure(Failure::INVALID_MESSAGE_BODY);
                source->send(&response);
            }
            catch(DatastoreError& e)
            {
                LOG_WARNING(logger, QString("Database error %1").arg(e.what()));

                auto response = Responses::Failure(Failure::REQUEST_DROPPED);
                source->send(&response);
            }
        }
    }
    catch(Requests::IncompleteRequest& e)
    {
        LOG_TRACE(logger, QString("Request not yet ready: %1").arg(e.what()));
    }
    catch(...)
    {
        LOG_ERROR(logger, QString("Connection(%1) has thrown unknown exception")
                          .arg(userId.value));

        auto response = Responses::Failure(Failure::REQUEST_DROPPED);
        source->send(&response);
    }
}

void TradingServer::removeConnection(UserIdType userId)
{
    auto logger = _loggerFactory->createLoggingSession();

    LOG_INFO(logger, QString("Removing user connection(%1)").arg(userId.value));

    auto connection = _userConnections[userId];
    connection->disconnect();

    _userConnections.remove(userId);
    _online_users->remove(userId);

    LOG_INFO(logger, QString("Users on current trading server: %1")
                    .arg(_userConnections.size()));

    LOG_DEBUG(logger, QString("Overall user count: %1")
              .arg(_online_users->size()));

    // Remove user subscriptions
    for(auto subscribers = _stock_subscribers.begin();
             subscribers != _stock_subscribers.end(); subscribers++)
    {
        subscribers->remove(userId);
    }

    connection->deleteLater();
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::BuyStock* request, UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) ordered buying %3 stock(%2) with limit %4.")
              .arg(userId.value).arg(request->getStockId().value)
              .arg(request->getAmount().value).arg(request->getPrice().value));

    auto source = _userConnections[userId];
    auto data_session = _dataStorageFactory->openSession();

    Failure::FailureType status;
    auto order_id = data_session->buyStock(userId, request->getStockId(),
                                           request->getAmount(), request->getPrice(),
                                           &status);
    if(status == Failure::NO_FAILURE)
    {
        LOG_DEBUG(logger, QString("Order(%1) accepted.").arg(order_id.value));

        auto response = Responses::OrderAccepted(order_id);
        source->send(&response);
    }
    else
    {
        LOG_DEBUG(logger, QString("Request failed. Failure status: %1")
                          .arg(status));

        auto response = Responses::Failure(status);
        source->send(&response);
    }
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::SellStock* request, UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) ordered selling %3 stock(%2) with limit %4.")
              .arg(userId.value).arg(request->getStockId().value)
              .arg(request->getAmount().value).arg(request->getPrice().value));

    auto source = _userConnections[userId];
    auto data_session = _dataStorageFactory->openSession();

    Failure::FailureType status;
    auto order_id = data_session->sellStock(userId, request->getStockId(),
                                           request->getAmount(), request->getPrice(),
                                           &status);
    if(status == Failure::NO_FAILURE)
    {
        LOG_DEBUG(logger, QString("Order(%1) accepted.").arg(order_id.value));

        auto response = Responses::OrderAccepted(order_id);
        source->send(&response);
    }
    else
    {
        LOG_DEBUG(logger, QString("Request failed. Failure status: %1")
                          .arg(status));

        auto response = Responses::Failure(status);
        source->send(&response);
    }
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::CancelOrder* request,
                                  UserIdType userId)
{
    auto order_id = request->getOrderId();
    auto source = _userConnections[userId];

    LOG_DEBUG(logger, QString("User(%1) wants to cancel order(%2).")
                      .arg(userId.value).arg(order_id.value));

    auto data_session = _dataStorageFactory->openSession();

    Failure::FailureType status;
    data_session->cancelOrder(userId, order_id, &status);

    if(status == Failure::NO_FAILURE)
    {
        LOG_DEBUG(logger, QString("Order(%1) have been successfully canceled.")
                          .arg(order_id.value));

        Responses::Ok response;
        source->send(&response);
    }
    else
    {
        LOG_DEBUG(logger, QString("Cannot cancel order(%1). Failure status: %2.")
                          .arg(order_id.value).arg(status));

        auto response = Responses::Failure(status);
        source->send(&response);
    }
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::GetMyOrders*,
                                  UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) requested list of his orders.")
                      .arg(userId.value));

    auto source = _userConnections[userId];
    auto data_session = _dataStorageFactory->openSession();

    Failure::FailureType status;
    auto user_orders = data_session->getUserOrders(userId, &status);

    if(status == Failure::NO_FAILURE)
    {
        Responses::ListOfOrders response;

        for(auto it = user_orders.begin(); it != user_orders.end(); it++)
        {
            LOG_TRACE(logger, QString("Adding to list of orders: (orderId(%1), "\
                                      "orderType(%2), stockId(%3), amount(%4), price(%5)).")
                      .arg((*it)->getOrderId().value).arg((*it)->getOrderType())
                      .arg((*it)->getStockId().value).arg((*it)->getAmount().value)
                      .arg((*it)->getPrice().value));

            response.addOrder(*it);
        }

        LOG_DEBUG(logger, QString("Sending list(%1) of orders to user(%2).")
                          .arg(user_orders.size()).arg(userId.value));

        source->send(&response);
    }
    else
    {
        LOG_WARNING(logger, QString("That shouldn't happen: %1").arg(status));

        auto response = Responses::Failure(status);
        source->send(&response);
    }
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::GetMyStocks*,
                                  UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) requested list of his stocks.")
                      .arg(userId.value));

    auto source = _userConnections[userId];
    auto data_session = _dataStorageFactory->openSession();

    Failure::FailureType status;
    auto user_stocks = data_session->getUserStocks(userId, &status);

    if(status == Failure::NO_FAILURE)
    {
        Responses::ListOfStocks response;

        auto stocks = user_stocks.getUserStocks();

        for(auto it = stocks->begin(); it != stocks->end(); it++)
        {
            LOG_TRACE(logger, QString("Adding to list of stocks: (stockId(%1), amount(%2))")
                              .arg(it->first.value).arg(it->second.value));

            response.addStock(it->first, it->second);
        }

        LOG_DEBUG(logger, QString("Sending list(%1) of stocks to user(%2).")
                          .arg(stocks->size()).arg(userId.value));

        source->send(&response);
    }
    else
    {
        LOG_WARNING(logger, QString("That shouldn't happen: %1").arg(status));

        auto response = Responses::Failure(status);
        source->send(&response);
    }
}

/// FIXME:
///  We allow user to subscribe to nonexistent stock - this is a potential bug,
///  as it allows user to allocate new QSet<UserId> objects for
///  every value possible of StockIdType (which is quint32).
///
///  Possible solution is to ask database (before accepting any users) for list
///  of subscribeable stockIds.
///
void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::SubscribeStock* request,
                                  UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) request subscription for stock(%2).")
                      .arg(userId.value).arg(request->getStockId().value));

    auto source = _userConnections[userId];

    auto subscribers = _stock_subscribers[request->getStockId()];
    subscribers.insert(userId);

    Responses::Ok response;
    source->send(&response);
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::UnsubscribeStock* request,
                                  UserIdType userId)
{
    LOG_DEBUG(logger, QString("User(%1) request canceling subscription for stock(%2).")
                      .arg(userId.value).arg(request->getStockId().value));

    auto source = _userConnections[userId];

    auto subscribers = _stock_subscribers[request->getStockId()];
    subscribers.remove(userId);

    Responses::Ok response;
    source->send(&response);
}

void TradingServer::handleRequest(std::shared_ptr<AbstractLogger> logger,
                                  Requests::Request* request,
                                  UserIdType userId)
{
    auto source = _userConnections[userId];

    LOG_DEBUG(logger, QString("Sending failure: not authorized to request %1")
             .arg(request->type()));

    auto response = Responses::Failure(Failure::NOT_AUTHORIZED);
    source->send(&response);
}

void TradingServer::connectDataHub(const AbstractDataHub& hub)
{
    connect(&hub, SIGNAL(orderChange(NetworkProtocol::DTO::Types::UserIdType,
                                               NetworkProtocol::DTO::Types::OrderIdType,
                                               NetworkProtocol::DTO::Types::AmountType,
                                               NetworkProtocol::DTO::Types::PriceType)),
            this, SLOT(orderChange(NetworkProtocol::DTO::Types::UserIdType,
                                        NetworkProtocol::DTO::Types::OrderIdType,
                                        NetworkProtocol::DTO::Types::AmountType,
                                        NetworkProtocol::DTO::Types::PriceType)));

    connect(&hub, SIGNAL(orderCompleted(NetworkProtocol::DTO::Types::UserIdType,
                                        NetworkProtocol::DTO::Types::OrderIdType)),
            this, SLOT(orderCompleted(NetworkProtocol::DTO::Types::UserIdType,
                                      NetworkProtocol::DTO::Types::OrderIdType)));
}

void TradingServer::orderCompleted(UserIdType userId, OrderIdType orderId)
{
    auto logger = _loggerFactory->createLoggingSession();

    if(_userConnections.contains(userId))
    {
        auto target = _userConnections[userId];

        LOG_DEBUG(logger, QString("Sending notification to user(%1) of"\
                                  "completion of order(%2)")
                          .arg(userId.value).arg(orderId.value));

        auto response = Responses::OrderCompleted(orderId);
        target->send(&response);
    }
    else
    {
        LOG_TRACE(logger, QString("User(%1) not found on current trading server")
                            .arg(userId.value));
    }
}

void TradingServer::orderChange(UserIdType userId, OrderIdType orderId,
                                     AmountType amount, PriceType price)
{
    auto logger = _loggerFactory->createLoggingSession();

    if(_userConnections.contains(userId))
    {
        auto target = _userConnections[userId];

        LOG_DEBUG(logger, QString("Sending notification to user(%1) of"\
                                  "(semi-)realization of order(%2) on "\
                                  "amount(%3) and price(%4).")
                          .arg(userId.value).arg(orderId.value)
                          .arg(amount.value).arg(price.value));

        auto response = Responses::OrderChange(logger, orderId, amount, price);
        target->send(&response);
    }
    else
    {
        LOG_TRACE(logger, QString("User(%1) not found on current trading server.")
                            .arg(userId.value));
    }
}

void TradingServer::newLastTransaction(LastTransaction* lastTransaction)
{
    auto logger = _loggerFactory->createLoggingSession();

    LOG_TRACE(logger, QString("New last transaction - amount(%1), price(%2), date(%3)")
                      .arg(lastTransaction->getAmount().value)
                      .arg(lastTransaction->getPrice().value)
                      .arg(lastTransaction->getDateTime()));

    _lastTransaction.reset(lastTransaction);
}

void TradingServer::newBestBuyOrder(BestOrder* bestBuy)
{
    auto logger = _loggerFactory->createLoggingSession();

    assert(bestBuy->getOrderType() == Order::BUY);

    LOG_TRACE(logger, QString("New best buy order - stockId(%1), amount(%2), price(%3).")
                      .arg(bestBuy->getStockId().value)
                      .arg(bestBuy->getAmount().value)
                      .arg(bestBuy->getPrice().value));

    auto stockId = bestBuy->getStockId();
    _bestBuyOrder[stockId].reset(bestBuy);

    // Broadcast it to subscribers
    auto subscribers = _stock_subscribers[stockId];
    auto show_best_order = Responses::ShowBestOrder(*bestBuy);

    for(auto user_id = subscribers.begin(); user_id != subscribers.end(); user_id++)
    {
        if(_userConnections.contains(*user_id))
        {
            _userConnections[*user_id]->send(&show_best_order);
        }
    }
}

void TradingServer::newBestSellOrder(BestOrder* bestSell)
{
    auto logger = _loggerFactory->createLoggingSession();

    assert(bestSell->getOrderType() == Order::SELL);

    LOG_TRACE(logger, QString("New best sell order - stockId(%1), amount(%2), price(%3).")
                      .arg(bestSell->getStockId().value)
                      .arg(bestSell->getAmount().value)
                      .arg(bestSell->getPrice().value));

    auto stockId = bestSell->getStockId();
    _bestSellOrder[stockId].reset(bestSell);

    // Broadcast it to subscribers
    auto subscribers = _stock_subscribers[stockId];
    auto show_best_order = Responses::ShowBestOrder(*bestSell);

    for(auto user_id = subscribers.begin(); user_id != subscribers.end(); user_id++)
    {
        if(_userConnections.contains(*user_id))
        {
            _userConnections[*user_id]->send(&show_best_order);
        }
    }
}
