#ifndef DIALOG_INSERT_H
#define DIALOG_INSERT_H

#include <QDialog>
#include "Core_sims.h"
#include <QMetaType>
#include <QVariant>

namespace Ui {
class Dialog_insert;
}

//输入窗口类
class Dialog_insert : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_insert(QWidget *parent = nullptr);
    ~Dialog_insert();

signals:
    void offer_data_toMainwindow(QVariant data);//发送一个信号通知传输数据

public slots:
    void offer_data();//传输从窗口中获取的数据

private:
    Ui::Dialog_insert *ui;
};

#endif // DIALOG_INSERT_H
