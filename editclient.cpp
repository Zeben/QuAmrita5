#include "editclient.h"
#include "ui_editclient.h"

EditClient::EditClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditClient)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи
    connect(ui->pushSetCurrentDate, SIGNAL(clicked()), this, SLOT(setCurrentDate())); // коннектор "кнопка - слот"
    connect(ui->pushApply, SIGNAL(clicked()), this, SLOT(apply()));
}

EditClient::~EditClient()
{
    delete ui;
}

void EditClient::setCurrentDate()   // слот для установки текущей даты в виджете
{
    qDebug() << "setCurrentDate()";
    ui->dateEdit->setDisplayFormat("dd.MM.yy");
    ui->dateEdit->setDate(QDate::currentDate());
}

void EditClient::getEditInformation(QString stred)
{
    qDebug() << "EDIT, stred == " << stred;

    // очищаем поля редактирования
    ui->lineEditFIOp->clear();
    ui->lineEditOrderNumber->clear();

    // драйвер запроса данных из БД
    QSqlQuery queryEdit;

    queryEdit.prepare("SELECT fam,ioch,nomer FROM struktura WHERE nomer = :numsp");
    queryEdit.bindValue(":numsp", stred);
    queryEdit.exec();
    queryEdit.next();
    qDebug() << "queryEdit.lastError() == " << queryEdit.lastError();

    // заполняем данные о спонсоре
    ui->lineEditFIOp->insert(queryEdit.value(0).toString() + " " + queryEdit.value(1).toString());
    ui->lineEditOrderNumber->insert(queryEdit.value(2).toString());

    globalSpID = stred; // заносим номер спонсора в глобальную видимость
}

void EditClient::getEditInformation2(QString stred2)
{
    qDebug() << "EDIT__2, stred2 == " << stred2;

    // очистка полей редактирования данных об агенте
    ui->lineEditAddress->clear();
    ui->lineEditMail->clear();
    ui->lineEditNameAndOtch->clear();
    ui->lineEditNumber->clear();
    ui->lineEditPassportData->clear();
    ui->lineEditPhone->clear();
    ui->lineEditSurname->clear();

    // создаем драйвер для доступа к БД
    QSqlQuery queryEdit2;
    queryEdit2.prepare("SELECT fam,ioch,nomer,date,passp,address,spid,mail,telefon FROM struktura WHERE id = :id");
    queryEdit2.bindValue(":id", stred2);
    queryEdit2.exec();
    queryEdit2.next();
    qDebug() << "queryEdit2.lastError() == " << queryEdit2.lastError().text();

    // вставляем данные о выбранном агенте в поля
    ui->lineEditNumber->insert(queryEdit2.value(2).toString());
    ui->lineEditSurname->insert(queryEdit2.value(0).toString());
    ui->lineEditNameAndOtch->insert(queryEdit2.value(1).toString());
    ui->lineEditPassportData->insert(queryEdit2.value(4).toString());
    ui->lineEditAddress->insert(queryEdit2.value(5).toString());
    ui->lineEditPhone->insert(queryEdit2.value(7).toString());
    ui->lineEditMail->insert(queryEdit2.value(8).toString());

    globalID = stred2; // номер агента в глобальную видимость
}

void EditClient::apply()
{
    qDebug() << "apply()...";
    QSqlQuery queryApply;
    queryApply.prepare("UPDATE struktura SET fam = :fam, ioch = :ioch, nomer = :nomer, date = :date, passp = :passp, address = :address, mail = :mail, telefon = :telefon WHERE id = :id");
    queryApply.bindValue(":fam", QString::fromStdWString(ui->lineEditSurname->text().toStdWString()));
    queryApply.bindValue(":ioch", QString::fromStdWString(ui->lineEditNameAndOtch->text().toStdWString()));
    queryApply.bindValue(":nomer", QString::fromStdWString(ui->lineEditNumber->text().toStdWString()));
    queryApply.bindValue(":date", QString::fromStdWString(ui->dateEdit->text().toStdWString()));
    queryApply.bindValue(":passp", QString::fromStdWString(ui->lineEditPassportData->text().toStdWString()));
    queryApply.bindValue(":address", QString::fromStdWString(ui->lineEditAddress->text().toStdWString()));
    queryApply.bindValue(":spid", globalSpID);
    queryApply.bindValue(":mail", QString::fromStdWString(ui->lineEditMail->text().toStdWString()));
    queryApply.bindValue(":telefon", QString::fromStdWString(ui->lineEditPhone->text().toStdWString()));
    queryApply.bindValue(":id", globalID);

    queryApply.exec();  // выполняем запрос на изменение данных
    qDebug() << "done.";
    emit updateTableNOW();// сигнал обязательного обновления таблицы
    this->close(); // закрываем окно редактирования по применению
}
