QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wfloat-equal -Wundef \
                  -Wpointer-arith

TARGET = NetworkProtocol_Test

QT       += testlib

QT       -= gui

CONFIG   += console
CONFIG   += testcase
CONFIG   -= app_bundle

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    DataTransferObjects/order_test.cpp \
    DataTransferObjects/lasttransaction_test.cpp \
    DataTransferObjects/bestorder_test.cpp \
    Requests/unsubscribestockmsg_test.cpp \
    Requests/subscribestockmsg_test.cpp \
    Requests/sellstockmsg_test.cpp \
    Requests/request_test.cpp \
    Requests/registerusermsg_test.cpp \
    Requests/loginusermsg_test.cpp \
    Requests/getstockinfomsg_test.cpp \
    Requests/getmystocksmsg_test.cpp \
    Requests/getmyordersmsg_test.cpp \
    Requests/companystatusmsg_test.cpp \
    Requests/cancelordermsg_test.cpp \
    Requests/buystockmsg_test.cpp \
    Responses/stockinfomsg_test.cpp \
    Responses/showuserordersmsg_test.cpp \
    Responses/showordermsg_test.cpp \
    Responses/response_test.cpp \
    Responses/failuremsg_test.cpp \
    Responses/okmsg_test.cpp \
    Responses/registerusersuccessmsg_test.cpp \
    Responses/showbestordermsg_test.cpp \
    Responses/showuserstocksmsg_test.cpp

HEADERS += \
    DataTransferObjects/order_test.h \
    DataTransferObjects/lasttransaction_test.h \
    DataTransferObjects/bestorder_test.h \
    Requests/unsubscribestockmsg_test.h \
    Requests/subscribestockmsg_test.h \
    Requests/sellstockmsg_test.h \
    Requests/request_test.h \
    Requests/registerusermsg_test.h \
    Requests/loginusermsg_test.h \
    Requests/getstockinfomsg_test.h \
    Requests/getmystocksmsg_test.h \
    Requests/getmyordersmsg_test.h \
    Requests/companystatusmsg_test.h \
    Requests/cancelordermsg_test.h \
    Requests/buystockmsg_test.h \
    Responses/stockinfomsg_test.h \
    Responses/showuserstocksmsg_test.h \
    Responses/showuserordersmsg_test.h \
    Responses/showordermsg_test.h \
    Responses/response_test.h \
    Responses/failuremsg_test.h \
    Responses/okmsg_test.h \
    Responses/registerusersuccessmsg_test.h \
    Responses/showbestordermsg_test.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../NetworkProtocol/release/ -lNetworkProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../NetworkProtocol/debug/ -lNetworkProtocol
else:unix: LIBS += -L$$OUT_PWD/../NetworkProtocol/ -lNetworkProtocol

INCLUDEPATH += $$PWD/../NetworkProtocol
DEPENDPATH += $$PWD/../NetworkProtocol


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Utilities/Utilities/release/ -lUtilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Utilities/Utilities/debug/ -lUtilities
else:unix: LIBS += -L$$OUT_PWD/../../Utilities/Utilities/ -lUtilities

INCLUDEPATH += $$PWD/../../Utilities/Utilities
DEPENDPATH += $$PWD/../../Utilities/Utilities

win32:gcc:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Utilities/Utilities/release/libUtilities.a
else:win32:gcc:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Utilities/Utilities/debug/libUtilities.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Utilities/Utilities/libUtilities.a
