#include "overhead.h"
#include "ui_overhead.h"

Overhead::Overhead(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Overhead)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose ); // очищать память из кучи
    Overhead::setupOH();
    ui->groupBoxEdit->hide();
    ui->groupBoxCell->hide();
    ui->groupBoxHackGet->hide();

    ui->pushOHPureDel->hide();
    ui->pushOHPrint->hide();

    // INITS
    connect(ui->pushOHEdit, SIGNAL(clicked()), this, SLOT(EditParam())); // управление редактированием
    connect(ui->pushEditApply, SIGNAL(clicked()), this, SLOT(apply())); // применение параметров редактирования
    connect(ui->pushOHDel, SIGNAL(clicked()), this, SLOT(del()));
    connect(ui->pushOHCell, SIGNAL(clicked()), this, SLOT(cell())); // хак для динамического изменения окна
    connect(ui->pushOHEditHack, SIGNAL(clicked()), this,  SLOT(edit_hack())); // тоже самое

    // коннекторы для удаления, редактирования, получения долга на товар
    connect(ui->pushOHReturnedYes, SIGNAL(clicked()), this, SLOT(isReturnedYes()));
    connect(ui->pushOHReturnedYes, SIGNAL(clicked()), this, SLOT(on_tableOH_clicked()));

    connect(ui->pushOHReturnedNo, SIGNAL(clicked()), this, SLOT(isReturnedNo()));
    connect(ui->pushOHReturnedNo, SIGNAL(clicked()), this, SLOT(on_tableOH_clicked()));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(duty()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_tableOH_clicked()));

    // тест драйвера печати
    connect(ui->pushOHPrint, SIGNAL(clicked()), this, SLOT(printDialog()));
}

Overhead::~Overhead()
{
    delete ui;
}

void Overhead::edit_hack()
{
    if(ui->pushOHEditHack->text() == "Получение по накладной...")
    {
        //ui->pushOHPureDel->show();
        //ui->pushOHPrint->show();
        ui->pushOHEditHack->setText("Закрыть получение...");
        ui->groupBoxHackGet->show();
        ui->pushOHCell->setEnabled(false);
        Overhead::on_tableOH_clicked();
    }
    else if(ui->pushOHEditHack->text() == "Закрыть получение...")
    {
//        ui->pushOHPureDel->hide();
//        ui->pushOHPrint->hide();
        ui->pushOHEditHack->setText("Получение по накладной...");
        ui->groupBoxHackGet->hide();
        ui->pushOHCell->setEnabled(true);
        Overhead::on_tableOH_clicked();
    }
}

void Overhead::cell()
{
    if(ui->pushOHCell->text() == "Продажа по накладной...")
    {
        ui->pushOHPureDel->show();
        ui->pushOHPrint->show();
        ui->pushOHCell->setText("Закрыть продажу...");
        ui->groupBoxCell->show();
        ui->pushOHEditHack->setEnabled(false);
    }
    else if(ui->pushOHCell->text() == "Закрыть продажу...")
    {
        ui->pushOHCell->setText("Продажа по накладной...");

        ui->pushOHPureDel->hide();
        ui->pushOHPrint->hide();

        ui->groupBoxCell->hide();
        ui->groupBoxEdit->hide();
        ui->pushOHEditHack->setEnabled(true);
        ui->pushOHEdit->setText("Редактировать...");
    }
}

void Overhead::on_tableOH_clicked()
{
    qDebug() << "Overhead::on_tableOH_clicked()";
    // опишем драйвер доступа к БД (доступ к содержимому накладной по клику)
    modelOHCellingQuery->setQuery("SELECT * FROM nakladnie WHERE name = " + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 1)).toString());
    modelOHGetHack->setQuery("SELECT * FROM nakladnie WHERE name = " + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 1)).toString());

    // запишем заголовки (изменение)
    modelOHCellingQuery->setHeaderData(2,Qt::Horizontal, QObject::tr("Артикул"));
    modelOHCellingQuery->setHeaderData(3,Qt::Horizontal, QObject::tr("Наименование товара"));
    modelOHCellingQuery->setHeaderData(4,Qt::Horizontal, QObject::tr("Цена (грн)"));
    modelOHCellingQuery->setHeaderData(5,Qt::Horizontal, QObject::tr("Кол-во"));
    modelOHCellingQuery->setHeaderData(6,Qt::Horizontal, QObject::tr("Балл"));

    // запишем заголовки (изменение)
    modelOHGetHack->setHeaderData(2,Qt::Horizontal, QObject::tr("Артикул"));
    modelOHGetHack->setHeaderData(3,Qt::Horizontal, QObject::tr("Наименование товара"));
    modelOHGetHack->setHeaderData(4,Qt::Horizontal, QObject::tr("Цена (грн)"));
    modelOHGetHack->setHeaderData(5,Qt::Horizontal, QObject::tr("Кол-во"));
    modelOHGetHack->setHeaderData(6,Qt::Horizontal, QObject::tr("Балл"));
    modelOHGetHack->setHeaderData(11,Qt::Horizontal, QObject::tr("Получено"));

    // привяжем таблицу к модели для отображения содержимого БД
    ui->tableOHSelling->setModel(modelOHCellingQuery); // изменение
    ui->tableOHHackGet->setModel(modelOHGetHack); // продажа

    // настроить свойства отображения!
    Overhead::UICellSetup();
    Overhead::UIGetHackSetup();

    // сообщение для отладки
    qDebug() << modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 1)).toString();

    // пересчитываем графы баллов и цены
    // здесь вызываем ф-цию пересчета баллов и цены (предв. очищаем эти поля)
    ui->lineAllCash->clear();
    ui->lineAllPoints->clear();
    ui->lineAllCash->setText(Overhead::cashCalc());
    ui->lineAllPoints->setText(Overhead::pointCalc());
    ui->lineGetHackAllPrice->setText(Overhead::cashCalc());
    ui->lineGetHackAllPoints->setText(Overhead::pointCalc());

    // а здесь мы будем запиливать доп. 5% от суммы всех товаров накладной
    ui->lineGetHackAllPointsP->setText(Overhead::cashCalcPercent());

    // при открыой графе редактирования обновляем кол-во выделенного товара:
    ui->lineOHCount->setText(modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 5)).toString());

}

void Overhead::UISetup()
{
    // настройка параметров выделения строк
    ui->tableOH->resizeColumnsToContents();
    ui->tableOH->resizeRowsToContents();
    ui->tableOH->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableOH->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableOH->setSelectionMode(QAbstractItemView::SingleSelection);

    // выыыыыыыыствляем ширину стоблцов
    ui->tableOH->setColumnWidth(0, 30);
    ui->tableOH->setColumnWidth(1, 60);
    ui->tableOH->setColumnWidth(2, 55);
    ui->tableOH->setColumnWidth(3, 60);

    // выставляем свойства таблицы
    ui->tableOH->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableOH->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableOH->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableOH->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableOH->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
}

void Overhead::UICellSetup()
{
    qDebug() << "Overhead::UICellSetup()";
    // настройка параметров выделения строк
    ui->tableOHSelling->resizeColumnsToContents();
    ui->tableOHSelling->resizeRowsToContents();
    ui->tableOHSelling->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableOHSelling->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableOHSelling->setSelectionMode(QAbstractItemView::SingleSelection);

    // выыыыыыыыствляем ширину стоблцов (скрываем ненужные столбцы)
    ui->tableOHSelling->setColumnHidden(1, true);
    ui->tableOHSelling->setColumnHidden(7, true);
    ui->tableOHSelling->setColumnHidden(8, true);
    ui->tableOHSelling->setColumnHidden(9, true);
    ui->tableOHSelling->setColumnHidden(10, true);
    ui->tableOHSelling->setColumnHidden(11, true);

    ui->tableOHSelling->setColumnWidth(0, 30);
    ui->tableOHSelling->setColumnWidth(2, 55);
    ui->tableOHSelling->setColumnWidth(4, 68);
    ui->tableOHSelling->setColumnWidth(5, 45);
    ui->tableOHSelling->setColumnWidth(6, 45);

    // выставляем свойства таблицы
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tableOHSelling->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
}

void Overhead::UIGetHackSetup()
{
    qDebug() << "Overhead::UIGetHackSetup()";
    // настройка параметров выделения строк
    ui->tableOHHackGet->resizeColumnsToContents();
    ui->tableOHHackGet->resizeRowsToContents();
    ui->tableOHHackGet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableOHHackGet->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableOHHackGet->setSelectionMode(QAbstractItemView::SingleSelection);

    // выыыыыыыыствляем ширину стоблцов (скрываем ненужные столбцы)
    ui->tableOHHackGet->setColumnHidden(1, true);
    ui->tableOHHackGet->setColumnHidden(7, true);
    ui->tableOHHackGet->setColumnHidden(8, true);
    ui->tableOHHackGet->setColumnHidden(9, true);
    ui->tableOHHackGet->setColumnHidden(10, true);

    ui->tableOHHackGet->setColumnWidth(0, 30);
    ui->tableOHHackGet->setColumnWidth(2, 55);

    ui->tableOHHackGet->setColumnWidth(4, 60);
    ui->tableOHHackGet->setColumnWidth(5, 45);
    ui->tableOHHackGet->setColumnWidth(6, 55);
    ui->tableOHHackGet->setColumnWidth(11, 60);

    // выставляем свойства таблицы
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);

    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    ui->tableOHHackGet->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Fixed);

}

void Overhead::setupOH()
{
    qDebug() << "OH(), setup() init...";

    // устанавливаем стандартную кодировку
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);

    // создаем драйвер модели запроса в БД
    modelOHQuery = new QSqlQueryModel(this);
    modelOHQuery->setQuery("SELECT * FROM nomnakl");

    // здесь запишем хидеры
    modelOHQuery->setHeaderData(1,Qt::Horizontal, QObject::tr("№ накл-й"));
    modelOHQuery->setHeaderData(2,Qt::Horizontal, QObject::tr("Дата"));
    modelOHQuery->setHeaderData(3,Qt::Horizontal, QObject::tr("№ агента"));
    modelOHQuery->setHeaderData(4,Qt::Horizontal, QObject::tr("ФИО"));
    //-------------------//

    modelOHCellingQuery = new QSqlQueryModel(this);
    modelOHGetHack = new QSqlQueryModel(this);

    ui->tableOH->setModel(modelOHQuery);
    Overhead::UISetup();
//"SELECT * FROM nakladnie
}

void Overhead::EditParam()
{
    if(ui->pushOHEdit->text() == "Редактировать...")
    {
        ui->groupBoxEdit->show();

        // заполнение результатами запроса из БД
        ui->lineOHCount->setText(modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 5)).toString());

        // меняем название кнопельки
        ui->pushOHEdit->setText("Закрыть...");

    }
    else if(ui->pushOHEdit->text() == "Закрыть...")
    {
        //очистка полей редактирования
        ui->lineOHCount->clear();

        ui->groupBoxEdit->hide();
        // меняем название кнопельки
        ui->pushOHEdit->setText("Редактировать...");
    }
}

void Overhead::apply()
{
    qDebug() << "Overhead::apply()";

    // здесь мы извлекаем параметры товара из таблицы БД
    float floatKol = modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 5)).toFloat();
    float floatPrice = modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 4)).toFloat();
    float floatPoints = modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 6)).toFloat();

    // для корректности расчетов нужно знать параметры 1 шт. товара, а если у нас их 5?
    // поэтому сначала нужно парам. товара делить на их кол. в БД, затем расчитать их кол-во по польз. кол-ву
    // возможно, проверка условий не понадобится - нужно лишь это подвердить и проверить
    // кол-во товаров мы будем знать сразу - за нас будет работать другая ф-ция, дабы не дублировать
    // ф-ционал.

    float cfloatPrice = (floatPrice / floatKol) * ui->lineOHCount->text().toFloat();
    float cfloatPoints = (floatPoints / floatKol) * ui->lineOHCount->text().toFloat();

    QString sfloatPrice = QString::number(cfloatPrice);
    QString sfloatPoints = QString::number(cfloatPoints);

    qDebug() << "FPrice = " << sfloatPrice;
    qDebug() << "FPoints = " << sfloatPoints;

    QSqlQuery queryApply;
    queryApply.prepare("UPDATE nakladnie SET cena = :cena, kol = :kol, ball = :ball WHERE id = :id");
    queryApply.bindValue(":cena", sfloatPrice);
    queryApply.bindValue(":kol", QString::fromStdWString(ui->lineOHCount->text().toStdWString()));
    queryApply.bindValue(":ball", sfloatPoints);
    queryApply.bindValue(":id", modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 0)).toString());
    queryApply.exec();
    qDebug() << queryApply.lastError().text();


    //modelOHCellingQuery->setQuery();
    ui->groupBoxEdit->close();
    Overhead::on_tableOH_clicked();

    ui->pushOHEdit->setText("Редактировать...");
}

void Overhead::del()
{
    qDebug("del...");
    modelOHCellingQuery->setQuery("DELETE FROM nakladnie WHERE id = " + modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 0)).toString());
    Overhead::setupOH();
}

QString Overhead::cashCalc()
{
    float cash = 0;
    for(int iii = 0; iii < modelOHCellingQuery->rowCount(); iii++)
    {
        cash = cash + modelOHCellingQuery->data(modelOHCellingQuery->index(iii, 4)).toFloat();
    }
    return tr("%1").arg(cash, 0, 'f', 2);
}

QString Overhead::pointCalc()
{
    float points = 0;
    for(int iii = 0; iii < modelOHCellingQuery->rowCount(); iii++)
    {
        points = points + modelOHCellingQuery->data(modelOHCellingQuery->index(iii, 6)).toFloat();
    }
    return QString::number(points);
}

QString Overhead::cashCalcPercent()
{
    float cash = 0;
    for(int iii = 0; iii < modelOHCellingQuery->rowCount(); iii++)
    {
        cash = cash + modelOHCellingQuery->data(modelOHCellingQuery->index(iii, 4)).toFloat();
    }
    float res = cash + ((cash * 5) / 100);
    //return QString::number(cash + ((cash * 5) / 100)).arg(cash, 0, 'f', 2);
    return tr("%1").arg(res, 0, 'f', 2);
}

void Overhead::on_tableOHSelling_clicked()
{
    ui->lineOHCount->setText(modelOHCellingQuery->data(modelOHCellingQuery->index(ui->tableOHSelling->currentIndex().row(), 5)).toString());
}

void Overhead::isReturnedYes()
{
    qDebug() << "Overhead::isReturnedYes()";
    modelOHGetHack->setQuery("UPDATE nakladnie SET poluch = 'да' WHERE id = " + modelOHGetHack->data(modelOHGetHack->index(ui->tableOHHackGet->currentIndex().row(), 0)).toString() );
    Overhead::on_tableOH_clicked();

}

void Overhead::isReturnedNo()
{
    qDebug() << "Overhead::isReturnedNo()";
    modelOHGetHack->setQuery("UPDATE nakladnie SET poluch = 'нет' WHERE id = " + modelOHGetHack->data(modelOHGetHack->index(ui->tableOHHackGet->currentIndex().row(), 0)).toString() );
    Overhead::on_tableOH_clicked();

}

void Overhead::duty()
{
    qDebug() << "Overhead::duty()";
    modelOHGetHack->setQuery("UPDATE nakladnie SET poluch = 'долг ("+ ui->lineEdit_4->text() + ")' WHERE id = " + modelOHGetHack->data(modelOHGetHack->index(ui->tableOHHackGet->currentIndex().row(), 0)).toString() );
    Overhead::on_tableOH_clicked();
}

void Overhead::printDialog()
{
    QPrinter printer(QPrinter::HighResolution);
    //printer.setResolution(QPrinter::A4);
    printer.setPaperSize(QPrinter::A4);
    //printer.setOrientation(QPrinter::Portrait);
    QPrintPreviewDialog *pd = new QPrintPreviewDialog(&printer);
    connect(pd, SIGNAL(paintRequested(QPrinter*)),
            this, SLOT(printDriver(QPrinter*)));
    pd->exec();
}

void Overhead::printDriver(QPrinter *p)
{
    qDebug("Call printDriver...");
    QString strStream;
    QTextStream out(&strStream);
    out.setCodec("UTF-8");

    const int rowCount = ui->tableOHHackGet->model()->rowCount();
    const int columnCount = ui->tableOHHackGet->model()->columnCount();

    QString css;
    css = "<style type=\"text/css\">";
    css += "table.tbl {border-width: 3px;border-style: solid;border-color: black;margin-top: 0px;margin-bottom: 0px;color: black;}";
    css += "</style>";

    QString str = "<h3 align=center>Компания \"Амрита\"</h3>";
            str += "<h3 align=center> Накладная №" + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 1)).toString() + " по состоянию на " + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 2)).toString() + "</h3>";
            str += "Дистрибьютор: " + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 4)).toString() + "<br>";
            str += "Номер: " + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 3)).toString();
            str += "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"2\" class=\"tbl\">";
            str += "<tr><th>Артикул</th><th>Название товара</th><th>Цена<br>дистрибютора (грн.)</th><th>Количество</th><th>Балл</th> <th>Получено</th></tr>";

            for (int row = 0; row < rowCount; row++)
            {
                str += "<tr>";
                    for (int column = 0; column < columnCount; column++)
                    {
                        if (ui->tableOHHackGet->isColumnHidden(column) != true)
                        {
                            if(column != 2)
                            {
                                QString data = ui->tableOHHackGet->model()->data(ui->tableOHHackGet->model()->index(row, column)).toString().simplified();
                                str += QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
                            }
                        }
                    }
             str += "</tr>\n";
             }
            str += "</table>";
            str += "<br>Всего товара на: " + ui->lineGetHackAllPrice->text() + " грн.";
            str += "<br>С учётом доставки (5%): " + ui->lineGetHackAllPointsP->text() + " грн.";
            str += "<br>Всего баллов: " + ui->lineGetHackAllPoints->text() + ".<br><br>";

            str += "Отпустил _____________________ (Козачук В.П.)<br><br>";

            str += "Получил ________________ (" + modelOHQuery->data(modelOHQuery->index(ui->tableOH->currentIndex().row(), 4)).toString() + ")<br>";


    out << str;

        QTextDocument *document = new QTextDocument();
        document->setHtml(css+strStream);

        document->print(p);

        delete document;
}
