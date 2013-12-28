#ifndef EDITCLIENT_H
#define EDITCLIENT_H

#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDialog>

namespace Ui {
class EditClient;
}

class EditClient : public QDialog
{
    Q_OBJECT
    
public:
    explicit EditClient(QWidget *parent = 0);
    ~EditClient();
    QString globalSpID, globalID;
    
private:
    Ui::EditClient *ui;

public slots:
    void getEditInformation(QString stred);
    void getEditInformation2(QString stred2);

private slots:
    void setCurrentDate();
    void apply();

signals:
    void updateTableNOW();
};

#endif // EDITCLIENT_H
