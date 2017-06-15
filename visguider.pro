#-------------------------------------------------
#
# Project created by QtCreator 2017-06-07T18:06:34
#
#-------------------------------------------------
QMAKE_MAC_SDK = macosx10.12
include(/Users/hehao/marble/qt_Marble.pri)

QT += Marble
QT       += core gui

CONFIG   += precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = stdafx.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
  DEFINES += USING_PCH
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visguider
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    visuallayer.cpp

HEADERS  += mainwindow.h \
    visuallayer.h \
    stdafx.h

FORMS    += mainwindow.ui

RESOURCES += \
    visguider.qrc

DISTFILES += \
    res/temp.html \
    test.json \
    test_land.json \
    test_road.json \
    test_plane.json

INCLUDEPATH +=  /usr/local/include
LIBS += /usr/local/lib/libjansson.a
