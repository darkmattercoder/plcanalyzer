#include "connectionsettings.h"


ConnectionSettings::ConnectionSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConnectionSettings)
{
    // Userinterface aufbauen
    ui->setupUi(this);

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
    m_DiagSets.IP_Adr = ui->lineEdit_IP->text();
    m_DiagSets.localMPI = ui->lineEdit_local_MPI->text().toInt();
    m_DiagSets.plcMPI = ui->lineEdit_CPU_MPI->text().toInt();
    m_DiagSets.speed = ui->comboBox_Speed->currentIndex();
    m_DiagSets.useProto = ui->ComboBox_Protokoll->itemData(ui->ComboBox_Protokoll->currentIndex()).toInt();
    m_DiagSets.rack = ui->lineEdit_Rack->text().toInt();
    m_DiagSets.slot = ui->lineEdit_Slot->text().toInt();

    // Signal "Einstellungen geändert" auslösen
    emit SettingsChanged(m_DiagSets);
}

void ConnectionSettings::SetSettings(ConSets CurrentSets)
{
    // Membervariable mit den aktuellen Einstellungen setzten
    m_DiagSets = CurrentSets;

    // Eingaben mit den akuellen Werten setzten
    ui->lineEdit_IP->setText(m_DiagSets.IP_Adr);
    ui->lineEdit_local_MPI->setText(QString::number(m_DiagSets.localMPI));
    ui->lineEdit_CPU_MPI->setText(QString::number(m_DiagSets.plcMPI));
    ui->comboBox_Speed->setCurrentIndex(m_DiagSets.speed);
    ui->ComboBox_Protokoll->setCurrentIndex(ui->ComboBox_Protokoll->findData(m_DiagSets.useProto));
    ui->lineEdit_Rack->setText(QString::number(m_DiagSets.rack));
    ui->lineEdit_Slot->setText(QString::number(m_DiagSets.slot));
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
