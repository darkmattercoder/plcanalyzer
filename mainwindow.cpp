    #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textEdit->append(QString().sprintf("Willkommen\n"));

    //MyRedirector.setOutputTF(ui->textEdit);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeOut()));
    timer->start(100);

    connect(&ConDiag, SIGNAL(SettingsChanged(ConSets)), this, SLOT(ChangeSettings(ConSets)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNeues_Projekt_triggered()
{
    qDebug("Gewaehltes Protokoll: %i", MyS7Connection.MyConSet.useProto);


}


void MainWindow::on_actionProjekt_ffnen_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, tr("Projekt oeffnen"), "", tr("Files (*.xml)"));

    QMessageBox::information(
            this,
            tr("Gewaehlter Pfad"),
                tr(fileName.toUtf8().constData()));

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

// Neunen Wert aus der SPS anfordern
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
    // Redirect STDOUTs to TextEdit
    //MyRedirector.readOutsToTF();
}

// Event Werte aus Dialog sollen übernommen werden
void MainWindow::ChangeSettings(ConSets NewConSets)
{
    MyS7Connection.MyConSet = NewConSets;

}

// Button Verbindungseinstellungen
void MainWindow::on_pushButton_ConSets_clicked()
{
    ConDiag.SetSettings(MyS7Connection.MyConSet);
    ConDiag.show();
}


void MainWindow::updateOutput()
{
    QByteArray bytes = process->readAllStandardOutput();
    ui->textEdit->insertPlainText(bytes);
    printf("Test\n");
}

void MainWindow::on_Button_read_slots_clicked()
{
    ConSlot MySlot[5];
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
