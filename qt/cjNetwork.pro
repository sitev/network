#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T13:43:38
#
#-------------------------------------------------

QT       -= core gui
z
TARGET = cjNetwork
TEMPLATE = lib
CONFIG += c++14 staticlib

HEADERS += \
    ../src/cjNetwork.h \
    ../src/socket.h \
    ../src/webServer.h

SOURCES += \
    ../src/socket.cpp \
    ../src/webServer.cpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
