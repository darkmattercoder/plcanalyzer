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
delete openedConSets;
}

void xmlSettingsHandler::newProject()
{

}

void xmlSettingsHandler::openProject()
{
   filename = QFileDialog::getOpenFileName(this,"Öffnen",".","PLCANALYZER-Projekte (*.plcproj)");
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
               }else if(xmlReader.isStartElement() && xmlReader.name().contains("SLOT_")){



                           xmlReader.readNextStartElement();
                           std::cout << xmlReader.name().toString().toStdString() << std::endl;

                        tempSlot.iAdrBereich=xmlReader.readElementText().toInt();
                        xmlReader.readNextStartElement();
                         std::cout << xmlReader.name().toString().toStdString() << std::endl;
                        tempSlot.iStartAdr=xmlReader.readElementText().toInt();
                        xmlReader.readNextStartElement();
                         std::cout << xmlReader.name().toString().toStdString() << std::endl;
                        tempSlot.iAnzFormat=xmlReader.readElementText().toInt();
                        xmlReader.readNextStartElement();
                         std::cout << xmlReader.name().toString().toStdString() << std::endl;
                        tempSlot.iBitnummer=xmlReader.readElementText().toInt();
                        xmlReader.readNextStartElement();
                         std::cout << xmlReader.name().toString().toStdString() << std::endl;
                        tempSlot.iDatenlaenge=xmlReader.readElementText().toInt();
                        xmlReader.readNextStartElement();
                         std::cout << xmlReader.name().toString().toStdString() << std::endl;
                        tempSlot.iDBnummer=xmlReader.readElementText().toInt();
                        openedConSlots.append(tempSlot);



               }





           }

           if(xmlReader.hasError())
           {
               std::cout << "XML-READER Error: " << xmlReader.errorString().toStdString() << std::endl;
               QMessageBox *msgBoxOpenError = new QMessageBox(this);
               msgBoxOpenError->setIcon(QMessageBox::Critical);
               msgBoxOpenError->setText("Fehler beim oeffnen");
               msgBoxOpenError->show();

           }else emit newSlotsOpened(openedConSlots);

file.close();

}

void xmlSettingsHandler::saveProject(ConSets* currentConSets, QVector<ConSlot> currentConSlots)
{

    filename = QFileDialog::getSaveFileName(this,"Speichern",".","PLCANALYZER-Projekte (*.plcproj)");
    if(!filename.contains(".plcproj")) {filename.append(".plcproj");}
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("DATA");
        xmlWriter.writeStartElement("SETTINGS");
    xmlWriter.writeTextElement("IP_ADDR", currentConSets->IP_Adr);
    xmlWriter.writeTextElement("PROTOCOL",QString::number(currentConSets->useProto));
    xmlWriter.writeTextElement("LOCAL_MPI",QString::number(currentConSets->localMPI));
    xmlWriter.writeTextElement("PLC_MPI",QString::number(currentConSets->plcMPI));
    xmlWriter.writeTextElement("PLC_2_MPI",QString::number(currentConSets->plc2MPI));
    xmlWriter.writeTextElement("RACK",QString::number(currentConSets->rack));
    xmlWriter.writeTextElement("SLOT",QString::number(currentConSets->slot));
    xmlWriter.writeTextElement("SPEED",QString::number(currentConSets->speed));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("SLOTS");
    for(int i=0;i<currentConSlots.size();++i)
    {
        QString slotDescriptor = "SLOT_" + QString::number(i);
        xmlWriter.writeStartElement(slotDescriptor);
        xmlWriter.writeTextElement("AREA",QString::number(currentConSlots[i].iAdrBereich));
        xmlWriter.writeTextElement("START_ADDRESS",QString::number(currentConSlots[i].iStartAdr));
        xmlWriter.writeTextElement("FORMAT",QString::number(currentConSlots[i].iAnzFormat));
        xmlWriter.writeTextElement("BIT",QString::number(currentConSlots[i].iBitnummer));
        xmlWriter.writeTextElement("LENGTH",QString::number(currentConSlots[i].iDatenlaenge));
        xmlWriter.writeTextElement("DB_NUMBER",QString::number(currentConSlots[i].iDBnummer));

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
