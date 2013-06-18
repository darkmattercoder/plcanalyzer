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

    MyRedirector.setOutputTF(ui->textEdit);

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
    printf("Gewähltes Protokoll: %i\n", MyS7Connection.MyConSet.useProto);


}


void MainWindow::on_actionProjekt_ffnen_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, tr("Projekt oeffnen"), "", tr("Files (*.xml)"));

    QMessageBox::information(
            this,
            tr("Gewaehlter Pfad"),
                tr(fileName.toUtf8().constData()));

}


void MainWindow::on_pushButton_clicked()
{

    ConDiag.SetSettings(MyS7Connection.MyConSet);
    ConDiag.show();

}

void MainWindow::on_Button_Connect_clicked()
{
    if(MyS7Connection.startConnection(this->winId()))
    {
        ui->Button_Get_Val->setEnabled(true);
    }
}

void MainWindow::on_Button_Get_Val_clicked()
{
    if (MyS7Connection.isConnected())
    {
        ui->lcdNumber->display(MyS7Connection.getValue());
    }
}

void MainWindow::TimeOut()
{
    // Redirect STDOUTs to TextEdit
    MyRedirector.readOutsToTF();
}

void MainWindow::ChangeSettings(ConSets NewConSets)
{
    MyS7Connection.MyConSet = NewConSets;

}
