#include "addclient.h"
#include "ui_addclient.h"

addClient::addClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addClient)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );
    connect(ui->pushAddSend, SIGNAL(clicked()), this, SLOT(addApply()));
}

addClient::~addClient()
{
    delete ui;
}

void addClient::getIndexAndFIO(QString str)
{
    qDebug() << "CALL getIndexAndFIO(QString str)... STR = " << str;
    // очистка всех полей
    ui->lineAddAddress->clear();
    ui->lineAddFIO->clear();
    ui->lineAddIdOfSponsor->clear();
    ui->lineAddMail->clear();
    ui->lineAddNameAndOtch->clear();
    ui->lineAddNumber->clear();
    ui->lineAddPassportData->clear();
    ui->lineAddPhone->clear();
    ui->lineAddSurname->clear();

    //QDate date = QDate::currentDate();
    //QString dateString = date.toString();

    ui->dateEdit->setDisplayFormat("dd.MM.yy");
    ui->dateEdit->setDate(QDate::currentDate());

    // Создание ф-ции БД и запрос по STR
    QSqlQuery queryAdd;
    queryAdd.prepare("SELECT fam,ioch,nomer,glubina FROM struktura WHERE id = :id");
    queryAdd.bindValue(":id", str);
    queryAdd.exec();
    queryAdd.next();
    qDebug() << queryAdd.lastError().text();

    // вставка номера спонсора и ФИО в нередактируемые поля
    ui->lineAddIdOfSponsor->insert(queryAdd.value(2).toString());
    ui->lineAddFIO->insert(queryAdd.value(0).toString() + " " + queryAdd.value(1).toString());

    // кэширование ИД спонсора и глубины вложения
    globalIdOfSponsor = queryAdd.value(2).toString();
    globalDeepEmbedding = queryAdd.value(3).toInt();
}
void addClient::addApply()
{
    // вставляем данные со строк в запрос. Строки вставляются в качестве аргументов.
    QSqlQuery query;
    qDebug() << "init...";
    query.prepare("INSERT INTO struktura (fam, ioch, nomer, date, passp, address, spid, glubina, mail, telefon)"
                  "VALUES (:fam, :ioch, :nomer, :date, :passp, :address, :spid, :glubina, :mail, :telefon);");
    query.bindValue(":fam", QString::fromStdWString(ui->lineAddSurname->text().toStdWString())); // фамилия
    query.bindValue(":ioch", QString::fromStdWString(ui->lineAddNameAndOtch->text().toStdWString())); // имя очество
    query.bindValue(":nomer", QString::fromStdWString(ui->lineAddNumber->text().toStdWString())); // личный номер
    query.bindValue(":date", QString::fromStdWString(ui->dateEdit->text().toStdWString())); // дата
    query.bindValue(":passp", QString::fromStdWString(ui->lineAddPassportData->text().toStdWString())); // номер паспорта
    query.bindValue(":address", QString::fromStdWString(ui->lineAddAddress->text().toStdWString())); // адрес
    query.bindValue(":spid", globalIdOfSponsor);   // номер спонсора
    query.bindValue(":glubina", QString::number(globalDeepEmbedding + 1)); // глубина вложения
    query.bindValue(":mail", QString::fromStdWString(ui->lineAddMail->text().toStdWString())); // мыло
    query.bindValue(":telefon", QString::fromStdWString(ui->lineAddPhone->text().toStdWString())); // телефон

    // выполнить запрос.
    query.exec();
                qDebug() << "update EXEC!";
    emit updateTableNOW(); // сигнал обязательного обновления таблицы
    this->close();
}
