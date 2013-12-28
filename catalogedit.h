#ifndef CATALOGEDIT_H
#define CATALOGEDIT_H

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDialog>

namespace Ui {
class catalogEdit;
}

class catalogEdit : public QDialog
{
    Q_OBJECT
    
public:
    explicit catalogEdit(QWidget *parent = 0);
    ~catalogEdit();
    int globalCatID, globalDiscount;
    
private:
    Ui::catalogEdit *ui;

public slots:
    void catEditSetup(int catID);

private slots:
    void apply();

signals:
    void updateCatNow();
};

#endif // CATALOGEDIT_H
