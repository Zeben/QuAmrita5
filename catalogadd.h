#ifndef CATALOGADD_H
#define CATALOGADD_H

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDialog>

namespace Ui {
class catalogAdd;
}

class catalogAdd : public QDialog
{
    Q_OBJECT
    
public:
    explicit catalogAdd(QWidget *parent = 0);
    ~catalogAdd();
    
private:
    Ui::catalogAdd *ui;

private slots:
    void catalogSetup();
    void apply();

signals:
    void updateCatNow();
};

#endif // CATALOGADD_H
