/*******************************************************************************
 *   *File: connectionsettings.cpp                                             *
 *   *Date: 2013-09-04                                                         *
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

#include "connectionsettings.h"
#include <iostream>

ConnectionSettings::ConnectionSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConnectionSettings)
{
    // Build userinterface
    ui->setupUi(this);

    // Fill comboboxes for area and formats with data
    comboBoxesArea = findChildren<QComboBox*>(QRegExp("comboBoxArea_.*"));
    comboBoxesFormat = findChildren<QComboBox*>(QRegExp("comboBoxFormat_.*"));
    comboBoxesLength = findChildren<QComboBox*>(QRegExp("comboBoxLength_.*"));
    lineEditsBits = findChildren<QLineEdit*>(QRegExp("lineEditBit_.*"));
    lineEditsAddress = findChildren<QLineEdit*>(QRegExp("lineEditAddress_.*"));
    lineEditsDB = findChildren<QLineEdit*>(QRegExp("lineEditDB_.*"));

    // Sorting algorithms for the right order of the objects
    qSort(comboBoxesArea.begin(), comboBoxesArea.end(),
          comboBoxPointerLessThan);
    qSort(comboBoxesFormat.begin(), comboBoxesFormat.end(),
          comboBoxPointerLessThan);
    qSort(comboBoxesLength.begin(), comboBoxesLength.end(),
          comboBoxPointerLessThan);
    qSort(lineEditsBits.begin(), lineEditsBits.end(),
          lineEditPointerLessThan);
    qSort(lineEditsDB.begin(), lineEditsDB.end(),
          lineEditPointerLessThan);
    qSort(lineEditsAddress.begin(), lineEditsAddress.end(),
          lineEditPointerLessThan);

    comboItemsArea << " " << "E" << "A" << "M" << "DB";
    comboItemsLength << " " << "BIT" << "BYTE" << "WORD" << "DWORD";
    comboItemsFormat << " " << "BOOL" << "BIN" << "DEZ" << "HEX" << "FLOAT"
                     << "CHAR";
    comboValuesArea << 0x0 << daveInputs << daveOutputs << daveFlags << daveDB;
    comboValuesLength << 0x0 << DatLenBit << DatLenByte << DatLenWord
                      << DatLenDWord;
    comboValuesFormat << 0x0 << AnzFormatBool << AnzFormatBinaer
                      << AnzFormatDezimal << AnzFormatHexadezimal
                      << AnzFormatGleitpunkt << AnzFormatZeichen;

    // Initialize with two for loops. Q_FOREACH would fit too, 
    // seems simpler and faster but then an additional iterator 
    // value declaration outside the loop was necessary.
    for(int i=0;i<comboBoxesArea.count();++i)
    {
        for(int j=0;j<comboItemsArea.count();++j)
            comboBoxesArea[i]->addItem(comboItemsArea[j],comboValuesArea[j]);
    }

    for(int i=0;i<comboBoxesFormat.count();++i)
    {
        for(int j=0;j<comboItemsFormat.count();++j)
            comboBoxesFormat[i]->addItem(comboItemsFormat[j],
                                         comboValuesFormat[j]);
    }
    for(int i=0;i<comboBoxesLength.count();++i)
    {
        for(int j=0;j<comboItemsLength.count();++j)
            comboBoxesLength[i]->addItem(comboItemsLength[j],
                                         comboValuesLength[j]);
    }

    //Connect the format- and length-combo boxes' indexChanged signal to the handling
    //Slot for conditional disabling of Bit field
    for(int i=0;i<comboBoxesArea.count();++i)
    {
        connect(comboBoxesArea[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(comboBoxIndexChanged(int)));
        connect(comboBoxesLength[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(comboBoxIndexChanged(int)));
    }

    // Fill ComboBox "Protocol" with cuttent settings
    ui->ComboBox_Protokoll->addItem("MPI for S7 300/400", daveProtoMPI);
    ui->ComboBox_Protokoll->addItem("PPI for S7 200", daveProtoPPI);
    ui->ComboBox_Protokoll->addItem("S5 programming port protocol",
                                    daveProtoAS511);
#ifdef BCCWIN
    ui->ComboBox_Protokoll->addItem("use s7onlinx.dll for transport",
                                    daveProtoS7online);
#endif
    ui->ComboBox_Protokoll->addItem("ISO over TCP", daveProtoISOTCP);
    ui->ComboBox_Protokoll->addItem("ISO over TCP with CP243",
                                    daveProtoISOTCP243);
    ui->ComboBox_Protokoll->addItem("ISO over TCP with Routing",
                                    daveProtoISOTCPR);

    // Validator for IP Address Input
    QRegExpValidator *RegExp_IP = new QRegExpValidator(this);
    QRegExp rx("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.)"\
               "{3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
    RegExp_IP->setRegExp(rx);
    ui->lineEdit_IP->setValidator(RegExp_IP);

    // Validator for MPI Address (0..127) input
    QRegExpValidator *RegExp_MPI = new QRegExpValidator(this);
    rx.setPattern("[0-9]{0,2}|1{1,1}[0-1]?[0-9]?|1{1,1}2{1,1}[0-7]{1,1}");
    RegExp_MPI->setRegExp(rx);
    ui->lineEdit_CPU_MPI->setValidator(RegExp_MPI);
    ui->lineEdit_local_MPI->setValidator(RegExp_MPI);
    graphColors << Qt::green << Qt::black << Qt::blue << Qt::cyan
                << Qt::darkYellow << Qt::magenta << Qt::darkRed << Qt::darkCyan;
    moduloValue=graphColors.size();

    //Integer Validator for the Address and Bit fields
    myBitValidator = new QIntValidator(0,7,this);
    myByteAddressValidator = new QIntValidator(0,4096,this);
    for(int i=0;i<lineEditsBits.count();++i)
    {
        lineEditsBits[i]->setValidator(myBitValidator);
        lineEditsAddress[i]->setValidator(myByteAddressValidator);
        lineEditsDB[i]->setValidator(myByteAddressValidator);
    }
}

// Destructor of ConnectionSettings
ConnectionSettings::~ConnectionSettings()
{
    delete ui;
}

// Changes confirmed with "OK" button
void ConnectionSettings::on_buttonBox_accepted()
{
    // Write changes to member variable
    m_DiagSets->IP_Adr = ui->lineEdit_IP->text();
    m_DiagSets->localMPI = ui->lineEdit_local_MPI->text().toInt();
    m_DiagSets->plcMPI = ui->lineEdit_CPU_MPI->text().toInt();
    m_DiagSets->speed = ui->comboBox_Speed->currentIndex();
    m_DiagSets->useProto = ui->ComboBox_Protokoll->itemData(ui->ComboBox_Protokoll->currentIndex()).toInt();
    m_DiagSets->rack = ui->lineEdit_Rack->text().toInt();
    m_DiagSets->slot = ui->lineEdit_Slot->text().toInt();

    // Emit Signal "Settings Changed"
    emit SettingsChanged(m_DiagSets);

    int iAnzahlSlots = 0;

    // Count number of areas != NULL
    for(int i = 0; i < comboBoxesArea.count(); i++)
    {
        if(comboBoxesArea[i]->itemData(comboBoxesArea[i]->currentIndex()).toInt())
        {
            // This Area setting is choosed, need for a slot
            iAnzahlSlots++;
        }
        else
        {
            // The first empty entry -> ignore all other entries
            break;
        }
    }

    // Has the number of Slots changed?
    if(newSlots.isEmpty() || newSlots.size()!=iAnzahlSlots)
    {
        //initialize the conslot vector with the number of slots
        newSlots.resize(iAnzahlSlots);
    }

    //Insert Slot data in Vector
    for(int i=0; i < iAnzahlSlots; ++i){
        newSlots[i].iBitnummer = lineEditsBits[i]->text().toInt();
        newSlots[i].iStartAdr = lineEditsAddress[i]->text().toInt();
        newSlots[i].iAnzFormat = comboBoxesFormat[i]->
                itemData(comboBoxesFormat[i]->currentIndex()).toInt();
        newSlots[i].iAdrBereich = comboBoxesArea[i]->
                itemData(comboBoxesArea[i]->currentIndex()).toInt();
        newSlots[i].iDBnummer = lineEditsDB[i]->text().toInt();
        newSlots[i].graphColor = graphColors[i % moduloValue];
        newSlots[i].iDatenlaenge = comboBoxesLength[i]->
                itemData(comboBoxesLength[i]->currentIndex()).toInt();
    }

    // Emit the signal that the Slots has changed
    emit SlotsChanged(newSlots);
}

void ConnectionSettings::SetSettings(ConSets *CurrentSets)
{
    // Set the membervariables with the cuttent changes
    m_DiagSets = CurrentSets;

    // Set input boxes with the current values
    ui->lineEdit_IP->setText(m_DiagSets->IP_Adr);
    ui->lineEdit_local_MPI->setText(QString::number(m_DiagSets->localMPI));
    ui->lineEdit_CPU_MPI->setText(QString::number(m_DiagSets->plcMPI));
    ui->comboBox_Speed->setCurrentIndex(m_DiagSets->speed);
    ui->ComboBox_Protokoll->setCurrentIndex(ui->ComboBox_Protokoll->findData(m_DiagSets->useProto));
    ui->lineEdit_Rack->setText(QString::number(m_DiagSets->rack));
    ui->lineEdit_Slot->setText(QString::number(m_DiagSets->slot));
}

void ConnectionSettings::on_ComboBox_Protokoll_currentIndexChanged(int index)
{
    // Set the enable status of input boxes coredponding the current protocol type
    int iCurrProto = ui->ComboBox_Protokoll->itemData(index).toInt();

    switch(iCurrProto)
    {
    case daveProtoMPI:
    case daveProtoPPI:
    case daveProtoAS511:
        ui->comboBox_Speed->setEnabled(true);
        ui->lineEdit_CPU_MPI->setEnabled(true);
        ui->lineEdit_IP->setEnabled(false);
        ui->lineEdit_local_MPI->setEnabled(true);
        ui->lineEdit_Rack->setEnabled(true);
        ui->lineEdit_Slot->setEnabled(true);
        break;
    case daveProtoS7online:
        ui->comboBox_Speed->setEnabled(false);
        ui->lineEdit_CPU_MPI->setEnabled(false);
        ui->lineEdit_IP->setEnabled(false);
        ui->lineEdit_local_MPI->setEnabled(false);
        ui->lineEdit_Rack->setEnabled(true);
        ui->lineEdit_Slot->setEnabled(true);
        break;
    case daveProtoISOTCP:
    case daveProtoISOTCP243:
    case daveProtoISOTCPR:
        ui->comboBox_Speed->setEnabled(false);
        ui->lineEdit_CPU_MPI->setEnabled(false);
        ui->lineEdit_IP->setEnabled(true);
        ui->lineEdit_local_MPI->setEnabled(false);
        ui->lineEdit_Rack->setEnabled(true);
        ui->lineEdit_Slot->setEnabled(true);
        break;
    default:
        ui->comboBox_Speed->setEnabled(false);
        ui->lineEdit_CPU_MPI->setEnabled(false);
        ui->lineEdit_IP->setEnabled(false);
        ui->lineEdit_local_MPI->setEnabled(false);
        ui->lineEdit_Rack->setEnabled(false);
        ui->lineEdit_Slot->setEnabled(false);
        break;
    }
}

void ConnectionSettings::comboBoxIndexChanged(int index)
{
    QComboBox *sendingBox = (QComboBox *)sender();
    int dataItem = sendingBox->itemData(index).toInt();
    int lineNumber = 0;

    if(comboBoxesArea.contains(sendingBox)){
        lineNumber = findCorrespondingLine(comboBoxesArea,sendingBox);
    }else if(comboBoxesFormat.contains(sendingBox)){
        lineNumber = findCorrespondingLine(comboBoxesFormat,sendingBox);
    }else if(comboBoxesLength.contains(sendingBox))
    {
        lineNumber = findCorrespondingLine(comboBoxesLength,sendingBox);
    }

  switch (dataItem)
    {
    case daveInputs:
    case daveOutputs:
        if(comboBoxesLength[lineNumber]->itemData(comboBoxesLength[lineNumber]->currentIndex()).toInt() == DatLenBit)
        {
            lineEditsBits[lineNumber]->setEnabled(true);
        }
        lineEditsDB[lineNumber]->clear();
        lineEditsDB[lineNumber]->setDisabled(true);
        break;
    case daveFlags:
        lineEditsDB[lineNumber]->clear();
        lineEditsDB[lineNumber]->setDisabled(true);
        if(comboBoxesLength[lineNumber]->itemData(comboBoxesLength[lineNumber]->currentIndex()).toInt() == DatLenBit)
        {
            lineEditsBits[lineNumber]->setEnabled(true);
        }
        break;
    case daveDB:
        lineEditsDB[lineNumber]->setEnabled(true);
        if(comboBoxesLength[lineNumber]->itemData(comboBoxesLength[lineNumber]->currentIndex()).toInt() == DatLenBit)
        {
            lineEditsBits[lineNumber]->setEnabled(true);
        }
        break;
    case DatLenWord:
    case DatLenByte:
    case DatLenDWord:
        lineEditsBits[lineNumber]->setDisabled(true);
        lineEditsBits[lineNumber]->clear();
        break;
    default:
        if(comboBoxesLength[lineNumber]->itemData(comboBoxesLength[lineNumber]->currentIndex()).toInt() == DatLenBit)
        {
            lineEditsBits[lineNumber]->setEnabled(true);
        }else
        {
            lineEditsBits[lineNumber]->setDisabled(true);
            lineEditsBits[lineNumber]->clear();
        }
        if(!(comboBoxesArea[lineNumber]->itemData(comboBoxesArea[lineNumber]->currentIndex()).toInt() == daveDB))
           {
            lineEditsDB[lineNumber]->setDisabled(true);
            lineEditsDB[lineNumber]->clear();
        }else            lineEditsDB[lineNumber]->setEnabled(true);
    }
}

int ConnectionSettings::findCorrespondingLine(QList<QComboBox*> areaBoxes,
                                              QComboBox* sendingBox)
{
    int lineNumber = 0;
    int count = 0;

    Q_FOREACH(QComboBox *areaBox,areaBoxes)
    {
        if(areaBox == sendingBox) {lineNumber = count;}
        count++;
    }
    return lineNumber;
}

void ConnectionSettings::SetSlots(QVector<ConSlot> &currentSlots)
{
    for(int i=0; i<currentSlots.count();++i){
        comboBoxesArea[i]->setCurrentIndex(comboValuesArea.indexOf(currentSlots[i].iAdrBereich));
        comboBoxesFormat[i]->setCurrentIndex(comboValuesFormat.indexOf(currentSlots[i].iAnzFormat));
        lineEditsBits[i]->setText(QString::number(currentSlots[i].iBitnummer));
        lineEditsAddress[i]->setText(QString::number(currentSlots[i].iStartAdr));
        lineEditsDB[i]->setText(QString::number(currentSlots[i].iDBnummer));
        comboBoxesLength[i]->setCurrentIndex(comboValuesLength.indexOf(currentSlots[i].iDatenlaenge));
    }
}

//LessThan Comparison for combo boxes
bool ConnectionSettings::comboBoxPointerLessThan(QComboBox *cb1, QComboBox *cb2)
{
    return cb1->objectName() < cb2->objectName();
}

//LessThan Comparison for line edits
bool ConnectionSettings::lineEditPointerLessThan(QLineEdit* le1, QLineEdit* le2)
{
    return le1->objectName() < le2->objectName();
}
