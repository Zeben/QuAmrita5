#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "overhead.h"
#include <QDebug>
#include "catalog.h"
#include "editclient.h"
#include "addclient.h"
#include "settings.h"
#include <QSqlQueryModel>
#include <QSettings>
#include <QSqlDatabase>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlQueryModel *model;
    QSettings *appsets;
    addClient *addC;
    EditClient *editC;
    Catalog *cat;
    Overhead *pOH;

private:
    Ui::MainWindow *ui;
    Settings *sets;

private slots:
    void addSender();
    void editSender();
    void settings(); 
    void onPushDelUnitClick();
    void add();
    void edit();
    void catalog();
    void emitIDOfOrderer();
    void callOH();

public slots:
    void openDb();
    void setupCodec();
    void setupTable();

signals:
    void callOpenDB();
    void callCatalogSetup();
    void numStr(QString numberStr);
    void numSp(QString numberSp);
    void sendIDOFOR(QString IDOFOR, QString NAMEOFOR);

};

#endif // MAINWINDOW_H
