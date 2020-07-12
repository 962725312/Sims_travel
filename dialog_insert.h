#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QDialog>

namespace Ui {
class Dialog_insert;
}

class Dialog_insert : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_insert(QWidget *parent = nullptr);
    ~Dialog_insert();

private:
    Ui::Dialog_insert *ui;
};

#endif // DIALOG_INSERT_H
