#ifndef ADDCLIENT_H
#define ADDCLIENT_H

#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QDialog>

namespace Ui {
class addClient;
}

class addClient : public QDialog
{
    Q_OBJECT
    
public:
    explicit addClient(QWidget *parent = 0);
    ~addClient();
    QString globalIdOfSponsor;
    int globalDeepEmbedding;
    
private:
    Ui::addClient *ui;

public slots:
    void getIndexAndFIO(QString str);
    void addApply();

signals:
    void updateTableNOW();
};

#endif // ADDCLIENT_H
