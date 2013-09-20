#include "filestream.h"
/*************************************************************************/
/* class for binary files for opening a stream with read or write access */
/*************************************************************************/
/*constructor for binary file with a fileparameter and a parameter
 * setting the read and write attributes for interaction with the file*/
BinFile::BinFile(string szFilepath, bool bWrite)
{
    /*open a file for read/write*/
	OpenFileStream(szFilepath, bWrite);
}

/*standard constructor of this class*/
BinFile::BinFile(){}

/*function used for opening a file with the given path*/
BinFile::OpenFileStream(QString szFilepath, bool bWrite)
{
    /*open the file with the read or write access*/
    myFile(szFilepath);
    if (bWrite)
    { myFile.open(QIODevice::WriteOnly);}
    else
    { myFile.open(QIODevice::ReadOnly); }

    /*set the device to the opened file*/
    if (myFile.isOpen())
    { myFileStream.setDevice(&myFile); }
}

/*standard deconstructor*/
BinFile::~BinFile()
{
    /*close the file*/
    myFile.close();
}
/*************************************************************************/


/*************************************************************************/
/* class with functions used for writing to a binary file, inherits from */
/* the BinFile                                                           */
/*************************************************************************/
/*standard constructor of class binwriter*/
BinWriter::BinWriter(QString szFilepath, int iSlots) : BinFile(szFilepath, true)
{
    if (myFile.is_open())
    {
        myFileStream << iSlots;
    }
    else
    {
        std::cout << "Couldn't open file " << printable(szFilepath)
                  << ":" << myFile.errorString() << endl;
    }
}

BinWriter::BinWriter(){}

/*function used to write a complete data- and timestampvector*/
int BinWriter::WriteVector(QVector<QVector<double>> Data, QVector<double> Time)
{
    /*check if the file is opened and ready for use*/
    if (myFile.is_open())
    {
        //loop through the datavector
        for (int i = 0; i < Data[0].size(); i++)
        {
            //check if the values have changed since last time
            if (CheckValues(Data, i))
            {
                /*write the timestamp into the file*/
                myFileStream << Time[i];

                /*write the numbers*/
                for (int j = 0; j < Data.size(); j++)
                { myFileStream << Data[j][i]; }
            }
        }
        /*tell the user that the action was successful*/
        return BINFILE_OPERATION_OK;
    }
    else
    {
        /*tell the user that the file is not ready for use*/
        return BINFILE_STREAM_NOT_OPENED;
    }
}

//this function checks if there were changes in the data
bool BinWriter::CheckValues(QVector<QVector<double>> Data, int iIndex)
{
    //check for first time
    if (LastVals.isEmpty())
    {
        for (int i = 0; i < Data.size(); i++)
        {
            //transfer the data from Data at the specified index
            //to the local dataholder LastVals
            LastVals.append(Data[i][iIndex]);
        }
        //return true because the data have changed
        return true;
    }
    else
    {
        //create local variable and set it to false
        bool bChanged = false;

        //check every item for changes
        for (int i = 0; i < Data.size(); i++)
        {
            bChanged = bChanged || (Data[i][iIndex] != LastVals[i]);
        }

        //return the changes
        return bChanged;
    }
}


/*method for writing a string into an binaryfile*/
int BinWriter::WriteString (string szStringToWrite)
{
	if (myFileStream.is_open())
	{
		/*create a temporary buffer*/
		char *a = new char[szStringToWrite.size()+1];

		/*set the last element to 0, for termination*/
		a[szStringToWrite.size()] = 0;

		/*copy the string into the chararray*/
		memcpy(a,szStringToWrite.c_str(), szStringToWrite.size());

		/*write the buffer into the file*/
		myFileStream.write(a, szStringToWrite.size());

		/*free the memory of the buffer*/
		delete a;
		
		/*return OK because the action was successful*/
		return BINFILE_OPERATION_OK;
	}
	else
	{
		/*return that the stream is not opened*/
		return BINFILE_STREAM_NOT_OPENED;
	}
}

/*method for writing a doublearray into a binary file
parameters:	ptr -> is a pointer to the doublearray
			length -> is the number of items of the array*/
int BinWriter::WriteDoubleArray (double* ptr, int length)
{
	/*check if the file is opened and ready for use*/
	if (myFileStream.is_open())
	{
		/*Union, used for easy transformation into a chararray*/
		uDoubleByte Numb;

		/*write the numbers*/
		for (int i = 0; i < length; i++)
		{
			/*set numb.dNumber to the value of the doublearray at index i*/
			Numb.dNumber = ptr[i];
			/*write dNumber as binary into the file*/
			myFileStream.write(Numb.cBytes, SIZEOFDOUBLE);
		}
		/*tell the user that the action was successful*/
		return BINFILE_OPERATION_OK;
	}
	else
	{
		/*tell the user that the file is not ready for use*/
		return BINFILE_STREAM_NOT_OPENED;
	}
}

/*writes the header of the file containing the number of slots and
the type of every slot*/
int BinWriter::WriteHeader(int* iHead, int iLength)
{
	for (int i = 0; i < iLength; i++)
	{
		uIntByte number;
		number.iNumber = iHead[i];
		myFileStream.write(number.cBytes, SIZEOFINT);
	}
    return BINFILE_OPERATION_OK;
}

int BinWriter::WriteData(char* pcPtr, int iLength)
{
	if (myFileStream.is_open())
	{
		/*create temporary array*/
		char *cTempArray = new char[iLength];

		/*copy data to temporary array*/
		for (int i = 0; i < iLength; i++)
		{
			cTempArray[i] = pcPtr[i];
		}

		/*write the buffer into the file*/
		myFileStream.write(cTempArray, iLength);

		delete cTempArray;

		/*tell the user that the action was successful*/
		return BINFILE_OPERATION_OK;
	}
	else
	{
		/*tell the user that the file is not ready for use*/
		return BINFILE_STREAM_NOT_OPENED;
	}
}

//write the header contained in a datastructure
//int BinWriter::WriteDatastructureHeader(Datastructure dsHeader)
//{
	
//}

BinWriter::~BinWriter()
{}
#pragma endregion


////////////////////////////////////////////////////////////////////////////////////////////
/// Region BinReader with functions for reading from a binary file
////////////////////////////////////////////////////////////////////////////////////////////

#pragma region BinReader
//constructor of the class binreader, opening a file with readingaccess only
BinReader::BinReader(string szFilepath):BinFile(szFilepath, false)
{}

//reads the length of the opened file in bytes
long BinReader::FileLength()
{
    long lBegin, lEnd, lActual;
    //get the actual position for getting bacj to here later
    lActual = myFileStream.tellg();

    //jump to beginning of the file and read position
    myFileStream.seekg (0, ios::beg);
    lBegin = myFileStream.tellg();

    //jump to end of file and read position
    myFileStream.seekg (0, ios::end);
    lEnd = myFileStream.tellg();

    //jump back to the position it was before
    myFileStream.seekg (lActual, ios::beg);

    //calculate the length of the file in bytes
    return (lEnd - lBegin);
}

/*read the number of slots that are in the file*/
int BinReader::GetSlots()
{
    SavePos();

    //set the pointer to the beginning
    myFileStream.seekg(0, ios::beg);

    //get a temporary part of memory for the returnvalue
    uIntByte uReturnvalue;

    //read the number of slots
    myFileStream.read(uReturnvalue.cBytes, SIZEOFINT);

    RetPos();

    //return the number of slots
    return uReturnvalue.iNumber;
}

//save actual position in file
void BinReader::SavePos()
{
    //save the actual position of readpointer
    lActual = myFileStream.tellg();
}

//return to saved position
void BinReader::RetPos()
{
    //get pointer back to previous position
    myFileStream.seekg(lActual, ios::beg);
}

/*gets the last timestamp of the data*/
double BinReader::LastTimestamp()
{
    SavePos();

     //get a temporary part of memory for the returnvalue
    uDoubleByte uReturnvalue;

    //set the streampointer to the position of the last
    myFileStream.seekg(SIZEOFDOUBLE, ios::end);

    //read the last timestamp
    myFileStream.read(uReturnvalue, SIZEOFDOUBLE);

    RetPos();

    return uReturnvalue.dNumber;
}

//reads the data out of the file
int BinReader::ReadVector(int iSlots, int iItems, QVector<QVector<double>>* Data, QVector<double>* Time)
{
    if (myFileStream.is_open())
    {
        //get the filelength
        long lLengthOfFile = FileLength();

        //get the number of slots
        int iSlots = GetSlots();


        uIntByte Slots;
        myFileStream.read(Slots.cBytes, SIZEOFINT);


        uDoubleByte Numb;
        QVector<double> myTempVector;
        for (int i = 0; i < iSlots; i++)
        {
            //read the timestamp
            myFileStream.read(Numb.cBytes, SIZEOFDOUBLE);
            Time->append(Numb.dNumber);

            myTempVector.clear();
            //read the slotvalues
            for (int j = 0; j < iItems; j++ )
            {
                myFileStream.read(Numb.cBytes, SIZEOFDOUBLE);
                myTempVector.append(Numb.dNumber);
            }
            Data->append(myTempVector);
        }
        return BINFILE_OPERATION_OK;
    }
    else
    {
        return BINFILE_STREAM_NOT_OPENED;
    }
}

//function used to read the header
int BinReader::ReadHeader(int* iHead, int* iLength)
{
	if (myFileStream.is_open())
	{
		//get the first integervalue -> the number of slots
		uIntByte number;
		myFileStream.read(number.cBytes, SIZEOFINT);
		iHead[0] = number.iNumber;

		//now get the slots and their types
		for (int i = 0; i < iHead[0]; i++)
		{
			myFileStream.read(number.cBytes, SIZEOFINT);
			iHead[i+1] = number.iNumber;
		}

		/*return opertaion OK because the operation was successful*/
		return BINFILE_OPERATION_OK;
	}
	else
	{
		/*return that stream is not opened so user knows that the action
		was not done*/
		return BINFILE_STREAM_NOT_OPENED;
	}
}

/*methoid for reading a doublearray out of a binaryfile*/
int BinReader::ReadDoubleArray (double* ptr, int length)
{
	if (myFileStream.is_open())
	{
		/*Union used for easy conversion between double and chararray*/
		uDoubleByte Numb;
		for (int i = 0; i < length; i++)
		{
			/*read the first number*/
			myFileStream.read(Numb.cBytes, SIZEOFDOUBLE);
			if (!myFileStream)
			{
				ptr[i] = i;
			}

			/*save number into the doublevalue*/
			ptr[i] = Numb.dNumber;
		}

		/*return opertaion OK because the operation was successful*/
		return BINFILE_OPERATION_OK;
	}
	else
	{
		/*return that stream is not opened so user knows that the action
		was not done*/
		return BINFILE_STREAM_NOT_OPENED;
	}
}

#pragma endregion




/********************************************************************/
/*  the folling functions are used to get the date and time in a    */
/*  stringformat, used to create a filepath with actual date/time   */
/********************************************************************/

/*gets the date in the format YYYY_MM_DD*/
string DateAndTime::GetDate()
{
    QDate qDate;
    qDate = qDate.currentDate();

    //set of temporary variables
    int iYear, iMonth, iDay;
    int* piYear = &iYear;
    int* piMonth = &iMonth;
    int* piDay = &iDay;

    //get the actual date
    qDate.getDate(piYear, piMonth, piDay);

    //convert the data to stringformat
    string szDate = Convertingfunctions.Val2String(iYear) + "_" +
                    AddZero(iMonth) + Convertingfunctions.Val2String(iMonth) + "_" +
                    AddZero(iDay) +  Convertingfunctions.Val2String(iDay);
    return szDate;
}

/*gets the time in the format HH_MM_SS*/
string DateAndTime::GetTime()
{
    QTime qTime;

    //read the actual time
    qTime.start();
    int iHour = qTime.hour();
    int iMinute = qTime.minute();
    int iSecond = qTime.second();

    //convert data into stringformat and form time into the format
    //e.g.: 07_04_08 stands for 4 minutes and 8 seconds past 7
    string szTime = AddZero(iHour) + Convertingfunctions.Val2String(iHour) + "_" +
                    AddZero(iMinute) + Convertingfunctions.Val2String(iMinute) + "_" +
                    AddZero(iSecond) + Convertingfunctions.Val2String(iSecond);
    return szTime;
}

/*returns a zero in a string if the number is less than 10
else it returns an empty string*/
string DateAndTime::AddZero(int iNumber)
{
    if (iNumber < 10)
    {return "0";}
    else
    {return "";}
}

QString DateAndTime::GetDate()
{
    QDate qDate;
    qDate.currentDate();
    return qDate.toString("YYYY_MM_DD");
}

QString DateAndTime::GetTime()
{
    QTime qTime;
    qTime.start();
    return qTime.toString("HH_MM_SS");
}
