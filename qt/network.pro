#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T13:58:03
#
#-------------------------------------------------

QT       -= core gui

TARGET = network
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../../core/src

SOURCES += \
    ../src/socket.cpp \
    ../src/func.cpp \
    ../src/socket_handler.cpp \
    ../src/wsa_socket_handler.cpp

HEADERS += \
    ../src/network.h \
    ../src/socket.h \
    ../src/func.h \
    ../src/socket_handler.h \
    ../src/wsa_socket_handler.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
