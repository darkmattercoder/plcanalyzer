/*******************************************************************************
 *   *File: stdoutredirector.h                                                 *
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

#ifndef STDOUTREDIRECTOR_H
#define STDOUTREDIRECTOR_H
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMainWindow>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef LINUX
#include <unistd.h>
#endif

#ifdef BCCWIN
#include <io.h>
#define pipe(X) _pipe(X,4096,O_BINARY)
#define fileno _fileno
#define dup2 _dup2
#define read _read
#endif

#include <assert.h>

class stdoutRedirector :  public QThread
{
      Q_OBJECT
public:

    stdoutRedirector();
    ~stdoutRedirector();
    void run();

signals:

    void writeOut(QString);

public slots:

    void terminate();

private:

    int fds[2];
    int res;
    char buf[256];
    int so;
    bool terminate_;

};


#endif // STDOUTREDIRECTOR_H
