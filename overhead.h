#ifndef OVERHEAD_H
#define OVERHEAD_H

#include <QTextDocument>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QDebug>
#include <QDialog>

namespace Ui {
class Overhead;
}

class Overhead : public QDialog
{
    Q_OBJECT
    
public:
    explicit Overhead(QWidget *parent = 0);
    ~Overhead();
    QSqlQueryModel *modelOHQuery, *modelOHCellingQuery, *modelOHGetHack;
    QString cashCalc();
    QString pointCalc();
    QString cashCalcPercent();
    
private slots:
    void on_tableOH_clicked();
    void setupOH();
    void EditParam();
    void apply();
    void del();
    void on_tableOHSelling_clicked();

    void cell();
    void edit_hack();

    void isReturnedYes();
    void isReturnedNo();
    void duty();

    void printDialog();
    void printDriver(QPrinter *p);

private:
    Ui::Overhead *ui;
    void UISetup();
    void UICellSetup();
    void UIGetHackSetup();
};

#endif // OVERHEAD_H
