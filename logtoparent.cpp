/******************************************************************************
*   *File: logtoparent.cpp                                                    *
*   *Date: 2013-09-29                                                         *
*   *Version: 1.0                                                             *
*   *Author(s): Jochen Bauer <devel@jochenbauer.net>                          *
*               Lukas Kern <lukas.kern@online.de>                             *
*               Carsten Klein <hook-the-master@gmx.net>                       *
*                                                                             *
*   *License information:                                                     *
*                                                                             *
*   Copyright (C) [2013] [Jochen Bauer, Lukas Kern, Carsten Klein]            *
*                                                                             *
*   This file is part of PLCANALYZER. PLCANALYZER is free software; you can   *
*   redistribute it and/or modify it under the terms of the GNU General       *
*   Public License as published by the Free Software Foundation; either       *
*   version 2 of the License, or (at your option) any later version.          *
*                                                                             *
*   This program is distributed in the hope that it will be useful, but       *
*   WITHOUT ANY WARRANTY; without even the implied warranty of                *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA             *
*   02110-1301, USA.                                                          *
*                                                                             *
*   *Program name: PLCANALYZER                                                *
*   *What this program does:    Connects to most SIMATIC S7/S5 Controllers    *
*                               * Reads memory areas                          *
*                               * Draws a graph over time for operands        *
*   *Have fun!                                                                *
******************************************************************************/

#include "logtoparent.h"


//construct a redirector for piping the outputs to the
//textedit of the mainwindow
logToParent::logToParent(MainWindow *parent) :
    QMainWindow(parent)
{
    // See also the destructor description
    myRedirector = new stdoutRedirector;
    connect(this, SIGNAL(threadTerminator()),myRedirector,SLOT(terminate()));
    myRedirector->start();
    parentStatbar = parent->ui->statusBar;
    parentTextEdit = parent->ui->textEdit;
    connect(myRedirector, SIGNAL(writeOut(QString)),
            this,SLOT(updateParentLog(QString)));
}

logToParent::~logToParent()
{
    emit threadTerminator();
    // myRedirector should have been removed here
    // unfortunately that leads to hard thread destroying

}

//write the data to the statusbar and the textedit
//statbar will hold text for 2 and a half seconds
void logToParent::updateParentLog(const QString &logString)
{
    parentStatbar->showMessage(logString,2500);
    parentTextEdit->append(logString);

    // debug output with timestamp
    qDebug() << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss") << "Debug :" << logString;
}
