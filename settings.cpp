#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );
    // connectors:
    connect(ui->pushButtonGETDBBrowse, SIGNAL(clicked()), this, SLOT(GETDB()));
    //connect(ui->pushButtonGETDBApply, SIGNAL(clicked()), this, SLOT(GETDBApply()));

    connect(ui->pushButtonADDDBBrowse, SIGNAL(clicked()), this, SLOT(CreateDB()));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::GETDB()
{
    QString getpath = QFileDialog::getOpenFileName(this,
                                              QString::fromLocal8Bit("Open"),
                                              QDir::currentPath(),
                                              "DB File (*.sqlite)");
    ui->lineEditGETDB->setText(getpath);
    if(getpath.isEmpty())
    {
        return;
    } else
    {
        appsets = new QSettings("conf.ini", QSettings::IniFormat);
        appsets->setValue("main/db", ui->lineEditGETDB->text());
        appsets->sync();
        QTextCodec *codec = QTextCodec::codecForName("UTF8");
        QTextCodec::setCodecForLocale(codec);
        QMessageBox::information(this,
                                 tr("Готово"),
                                 tr("Настройки сохранены. Пожалуйста, перезапустите приложения для загрузки файла базы данных."));
    }

}

void Settings::CreateDB()
{
    ui->lineEditADDDB->clear();
    QString fname = QFileDialog::getSaveFileName(
                this,
                tr("Создать файл БД"),
                QDir::currentPath(),
                tr("Файл базы данных (*.sqlite)") );
    if(fname.isEmpty())
    {
        return;
    } else
    {
            QFile file(fname);
            if(!file.open(QIODevice::WriteOnly))
            {
                QMessageBox::information(this, tr("Невозможно открыть / сохранить файл."), file.errorString());
                return;
            }
            ui->lineEditADDDB->insert(fname);
        QSqlDatabase dbopen = QSqlDatabase::addDatabase("QSQLITE");
        //dbopen.setDatabaseName(fname + ".sqlite");
        dbopen.setDatabaseName(fname);
        if(!dbopen.open()) qDebug() << "Error openning saved DB!"; else qDebug() << "Done opening saved DB!";
        QSqlQuery ins = QSqlQuery(dbopen);
        ins.exec("CREATE TABLE struktura (id INTEGER PRIMARY KEY AUTOINCREMENT, fam CHAR (255), ioch CHAR (255), nomer CHAR (255), date CHAR (255), passp CHAR (255), address CHAR (255), spid CHAR (255), glubina CHAR (255), mail CHAR (255), telefon CHAR (255))");
        ins.exec("CREATE TABLE nakladnie (id INTEGER PRIMARY KEY AUTOINCREMENT, name CHAR (255), art CHAR (255), desckr CHAR (255), cena CHAR (255), kol CHAR (255), ball CHAR (255), bonus CHAR (255), nomer CHAR (255), datep CHAR (255), vozvrat CHAR (255), poluch CHAR (255))");
        ins.exec("CREATE TABLE korzina (id INTEGER PRIMARY KEY AUTOINCREMENT, name CHAR (255), art CHAR (255), desckr CHAR (255), cena CHAR (255), kol CHAR (255), ball CHAR (255), bonus CHAR (255), nomer CHAR (255), datep CHAR (255), vozvrat CHAR (255))");
        ins.exec("CREATE TABLE tovar (id INTEGER PRIMARY KEY AUTOINCREMENT, art CHAR (255), desckr CHAR (255), cend CHAR (255), cenroz CHAR (255), bal CHAR (255), skidka CHAR (255), cenawskidka CHAR (255))");
        ins.exec("CREATE TABLE nomnakl (id INTEGER PRIMARY KEY AUTOINCREMENT, name CHAR (255), date CHAR (255), nomer CHAR (255), agent CHAR (255))");

        dbopen.close();
        QMessageBox::information(this, tr("Готово"), tr("Файл базы данных успешно создан и сохранён."));
    }

}
