#ifndef GETMYORDERSRESPMSG_H
#define GETMYORDERSRESPMSG_H


#include "omessage.h"

#include "DataTransferObjects/order.h"

#include <QVector>
#include <QPair>

namespace NetworkProtocol {

class NETWORKPROTOCOLSHARED_EXPORT  GetMyOrdersRespMsg : public OMessage
{
    QVector<QPair<qint32,DTO::Order> > m_orders;

    qint16 length() const;
public:
    GetMyOrdersRespMsg();

    MessageType type() const;
    void send(QIODevice* connection);

    void addOrder(qint32 orderId, DTO::Order::OrderType m_transactionType, qint32 m_stockId, qint32 m_amount,
                  qint32 m_price);
    void addOrder(qint32 orderId, DTO::Order order);
};

}
#endif // GETMYORDERSRESPMSG_H