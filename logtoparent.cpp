/*******************************************************************************
 *   *File: logtoparent.cpp                                                    *
 *   *Date: 2013-06-01                                                         *
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

logToParent::logToParent(MainWindow *parent) :
    QMainWindow(parent)
{
    myRedirector = new stdoutRedirector;
    connect(this, SIGNAL(threadTerminator()),myRedirector,SLOT(terminate()));
    std::cout << "Debug: Qthread for logging run: " << myRedirector->
                 isRunning() << std::endl;
    myRedirector->start();
    std::cout << "Debug: Logger constructed" << std::endl;
    std::cout << "Debug: Qthread for logging run: " << myRedirector->
                 isRunning() << std::endl;
    parentStatbar = parent->ui->statusBar;
    parentTextEdit = parent->ui->textEdit;
    connect(myRedirector, SIGNAL(writeOut(QString)),
            this,SLOT(updateParentLog(QString)));
}

logToParent::~logToParent()
{
//
    emit threadTerminator();
}

void logToParent::updateParentLog(QString logString)
{
    parentStatbar->showMessage(logString,2500);
    parentTextEdit->append(logString);

}
