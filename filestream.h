/******************************************************************************
*   *File: filestream.h                                                       *
*   *Date: 2013-09-23                                                         *
*   *Version: 1.0                                                             *
*   *Author(s): Carsten Klein <hook-the-master@gmx.net>                       *
*                                                                             *
*   *License information:                                                     *
*                                                                             *
*   Copyright (C) [2013] [Carsten Klein]                                      *
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

/*include ward*/
#ifndef _FILESTREAMER_H_INCLUDED_
#define _FILESTREAMER_H_INCLUDED_

#include <iostream>				//include the standard stream
#include <fstream>				//include the fstream for file manipulation
using namespace std;			//use the standard namespace

#include <QString>              //used for stringoperations
#include <QVector>              //used for handling of dynamic arrays
#include <QDate>                //used to get actual date
#include <QTime>                //used to get actual time
#include <QDataStream>          //used to write data to a file
#include <QFile>                //used to open and close a file
#include <QDir>                 //used to get the current path
#include <QCoreApplication>     //used to get the path of the executable

/*defines: the size of datatypes in byte*/
#define SIZEOFDOUBLE sizeof(double)
#define SIZEOFINT sizeof(int)

#define BINFILE_OPERATION_OK 1
#define BINFILE_STREAM_NOT_OPENED 10

/*motherclass for all binary file IOs, that contains all things that are
in a binary file equal if reading to or writing from a file*/
class BinFile //: Datastructure
{
protected:
    QDataStream myFileStream;
    QFile myFile;

    /*function used for opening the filestream*/
    void OpenFileStream(QString &szFilepath, bool bWrite);

    /*closes the file and saves the data on disk*/
    void CloseFile();

public:
    /*standard constructor*/
    BinFile();

    /*constructor with a filepath for opening the filestream*/
    BinFile(QString &szFilepath, bool bWrite);

    /*deconstructor, used for flushing and closing a file*/
    ~BinFile();


};

/*standard class for writing data to a binary file*/
class BinWriter : BinFile
{
private:
    QVector<double> LastVals;

    /*function used to write new vals to lastvals vector*/
    void ValsChanged(QVector<QVector<double>> &Data, int iIndex);

    /*function used for checking if data has changed*/
    bool CheckValues(QVector<QVector<double>> &Data, int iIndex);

public:
    /*standard constructor*/
    BinWriter();

    ~BinWriter();

    /*constructor of class binfile used for setting the filepath*/
    BinWriter(QString &szFilepath, int iSlots);

    /*function used for writing the number of slots*/
    int WriteSlots(QString &szFilepath, int iSlots);

    /*function used for writing data from a vector to the file*/
    int WriteVector(QVector<QVector<double>> &Data, QVector<double> Time);

    /*returns true when the file is already opened*/
    bool AlreadyOpen();

    /*close the writer*/
    void Close();
};

/*class used for creating a path using date and time*/
class TimeNDate
{
private:
    /*gets the actual date as QString*/
    static QString GetDate();
    /*gets the actual time as QString*/
    static QString GetTime();
public:
    /*creates a path relative to the executable with date and time in the name*/
    static QString CreatePath();
};

#endif
