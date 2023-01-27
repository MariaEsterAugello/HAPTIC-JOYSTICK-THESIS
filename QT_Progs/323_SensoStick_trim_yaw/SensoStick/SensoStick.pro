QT       += core gui network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH +=  D:/external_libs/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include\
                D:\external_libs\VJoy\vJoy218SDK-291116\SDK\inc
QMAKE_LIBDIR += D:/external_libs/SDL2-devel-2.0.10-VC/SDL2-2.0.10/lib/x64\
                D:\external_libs\VJoy\vJoy218SDK-291116\SDK\lib\amd64

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qJoyStick.cpp

HEADERS += \
    mainwindow.h \
    qJoyStick.h \
    settings.h

FORMS += \
    mainwindow.ui

LIBS += -lSDL2 \
        -lvJoyInterface
win32
{
    DEFINES += SDL_WIN
}

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
