    #include "mainwindow.h"
//uncommented for strange linux compile errrors and mpoved to mainwindow.h
//#include "ui_mainwindow.h"
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

    // Validator für die Eingabe der Aufnahmedauer
    QRegExpValidator *RegExp_Duration = new QRegExpValidator(this);
    QRegExp rx("[0-9]{0,2}|1{1,1}0{0,2}");
    RegExp_Duration->setRegExp(rx);
    ui->lineEdit_Duration->setValidator(RegExp_Duration);

    recordings = MAXINT;

    //####################ZU BEARBEITEN######################################
    numberOfSlots = 2;
    //#######################################################################
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
    // Zyklisches lesen
    if (recordings < ui->lineEdit_Duration->text().toInt() * 10)
    {
        // Read new values to slots
        MyS7Connection.readSlots(&MySlot[0], numberOfSlots);

        // Write the time vector
        x[recordings] = 0.1 * recordings; //Abtastung alle 100ms

        // Write all dimensions of value vector
        for (int i = 0; i < numberOfSlots; i++)
        {
            // Write actual value
            y[i][recordings] = MyS7Connection.interpret(MySlot[i]).toFloat();

            // Assign vector to graph
            ui->customPlot->graph(i)->setData(x, y[i]);
        }
        ui->customPlot->replot();
        recordings++;
    }
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
    MySlot[1].iAnzFormat = AnzFormatDezimal;

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

    // Reset recording couter
    recordings = 0;

    //Delete old graphs
    int numberOfGraphs = ui->customPlot->graphCount();
    qDebug("Number of Graphs is: %i", numberOfGraphs);

    for (int i = numberOfGraphs; i > numberOfSlots; i--)
    {
        // Remove not needed graphs
        ui->customPlot->removeGraph(i);
        qDebug("Remove Graph: %i", i);
    }

    for (int i = 0; i < numberOfSlots; i++)
    {
        // Add new graphs
        ui->customPlot->addGraph();
        qDebug("Adding Graph number %i", i);
    }

    // Resize vectors
    int amountOfPoints = ui->lineEdit_Duration->text().toInt() * 10; //Abtastung mit 10 Hz
    x.resize(amountOfPoints);
    y.resize(numberOfSlots);

    // Resize 2nd dimension
    for (int i = 0; i < numberOfSlots; i++)
    {
        y[i].resize(amountOfPoints);
    }

    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("[Zeit] s");
    ui->customPlot->yAxis->setLabel("Wert");

    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0.0, ui->lineEdit_Duration->text().toInt());
    ui->customPlot->yAxis->setRange(-1.0, 1.0);

}

// Autoscale axes
void MainWindow::on_pushButton_clicked()
{
    ui->customPlot->rescaleAxes();
}
