/*******************************************************************************
 *   *File: mainwindow.cpp                                                     *
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

#include "mainwindow.h"
//uncommented for strange linux compile errrors and mpoved to mainwindow.h
//#include "ui_mainwindow.h"
#include "logtoparent.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#ifdef LINUX
#include <limits>
#endif

#include "xmlsettingshandler.h"
//#include "filestream.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->append(QString().sprintf("Willkommen\n"));
    connect(&ConDiag, SIGNAL(SettingsChanged(ConSets*)), this, SLOT(ChangeSettings(ConSets*)));
    connect(&ConDiag, SIGNAL(SlotsChanged(QVector<ConSlot>)), this, SLOT(changeSlots(QVector<ConSlot>)));

    //The "logger" starts a thread which delivers stdout content to a pipe and
    //reads from it to push it to the debug console and/or a logging widget (e.g. the Statusbar)
    //in the main window. At least this is needed to catch the plain c printf-statements
    //somewhere on the console
    logToParent* logger = new logToParent(this);

    //The xml settings handler provides mechanisms to read and write settings files for the project
    xmlSettings = new xmlSettingsHandler(this);
    connect(xmlSettings,SIGNAL(newSlotsOpened(QVector<ConSlot>)),this,SLOT(changeSlots(QVector<ConSlot>)));

    currentConsets = new ConSets;

    // Start the timer for slot reading
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOut()));
    timer->start(100);
    currentConsets = MyS7Connection.MyConSet;
    ConDiag.SetSettings(MyS7Connection.MyConSet);
    
    // Validator für die Eingabe der Aufnahmedauer
    QRegExpValidator *RegExp_Duration = new QRegExpValidator(this);
    QRegExp rx("[0-9]{0,2}|1{1,1}0{0,2}");
    RegExp_Duration->setRegExp(rx);
    ui->lineEdit_Duration->setValidator(RegExp_Duration);

#ifdef LINUX
#define MAXIMUMINT std::numeric_limits<int>::max()
#else
#define MAXIMUMINT MAXINT
#endif

    recordings = MAXIMUMINT;

    //####################ZU BEARBEITEN######################################
    numberOfSlots = 2;
    //#######################################################################


    /*set the filepath here*/

    //myWriter.OpenFileStream(,true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenProject_triggered()
{
    xmlSettings->openProject();
    currentConsets = xmlSettings->openedConSets;
    ConDiag.SetSettings(xmlSettings->openedConSets);
    ConDiag.newSlots = MySlot;
}

// Button Verbinden / Trennen
void MainWindow::on_Button_Connect_clicked()
{
    if (ui->Button_Connect->text().compare("Connect") == 0)
    {
        if(MyS7Connection.startConnection(this->winId()))
        {
            ui->Button_Get_Val->setEnabled(true);
            ui->Button_read_slots->setEnabled(true);
            ui->Button_Connect->setText("Disconnect");
        }
    }
    else
    {
        MyS7Connection.disconnect();
        ui->Button_Get_Val->setEnabled(false);
        ui->Button_read_slots->setEnabled(false);
        ui->Button_Connect->setText("Connect");
    }
}

// Neuen Wert aus der SPS anfordern
void MainWindow::on_Button_Get_Val_clicked()
{
    if (MyS7Connection.isConnected())
    {
        ui->lcdNumber->display(MyS7Connection.getValue());
    }
}

// Zyklisches Event
void MainWindow::TimeOut()
{
    // Zyklisches lesen
    if (recordings < amountOfPoints && MyS7Connection.isConnected())
    {
        // Read new values to slots
        MyS7Connection.readSlots(&MySlot[0], numberOfSlots);

        // Write the time vector
        x[recordings] = 0.1 * recordings; //Abtastung alle 100ms

        // Write all dimensions of value vector
        for (int i = 0; i < numberOfSlots; i++)
        {
            // Write current value
            y[i][recordings] = MyS7Connection.interpret(MySlot[i]).toFloat();

            // Assign vector to graph
            ui->customPlot->graph(i)->setData(x, y[i]);

        }
        ui->customPlot->replot();
        recordings++;

        /*save data if the amount of points is reached*/
        if (recordings == amountOfPoints)
        {
            /*save the data to the file*/
            myWriter.WriteVector(y, x);

            /*clear the vectors*/
            y.clear();
            x.clear();
        }
    }
    else
    {
        /*save data if recordings started and connection is disconnected*/
        if (recordings != MAXIMUMINT)
        {
            /*write data to file*/
            myWriter.WriteVector(y, x);
            /*reset recordings to MAXINT*/
            recordings = MAXIMUMINT;
        }
    }
}

// Event Werte aus Dialog sollen übernommen werden
void MainWindow::ChangeSettings(ConSets* NewConSets)
{
    MyS7Connection.MyConSet = NewConSets;
    currentConsets = NewConSets;
}

void MainWindow::changeSlots(QVector<ConSlot> newConSlots)
{

    currentConsets = xmlSettings->openedConSets;
    MySlot = newConSlots;
    ConDiag.SetSlots(MySlot);
}

// Button Verbindungseinstellungen
void MainWindow::on_pushButton_ConSets_clicked()
{
    ConDiag.show();
}

void MainWindow::on_Button_read_slots_clicked()
{   
    // Slot 1
    MySlot[0].iAdrBereich = daveFlags;
    MySlot[0].iDatenlaenge = DatLenDWord;
    MySlot[0].iStartAdr = 4;
    MySlot[0].iDBnummer = 0;
    MySlot[0].iBitnummer = 0;
    MySlot[0].iAnzFormat = AnzFormatGleitpunkt;
    MySlot[0].graphColor = Qt::black;

    // Slot 2
    MySlot[1].iAdrBereich = daveFlags;
    MySlot[1].iDatenlaenge = DatLenBit;
    MySlot[1].iStartAdr = 1;
    MySlot[1].iDBnummer = 0;
    MySlot[1].iBitnummer = 7;
    MySlot[1].iAnzFormat = AnzFormatDezimal;
    MySlot[1].graphColor = Qt::red;

    // Slot 3
    MySlot[2].iAdrBereich = daveFlags;
    MySlot[2].iDatenlaenge = DatLenDWord;
    MySlot[2].iStartAdr = 4;
    MySlot[2].iDBnummer = 0;
    MySlot[2].iBitnummer = 0;
    MySlot[2].iAnzFormat = AnzFormatBinaer;
    MySlot[2].graphColor = Qt::green;

    // Slot 4
    MySlot[3].iAdrBereich = daveFlags;
    MySlot[3].iDatenlaenge = DatLenWord;
    MySlot[3].iStartAdr = 8;
    MySlot[3].iDBnummer = 0;
    MySlot[3].iBitnummer = 0;
    MySlot[3].iAnzFormat = AnzFormatHexadezimal;
    MySlot[3].graphColor = Qt::blue;

    // Slot 5
    MySlot[4].iAdrBereich = daveFlags;
    MySlot[4].iDatenlaenge = DatLenByte;
    MySlot[4].iStartAdr = 10;
    MySlot[4].iDBnummer = 0;
    MySlot[4].iBitnummer = 0;
    MySlot[4].iAnzFormat = AnzFormatZeichen;
    MySlot[4].graphColor = Qt::magenta;

    // Reset recording counter
    recordings = 0;

    //Delete old graphs
    int numberOfGraphs = ui->customPlot->graphCount();
    qDebug("Number of Graphs is: %i", numberOfGraphs);

    // If numberOfGraphs < numberOfSlots -> add graphs
    for (int i = numberOfGraphs; i < numberOfSlots; i++)
    {
        // Add new graphs
        ui->customPlot->addGraph();
        ui->customPlot->graph(i)->setPen(MySlot[i].graphColor);
        qDebug("Adding Graph number %i", i);
    }

    // refresh numberOfGraphs
    numberOfGraphs = ui->customPlot->graphCount();

    // If numberOfGraphs > numberOFSlots -> remove graphs
    for (int i = numberOfGraphs; i > numberOfSlots; i--)
    {
        // Remove not needed graphs
        ui->customPlot->removeGraph(i);
        qDebug("Remove Graph: %i", i);
    }

    // Resize vectors
    amountOfPoints = ui->lineEdit_Duration->text().toInt() * 10; //Abtastung mit 10 Hz
    x.resize(amountOfPoints);
    y.resize(numberOfSlots);

    // Resize 2nd dimension
    for (int i = 0; i < numberOfSlots; i++)
    {
        y[i].resize(amountOfPoints);
    }

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("[time] s");
    ui->customPlot->yAxis->setLabel("value");

    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0.0, ui->lineEdit_Duration->text().toInt());
    ui->customPlot->yAxis->setRange(-1.0, 1.0);
}

// Autoscale axes
void MainWindow::on_pushButton_clicked()
{
    ui->customPlot->rescaleAxes();
}

void MainWindow::on_actionNewProject_triggered()
{    
    std::cout << "Gewaehltes Protokoll: %i" << MyS7Connection.MyConSet->useProto << std::endl;
}

void MainWindow::on_actionSaveProject_triggered()
{    
    xmlSettings->saveProject(MyS7Connection.MyConSet,MySlot);
}

////////////////////////////////////////////////////////
/// casis testbuttongedöhhns
///
////////////////////////////////////////////////////////

//test for writing functions
void MainWindow::on_Write_Test_clicked()
{
    MySlot.resize(5);

    BinWriter theWriter(TimeNDate::CreatePath(), MySlot.size());

    ui->textEdit->append(QString::number(MySlot.size()));

    WriteXY();

    ui->textEdit->append(QString::number(y.size()));
    ui->textEdit->append(QString::number(y[0].size()));

    /*write the datavector and the timevector*/
    theWriter.WriteVector(y, x);
}

void MainWindow::WriteXY()
{
    //clear the x and y vectors
    x.clear();
    y.clear();

    //temporaray vector
    QVector<double> myTempVector;

    //append 5 values to the x vector
    for (int i = 0; i < 5; i++)
    {
        x.append(i);
    }

    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            myTempVector.append(j * 0.2);
        }
        y.append(myTempVector);
        myTempVector.clear();
    }

}

void MainWindow::on_Read_Test_clicked()
{
//    QString filename;
//    filename = QFileDialog::getOpenFileName(this,"Öffnen",".","PLC-Log (*.txt)");

//    string szPath = QString2String(filename);

//    //string szPath = "release\\Test\\Testfile.txt";
//    BinReader theReader(szPath);

//    ui->textEdit->append(QString::fromUtf8(szPath.c_str()));

//    y.clear();
//    x.clear();

//    theReader.ReadVector(5, 3, &y, &x);

//    for (int i = 0; i < 5; i++)
//    {
//        ui->textEdit->append(Double2String(x[i]));
//        for (int j = 0; j < 3; j++)
//        {
//            ui->textEdit->append(Double2String(y[i][j]));
//        }
//    }
}
