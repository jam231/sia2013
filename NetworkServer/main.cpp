#include <QCoreApplication>
#include <QString>


#include "market.h"

#include <configmanager.h>

#include <utilities.h>
#include <logger.h>

int main(int argv, char **args)
{
    QCoreApplication app(argv, args);

    LoggerFactory* factory = new LoggerFactory();
    GlobalUtilities::setLogger(factory->create(LoggingLevel::Trace, new FileWriter("hejho.txt")));
    delete factory;

    try {

        QString configFilePath = "Files/config.ini";
        ConfigManager<> config(configFilePath);

        LOG_INFO("\t!TEST!");

        Market market(config);

        return app.exec();


    }catch(const std::exception& e)
    {
        qDebug() << "[main] Złapano wyjątek: "
                 << e.what();

        return 1;
    }
    catch(...)
    {
        qDebug() << "[main] Załapano nieznany wyjątek.";

        return 2;
    }
    return 0;
}
