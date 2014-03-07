#ifndef ORDERACCEPTEDMSG_H
#define ORDERACCEPTEDMSG_H

#include "response.h"

namespace NetworkProtocol
{
namespace Responses
{
/*
 *  Responses :: OrderAccepted
 *
 *  This message is for sending confirmation of acceptance of new order.
 *  Message contains order's id.
 *
 *  Message format:
 *  <message_length : 2><message_type : 1><order_id : 4>
 *
 *  where
 *      message_type         = MessageType::RESPONSE_ORDER_ACCEPTED;
 */
class NETWORKPROTOCOLSHARED_EXPORT OrderAccepted : public Response
{
    DTO::Types::OrderIdType _orderId;

public:
    OrderAccepted(DTO::Types::OrderIdType orderId);

    DTO::Types::Message::MessageLengthType length() const;
    DTO::Types::Message::MessageType type() const;


    void send(QIODevice *connection);
};

}
}
#endif // ORDERACCEPTEDMSG_H
