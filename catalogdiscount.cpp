#include "catalogdiscount.h"
#include "ui_catalogdiscount.h"

catalogDiscount::catalogDiscount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::catalogDiscount)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи
    // inits:
    connect(ui->lineDiscount, SIGNAL(textEdited(QString)), this, SLOT(counter()));
    connect(ui->pushApply, SIGNAL(clicked()), this, SLOT(apply()));

}

catalogDiscount::~catalogDiscount()
{
    delete ui;
}

void catalogDiscount::discountSetup(int disID)
{
    qDebug() << "main(), catalog(), discount(), setup() init...";
    // очищаем поля
    ui->lineArt->clear();
    ui->lineDiscount->clear();
    ui->lineProduct->clear();
    ui->lineSpecPrice->clear();
    ui->lineWithDiscount->clear();

    // глобализируем ИД товара
    globalDisID = disID;
    qDebug() << "disID = " << globalDisID;

    // инициализируем драйвер БД
    QSqlQuery queryDisc;
    queryDisc.prepare("SELECT desckr,art,cend,skidka,cenawskidka FROM tovar where id = :id");
    queryDisc.bindValue(":id", globalDisID);
    queryDisc.exec();
    queryDisc.next();

    // заполняем поля
    ui->lineProduct->insert(queryDisc.value(0).toString());
    ui->lineArt->insert(queryDisc.value(1).toString());
    ui->lineSpecPrice->insert(queryDisc.value(2).toString());
    ui->lineDiscount->insert(queryDisc.value(3).toString());
    ui->lineWithDiscount->insert(queryDisc.value(4).toString());
    catalogDiscount::counter();
}

void catalogDiscount::counter()
{
    ui->lineWithDiscount->clear();
    if(ui->lineDiscount->text().toFloat() == 0)
        ui->lineWithDiscount->insert(ui->lineSpecPrice->text());
    else
        ui->lineWithDiscount->insert(QString::number((1 - ui->lineDiscount->text().toFloat() / 100) * ui->lineSpecPrice->text().toFloat()));
    qDebug() << "percentCounter = " << ui->lineDiscount->text().toFloat();
}

void catalogDiscount::apply()
{
    qDebug() << "main(), catalog(), discount(), apply() init...";
    // создаем драйвер доступа к БД
    QSqlQuery queryapp;
    queryapp.prepare("UPDATE tovar SET skidka = :skidka, cenawskidka = :cenawskidka WHERE id = :id");
    queryapp.bindValue(":skidka", QString::fromStdWString(ui->lineDiscount->text().toStdWString()));
    queryapp.bindValue(":cenawskidka", QString::fromStdWString(ui->lineWithDiscount->text().toStdWString()));
    queryapp.bindValue(":id", globalDisID);
    queryapp.exec();
    this->close();
    emit updateCatNow();
}
