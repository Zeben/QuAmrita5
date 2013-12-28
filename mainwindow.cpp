#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    // INITS:

    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settings())); // коннектор настроек
    /****/
    connect(ui->pushAddUnit, SIGNAL(clicked()), this, SLOT(add())); // откр. диалог добавления
    connect(ui->pushAddUnit, SIGNAL(clicked()), this, SLOT(addSender())); // отправление № строки
    /****/
    connect(ui->pushEditUnit, SIGNAL(clicked()), this, SLOT(edit())); // откр. диалог редактирования
    connect(ui->pushEditUnit, SIGNAL(clicked()), this, SLOT(addSender()));
    connect(ui->pushEditUnit, SIGNAL(clicked()), this, SLOT(editSender())); // отпр. № агента
    /****/
    connect(ui->pushDeleteUnit, SIGNAL(clicked()), this, SLOT(onPushDelUnitClick())); // удаление юнита
    /****/
    connect(ui->actionGoodsCatalog, SIGNAL(triggered()), this, SLOT(catalog())); // форма каталога товаров
    connect(ui->actionGoodsCatalog, SIGNAL(triggered()), this, SLOT(emitIDOfOrderer())); // вызываем слот, отпр. сигнал
    /****/
    connect(ui->actionOverheads, SIGNAL(triggered()), this, SLOT(callOH())); // вызов формы накладной
    /****/
    connect(ui->pushCreateOrder, SIGNAL(clicked()), this, SLOT(catalog()));
    connect(ui->pushCreateOrder, SIGNAL(clicked()), this, SLOT(emitIDOfOrderer())); // вызываем слот, отпр. сигнал
                                                                                      // коннектор исправляет ошибку, когда по нажатии "создать заказ" не передавались данные о дистрибьюторе
    // Петрович, баг №3 (см. выше)
    // Исправляет также баг №4


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::settings()
{
    sets = new Settings(this);
    sets->show();
}

void MainWindow::openDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    appsets = new QSettings("conf.ini", QSettings::IniFormat);
    db.setDatabaseName(appsets->value("main/db").toString());
    if(!db.open())
    {
        qDebug("Error opening SQLITE DB");
    }
}

void MainWindow::setupCodec()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);
}

void MainWindow::setupTable()
{
    qDebug() << "init...";
    MainWindow::setupCodec();
    model = new QSqlQueryModel(this);
    model->setQuery("SELECT * FROM struktura");
    qDebug("main(), setup(), select done");
    model->setHeaderData(1,Qt::Horizontal, QObject::tr("Фамилия"));
    model->setHeaderData(2,Qt::Horizontal, QObject::tr("Имя Отчество"));
    model->setHeaderData(3,Qt::Horizontal, QObject::tr("Номер"));
    model->setHeaderData(4,Qt::Horizontal, QObject::tr("Дата"));
    model->setHeaderData(5,Qt::Horizontal, QObject::tr("Номер паспорта"));
    model->setHeaderData(6,Qt::Horizontal, QObject::tr("Адрес"));
    model->setHeaderData(7,Qt::Horizontal, QObject::tr("Номер спонсора"));
    model->setHeaderData(8,Qt::Horizontal, QObject::tr("Глубина"));
    model->setHeaderData(9,Qt::Horizontal, QObject::tr("E-mail"));
    model->setHeaderData(10,Qt::Horizontal, QObject::tr("Номер телефона"));
    ui->tableViewModel->setModel(model);
    ui->tableViewModel->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewModel->resizeColumnsToContents();
    ui->tableViewModel->resizeRowsToContents();

    ui->tableViewModel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->tableViewModel->setSelectionBehavior(QAbstractItemView::SelectRows);
    qDebug("main(), setup(), setup done");
}

void MainWindow::onPushDelUnitClick()
{
    model->setQuery("DELETE FROM struktura WHERE id = " + model->data(model->index(ui->tableViewModel->currentIndex().row(), 0)).toString());
    MainWindow::setupTable();
    qDebug() << "delete FROM DB!";
}

void MainWindow::addSender()
{
    QString str = model->data(model->index(ui->tableViewModel->currentIndex().row(), 0)).toString();
    qDebug() << "addSender()... SpNumber == " << str;
    emit numStr(str); // отправляем НОМЕР СТРОКИ по выделенной строке
}

void MainWindow::editSender()
{
    QString str = model->data(model->index(ui->tableViewModel->currentIndex().row(), 7)).toString();
    qDebug() << "editSender(), signal == " << str;
    emit numSp(str); // отправляем НОМЕР СПОНСОРА по выделенной строке
}

void MainWindow::emitIDOfOrderer()
{
    emit sendIDOFOR(
                    model->data(model->index(ui->tableViewModel->currentIndex().row(), 3)).toString(),
                    model->data(model->index(ui->tableViewModel->currentIndex().row(), 1)).toString()
                + " " +
                    model->data(model->index(ui->tableViewModel->currentIndex().row(), 2)).toString()
                    );
}

void MainWindow::add()
{
    qDebug() << "add()...";
    addC = new addClient(this);
    connect(this, SIGNAL(numStr(QString)), addC, SLOT(getIndexAndFIO(QString))); // вызов слота с другой формы
    connect(addC, SIGNAL(updateTableNOW()), this, SLOT(setupTable()));
    addC->show();
}

void MainWindow::edit()
{
    qDebug() << "edit()...";
    editC = new EditClient(this);
    connect(this, SIGNAL(numStr(QString)), editC, SLOT(getEditInformation2(QString)));
    connect(this, SIGNAL(numSp(QString)), editC, SLOT(getEditInformation(QString))); // отправляем № агента в другую форму
    connect(editC, SIGNAL(updateTableNOW()), this, SLOT(setupTable()));
    editC->show();
}

void MainWindow::callOH()
{
    qDebug() << "call OH()...";
    pOH = new Overhead(this);
    pOH->show();
}

void MainWindow::catalog()
{   emit callCatalogSetup();
    qDebug() << "call catalog()...";
    cat = new Catalog(this);
    connect(this, SIGNAL(sendIDOFOR(QString, QString)), cat, SLOT(setIDTitle(QString, QString))); // задаем ID заказчика
    //connect(ui->pushAddUnit, SIGNAL(clicked()), cat, SLOT(setup()));
    // ошибка: мы получаем сигнал до того
    // как создаем коннектор
    cat->show();
}
