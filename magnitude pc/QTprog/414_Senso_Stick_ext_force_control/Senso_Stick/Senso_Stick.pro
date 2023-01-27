#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T16:10:06
#
#-------------------------------------------------

QT       += core gui network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Senso_Stick
TEMPLATE = app

win32:INCLUDEPATH += C:/Libs/CAN/pcan-light/Include/C++

LIBS += -LC:/Libs/CAN/pcan-light/x64/VC_LIB
LIBS += -lPCAN_PCI

DESTDIR = bin


SOURCES += main.cpp\
        senso_stick.cpp \
    can.cpp

HEADERS  += senso_stick.h \
    can.h

FORMS    += senso_stick.ui
