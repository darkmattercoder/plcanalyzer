/******************************************************************************
*   *File: filestream.h                                                       *
*   *Date: 2013-09-26                                                         *
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

/******************************************************************************
* class for binary files for opening a stream with read or write access       *
******************************************************************************/
#include "filestream.h"

// constructor for binary file with a fileparameter and a parameter
// setting the read and write attributes for interaction with the file
BinFile::BinFile(QString &szFilepath, bool bWrite)
{
    // open a file for read/write
    OpenFileStream(szFilepath, bWrite);
}

// standard constructor of this class
BinFile::BinFile(){}

// function used for opening a file with the given path
void BinFile::OpenFileStream(QString &szFilepath, bool bWrite)
{
    myFile.setFileName(szFilepath);
    // open the file with the read or write access
    if (bWrite)
    { myFile.open(QIODevice::WriteOnly);}   //used for BinWriter
    else
    { myFile.open(QIODevice::ReadOnly); }   //preparation for filereader

    // set the device to the opened file
    if (myFile.isOpen())
    { myFileStream.setDevice(&myFile); }
}

// standard deconstructor
BinFile::~BinFile()
{
    CloseFile();
}

// closes the file if it is open
void BinFile::CloseFile()
{
    if (myFile.isOpen())
    { myFile.close(); }
}

/******************************************************************************
* class with functions used for writing to a binary file, inherits from       *
* the BinFile                                                                 *
******************************************************************************/

// standard constructor of class binwriter
BinWriter::BinWriter(QString &szFilepath, int iSlots) : BinFile(szFilepath, true)
{
    // check if the data can be written
    if (myFile.isOpen())
    {
        // write the number of slots into the file at the first place
        myFileStream << iSlots;
    }
}

void BinWriter::Close()
{
    CloseFile();
}

// write the number of slots into the file after setting it to correct
// path and openeing the file in writemode
int BinWriter::WriteSlots(QString &szFilepath, int iSlots)
{
    // open file woith writeaccess
    OpenFileStream(szFilepath, true);
    if (myFile.isOpen())
    {
        // write the number of slots into the file at the first place
        myFileStream << iSlots;
        return BINFILE_OPERATION_OK;
    }
    else
    {
        // tell the user that the file is not ready for use
        return BINFILE_STREAM_NOT_OPENED;
    }
}

BinWriter::BinWriter(){}

// function used to write a complete data- and timestampvector
int BinWriter::WriteVector(QVector<QVector<double>> &Data, QVector<double> Time)
{
    // check if the file is opened and ready for use
    if (myFile.isOpen())
    {
        //loop through the datavector
        for (int i = 0; i < Data[0].size(); i++)
        {
            //check if the values have changed since last time
            if (CheckValues(Data, i))
            {
                // write the timestamp into the file
                myFileStream << Time[i];

                // write the numbers
                for (int j = 0; j < Data.size(); j++)
                { myFileStream << Data[j][i]; }
            }
        }
        // tell the user that the action was successful
        return BINFILE_OPERATION_OK;
    }
    else
    {
        // tell the user that the file is not ready for use
        return BINFILE_STREAM_NOT_OPENED;
    }
}

//this function checks if there were changes in the data
bool BinWriter::CheckValues(QVector<QVector<double>> &Data, int iIndex)
{
    // check for first time
    if (LastVals.isEmpty())
    {
        // save the new data
        ValsChanged(Data, 0);
        // return true because the data have changed
        return true;
    }
    else
    {
        // create local variable and set it to false
        bool bChanged = false;

        // check every item for changes
        for (int i = 0; i < Data.size(); i++)
        { bChanged = bChanged || (Data[i][iIndex] != LastVals[i]); }

        // save new data to lastvalsvector
        if (bChanged)
        { ValsChanged(Data, iIndex); }

        // return the changes
        return bChanged;
    }
}

//returns tre, if the file is already opened
bool BinWriter::AlreadyOpen()
{
    return myFile.isOpen();
}

// function used to write new vals to lastvals vector
void BinWriter::ValsChanged(QVector<QVector<double>> &Data, int iIndex)
{
    // clear lastvals vector
    LastVals.clear();

    // transfer the data to the vector
    for (int i = 0; i < Data.size(); i++)
    {
        // add data to the vector lastvals
        LastVals.append(Data[i][iIndex]);
    }
}

BinWriter::~BinWriter()
{

}

/******************************************************************************
*  the following functions are used to get the date and time in a             *
*  string format, used to create a filepath with actual date/time             *
*******************************************************************************/

// returns the current date in QString format
QString TimeNDate::GetDate()
{
    QDate qDate = QDate::currentDate();
    return qDate.toString("yyyy_MM_dd");
}

// returns the actual time in QStringformat
QString TimeNDate::GetTime()
{
    QTime qTime;
    qTime.start();
    return qTime.toString("HH_mm_ss");
}

// creates a path depending on executable path, date and time
QString TimeNDate::CreatePath()
{
    // get the executables path
    QDir path(QCoreApplication::applicationDirPath());

    // create the path for the logfile
    QString szPath = ( path.absolutePath() + QDir::separator() + "Log_" + GetDate() + "__" + GetTime() +".bin");
    return szPath;
}

