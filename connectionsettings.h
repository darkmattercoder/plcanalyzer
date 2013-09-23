/*******************************************************************************
 *   *File: connectionsettings.h                                               *
 *   *Date: 2013-09-04                                                        *
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

#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H
#include <QDialog>
#include <QRegExp>
#include <QSignalMapper>
#include <QIntValidator>
#include "ui_connectionsettings.h"
#include "s7connection.h"

namespace Ui {
class ConnectionSettings;
}

class ConnectionSettings : public QDialog
{
    Q_OBJECT
    
public:

    explicit ConnectionSettings(QWidget *parent = 0);
    ~ConnectionSettings();
    void SetSettings(ConSets* CurrentSets);
    void SetSlots(QVector<ConSlot> &currentSlots);
    ConSets* m_DiagSets;
    QVector<ConSlot> newSlots;
    
private slots:

    void on_buttonBox_accepted();
    void on_ComboBox_Protokoll_currentIndexChanged(int index);
    void comboBoxIndexChanged(int index);

signals:

    void SettingsChanged(ConSets* DiagSets);
    void SlotsChanged(QVector<ConSlot> newConSlots);

private:

    Ui::ConnectionSettings *ui;
    QStringList szListConnection;
    QStringList comboItemsArea;
    QStringList comboItemsFormat;
    QStringList comboItemsLength;
        QList<int> comboValuesArea;
    QList<int> comboValuesFormat;
    QList<int> comboValuesLength;
    QList<QComboBox*> comboBoxesArea;
    QList<QComboBox*> comboBoxesFormat;
    QList<QComboBox*> comboBoxesLength;
    QList<QLineEdit*> lineEditsBits;
    QList<QLineEdit*> lineEditsAddress;
    QList<QLineEdit*> lineEditsDB;
    int findCorrespondingLine(QList<QComboBox*> boxesArea,
                              QComboBox* sendingBox);

    //Compare funqtions for the qsort algorithm
    static bool comboBoxPointerLessThan(QComboBox* cb1, QComboBox* cb2);
    static bool lineEditPointerLessThan(QLineEdit* le1, QLineEdit* le2);

    //Graph colors as a "resorted" int-vector of Qt::GlobalColors
    QVector<QColor> graphColors;
//    Qt::GlobalColor /*graphColors[8]*/;
    //Modulo value for dynamic graphColor generation
    int moduloValue;

    QIntValidator* myBitValidator;
    QIntValidator* myByteAddressValidator;

    //functions for selecting the selectable items
    void Enabler (int iLinenumber, bool bEnable, int iIndex);
    void EnableSwticher(int iDataitem, int iLinenumber);

};

#endif // CONNECTIONSETTINGS_H
