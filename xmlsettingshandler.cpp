/*******************************************************************************
 *   *File: xmlsettingshandler.cpp                                             *
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

#include<QFileDialog>
//#include <mainwindow.h>
#include "xmlsettingshandler.h"
#include <iostream>

xmlSettingsHandler::xmlSettingsHandler(QMainWindow *parent)
    : QMainWindow(parent)
{
    openedConSets = new ConSets;
}

xmlSettingsHandler::~xmlSettingsHandler()
{

}

void xmlSettingsHandler::newProject()
{

}

void xmlSettingsHandler::openProject()
{
    filename = QFileDialog::getOpenFileName(this,"Öffnen",".","PLCANALYZER-"
                                            "Projekte (*.plcproj)");
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << "Error: Cannot read file " << qPrintable(filename)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;

    }

    xmlReader.setDevice(&file);
    openedConSlots.clear();
    ConSlot tempSlot;
    while(!xmlReader.atEnd())
    {
        xmlReader.readNext();
        if(xmlReader.isStartElement() && xmlReader.name()=="IP_ADDR")
        {

            openedConSets->IP_Adr = xmlReader.readElementText();
            xmlReader.readNextStartElement();
            openedConSets->useProto = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->localMPI = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->plcMPI = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->plc2MPI = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->rack = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->slot = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            openedConSets->speed = xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
        }else if(xmlReader.isStartElement() &&
                 xmlReader.name().contains("SLOT_")){

            xmlReader.readNextStartElement();
            tempSlot.iAdrBereich=xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            tempSlot.iStartAdr=xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            tempSlot.iAnzFormat=xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            tempSlot.iBitnummer=xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            tempSlot.iDatenlaenge=xmlReader.readElementText().toInt();
            xmlReader.readNextStartElement();
            tempSlot.iDBnummer=xmlReader.readElementText().toInt();
            openedConSlots.append(tempSlot);
        }
    }

    if(xmlReader.hasError())
    {
        std::cout << "XML-READER Error: " <<
                     xmlReader.errorString().toStdString() << std::endl;
        QMessageBox *msgBoxOpenError = new QMessageBox(this);
        msgBoxOpenError->setIcon(QMessageBox::Critical);
        msgBoxOpenError->setText("Fehler beim oeffnen");
        msgBoxOpenError->show();

    }else emit newSlotsOpened(openedConSlots);

    file.close();

}

void xmlSettingsHandler::saveProject(ConSets* currentConSets,
                                     QVector<ConSlot> currentConSlots)
{

    filename = QFileDialog::getSaveFileName(this,"Speichern",".",
                                            "PLCANALYZER-Projekte (*.plcproj)");
    if(!filename.contains(".plcproj")) {filename.append(".plcproj");}
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("DATA");
    xmlWriter.writeStartElement("SETTINGS");
    xmlWriter.writeTextElement("IP_ADDR", currentConSets->IP_Adr);
    xmlWriter.writeTextElement("PROTOCOL",
                               QString::number(currentConSets->useProto));
    xmlWriter.writeTextElement("LOCAL_MPI",
                               QString::number(currentConSets->localMPI));
    xmlWriter.writeTextElement("PLC_MPI",
                               QString::number(currentConSets->plcMPI));
    xmlWriter.writeTextElement("PLC_2_MPI",
                               QString::number(currentConSets->plc2MPI));
    xmlWriter.writeTextElement("RACK",
                               QString::number(currentConSets->rack));
    xmlWriter.writeTextElement("SLOT",
                               QString::number(currentConSets->slot));
    xmlWriter.writeTextElement("SPEED",
                               QString::number(currentConSets->speed));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("SLOTS");
    for(int i=0;i<currentConSlots.size();++i)
    {
        QString slotDescriptor = "SLOT_" + QString::number(i);
        xmlWriter.writeStartElement(slotDescriptor);
        xmlWriter.writeTextElement("AREA",QString::number(
                                       currentConSlots[i].iAdrBereich));
        xmlWriter.writeTextElement("START_ADDRESS",QString::number(
                                       currentConSlots[i].iStartAdr));
        xmlWriter.writeTextElement("FORMAT",QString::number(
                                       currentConSlots[i].iAnzFormat));
        xmlWriter.writeTextElement("BIT",QString::number(
                                       currentConSlots[i].iBitnummer));
        xmlWriter.writeTextElement("LENGTH",QString::number(
                                       currentConSlots[i].iDatenlaenge));
        xmlWriter.writeTextElement("DB_NUMBER",QString::number(
                                       currentConSlots[i].iDBnummer));

        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();
    std::cout << "File Saved!" << std::endl;

}

void xmlSettingsHandler::saveProjectAs()
{

}
