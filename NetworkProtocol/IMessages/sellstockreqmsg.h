#ifndef SELLSTOCKREQMSG_H
#define SELLSTOCKREQMSG_H

#include "imessage.h"

namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT SellStockReqMsg : public IMessage
{
    qint32 m_stockId;
    qint32 m_amount;
    qint32 m_price;

    qint16 length() const;
public:
    SellStockReqMsg(QDataStream& msg);

    MessageType type() const;
    qint32 getStockId() const;
    qint32 getAmount() const;
    qint32 getPrice() const;
};

}

#endif // SELLSTOCKREQMSG_H