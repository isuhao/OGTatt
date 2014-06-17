#-------------------------------------------------
#
# Project created by QtCreator 2014-06-11T04:38:47
#
#-------------------------------------------------

QT       += core gui svg opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ogtatt
TEMPLATE = app


SOURCES += main.cpp\
        gamewidget.cpp \
    gameobject.cpp \
    vehicle.cpp \
    person.cpp \
    player.cpp

HEADERS  += gamewidget.h \
    gameobject.h \
    vehicle.h \
    person.h \
    player.h

FORMS    +=

RESOURCES += \
    ogtatt.qrc
