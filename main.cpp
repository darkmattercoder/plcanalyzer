/******************************************************************************
*   *File: main.cpp                                                           *
*   *Date: 2013-06-01                                                         *
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

//uncommented for strange linux compile errors
//#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

#ifdef LINUX
    // This function causes in Windows a newline after the first letter and is
    // not needed there
    setvbuf(stdout, NULL, _IOLBF, 0);
#endif

    //start the mainwindow
    w.show();

    return a.exec();
}



