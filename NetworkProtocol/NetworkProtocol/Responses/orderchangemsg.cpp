#include "orderchangemsg.h"

#include <utilities.h>
#include <stdexcept>

namespace NetworkProtocol
{
namespace Responses
{

using namespace DTO;

OrderChange::OrderChange(Types::OrderIdType orderId,
                                   Types::AmountType amount,
                                   Types::PriceType price)
    : OrderChange(std::move(GlobalUtilities::getLogger()), orderId, amount,
                       price)
{}

OrderChange::OrderChange(std::shared_ptr<AbstractLogger> logger,
                                   Types::OrderIdType orderId,
                                   Types::AmountType amount,
                                   Types::PriceType price)
    : _orderId(orderId), _amount(amount), _price(price)
{

    if(_orderId <= 0 || _amount <= 0 || _price <= 0)
    {
        LOG_TRACE(logger,
                  QString("Invalid arguments: orderId(%1) <= 0 || amount(%2) <= 0"\
                          " || price(%3) <= 0.")
                  .arg(_orderId.value).arg(_amount.value).arg(_price.value));

        throw std::invalid_argument("One of orderId, amount, price is <= 0.");
    }
}

void OrderChange::send(QIODevice *connection)
{
    QDataStream out(connection);

    sendHeader(out);
    out  << _orderId
         << _amount
         << _price;
}

Types::Message::MessageType OrderChange::type() const
{
    return Types::Message::MessageType::RESPONSE_ORDER_CHANGE;
}

Types::Message::MessageLengthType OrderChange::length() const
{
    return Response::length() + sizeof(_orderId) +
            sizeof(_amount) + sizeof(_price);
}

Types::OrderIdType OrderChange::getOrderId() const
{
    return _orderId;
}
Types::AmountType OrderChange::getAmount() const
{
    return _amount;
}
Types::PriceType OrderChange::getPrice() const
{
    return _price;
}


}
}
