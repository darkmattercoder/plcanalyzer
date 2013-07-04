#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H
#include <QDialog>
#include <QRegExp>
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
    void SetSettings(ConSets CurrentSets);
    
private slots:
    void on_buttonBox_accepted();

    void on_ComboBox_Protokoll_currentIndexChanged(int index);

signals:
    void SettingsChanged(ConSets DiagSets);

private:
    Ui::ConnectionSettings *ui;
    QStringList szListConnection;
    ConSets m_DiagSets;
};

#endif // CONNECTIONSETTINGS_H
