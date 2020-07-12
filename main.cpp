/* 主模块 */
#include "mainwindow.h"
#include "init_data.h"
#include "Log.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    //程序开始运行
    QApplication a(argc, argv);

    //创建程序主窗口
    MainWindow w;

    //地图信息初始化
    lines_load();//班次信息
    safety_init();//城市安全度
    position_init();//城市坐标

    //日志记录
    log_file(KIND_system,to_logstring_system(SystemSTART));


    //计时器通过设置QTimer定时触发timeout()信号实时运行
    w.show();
    QTimer *ptimer = new QTimer();
    w.connect(ptimer,SIGNAL(timeout()),&w,SLOT(Timer_f1()));
    w.connect(ptimer,SIGNAL(timeout()),&w,SLOT(Lcdnumber()));
    ptimer->start(w.timer.fre);

    //进入事件循环
    return a.exec();
}
