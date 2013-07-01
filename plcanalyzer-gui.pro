#-------------------------------------------------
#
# Project created by QtCreator 2013-04-13T18:29:29
#
#-------------------------------------------------

#Change Linux to BCCWIN here
CONFIG += BCCWIN

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
    connectionsettings.h \
    openSocket.h

FORMS    += mainwindow.ui \
    connectionsettings.ui

OTHER_FILES += \
    lib/s7onlinx.dll \
    lib/libnodave.lib \
    lib/libnodave.dll \
    lib/libnodave64.so

#DEFINES depend on CONFIG above
LINUX {
DEFINES += LINUX \
    DAVE_LITTLE_ENDIAN \
    DONT_USE_GETHOSTBYNAME
}
BCCWIN {
DEFINES += BCCWIN \
    DAVE_LITTLE_ENDIAN \
    DONT_USE_GETHOSTBYNAME
}

#LIBS depend on CONFIG above
BCCWIN {
LIBS += -L$$PWD/lib/ -llibnodave
}

LINUX {
LIBS += -L$$PWD/lib/ -lnodave64
}
INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/lib/

PRE_TARGETDEPS += $$PWD/lib/libnodave.lib
