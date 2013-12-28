#ifndef OVERHEADGETPRODUCT_H
#define OVERHEADGETPRODUCT_H

#include <QDialog>

namespace Ui {
class OverheadGetProduct;
}

class OverheadGetProduct : public QDialog
{
    Q_OBJECT
    
public:
    explicit OverheadGetProduct(QWidget *parent = 0);
    ~OverheadGetProduct();
    
private:
    Ui::OverheadGetProduct *ui;
};

#endif // OVERHEADGETPRODUCT_H
