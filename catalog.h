#ifndef CATALOG_H
#define CATALOG_H

#include <QStandardItemModel>
#include "catalogdiscount.h"
#include "catalogedit.h"
#include "catalogadd.h"
#include <QSqlError>
#include <QDebug>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QDialog>

namespace Ui {
class Catalog;
}

class Catalog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Catalog(QWidget *parent = 0);
    ~Catalog();
    catalogAdd *catAdd;
    catalogEdit *catEdit;
    catalogDiscount *catDiscount;
    QSqlQueryModel *modelOH, *modelOrder;
    QSqlQuery queryFromOH;

private:
    Ui::Catalog *ui;

private slots:
    void setup();
    void catAddOpen();
    void catAddEdit();
    void IDEmitter();
    void discount();
    void deleteFrom();
    void orderer();
    void addToTrash();
    QString pointCalc();
    QString cashCalc();
    void delFromTrash();
    void tableCatPreparator();
    void tableTrashPreparator();
    void saver();

signals:
    void ID(int);

public slots:
    void setIDTitle(QString ORID, QString ORNAME);


};

#endif // CATALOG_H
