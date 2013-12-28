#ifndef CATALOGDISCOUNT_H
#define CATALOGDISCOUNT_H

#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QDialog>

namespace Ui {
class catalogDiscount;
}

class catalogDiscount : public QDialog
{
    Q_OBJECT
    
public:
    explicit catalogDiscount(QWidget *parent = 0);
    ~catalogDiscount();
    int globalDisID;
    
private:
    Ui::catalogDiscount *ui;

public slots:
    void discountSetup(int disID);

private slots:
    void counter();
    void apply();

signals:
    void updateCatNow();
};

#endif // CATALOGDISCOUNT_H
