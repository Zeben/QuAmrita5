#include <QTextCodec>
#include <QStyleFactory>
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.openDb();
    w.setupTable();
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForLocale(codec);

    return a.exec();
}
