/****************************************************************************************
* Filename:		filestream.h												*
* Author:		Carsten Klein															*
* Date:			2013/06/07																*
* Description:	this file is the header for a class that is used for reading and		*
*				writing to and from a binary file. In our project it will be used for	*
*				saving the data of the SPS-analyser to a file and showing thatr data at	*
*				any time the user wants to												*
****************************************************************************************/

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
        void OpenFileStream(QString szFilepath, bool bWrite);
		
	public:
        /*standard constructor*/
        BinFile();

		/*constructor with a filepath for opening the filestream*/
		BinFile(string szFilepath, bool bWrite);

		/*deconstructor, used for flushing and closing a file*/
		~BinFile();
};

/*standard class for writing data to a binary file*/
class BinWriter : BinFile
{
private:
    QVector<double> LastVals;

public:
    /*standard constructor*/
    BinWriter();

    ~BinWriter();

	/*constructor of class binfile used for setting the filepath*/
    BinWriter(QString szFilepath, int iSlots);

    /*function used for writing data from a vector to the file*/
    int WriteVector(QVector<QVector<double>> Data, QVector<double> Time);

    /*function used for checking if data has changed*/
    bool CheckValues(QVector<QVector<double>> Data, int iIndex);

	/*function used for writing a doublearray into a binary file*/
	int WriteDoubleArray (double* pdPtr, int iLength);

	/*function used for writing a string into a binary file*/
	int WriteString (string szStringToWrite);

	/*function used to write structured data to the binary file*/
	int WriteData(char* pcPtr, int iLength);

	/*function used for writing the Header of a file*/
	int WriteHeader(int* iHead, int iLength);

	/*function used for writing the header of the file*/
    //int WriteDatastructureHeader(Datastructure dsHeader);
};

/*standard class for reading data from a binary file*/
class BinReader : BinFile
{
	/*this function reads data from a binary file
	length: indicates how much doublevalues should be read
	ptr: is a pointer to a doublearray which will contain
	the read values after this method*/

private:
    //variables
    int iNumberOfSlots = 0;
    long lActualPosition;

    //functions
    void SavePos();
    void RetPos();

    //reads the filelength of the file in bytes
    long FileLength();

    //reads the number of slots that are saved in the file
    int GetSlots();

    //reads the last timestamp
    double LastTimestamp();

public: 
	int ReadDoubleArray (double* ptr, int length);

    //construtor of this class used to open the filestream
    BinReader(string szPath);

    //reads the header from the file
	int ReadHeader(int* iHead, int* iLength);

	int ReadSlots(int iHead, char* cData);

    //reads the saved data from the file into the vectors data and time
    int ReadVector(int i, int j, QVector<QVector<double>>* Data, QVector<double>* Time);
};

class DateAndTime
{
private:
    static string AddZero(int iNumber);

public:
    static string GetDate();
    static string GetTime();

    static QString GetDate();
    static QString GetTime();
};

#endif
