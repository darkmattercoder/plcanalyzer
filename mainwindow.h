#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//moved from initial mainwindow.cpp due to strange linux compile errors
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QTextEdit>
#include <iostream>


#include "stdoutredirector.h"

#include "s7connection.h"
#include "xmlsettingshandler.h"
#include "connectionsettings.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ConSets* currentConsets;
    S7Connection MyS7Connection;
    Ui::MainWindow *ui;
 ConnectionSettings ConDiag;
 //int numberOfSlots;
QVector<ConSlot> MySlot;

private slots:
    void on_Button_Connect_clicked();
    void on_Button_Get_Val_clicked();
    
    void ChangeSettings(ConSets* NewConSets);
   void TimeOut();
 void on_pushButton_ConSets_clicked();
    void on_Button_read_slots_clicked();
    void changeSlots(QVector<ConSlot> newConSlots);

    void on_pushButton_clicked();
void on_actionNewProject_triggered();

    void on_actionopenProject_triggered();

    void on_actionSaveProject_triggered();

signals:



private:
int numberOfSlots;
    int recordings;
    int amountOfPoints;
    QVector<double> x;          // 1d Vector
    QVector<QVector<double>> y; // 2d Vectot
    Ui::MainWindow *ui;
    ConnectionSettings ConDiag;
QTextEdit* debugText;
};

#endif // MAINWINDOW_H
