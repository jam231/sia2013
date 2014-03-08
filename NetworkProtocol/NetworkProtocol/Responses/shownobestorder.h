#ifndef SHOWNOBESTORDER_H
#define SHOWNOBESTORDER_H

#include "response.h"

namespace NetworkProtocol
{
namespace Responses
{
/*
 *  Responses :: ShowNoBestOrder
 *
 *  This message is for notyfing user that currently there is no best order
 *  of given type for stock with given id.
 *
 *  Message format:
 *  <message_length : 2><message_type : 1><order_type : 1><stock_id : 4>
 *
 *  where
 *      message_type         = MessageType::RESPONSE_SHOW_NO_BEST_ORDER (0x25)
 */
class NETWORKPROTOCOLSHARED_EXPORT ShowNoBestOrder : public Response
{
    DTO::Types::Order::OrderType _orderType;
    DTO::Types::StockIdType _stockId;
public:
    ShowNoBestOrder(DTO::Types::Order::OrderType, DTO::Types::StockIdType);

    DTO::Types::Message::MessageLengthType length() const;
    DTO::Types::Message::MessageType type() const;


    void send(QIODevice *connection);
};

}
}
#endif // SHOWNOBESTORDER_H