#ifndef OVERHEADEDIT_H
#define OVERHEADEDIT_H

#include <QDialog>

namespace Ui {
class OverheadEdit;
}

class OverheadEdit : public QDialog
{
    Q_OBJECT
    
public:
    explicit OverheadEdit(QWidget *parent = 0);
    ~OverheadEdit();
    
private:
    Ui::OverheadEdit *ui;
};

#endif // OVERHEADEDIT_H
