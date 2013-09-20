/*******************************************************************************
 *   *File: mainwindow.h                                                       *
 *   *Date: 2013-06-01                                                         *
 *   *Author(s): Jochen Bauer <devel@jochenbauer.net>                          *
 *               Lukas Kern <lukas.kern@online.de>                             *
 *               Carsten Klein <hook-the-master@gmx.net>                       *
 *                                                                             *
 *   *License information:                                                     *
 *                                                                             *
 *   Copyright (C) [2013] [Jochen Bauer, Lukas Kern, Carsten Klein]            *
 *                                                                             *
 *   This file is part of PLCANALYZER. PLCANALYZER is free software; you can   *
 *   redistribute it and/or modify it under the terms of the GNU General       *
 *   Public License as published by the Free Software Foundation; either       *
 *   version 2 of the License, or (at your option) any later version.          *
 *                                                                             *
 *   This program is distributed in the hope that it will be useful, but       *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program; if not, write to the Free Software               *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA             *
 *   02110-1301, USA.                                                          *
 *                                                                             *
 *   *Program name: PLCANALYZER                                                *
 *   *What this program does:    Connects to most SIMATIC S7/S5 Controllers    *
 *                               * Reads memory areas                          *
 *                               * Draws a graph over time for operands        *
 *   *Have fun!                                                                *
 ******************************************************************************/

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
#include <QColor>
#include "filestream.h"


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
    QVector<ConSlot> MySlot;

private slots:

    void on_Button_Connect_clicked();
    // uncommented due to obsolescence
    // void on_Button_Get_Val_clicked();
    void ChangeSettings(ConSets* NewConSets);
    void TimeOut();
    void on_pushButton_ConSets_clicked();
    void on_Button_read_slots_clicked();
    void changeSlots(QVector<ConSlot> newConSlots);
    void on_pushButton_clicked();

    //Menu entries
    void on_actionNewProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionSaveProject_triggered();

signals:

private:

    int numberOfSlots;
    int recordings;
    int amountOfPoints;
    QVector<double> x;          // 1d Vector
    QVector<QVector<double>> y; // 2d Vectot

    xmlSettingsHandler* xmlSettings;
    QTextEdit* debugText;

    //Lists for the OperandValues
    QList<QLabel*> labelsOperand;
    QList<QLineEdit*> lineEditsOperandValue;

    //Compare funqtions for the qsort algorithm
    static bool labelPointerLessThan(QLabel* label1, QLabel* label2);
    static bool lineEditPointerLessThan(QLineEdit* le1, QLineEdit* le2);

    //
    BinWriter myWriter;
};

#endif // MAINWINDOW_H
