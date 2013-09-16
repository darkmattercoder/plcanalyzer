/*******************************************************************************
 *   *File: xmlsettingshandler.h                                               *
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

#ifndef XMLSETTINGSHANDLER_H
#define XMLSETTINGSHANDLER_H
#include <QMainWindow>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "s7connection.h"

class xmlSettingsHandler : public QMainWindow
{
    Q_OBJECT

public:

   explicit xmlSettingsHandler(QMainWindow *parent=0);
   ~xmlSettingsHandler();
    ConSets* openedConSets;

signals:

    void newSlotsOpened(QVector<ConSlot> openedConSlots);
    
public slots:

    void newProject();
    void saveProject(ConSets* currentConSets, QVector<ConSlot> currentConSlots,
                     bool lastSets);
    bool openProject(bool lastSets = false);
    void saveProjectAs();

private:

    QString filename;
    QXmlStreamReader xmlReader;
    QVector<ConSlot> openedConSlots;

};

#endif // XMLSETTINGSHANDLER_H
