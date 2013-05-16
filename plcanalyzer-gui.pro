#-------------------------------------------------
#
# Project created by QtCreator 2013-04-13T18:29:29
#
#-------------------------------------------------

QT       += core gui

TARGET = plcanalyzer-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    openS7online.c \
    s7connection.cpp

HEADERS  += mainwindow.h \
    nodave.h \
    openS7online.h \
    log2.h \
    s7connection.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    lib/s7onlinx.dll \
    lib/libnodave.lib \
    lib/libnodave.dll

DEFINES += BCCWIN

unix:!macx|win32: LIBS += -L$$PWD/lib/ -llibnodave

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32: PRE_TARGETDEPS += $$PWD/lib/libnodave.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/lib/liblibnodave.a
