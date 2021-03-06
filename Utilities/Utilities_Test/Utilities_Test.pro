QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wundef \
                  -Wpointer-arith

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

TARGET = Utilities_TEST

QT       += testlib

QT       -= gui

CONFIG   += console
CONFIG   += testcase
CONFIG   -= app_bundle

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    globalutilities_test.cpp \
    mockobjects.cpp \
    bufferedwriter_test.cpp \
    concurrentwriter_test.cpp

HEADERS += \
    globalutilities_test.h \
    mockobjects.h \
    bufferedwriter_test.h \
    concurrentwriter_test.h

win32:gcc:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Utilities/release/ -lUtilities
else:win32:gcc:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Utilities/debug/ -lUtilities
else:unix: LIBS += -L$$OUT_PWD/../Utilities/ -lUtilities

INCLUDEPATH += $$PWD/../Utilities
DEPENDPATH += $$PWD/../Utilities

win32:gcc:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utilities/release/libUtilities.a
else:win32:gcc:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utilities/debug/libUtilities.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Utilities/libUtilities.a
