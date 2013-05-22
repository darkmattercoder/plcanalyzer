#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "s7connection.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //S7Connection::S7Connection();
    //S7Connection::connect(this->winId());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNeues_Projekt_triggered()
{
    printf("Hello World\r\n");

}


void MainWindow::on_actionProjekt_ffnen_triggered()
{
   QString fileName = QFileDialog::getOpenFileName(this, tr("Projekt öffnen"), "", tr("Files (*.xml)"));

    QMessageBox::information(
            this,
            tr("Gewaehlter Pfad"),
                tr(fileName.toUtf8().constData()));

}

void MainWindow::on_pushButton_2_clicked()
{
    S7Connection MyS7Connection;
    MyS7Connection.startConnection(this->winId());
    //    char szOutput[128];
    //    //sprintf_s(szOutput, "MB 1: %i", S7Connection::getValue());
    //    QMessageBox::information(
    //                this,
    //                tr("Wert von Merker Byte 1"),

    //                tr(szOutput));

}
