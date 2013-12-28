#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSqlQuery>
#include <QDebug>
#include <QSqlDatabase>
#include <QTextCodec>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    
private:
    Ui::Settings *ui;
    QSettings *appsets;

private slots:
    void GETDB();

    void CreateDB();

};

#endif // SETTINGS_H
