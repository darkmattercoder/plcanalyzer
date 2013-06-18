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
    s7connection.cpp \
    connectionsettings.cpp

HEADERS  += mainwindow.h \
    nodave.h \
    s7connection.h \
    log2.h \
    StdOutRedirector.h \
    connectionsettings.h

FORMS    += mainwindow.ui \
    connectionsettings.ui

OTHER_FILES += \
    lib/s7onlinx.dll \
    lib/libnodave.lib \
    lib/libnodave.dll

DEFINES += BCCWIN \
    DAVE_LITTLE_ENDIAN

LIBS += -L$$PWD/lib/ -llibnodave

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/lib/

PRE_TARGETDEPS += $$PWD/lib/libnodave.lib
