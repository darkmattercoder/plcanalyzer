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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->append(QString().sprintf("Willkommen\n"));
    connect(&ConDiag, SIGNAL(SettingsChanged(ConSets*)), this,
            SLOT(ChangeSettings(ConSets*)));
    connect(&ConDiag, SIGNAL(SlotsChanged(QVector<ConSlot>)), this,
            SLOT(changeSlots(QVector<ConSlot>)));

    //The "logger" starts a thread which delivers stdout content to a pipe and
    //reads from it to push it to the debug console and/or a logging widget
    //(e.g. the Statusbar) in the main window. At least this is needed to catch
    //the plain c printf-statements somewhere on the console
    logToParent* logger = new logToParent(this);

    //The xml settings handler provides mechanisms to read and write settings
    // files for the project
    xmlSettings = new xmlSettingsHandler(this);
    connect(xmlSettings,SIGNAL(newSlotsOpened(QVector<ConSlot>)),this,
            SLOT(changeSlots(QVector<ConSlot>)));
    connect(xmlSettings,SIGNAL(newSettingsOpened(ConSets*)),this,
            SLOT(ChangeSettings(ConSets*)));
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
    //Get Labels and Operand value displays
    labelsOperand = this->findChildren<QLabel*>(QRegExp("operandLabel_.*"));
    lineEditsOperandValue = this->findChildren<QLineEdit*>
            (QRegExp("operandValue_.*"));

    //Sorting algorithms for the right order of the objects
    qSort(labelsOperand.begin(), labelsOperand.end(),
          labelPointerLessThan);
    qSort(lineEditsOperandValue.begin(), lineEditsOperandValue.end(),
          lineEditPointerLessThan);


    //Open last connection Settings if existent
    if(xmlSettings->openProject(true))
    {
        currentConsets = xmlSettings->openedConSets;
        ConDiag.SetSettings(xmlSettings->openedConSets);
        MySlot = xmlSettings->openedConSlots;
        ConDiag.newSlots = MySlot;
    }
    /*set the filepath here*/

    //myWriter.OpenFileStream(,true);
}

MainWindow::~MainWindow()
{
    //Save current settings for next start settings
    xmlSettings->saveProject(MyS7Connection.MyConSet,MySlot,true);
    delete ui;
}

void MainWindow::on_actionOpenProject_triggered()
{
    xmlSettings->openProject();
    currentConsets = xmlSettings->openedConSets;
    ConDiag.SetSettings(currentConsets);
    MySlot = xmlSettings->openedConSlots;


}

// Button Verbinden / Trennen
void MainWindow::on_Button_Connect_clicked()
{
    if (ui->Button_Connect->text().compare("Connect") == 0)
    {
        if(MyS7Connection.startConnection(this->winId()))
        {
            //uncommented due to obsolescence
            //ui->Button_Get_Val->setEnabled(true);
            ui->Button_read_slots->setEnabled(true);
            ui->Button_Connect->setText("Disconnect");
        }
    }
    else
    {
        MyS7Connection.disconnect();

        // uncommented due to obsolescence
        // ui->Button_Get_Val->setEnabled(false);
        ui->Button_read_slots->setEnabled(false);
        ui->Button_Connect->setText("Connect");
    }
}

// Should be no need for that function later
//// Neuen Wert aus der SPS anfordern
//void MainWindow::on_Button_Get_Val_clicked()
//{
//    if (MyS7Connection.isConnected())
//    {
//       //Currently only Flag byte 1 is read!
//        ui->lcdNumber->display(MyS7Connection.getValue());
//    }
//}

// Zyklisches Event
void MainWindow::TimeOut()
{
    // Zyklisches lesen
    if (recordings < amountOfPoints && MyS7Connection.isConnected())
    {
        // Read new values to slots
        MyS7Connection.readSlots(MySlot, MySlot.size());

        // Write the time vector
        x[recordings] = 0.1 * recordings; //Abtastung alle 100ms

        // Write all dimensions of value vector and the Operand Value text
        for (int i = 0; i < MySlot.size(); i++)
        {
            // Write current value
            y[i][recordings] = MyS7Connection.interpret(MySlot[i]).toFloat();
            //also into the value displays
            lineEditsOperandValue[i]->setText(QString::number(y[i][recordings]));
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
    //Get the operand labels and displays working
    QVector<QString> opLabel (MySlot.size());
    for(int i=0;i<MySlot.size();++i)
    {
        switch(MySlot[i].iAdrBereich)
        {
        case daveInputs: opLabel[i] = "E";
            break;
        case daveOutputs: opLabel[i] = "A";
            break;
        case daveFlags: opLabel[i] = "M";
            break;
        case daveDB:    opLabel[i] = "DB";
            break;
        default:
            opLabel[i] = "";
        }
        if(opLabel[i] == "DB")
        {
            opLabel[i] += QString::number(MySlot[i].iDBnummer) + "." + "DB";

            switch(MySlot[i].iDatenlaenge)
            {
            case DatLenBit:
                opLabel[i] += "X ";
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                opLabel[i] += ".";
                opLabel[i] += QString::number(MySlot[i].iBitnummer);
                break;
            case DatLenWord:
                opLabel[i] += "W ";
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            case DatLenByte:
                opLabel[i] += "B ";
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            case DatLenDWord:
                opLabel[i] += "D ";
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            default:
                opLabel[i] += "";

            }
        }else{
            switch(MySlot[i].iDatenlaenge)
            {
            case DatLenBit:
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                opLabel[i] += ".";
                opLabel[i] += QString::number(MySlot[i].iBitnummer);
                break;
            case DatLenWord:
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            case DatLenByte:
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            case DatLenDWord:
                opLabel[i] += QString::number(MySlot[i].iStartAdr);
                break;
            default:
                opLabel[i] += "";
            }



        }
        QString lineeditStyle = "QLineEdit { background-color :" + MySlot[i].graphColor.name() + "; }";
        lineEditsOperandValue[i]->setStyleSheet(lineeditStyle);
        labelsOperand[0]->colorCount();

        labelsOperand[i]->setText(opLabel[i]);
    }

}

// Button Verbindungseinstellungen
void MainWindow::on_pushButton_ConSets_clicked()
{
    //Fill the fields with data
    ConDiag.SetSlots(MySlot);
    ConDiag.show();


}

void MainWindow::on_Button_read_slots_clicked()
{
    recordings = 0;

    //Delete old graphs
    int numberOfGraphs = ui->customPlot->graphCount();
    qDebug("Number of Graphs is: %i", numberOfGraphs);

    // If numberOfGraphs <  -> add graphs
    for (int i = numberOfGraphs; i < MySlot.size(); i++)
    {
        // Add new graphs
        ui->customPlot->addGraph();
        ui->customPlot->graph(i)->setPen(MySlot[i].graphColor);
        qDebug("Adding Graph number %i", i);
    }

    // refresh numberOfGraphs
    numberOfGraphs = ui->customPlot->graphCount();

    // If numberOfGraphs >  -> remove graphs
    for (int i = numberOfGraphs; i > MySlot.size(); i--)
    {
        // Remove not needed graphs
        ui->customPlot->removeGraph(i);
        qDebug("Remove Graph: %i", i);
    }

    // Resize vectors

    // Abtastung mit 10 Hz
    amountOfPoints = ui->lineEdit_Duration->text().toInt() * 10;
    x.resize(amountOfPoints);
    y.resize(MySlot.size());

    // Resize 2nd dimension
    for (int i = 0; i <MySlot.size() ; i++)
    {
        y[i].resize(amountOfPoints);
    }

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("[time] s");
    ui->customPlot->yAxis->setLabel("value");

    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0.0, ui->lineEdit_Duration->text().toInt());
    ui->customPlot->yAxis->setRange(-5.0, 5.0);

    //open filewriter and write the number of slots
    myWriter.WriteSlots(TimeNDate::CreatePath(), MySlot.size());
}

// Autoscale axes
void MainWindow::on_pushButton_clicked()
{
    ui->customPlot->rescaleAxes();
}

void MainWindow::on_actionNewProject_triggered()
{
    std::cout << "Gewaehltes Protokoll: %i" << MyS7Connection.MyConSet->
                 useProto << std::endl;
}

void MainWindow::on_actionSaveProject_triggered()
{
    xmlSettings->saveProject(MyS7Connection.MyConSet,MySlot,false);
}

//LessThan Comparison for labels
bool MainWindow::labelPointerLessThan(QLabel *label1, QLabel *label2)
{
    return label1->objectName() < label2->objectName();
}

//LessThan Comparison for line edits
bool MainWindow::lineEditPointerLessThan(QLineEdit* le1, QLineEdit* le2)
{
    return le1->objectName() < le2->objectName();
}
