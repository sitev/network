#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T13:58:03
#
#-------------------------------------------------

QT       -= core gui

TARGET = cjNetwork
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../../cjCore/src

SOURCES += \
    ../src/socket.cpp \
    ../src/webServer.cpp

HEADERS += \
    ../src/cjNetwork.h \
    ../src/socket.h \
    ../src/webServer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
