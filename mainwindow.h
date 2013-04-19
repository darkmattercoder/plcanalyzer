#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionNeues_Projekt_triggered();

    void on_actionProjekt_ffnen_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
