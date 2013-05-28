#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "s7connection.h"
#include "StdOutRedirector.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow//, public S7Connection
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionNeues_Projekt_triggered();

    void on_actionProjekt_ffnen_triggered();

    void on_pushButton_clicked();

    void on_Button_Connect_clicked();

    void on_Button_Get_Val_clicked();

    void on_Timer_clicked();

private:
    Ui::MainWindow *ui;
    S7Connection MyS7Connection;
    StdOutRedirector MyRedirector;
};

#endif // MAINWINDOW_H
