#include "catalogedit.h"
#include "ui_catalogedit.h"

catalogEdit::catalogEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::catalogEdit)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи
    connect(ui->pushCatAddApply, SIGNAL(clicked()), this, SLOT(apply()));
}

catalogEdit::~catalogEdit()
{
    delete ui;
}

void catalogEdit::catEditSetup(int catID)
{
    // драйвер БД, проброс ИД товара, вставка данных в форму
    qDebug() << "main(), catalog(), edit(), setup() init...";
    qDebug() << "ID = " << catID;
    QSqlQuery queryCatEdit;
    queryCatEdit.prepare("SELECT * FROM tovar WHERE id = :id");
    queryCatEdit.bindValue(":id", catID);
    queryCatEdit.exec();
    queryCatEdit.next();

    // очищаем поля редактирования
    ui->lineArt->clear();
    ui->linePoints->clear();
    ui->linePrice->clear();
    ui->lineProduct->clear();
    ui->lineSpecPrice->clear();

    // заполняем их данными по ИД
    ui->lineArt->insert(queryCatEdit.value(1).toString());
    ui->linePoints->insert(queryCatEdit.value(5).toString());
    ui->linePrice->insert(queryCatEdit.value(4).toString());
    ui->lineProduct->insert(queryCatEdit.value(2).toString());
    ui->lineSpecPrice->insert(queryCatEdit.value(3).toString());

    // глобализируем ИД товара и данные о скидке
    globalCatID = catID;
    globalDiscount = queryCatEdit.value(6).toInt();
}

void catalogEdit::apply()
{
    qDebug() << "main(), catalog(), edit(), apply() init...";
    QSqlQuery katapp;
    katapp.prepare("UPDATE tovar SET art = :art, desckr = :desckr, cenroz = :cenroz, cend = :cend, bal = :bal, cenawskidka = :cenawskidka WHERE id = :id");
    qDebug() << "id = " << globalCatID;
    katapp.bindValue(":art", QString::fromStdWString(ui->lineArt->text().toStdWString()));
    katapp.bindValue(":desckr", QString::fromStdWString(ui->lineProduct->text().toStdWString()));
    katapp.bindValue(":cenroz", QString::fromStdWString(ui->linePrice->text().toStdWString()));
    katapp.bindValue(":cend", QString::fromStdWString(ui->lineSpecPrice->text().toStdWString()));
    katapp.bindValue(":bal", QString::fromStdWString(ui->linePoints->text().toStdWString()));
    katapp.bindValue(":cenawskidka", QString::number(( 1 - static_cast<float>(globalDiscount) / 100 ) * ui->lineSpecPrice->text().toFloat()));
    qDebug() << "skidka = " << globalDiscount;
    qDebug() << "numberOfSk = " << QString::number(( 1 - static_cast<float>(globalDiscount) / 100 ) * ui->lineSpecPrice->text().toFloat());
    katapp.bindValue(":id", globalCatID);
    katapp.exec();
    qDebug() << katapp.lastError().text();
    emit updateCatNow();
    this->close();
}
