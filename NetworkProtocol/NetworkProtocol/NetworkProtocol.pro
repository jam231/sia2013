QMAKE_CXXFLAGS += -std=c++0x -Wall -Wextra -Wfloat-equal -Wundef \
                  -Wpointer-arith

QT       += network

QT       -= gui

TARGET = NetworkProtocol
TEMPLATE = lib

DEFINES += NETWORKPROTOCOL_LIBRARY

SOURCES += \
    ../NetworkProtocol/Requests/unsubscribestockmsg.cpp \
    ../NetworkProtocol/Requests/subscribestockmsg.cpp \
    ../NetworkProtocol/Requests/sellstockmsg.cpp \
    ../NetworkProtocol/Requests/registerusermsg.cpp \
    ../NetworkProtocol/Requests/loginusermsg.cpp \
    ../NetworkProtocol/Requests/getstockinfomsg.cpp \
    ../NetworkProtocol/Requests/getmystocksmsg.cpp \
    ../NetworkProtocol/Requests/getmyordersmsg.cpp \
    ../NetworkProtocol/Requests/companystatusmsg.cpp \
    ../NetworkProtocol/Requests/cancelordermsg.cpp \
    ../NetworkProtocol/Requests/buystockmsg.cpp \
    ../NetworkProtocol/Responses/unrecognizedusermsg.cpp \
    ../NetworkProtocol/Responses/transactionchangemsg.cpp \
    ../NetworkProtocol/Responses/selltransactionmsg.cpp \
    ../NetworkProtocol/Responses/showordermsg.cpp \
    ../NetworkProtocol/Responses/orderidmsg.cpp \
    ../NetworkProtocol/Responses/loginusersuccessmsg.cpp \
    ../NetworkProtocol/Responses/loginuserfailuremsg.cpp \
    ../NetworkProtocol/Responses/registerusersuccessmsg.cpp \
    ../NetworkProtocol/Responses/registeruserfailuremsg.cpp \
    ../NetworkProtocol/Responses/companyfrozenmsg.cpp \
    ../NetworkProtocol/Responses/companyactivemsg.cpp \
    ../NetworkProtocol/Responses/buytransactionmsg.cpp \
    ../NetworkProtocol/Responses/bestordermsg.cpp \
    ../NetworkProtocol/Responses/stockinfomsg.cpp \
    ../NetworkProtocol/Responses/showuserordersmsg.cpp \
    ../NetworkProtocol/Responses/showuserstocksmsg.cpp \
    ../NetworkProtocol/Requests/request.cpp \
    ../NetworkProtocol/Responses/response.cpp \
    ../NetworkProtocol/message.cpp \
    DataTransferObjects/order.cpp \
    DataTransferObjects/bestorder.cpp \
    DataTransferObjects/types.cpp \
    DataTransferObjects/lasttransaction.cpp


HEADERS += \
    ../NetworkProtocol/networkprotocol_global.h \
    ../NetworkProtocol/Requests/unsubscribestockmsg.h \
    ../NetworkProtocol/Requests/subscribestockmsg.h \
    ../NetworkProtocol/Requests/sellstockmsg.h \
    ../NetworkProtocol/Requests/registerusermsg.h \
    ../NetworkProtocol/Requests/loginusermsg.h \
    ../NetworkProtocol/Requests/getstockinfomsg.h \
    ../NetworkProtocol/Requests/getmystocksmsg.h \
    ../NetworkProtocol/Requests/getmyordersmsg.h \
    ../NetworkProtocol/Requests/companystatusmsg.h \
    ../NetworkProtocol/Requests/cancelordermsg.h \
    ../NetworkProtocol/Requests/buystockmsg.h \
    ../NetworkProtocol/Responses/unrecognizedusermsg.h \
    ../NetworkProtocol/Responses/transactionchangemsg.h \
    ../NetworkProtocol/Responses/selltransactionmsg.h \
    ../NetworkProtocol/Responses/showordermsg.h \
    ../NetworkProtocol/Responses/orderidmsg.h \
    ../NetworkProtocol/Responses/loginusersuccessmsg.h \
    ../NetworkProtocol/Responses/loginuserfailuremsg.h \
    ../NetworkProtocol/Responses/registerusersuccessmsg.h \
    ../NetworkProtocol/Responses/registeruserfailuremsg.h \
    ../NetworkProtocol/Responses/companyfrozenmsg.h \
    ../NetworkProtocol/Responses/companyactivemsg.h \
    ../NetworkProtocol/Responses/buytransactionmsg.h \
    ../NetworkProtocol/Responses/bestordermsg.h \
    ../NetworkProtocol/Responses/stockinfomsg.h \
    ../NetworkProtocol/Responses/showuserordersmsg.h \
    ../NetworkProtocol/Responses/showuserstocksmsg.h \
    ../NetworkProtocol/Requests/request.h \
    ../NetworkProtocol/Responses/response.h \
    ../NetworkProtocol/message.h \
    DataTransferObjects/order.h \
    DataTransferObjects/bestorder.h \
    DataTransferObjects/types.h \
    DataTransferObjects/lasttransaction.h




unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
