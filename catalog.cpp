#include <iostream>
#include "catalog.h"
#include "ui_catalog.h"

Catalog::Catalog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Catalog)
{ 
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи

    Catalog::setup();

    ui->groupBoxOrderer->hide();

    /****/
    // коннекторы
    connect(ui->pushCatAdd, SIGNAL(clicked()), this, SLOT(catAddOpen())); // открыть диалог добавления товара
    connect(ui->pushCatEdit, SIGNAL(clicked()), this, SLOT(catAddEdit())); // редактировать выделенный товар
    connect(ui->pushCatEdit, SIGNAL(clicked()), this, SLOT(IDEmitter())); // по редактированию вызываем эмиттер
    // эмиттер отправляет ИД выделенного товара

    /****/
    connect(ui->pushCatDiscount, SIGNAL(clicked()), this, SLOT(discount())); // вызов окна скидки
    connect(ui->pushCatDiscount, SIGNAL(clicked()), this, SLOT(IDEmitter())); // и с ним - эмиттера

    /****/
    connect(ui->pushCatDelete, SIGNAL(clicked()), this, SLOT(deleteFrom())); // удаление выбранного товара

    /****/
    connect(ui->pushCatOrderHider, SIGNAL(clicked()), this, SLOT(orderer())); // обновить данные по дин. изм. окна

    connect(ui->pushAddToTrash, SIGNAL(clicked()), this, SLOT(addToTrash())); // добавляем выделенную строку в корзину
    /****/
    connect(ui->pushDelFromTrash, SIGNAL(clicked()), this, SLOT(delFromTrash()));
    /****/
    connect(ui->pushTrashApply, SIGNAL(clicked()), this, SLOT(saver()));

}

Catalog::~Catalog()
{
    delete ui;
}

void Catalog::setup()
{
    qDebug() << "catalog(), setup() init...";

    // устанавливаем стандартную кодировку
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);

    // создаем драйвер модели запроса в БД
    modelOH = new QSqlQueryModel(this);
    modelOH->setQuery("SELECT * FROM tovar");

    modelOH->setHeaderData(1,Qt::Horizontal, QObject::tr("Артикул"));
    modelOH->setHeaderData(2,Qt::Horizontal, QObject::tr("Назв. товара"));
    modelOH->setHeaderData(3,Qt::Horizontal, QObject::tr("Спец. цена (грн.)"));
    modelOH->setHeaderData(4,Qt::Horizontal, QObject::tr("Цена (грн.)"));
    modelOH->setHeaderData(5,Qt::Horizontal, QObject::tr("Балл"));
    modelOH->setHeaderData(6,Qt::Horizontal, QObject::tr("Ск-ка (%)"));
    modelOH->setHeaderData(7,Qt::Horizontal, QObject::tr("Со ск-кой"));

    // привязываем драйвер модели к виджету
    ui->tableCatView->setModel(modelOH);
    qDebug() << "tableCatPreparator()...";
    Catalog::tableCatPreparator();

    qDebug() << "catalog(), setup(), done.";
}

void Catalog::catAddOpen()
{
    catAdd = new catalogAdd(this);
    connect(catAdd, SIGNAL(updateCatNow()), this, SLOT(setup())); // обновить таблицу по применению
    catAdd->show();
}

void Catalog::catAddEdit()
{
    catEdit = new catalogEdit(this);
    connect(this, SIGNAL(ID(int)), catEdit, SLOT(catEditSetup(int))); // эмиттер отправляет сигнал, который ловится сторонней формой
    connect(catEdit, SIGNAL(updateCatNow()), this, SLOT(setup())); // обновить таблицу по применению
    catEdit->show();
}

void Catalog::discount()
{
    catDiscount = new catalogDiscount(this);
    connect(this, SIGNAL(ID(int)), catDiscount, SLOT(discountSetup(int))); // эмиттер отправляет сигнал, который ловится сторонней формой
    connect(catDiscount, SIGNAL(updateCatNow()), this, SLOT(setup())); // обновить таблицу по применению
    catDiscount->show();
}

void Catalog::deleteFrom()
{
    modelOH->setQuery("DELETE FROM tovar WHERE id = " + modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 0)).toString());
    qDebug() << "cat setup...";
    modelOH->setQuery("SELECT * FROM tovar");
    Catalog::tableCatPreparator();
    qDebug() << "DEL FROM tovar!";
}

void Catalog::IDEmitter() // собственно, эмиттер
{
    qDebug() << "call EMITTER()";
    emit ID(modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 0)).toInt());
}

void Catalog::orderer()
{
    qDebug() << ui->pushCatOrderHider->text();
    if(ui->pushCatOrderHider->text() == "Создать заказ >>")
    {
        // показываем виджет
        ui->groupBoxOrderer->show();

        // выставляем номер накладной

        queryFromOH.exec("SELECT * FROM nakladnie ORDER BY id DESC LIMIT 1");
        queryFromOH.next();
        ui->lineNumOfOverhead->setText(QString::number(queryFromOH.value(1).toInt() + 1));

        //создаем драйвер доступа к БД
        modelOrder = new QSqlQueryModel(this);
        modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina WHERE nomer = " + ui->lineNumOfAgent->text());
        //modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina");
        qDebug() << ui->lineNumOfAgent->text();

        modelOrder->setHeaderData(1,Qt::Horizontal, QObject::tr("Артикул"));
        modelOrder->setHeaderData(2,Qt::Horizontal, QObject::tr("Название"));
        modelOrder->setHeaderData(3,Qt::Horizontal, QObject::tr("Балл"));
        modelOrder->setHeaderData(4,Qt::Horizontal, QObject::tr("Цена"));
        modelOrder->setHeaderData(5,Qt::Horizontal, QObject::tr("Кол-во"));

        // привязываем драйвер модели к виджету
        ui->tableCatOrderer->setModel(modelOrder);
        qDebug() << "trash setup...";
        Catalog::tableTrashPreparator();

        // устанавливаем текущую дату
        ui->dateEdit->setDisplayFormat("dd.MM.yy");
        ui->dateEdit->setDate(QDate::currentDate());
        ui->pushCatOrderHider->setText("<< Закрыть корзину");

        // устанавливаем значения полей баллов и общей цены
        ui->lineNumOfOrders->setText("1");
        ui->lineAllCash->clear(); ui->lineAllPoints->clear();
        ui->lineAllCash->setText(Catalog::cashCalc());
        ui->lineAllPoints->setText(Catalog::pointCalc());
    }
    else if(ui->pushCatOrderHider->text() == "<< Закрыть корзину")
    {
        ui->groupBoxOrderer->hide();
        ui->pushCatOrderHider->setText("Создать заказ >>");
    };
}

void Catalog::setIDTitle(QString ORID, QString ORNAME)
{
    qDebug() << "setIDTitleForOverhead: ORID = " << ORID << ", ORNAME = " << ORNAME;
    ui->lineFIOOfAgent->setText(ORNAME);
    ui->lineNumOfAgent->setText(ORID);
}


void Catalog::tableCatPreparator()
{
    qDebug() << "tableCatPreparator()...";
    ui->tableCatView->resizeColumnsToContents();
    ui->tableCatView->resizeRowsToContents();
    ui->tableCatView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCatView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCatView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableCatView->setColumnHidden(0, true);
    ui->tableCatView->setColumnWidth(1, 52);
    ui->tableCatView->setColumnWidth(3, 100);
    ui->tableCatView->setColumnWidth(4, 70);
    ui->tableCatView->setColumnWidth(5, 38);
    ui->tableCatView->setColumnWidth(6, 64);
    ui->tableCatView->setColumnWidth(7, 64);
    // то же самое
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    ui->tableCatView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    // указываем свойства таблицы

}

void Catalog::tableTrashPreparator()
{
    qDebug() << "tableTrashPreparator()...";

    ui->tableCatOrderer->resizeColumnsToContents();
    ui->tableCatOrderer->resizeRowsToContents();
    ui->tableCatOrderer->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCatOrderer->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCatOrderer->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableCatOrderer->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);



}

void Catalog::addToTrash()
{
    qDebug() << "Выполняем основной запрос, добавляем в корзину...";
    QSqlQuery queryAddToTrash;
    queryAddToTrash.prepare("INSERT INTO korzina (name, art, desckr, ball, cena, kol, nomer, datep)"
                            "VALUES (:name, :art, :desckr, :ball, :cena, :kol, :nomer, :datep);");
    qDebug() << "query result: " <<  queryFromOH.value(0).toString();
    queryAddToTrash.bindValue(":name", QString::fromStdWString(ui->lineNumOfOverhead->text().toStdWString()));
    queryAddToTrash.bindValue(":art", modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 1)).toString());
    queryAddToTrash.bindValue(":desckr", modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 2)).toString());
    queryAddToTrash.bindValue(":ball", QString::number(modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 5)).toInt() *   ui->lineNumOfOrders->text().toInt())); // исправили ошибку дублирования данных из графы цены в графу баллов (Петрович, баг №2)
    qDebug() << "ball = " << QString::number(modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 5)).toInt() *   ui->lineNumOfOrders->text().toInt());

    queryAddToTrash.bindValue(":cena", QString::number(modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 7)).toFloat() * ui->lineNumOfOrders->text().toFloat()));
    qDebug() << "cena = " << QString::number(modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 7)).toFloat() * ui->lineNumOfOrders->text().toFloat());

    if(ui->lineNumOfOrders->text().toInt() == 0)
        ui->lineNumOfOrders->insert("1");

    queryAddToTrash.bindValue(":kol", QString::number(ui->lineNumOfOrders->text().toInt()));
    queryAddToTrash.bindValue(":nomer", QString::fromStdWString(ui->lineNumOfAgent->text().toStdWString()));
    queryAddToTrash.bindValue(":datep", QString::fromStdWString(ui->dateEdit->text().toStdWString()));
    queryAddToTrash.exec();
    qDebug() << queryAddToTrash.lastError().text();
    qDebug() << "Обновление данных...";
    //modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina");
    modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina WHERE nomer = " + ui->lineNumOfAgent->text());

    // придется заново настраивать параметры таблицы
    qDebug() << "trash setup...";
    Catalog::tableTrashPreparator();

    // вычисление количества баллов и цены в формы
    ui->lineAllCash->clear(); ui->lineAllPoints->clear();
    ui->lineAllCash->setText(Catalog::cashCalc());
    ui->lineAllPoints->setText(Catalog::pointCalc());

    qDebug() << "str = " << modelOH->data(modelOH->index(ui->tableCatView->currentIndex().row(), 1)).toString();

}

void Catalog::delFromTrash()
{
    qDebug() << "del from trash()...";
    modelOrder->setQuery("DELETE FROM korzina WHERE id = " + modelOrder->data(modelOrder->index(ui->tableCatOrderer->currentIndex().row(), 0)).toString());
    qDebug() << "trash setup...";
    //modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina");
    modelOrder->setQuery("SELECT id, art, desckr, ball, cena, kol FROM korzina WHERE nomer = " + ui->lineNumOfAgent->text());

    ui->lineAllCash->setText(Catalog::cashCalc());
    ui->lineAllPoints->setText(Catalog::pointCalc());

    Catalog::tableTrashPreparator();

}

QString Catalog::pointCalc()
{
    int points = 0;
    for(int iii = 0; iii < modelOrder->rowCount(); iii++)
    {
        points = points + modelOrder->data(modelOrder->index(iii, 3)).toInt();
    }
    qDebug() << "Catalog::pointCalc()" << points;
    return QString::number(points);
}

QString Catalog::cashCalc()
{
    float cash = 0;
    for(int iii = 0; iii < modelOrder->rowCount(); iii++)
    {
        cash = cash + modelOrder->data(modelOrder->index(iii, 4)).toFloat();
    }
    qDebug() << "Catalog::cashCalc()";
    return QString::number(cash);
}

void Catalog::saver()
{
    qDebug() << "saver()...";
    QSqlQuery queryReplacer1;
    queryReplacer1.prepare("INSERT INTO nakladnie (name, art, desckr, ball, cena, kol, nomer, datep)"
                          "                SELECT name, art, desckr, ball, cena, kol, nomer, datep FROM korzina WHERE name = :name");
    queryReplacer1.bindValue(":name", QString::fromStdWString(ui->lineNumOfOverhead->text().toStdWString()));
    queryReplacer1.exec();

    QSqlQuery queryReplacer2;
    queryReplacer2.prepare("INSERT INTO nomnakl (name, date, nomer, agent)"
                           "VALUES ( :name, :date, :nomer, :agent);");
    queryReplacer2.bindValue(":name", QString::fromStdWString(ui->lineNumOfOverhead->text().toStdWString()));
    queryReplacer2.bindValue(":date", QString::fromStdWString(ui->dateEdit->text().toStdWString()));
    queryReplacer2.bindValue(":nomer", QString::fromStdWString(ui->lineNumOfAgent->text().toStdWString()));
    queryReplacer2.bindValue(":agent", QString::fromStdWString(ui->lineFIOOfAgent->text().toStdWString()));
    queryReplacer2.exec();

    modelOrder->setQuery("DELETE FROM korzina WHERE name = " + QString::fromStdWString(ui->lineNumOfOverhead->text().toStdWString())); // удаляем все данные из корзины по сохранению накладных (Петрович, баг №1)

    this->close();
}
