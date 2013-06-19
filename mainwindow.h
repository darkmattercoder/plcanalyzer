#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "s7connection.h"
#include "StdOutRedirector.h"
#include "connectionsettings.h"
#include "mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    S7Connection MyS7Connection;
    
private slots:
    void on_actionNeues_Projekt_triggered();

    void on_actionProjekt_ffnen_triggered();

    void on_Button_Connect_clicked();

    void on_Button_Get_Val_clicked();

    void TimeOut();

    void ChangeSettings(ConSets NewConSets);

    void on_pushButton_ConSets_clicked();

private:
    Ui::MainWindow *ui;
    StdOutRedirector MyRedirector;
    ConnectionSettings ConDiag;
};

#endif // MAINWINDOW_H


