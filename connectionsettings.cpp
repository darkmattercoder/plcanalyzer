#include "connectionsettings.h"
#include <iostream>
using namespace std;


ConnectionSettings::ConnectionSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConnectionSettings)
{

    // Userinterface aufbauen
    ui->setupUi(this);

    //setup a signal mapper for comboBox conditional signals
 //   signalMapper = new QSignalMapper(this);

    //Fill comboboxes for area and formats with data

      comboBoxesArea = findChildren<QComboBox*>(QRegExp("comboBoxArea_.*"));
   comboBoxesFormat = findChildren<QComboBox*>(QRegExp("comboBoxFormat_.*"));
   lineEditsBits = findChildren<QLineEdit*>(QRegExp("lineEditBit_.*"));
   lineEditsAddress = findChildren<QLineEdit*>(QRegExp("lineEditAddress_.*"));

      comboItemsArea << " " << "E" << "A" << "M" << "DB" << "Z" << "T" << "IEC_Z" << "IEC_T";
      comboItemsFormat << " " << "BOOL" << "BYTE" << "WORD" << "DWORD" << "INT" << "DINT" << "REAL";
      comboValuesArea << 0x0 << daveInputs << daveOutputs << daveFlags << daveDB << daveCounter << daveTimer << daveCounter200 << daveTimer200;
      comboValuesFormat << 0x0 << AnzFormatBool << AnzFormatHexadezimal << AnzFormatHexadezimal << AnzFormatHexadezimal << AnzFormatDezimal << AnzFormatDezimal << AnzFormatGleitpunkt;

      //Initialize with two for loops. Q_FOREACH would fit too, seems simpler and faster but then an additional iterator
      //value declaration outside the loop was necessary.
      for(int i=0;i<comboBoxesArea.count();++i)
      {
          for(int j=0;j<comboItemsArea.count();++j)
              comboBoxesArea[i]->addItem(comboItemsArea[j],comboValuesArea[j]);
      }

      for(int i=0;i<comboBoxesFormat.count();++i)
      {
          for(int j=0;j<comboItemsFormat.count();++j)
          comboBoxesFormat[i]->addItem(comboItemsFormat[j],comboValuesFormat[j]);
      }


      //Connect the format-combo boxes' indexChanged signal to the handling
      //Slot for conditional disabling of Bit field

      for(int i=0;i<comboBoxesArea.count();++i)
      {
          connect(comboBoxesArea[i],SIGNAL(currentIndexChanged(int)),this,SLOT(comboBoxIndexChanged(int)));
    //         signalMapper->setMapping();
        }
/*
      for(int i = 0; i < comboBoxesFormat.count();++i)
      {
          connect(comboBoxesFormat[i],SIGNAL(currentIndexChanged(int)),this,SLOT(comboBoxIndexChanged(int)));
      }*/


              // ComboBox "Protokoll" mit den Einstellungen füllen
    ui->ComboBox_Protokoll->addItem("MPI for S7 300/400", daveProtoMPI);
    ui->ComboBox_Protokoll->addItem("PPI for S7 200", daveProtoPPI);
    ui->ComboBox_Protokoll->addItem("S5 programming port protocol", daveProtoAS511);
#ifdef BCCWIN
    ui->ComboBox_Protokoll->addItem("use s7onlinx.dll for transport", daveProtoS7online);
#endif
    ui->ComboBox_Protokoll->addItem("ISO over TCP", daveProtoISOTCP);
    ui->ComboBox_Protokoll->addItem("ISO over TCP with CP243", daveProtoISOTCP243);
    ui->ComboBox_Protokoll->addItem("ISO over TCP with Routing", daveProtoISOTCPR);

    // Validator für die Eingabe der IP Adresse
    QRegExpValidator *RegExp_IP = new QRegExpValidator(this);
    QRegExp rx("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.){3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
    RegExp_IP->setRegExp(rx);
    ui->lineEdit_IP->setValidator(RegExp_IP);

    // Validator für die Eingabe der MPI Adresse 0..127
    QRegExpValidator *RegExp_MPI = new QRegExpValidator(this);
    rx.setPattern("[0-9]{0,2}|1{1,1}[0-1]?[0-9]?|1{1,1}2{1,1}[0-7]{1,1}");
    RegExp_MPI->setRegExp(rx);
    ui->lineEdit_CPU_MPI->setValidator(RegExp_MPI);
    ui->lineEdit_local_MPI->setValidator(RegExp_MPI);

}

ConnectionSettings::~ConnectionSettings()
{
    delete ui;
}

void ConnectionSettings::on_buttonBox_accepted()
{
    // Einstellungen in die Membervariable schreiben
    m_DiagSets->IP_Adr = ui->lineEdit_IP->text();
    m_DiagSets->localMPI = ui->lineEdit_local_MPI->text().toInt();
    m_DiagSets->plcMPI = ui->lineEdit_CPU_MPI->text().toInt();
    m_DiagSets->speed = ui->comboBox_Speed->currentIndex();
    m_DiagSets->useProto = ui->ComboBox_Protokoll->itemData(ui->ComboBox_Protokoll->currentIndex()).toInt();
    m_DiagSets->rack = ui->lineEdit_Rack->text().toInt();
    m_DiagSets->slot = ui->lineEdit_Slot->text().toInt();

    // Signal "Einstellungen geändert" auslösen
    emit SettingsChanged(m_DiagSets);
    if(newSlots.isEmpty() || !newSlots.size()==comboBoxesArea.count()){
        //initialize the conslot vector with the number of slots
        newSlots.resize(comboBoxesArea.count());
    }



    //Insert Slot data in Vector
    for(int i=0;i<lineEditsBits.count();++i){
        newSlots[i].iBitnummer = lineEditsBits[i]->text().toInt();
        newSlots[i].iStartAdr = lineEditsAddress[i]->text().toInt();
        newSlots[i].iAnzFormat = comboBoxesFormat[i]->itemData(comboBoxesFormat[i]->currentIndex()).toInt();
        newSlots[i].iAdrBereich = comboBoxesArea[i]->itemData(comboBoxesArea[i]->currentIndex()).toInt();
    }


    emit SlotsChanged(newSlots);
}

void ConnectionSettings::SetSettings(ConSets *CurrentSets)
{
    // Membervariable mit den aktuellen Einstellungen setzten
    m_DiagSets = CurrentSets;

    // Eingaben mit den akuellen Werten setzten
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
    // Setzt die Bedienbarkeit von Elementen in Abhängigkeit des gewählten Protokolltyps
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
    }

    switch (dataItem)
    {
    case daveCounter:
    case daveTimer:
    case daveCounter200:
    case daveTimer200:
    case AnzFormatHexadezimal:
       lineEditsBits[lineNumber]->clear();
        lineEditsBits[lineNumber]->setDisabled(true);

break;

     default:
        lineEditsBits[lineNumber]->setEnabled(true);
    }


 }

int ConnectionSettings::findCorrespondingLine(QList<QComboBox*> areaBoxes,QComboBox* sendingBox)
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

void ConnectionSettings::SetSlots(QVector<ConSlot> currentSlots)
{
    for(int i=0; i<currentSlots.count();++i){
        comboBoxesArea[i]->setCurrentIndex(comboValuesArea.indexOf(currentSlots[i].iAdrBereich));
        comboBoxesFormat[i]->setCurrentIndex(comboValuesFormat.indexOf(currentSlots[i].iAnzFormat));
        lineEditsBits[i]->setText(QString::number(currentSlots[i].iBitnummer));
        lineEditsAddress[i]->setText(QString::number(currentSlots[i].iStartAdr));
    }
}
