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

signals: void newSlotsOpened(QVector<ConSlot> openedConSlots);

    
public slots:

     void newProject();
     void saveProject(ConSets* currentConSets, QVector<ConSlot> currentConSlots);
     void openProject();
     void saveProjectAs();
private:
QString filename;
QXmlStreamReader xmlReader;

QVector<ConSlot> openedConSlots;

};

#endif // XMLSETTINGSHANDLER_H
