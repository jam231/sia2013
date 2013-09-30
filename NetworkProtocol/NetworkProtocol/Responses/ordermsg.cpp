#include "ordermsg.h"

namespace NetworkProtocol
{
namespace Responses
{

using namespace DTO;

OrderMsg::OrderMsg(Types::OrderIdType orderId, Types::OrderType orderType,
                   Types::StockIdType stockId, Types::AmountType amount, Types::PriceType price)
    : m_order(orderId, orderType, stockId, amount, price)
{
}

OrderMsg::OrderMsg(DTO::Order order) : m_order(order)
{
}

//OrderMsg::OrderMsg() : m_order()
//{
//}

Message::MessageType OrderMsg::type() const
{
    return ORDER;
}

void OrderMsg::send(QIODevice* connection)
{
    // Domyślnie BigEndian
    QDataStream out(connection);

    sendHeader(out);
    out << m_order;
}

qint16 OrderMsg::length() const
{
    return sizeof(MessageType) + DTO::Order::lengthInBytes;
}

const Order& OrderMsg::getOrder() const
{
    return m_order;
}

}
}
