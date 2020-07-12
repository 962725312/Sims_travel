#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define LogViewCOUNT 6 //日志记录文本框的最大显示行数限制

#include <QMainWindow>
#include <QCloseEvent>
#include "dialog_insert.h"
#include "init_data.h"
#include <QStringList>
#include <QTimer>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//计时器类
class Timer
{
public:
    int fre;//定时器间隔变量，每过fre ms计时器增加1hour
    bool isRunning;//是否在运行状态的标志
    Timer();//构造函数
    int output_day();//输出当前时间(天)
    int output_hour();//输出当前时间（小时）
    void f1();//计时器+1小时
    void start();//开始运行
    void stop();//停止运行
    void restart();//停止运行并清零重置

private:
    long day,hour;//系统当前时间
};

//主窗口类
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Timer timer;//计时器
    QList<Traveler_plan> travelers_match;//储存未解决的旅客行程

    bool eventFilter(QObject*, QEvent*);//事件监听过滤器
    void March_paintTime();//根据旅客状态进行绘图


public slots:
    void Dialog_insert_open();//打开插入旅客行程对话框
    void Timer_start();//开始计时
    void Timer_stop();//停止计时
    void Timer_restart();//重置计时器
    void Timer_f1();//计时器小时数加1
    void Lcdnumber();//显示当前timer的day和hour
    void textbrowser_refresh();//刷新日志记录显示框
    void getdata_fromDialog(QVariant Datavar);//定义接收Dialog窗口发来数据的槽函数

protected:
     void closeEvent(QCloseEvent *event);//处理响应程序关闭动作时的功能

private:
    Ui::MainWindow *ui;
    QTimer *ptimer;//计时器，发送时序信号用来监听获取日志记录
    void log_view(int markkind,QStringList strlist);//打印经过处理的日志记录
};

//日志信息传送类
class log_TranstoMView
{
public:
    int markind;//日志记录种类
    QStringList details;//日志内容
};

#endif // MAINWINDOW_H
