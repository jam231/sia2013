#ifndef ORDERIDRESP_H
#define ORDERIDRESP_H

#include "response.h"

namespace NetworkProtocol
{
namespace Responses
{

class NETWORKPROTOCOLSHARED_EXPORT OrderIdResp : public Response
{
    DTO::Types::OrderIdType m_orderId;

public:
    OrderIdResp(DTO::Types::OrderIdType orderId);

    DTO::Types::MessageLengthType length() const;
    DTO::Types::Message::MessageType type() const;

    static const QString getMessageName();

    void send(QIODevice *connection);
};

}
}
#endif // ORDERIDRESP_H