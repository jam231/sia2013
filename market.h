#ifndef MARKET_H
#define MARKET_H

#include "server.h"
#include "offer.h"
#include "configmanager.h"

#include <exception>


#include <QTimer>
#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QPair>
#include <bestorder.h>

class DatabaseError : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Database error";
    }
};

Q_DECLARE_METATYPE(Order)

class Market : public QObject
{
    Q_OBJECT
    const QString BUY_TRANSACTIONS_CHANNEL = "ch_zlecenia_kupna";
    const QString SELL_TRANSACTIONS_CHANNEL = "ch_zlecenia_sprzedazy";
    const QString CHANGE_CHANNEL = "ch_zmiana";

    Server* m_server;
    QSqlDatabase m_database;
    QTimer* m_sessionOnTimer;
    QTimer* m_sessionOffTimer;

    QHash<qint32, QPair<qint32, qint32> > m_cachedLastTransaction;
    QHash<qint32, QPair<qint32, qint32> > m_cachedBestSellOrders;
    QHash<qint32, QPair<qint32, qint32> > m_cachedBestBuyOrders;

protected:

   void changeCachedBestSellOrders(qint32 stockId);
   void changeCachedBestBuyOrders(qint32 stockId);

public:
    Market(const ConfigManager<>& config, QObject* parent = 0);
    ~Market();

public slots:
    void registerNewUser(Connection*, QString password);
    void loginUser(Connection*, qint32 userId, QString password);

    void startSession();
    void stopSession();

    void notificationHandler(const QString&, QSqlDriver::NotificationSource, const QVariant &);

    void sellStock(qint32 userId,qint32 stockId, qint32 amount, qint32 price);
    void buyStock(qint32 userId, qint32 stockId, qint32 amount, qint32 price);

    void getMyStocks(qint32 userId);
    void getMyOrders(qint32 userId);
    void getStockInfo(qint32 userId, qint32 stockId);

    void cancelOrder(qint32 userId, qint32 orderId);
};

#endif // MARKET_H
