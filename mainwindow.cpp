#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
   /* QString fileName = QFileDialog::getOpenFileName(this, tr("Projekt öffnen"), "", tr("Files (*.xml)"));

    QMessageBox::information(
            this,
            tr("Gewaehlter Pfad"),
                tr(fileName.toUtf8().constData()));
*/
}
