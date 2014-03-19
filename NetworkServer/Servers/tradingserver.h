#ifndef TRADINGSERVER_H
#define TRADINGSERVER_H

#include <memory>

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include <QEventLoop>

#include <../NetworkProtocol/DataTransferObjects/types.h>

#include <../Utilities/logger.h>

#include <DataStorage/datastorage.h>

#include "connections.h"



/*
 *  Problems:
 *
 *
 * When adding userConnection, how to handle deleting QTcpSocket* in sender of
 * socket_descriptor ?
 *
 * Who is the sender of descriptor ?
 *
 * QHash needs to be thread safe.
 *
 *
 *
 */
class TradingServer : public QObject, public QRunnable
{
    Q_OBJECT

    QEventLoop _event_loop;
    // I need thread safety 1!!!
    QHash<NetworkProtocol::DTO::Types::UserIdType,
          std::shared_ptr<UserConnection> > _userConnections;

    std::shared_ptr<AbstractLoggerFactory> _loggerFactory;
    std::shared_ptr<AbstractDataStorageFactory> _dataStorageFactory;

public:
    TradingServer(std::shared_ptr<AbstractLoggerFactory> logger_factory,
                  std::shared_ptr<AbstractDataStorageFactory> datastorage_factory);

    void run();

public slots:

   void addUserConnection(std::shared_ptr<UserConnection> connection);


};



#endif // TRADINGSERVER_H