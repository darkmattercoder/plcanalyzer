#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logtoparent.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "xmlsettingshandler.h"
using namespace std;


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
    //in the main window
    logToParent* logger = new logToParent(this);
    xmlSettings = new xmlSettingsHandler(this);
    connect(xmlSettings,SIGNAL(newSlotsOpened(QVector<ConSlot>)),this,SLOT(changeSlots(QVector<ConSlot>)));
currentConsets = new ConSets;
    /*numberOfSlots = 5;
   //Initialize MySlot Array
   for(int i = 0;i<numberOfSlots;++i)
   {
       MySlot[i].iAdrBereich = 0;
       MySlot[i].iAnzFormat = 0;
       MySlot[i].iBitnummer = 0;
       MySlot[i].iDatenlaenge = 0;
       MySlot[i].iDBnummer = 0;
       MySlot[i].iStartAdr = 0;
       MySlot[i].RetVal.Bit = false;
       MySlot[i].RetVal.Byte = ' ';
       MySlot[i].RetVal.DInt = 0;
       MySlot[i].RetVal.Int = 0;
       MySlot[i].RetVal.Real = 0.0;
       ConDiag.SetSlots(MySlot);

   }*/
   currentConsets = MyS7Connection.MyConSet;
    ConDiag.SetSettings(MyS7Connection.MyConSet);

}
MainWindow::~MainWindow()
{
    delete ui;

}


void MainWindow::on_actionopenProject_triggered()
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
            ui->Button_Connect->setText("Disconnect");
        }
    }
    else
    {
        MyS7Connection.disconnect();
        ui->Button_Get_Val->setEnabled(false);
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

    // Slot 2
    MySlot[1].iAdrBereich = daveFlags;
    MySlot[1].iDatenlaenge = DatLenBit;
    MySlot[1].iStartAdr = 1;
    MySlot[1].iDBnummer = 0;
    MySlot[1].iBitnummer = 7;
    MySlot[1].iAnzFormat = AnzFormatBool;

    // Slot 3
    MySlot[2].iAdrBereich = daveFlags;
    MySlot[2].iDatenlaenge = DatLenDWord;
    MySlot[2].iStartAdr = 4;
    MySlot[2].iDBnummer = 0;
    MySlot[2].iBitnummer = 0;
    MySlot[2].iAnzFormat = AnzFormatBinaer;

    // Slot 4
    MySlot[3].iAdrBereich = daveFlags;
    MySlot[3].iDatenlaenge = DatLenWord;
    MySlot[3].iStartAdr = 8;
    MySlot[3].iDBnummer = 0;
    MySlot[3].iBitnummer = 0;
    MySlot[3].iAnzFormat = AnzFormatHexadezimal;

// Slot 4
    MySlot[4].iAdrBereich = daveFlags;
    MySlot[4].iDatenlaenge = DatLenByte;
    MySlot[4].iStartAdr = 10;
    MySlot[4].iDBnummer = 0;
    MySlot[4].iBitnummer = 0;
    MySlot[4].iAnzFormat = AnzFormatZeichen;

    MyS7Connection.readSlots(&MySlot[0], 5);

    for(int i = 0; i < 5; i++)
    {
        ui->textEdit->append(MyS7Connection.interpret(MySlot[i]));
    }
}


void MainWindow::on_actionNewProject_triggered()
{

        cout << "Gewaehltes Protokoll: %i" << MyS7Connection.MyConSet->useProto << endl;

}


void MainWindow::on_actionSaveProject_triggered()
{

    xmlSettings->saveProject(MyS7Connection.MyConSet,MySlot);
}
