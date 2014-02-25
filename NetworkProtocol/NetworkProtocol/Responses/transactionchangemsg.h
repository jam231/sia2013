#ifndef TRANSACTIONCHANGEMSG_H
#define TRANSACTIONCHANGEMSG_H

#include "response.h"

namespace NetworkProtocol
{
namespace Responses
{

class NETWORKPROTOCOLSHARED_EXPORT TransactionChange : public Response
{
    DTO::Types::StockIdType m_stockId;
    DTO::Types::AmountType m_amount;
    DTO::Types::PriceType m_price;
    QString m_dateTime;

public:
    TransactionChange(DTO::Types::StockIdType, DTO::Types::AmountType,
                      DTO::Types::PriceType, QString);

    DTO::Types::Message::MessageLengthType length() const;
    DTO::Types::Message::MessageType type() const;

    static const QString getMessageName();

    void send(QIODevice *connection);
};

}
}
#endif // TRANSACTIONCHANGEMSG_H