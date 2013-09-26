/******************************************************************************
*   *File: stdoutredirector.cpp                                               *
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

#include "stdoutredirector.h"

stdoutRedirector::stdoutRedirector()
{
    res=pipe(fds);
    assert(res==0);

    so=fileno(stdout);
    // close stdout handle and make the writable part of fds the new stdout.
    res=dup2(fds[1],so);
    assert(res!=-1);
    terminate_ = false;
}

stdoutRedirector::~stdoutRedirector()
{

}

void stdoutRedirector::run()
{

    Q_FOREVER{
        res=read(fds[0],buf,sizeof(buf)-1);
        assert(res>=0 && res<sizeof(buf));
        buf[res]=0;
        emit writeOut(buf);
        qDebug() << buf;
        //fprintf(stderr,"printf: %s\n",buf);
        if (terminate_)
        {
            return;
        }
    }


}
void stdoutRedirector::terminate()
{
    terminate_ = true;
}



