#include "dialog_insert.h"
#include "ui_dialog_insert.h"

Dialog_insert::Dialog_insert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_insert)
{
    ui->setupUi(this);
}

Dialog_insert::~Dialog_insert()
{
    delete ui;
}
