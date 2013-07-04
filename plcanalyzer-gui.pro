#-------------------------------------------------
#
# Project created by QtCreator 2013-04-13T18:29:29
#
#-------------------------------------------------

#Change Linux to BCCWIN here

unix: CONFIG += LINUX
win32: CONFIG += BCCWIN
QT       += core gui

TARGET = plcanalyzer-gui
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    s7connection.cpp \
    connectionsettings.cpp \
    stdoutredirector.cpp \
    logtoparent.cpp

HEADERS  += mainwindow.h \
    s7connection.h \
        connectionsettings.h \
    stdoutredirector.h \
    logtoparent.h

FORMS    += mainwindow.ui \
    connectionsettings.ui

OTHER_FILES += \
    lib/s7onlinx.dll \
    lib/libnodave.lib \
    lib/libnodave.dll \
    lib/libnodave64.so \
    README.md

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

QMAKE_CXXFLAGS += -std=c++0x
