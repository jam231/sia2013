#ifndef SHOWUSERORDERSMSG_H
#define SHOWUSERORDERSMSG_H


#include "response.h"

#include "DataTransferObjects/order.h"

#include <QVector>
#include <QPair>

#include <memory>

namespace NetworkProtocol
{
namespace Responses
{
/*
 *  Responses :: ShowUserOrders
 *
 *  This message is for sending user information about his pending orders.
 *
 *  Message format:
 *  <message_length : 2><message_type : 1><orders_count : 2><order : >^order_count
 *
 *  where
 *      message_type    = MessageType::RESPONSE_SHOW_USER_ORDERS
 *      <order : 19>    = <order_id : 4><order_type : 1><stock_id : 4><amount : 4><price : 4>
 */
class NETWORKPROTOCOLSHARED_EXPORT  ShowUserOrders : public Response
{    
    QVector<std::shared_ptr<DTO::Order> > _orders;

public:
    ShowUserOrders();

    DTO::Types::Message::MessageType type() const;

    DTO::Types::Message::MessageLengthType length() const;

    void send(QIODevice* connection);

    void addOrder(DTO::Types::OrderIdType orderId, DTO::Types::Order::OrderType _orderType,
                  DTO::Types::StockIdType _stockId, DTO::Types::AmountType _amount,
                  DTO::Types::PriceType _price);

    void addOrder(std::shared_ptr<DTO::Order>);

};

}
}
#endif // SHOWUSERORDERSMSG_H
