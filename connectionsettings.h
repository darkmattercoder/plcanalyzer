#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H
#include <QDialog>
#include <QRegExp>
#include <QSignalMapper>
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
    void SetSlots(QVector<ConSlot> currentSlots);

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
    QList<int> comboValuesArea;
    QList<int> comboValuesFormat;
    QList<QComboBox*> comboBoxesArea;
    QList<QComboBox*> comboBoxesFormat;
    QList<QLineEdit*> lineEditsBits;
    QList<QLineEdit*> lineEditsAddress;

    int findCorrespondingLine(QList<QComboBox*> boxesArea,QComboBox* sendingBox);
};

#endif // CONNECTIONSETTINGS_H
