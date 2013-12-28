#include "overheadedit.h"
#include "ui_overheadedit.h"

OverheadEdit::OverheadEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OverheadEdit)
{
    ui->setupUi(this);
}

OverheadEdit::~OverheadEdit()
{
    delete ui;
}
