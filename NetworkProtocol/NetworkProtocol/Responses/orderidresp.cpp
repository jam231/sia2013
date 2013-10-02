#include "orderidresp.h"

namespace NetworkProtocol
{
namespace Responses
{

using namespace DTO;

OrderIdResp::OrderIdResp(Types::OrderIdType orderId) : m_orderId(orderId)
{
}

Types::MessageLengthType OrderIdResp::length() const
{
    return sizeof(Types::Message::MessageType) +
           sizeof(m_orderId);
}

Types::Message::MessageType OrderIdResp::type() const
{
    return Types::Message::MessageType::ORDER_ID_RESP;
}

void OrderIdResp::send(QIODevice* connection)
{
    // Domyślnie BigEndian.
    QDataStream out(connection);

    sendHeader(out);
    out << m_orderId;
}

const QString OrderIdResp::getMessageName()
{
    return Response::getMessageName() + " :: OrderIdResp";
}

}
}