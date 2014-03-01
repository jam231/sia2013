#include "bestorder_test.h"

#include <DataTransferObjects/types.h>

#include <utilities.h>
#include <stdexcept>
#include <assert.h>

using namespace NetworkProtocol::DTO;
using namespace Types;
using namespace Order;

Q_DECLARE_METATYPE(OrderType)
Q_DECLARE_METATYPE(StockIdType)
Q_DECLARE_METATYPE(AmountType)
Q_DECLARE_METATYPE(PriceType)


void BestOrderTest::initTestCase()
{
    GlobalUtilities::setLogger(make_logger(LoggingLevel::Off));
}

void BestOrderTest::generate_valid_data()
{
    QTest::addColumn<OrderType>("order_type");
    QTest::addColumn<StockIdType>("stock_id");
    QTest::addColumn<AmountType>("amount");
    QTest::addColumn<PriceType>("price");

    for(int i = 1; i < 60000; i += 10000)
    {
        QTest::newRow("SELL best order") << OrderType::SELL << StockIdType(i % 99)
                                         << AmountType(100) << PriceType(100);
    }
    for(int i = 1; i < 60000; i += 10000)
    {
        QTest::newRow("BUY best order") << OrderType::BUY << StockIdType(i % 99)
                                        << AmountType(100) << PriceType(100);
    }
}
void BestOrderTest::generate_invalid_data()
{
    QTest::addColumn<OrderType>("order_type");
    QTest::addColumn<StockIdType>("stock_id");
    QTest::addColumn<AmountType>("amount");
    QTest::addColumn<PriceType>("price");

    for(int i = 0; i < 60; i += 10)
    {
        QTest::newRow("BUY best order") << OrderType::BUY << StockIdType(-i)
                                       << AmountType(i) << PriceType(i);
    }
    for(int i = 0; i < 60; i += 10)
    {
        QTest::newRow("BUY best order") << OrderType::BUY << StockIdType(i % 20)
                                        << AmountType(-i) << PriceType(i);
    }
    for(int i = 0; i < 60; i += 10)
    {
        QTest::newRow("BUY best order") << OrderType::BUY << StockIdType(i % 10)
                                        << AmountType(i) << PriceType(-i);
    }
    for(int i = 1; i < 60; i += 10)
    {
        QTest::newRow("SELL best order") << OrderType::SELL << StockIdType(-i)
                                         << AmountType(i) << PriceType(i);
    }
    for(int i = 1; i < 60; i += 10)
    {
        QTest::newRow("SELL best order") << OrderType::SELL << StockIdType(i % 20)
                                         << AmountType(-i) << PriceType(i);
    }
    for(int i = 1; i < 60; i += 10)
    {
        QTest::newRow("SELL best order") << OrderType::SELL << StockIdType(i % 10)
                                         << AmountType(i) << PriceType(-i);
    }
}

void BestOrderTest::creation_valid_data()
{
    generate_valid_data();
}

void BestOrderTest::creation_valid()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    try
    {
        BestOrder best_order(order_type, stock_id, amount, price);
        QVERIFY2(best_order.getOrderType()  == order_type,  "Order type is corrupted.");
        QVERIFY2(best_order.getStockId()    == stock_id,    "Stock id is corrupted.");
        QVERIFY2(best_order.getAmount()     == amount,      "Amount is corrupted.");
        QVERIFY2(best_order.getPrice()      == price,       "Price corrupted.");
    }
    catch(...)
    {
        QFAIL("Exception has been thrown");
    }
}


void BestOrderTest::creation_invalid_data()
{
    generate_invalid_data();
}

void BestOrderTest::creation_invalid()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    try
    {
        BestOrder best_order(order_type, stock_id, amount, price);
        QFAIL("std::invalid_argument should have been thrown.");
    }
    catch(std::invalid_argument&)
    {
        return;
    }
    catch(...)
    {
        QFAIL("Unkown exception has been thrown");
    }
}

void BestOrderTest::toStream_valid_data()
{
    generate_valid_data();
}

void BestOrderTest::toStream_valid()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    try
    {
        BestOrder best_order(order_type, stock_id, amount, price);
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::ReadWrite);

        assert(stream.byteOrder() == QDataStream::BigEndian);

        stream << best_order;

        QVERIFY2(stream.device()->size() == best_order.lengthSerialized(),
                 qPrintable(QString("Bytes saved in device doesn't equal to (supposedly) written. "\
                            "Should be %1 is %2.")
                            .arg(best_order.lengthSerialized()).arg(stream.device()->size())));
    }
    catch(...)
    {
        QFAIL("Exception has been thrown");
    }
}


void BestOrderTest::fromStream_valid_data()
{
    generate_valid_data();
}


void BestOrderTest::fromStream_valid()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    try
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        assert(stream.byteOrder() == QDataStream::BigEndian);

        stream << order_type << stock_id << amount << price;
        stream.device()->reset();

        Types::Message::MessageLengthType correct_number_of_bytes =
                sizeof(OrderType) + sizeof(StockIdType) + sizeof(AmountType) + sizeof(PriceType);

        QVERIFY2(stream.device()->bytesAvailable() == correct_number_of_bytes,
                 qPrintable(QString("Bytes available in device doesn't equal to (supposedly) written. "\
                            "Should be %1 is %2.")
                            .arg(correct_number_of_bytes).arg(stream.device()->size())));

        BestOrder fro_stream = BestOrder::fromStream(stream);

        QVERIFY2(fro_stream.getOrderType() == order_type,
                 qPrintable(QString("Order type doesn't match. Is %1 should be %2")
                            .arg(fro_stream.getOrderType())
                            .arg(order_type)));
        QVERIFY2(fro_stream.getStockId() == stock_id,
                 qPrintable(QString("Stock id doesn't match. Is %1 should be %2")
                            .arg(fro_stream.getStockId().value)
                            .arg(stock_id.value)));
        QVERIFY2(fro_stream.getAmount() == amount,
                 qPrintable(QString("Amount doesn't match. Is %1 should be %2")
                            .arg(fro_stream.getAmount().value)
                            .arg(amount.value)));
        QVERIFY2(fro_stream.getPrice() == price,
                 qPrintable(QString("Price doesn't match. Is %1 should be %2")
                            .arg(fro_stream.getPrice().value)
                            .arg(price.value)));

    }
    catch(std::exception& e)
    {
        QFAIL(qPrintable(QString("Exception has been thrown : %1").arg(e.what())));
    }
    catch(...)
    {
        QFAIL("Unkown exception has been thrown.");
    }
}

void BestOrderTest::fromStream_invalid_data()
{
    generate_invalid_data();
}


void BestOrderTest::fromStream_invalid()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    try
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::ReadWrite);
        assert(stream.byteOrder() == QDataStream::BigEndian);

        stream << order_type << stock_id << amount << price;
        stream.device()->reset();

        Types::Message::MessageLengthType correct_number_of_bytes =
                sizeof(OrderType) + sizeof(StockIdType) + sizeof(AmountType) + sizeof(PriceType);

        QVERIFY2(stream.device()->bytesAvailable() == correct_number_of_bytes,
                 qPrintable(QString("Bytes available in device doesn't equal to (supposedly) written. "\
                            "Should be %1 is %2.")
                            .arg(correct_number_of_bytes ).arg(stream.device()->size())));

        BestOrder::fromStream(stream);
        QFAIL("std::invalid_argument should have been thrown.");

    }
    catch(std::invalid_argument&)
    {
        // This should have been thrown.
        return;
    }
    catch(...)
    {
        QFAIL("Unkown exception has been thrown.");
    }
}

void BestOrderTest::lengthSerialized_data()
{
    QTest::addColumn<OrderType>("order_type");
    QTest::addColumn<StockIdType>("stock_id");
    QTest::addColumn<AmountType>("amount");
    QTest::addColumn<PriceType>("price");

    for(int i = 1; i < 15000; i += 1000)
        QTest::newRow("SELL best order") << OrderType::SELL << StockIdType(i % 99)
                                         << AmountType(i) << PriceType(i);

    for(int i = 1; i < 15000; i += 1000)
        QTest::newRow("BUY best order") << OrderType::BUY << StockIdType(i % 99)
                                        << AmountType(i) << PriceType(i);

}

void BestOrderTest::lengthSerialized()
{
    QFETCH(OrderType, order_type);
    QFETCH(StockIdType, stock_id);
    QFETCH(AmountType, amount);
    QFETCH(PriceType, price);

    BestOrder best_order(order_type, stock_id, amount, price);

    Message::MessageLengthType su_of_sizeofs = sizeof(order_type) + sizeof(stock_id) +
                                                sizeof(amount) + sizeof(price);

    QVERIFY2(best_order.lengthSerialized() == su_of_sizeofs, "Best order length in bytes is incorrect.");
}

