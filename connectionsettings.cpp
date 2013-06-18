#include "connectionsettings.h"
#include "ui_connectionsettings.h"

ConnectionSettings::ConnectionSettings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConnectionSettings)


{

    ui->setupUi(this);
//#ifdef BCCWIN
//    szListConnection = (QStringList() << "TCP IP" << "S7 Online");
//#endif // BCCWIN

//#ifdef LINUX
//    szListConnection = (QStringList() << "TCP IP");
//#endif

    //ui->ComboBox_Protokoll->addItems(szListConnection);
    ui->ComboBox_Protokoll->insertItem(daveProtoMPI, "MPI for S7 300/400");
    ui->ComboBox_Protokoll->insertItem(daveProtoPPI, "PPI for S7 200");
    ui->ComboBox_Protokoll->insertItem(daveProtoAS511, "S5 programming port protocol");
#ifdef BCCWIN
    ui->ComboBox_Protokoll->insertItem(daveProtoS7online, "use s7onlinx.dll for transport");
#endif
    ui->ComboBox_Protokoll->insertItem(daveProtoISOTCP, "ISO over TCP");
    ui->ComboBox_Protokoll->insertItem(daveProtoISOTCP243, "ISO over TCP with CP243");
    ui->ComboBox_Protokoll->insertItem(daveProtoISOTCPR, "ISO over TCP with Routing");

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
    m_DiagSets.useProto = ui->ComboBox_Protokoll->currentIndex();

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
    ui->ComboBox_Protokoll->setCurrentIndex(m_DiagSets.useProto);

}
