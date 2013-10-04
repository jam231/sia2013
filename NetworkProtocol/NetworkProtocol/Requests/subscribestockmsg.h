#ifndef SUBSCRIBESTOCKMSG_H
#define SUBSCRIBESTOCKMSG_H

#include "request.h"

namespace NetworkProtocol {
namespace Requests
{
class NETWORKPROTOCOLSHARED_EXPORT SubscribeStock : public Request
{
    DTO::Types::StockIdType m_stockId;

public:
    SubscribeStock(QDataStream& msg);

    DTO::Types::MessageLengthType length() const;

    DTO::Types::Message::MessageType type() const;
    DTO::Types::StockIdType getStockId() const;

    static const QString getMessageName();
};
}
}
#endif // SUBSCRIBESTOCKMSG_H
