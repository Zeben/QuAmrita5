#include "catalogadd.h"
#include "ui_catalogadd.h"

catalogAdd::catalogAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::catalogAdd)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи
    catalogAdd::catalogSetup();
    // connectors:
    connect(ui->pushCatAddApply, SIGNAL(clicked()), this, SLOT(apply()));
    ui->linePoints->setValidator(new QDoubleValidator(0));
    ui->linePrice->setValidator(new QDoubleValidator(0));
    ui->lineSpecPrice->setValidator(new QDoubleValidator(0));
    ui->lineArt->setValidator(new QDoubleValidator(0));
}

catalogAdd::~catalogAdd()
{
    delete ui;
}

void catalogAdd::catalogSetup()
{
    qDebug() << "main(), catalog(), add(), setup() init...";
    // очищаем поля (если нужно, а так - не очищаем)
    QSqlQuery queryCatAdd;
    // запрос на ИД последней записи в БД
    queryCatAdd.exec("SELECT id FROM tovar ORDER BY id DESC LIMIT 1");
    queryCatAdd.next();
    qDebug() << queryCatAdd.lastError().text();
    qDebug() << "id = " << queryCatAdd.value(0).toString();
    // прибавляем 1 к номеру записи
    ui->lineArt->clear();
    ui->lineArt->insert(QString::number(queryCatAdd.value(0).toInt() + 1));
}

void catalogAdd::apply()
{
    qDebug() << "main(), catalog(), add(), apply() init...";
    QSqlQuery queryCatApply;
    queryCatApply.prepare("INSERT INTO tovar (art,desckr,cend,cenroz,bal,cenawskidka)"
                   "VALUES (:art, :desckr, :cend, :cenroz, :bal, :cenawskidka);");
    queryCatApply.bindValue(":art", QString::fromStdWString(ui->lineArt->text().toStdWString()));
    queryCatApply.bindValue(":desckr", QString::fromStdWString(ui->lineProduct->text().toStdWString()));
    queryCatApply.bindValue(":cenroz", QString::fromStdWString(ui->linePrice->text().toStdWString()));
    queryCatApply.bindValue(":cend", QString::fromStdWString(ui->lineSpecPrice->text().toStdWString()));
    queryCatApply.bindValue(":bal", QString::fromStdWString(ui->linePoints->text().toStdWString()));
    queryCatApply.bindValue(":cenawskidka", QString::fromStdWString(ui->lineSpecPrice->text().toStdWString()));
    queryCatApply.exec();
    emit updateCatNow();
    catalogAdd::catalogSetup();
}
