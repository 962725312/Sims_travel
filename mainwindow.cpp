/* 主窗口模块 */
#include "mainwindow.h"
#include "Log.h"
#include "ui_mainwindow.h"
#include <QEvent>
#include <QPainter>

std::queue<log_TranstoMView> queue_log_view;

/*计时器函数定义*/
//构造函数
Timer::Timer(void)
{
    day = 0;
    hour = 0;
    fre = 1000;
    isRunning = 0;
}

//计时器清零重置函数
void Timer::restart()
{
    log_file(KIND_timer,to_logstring_timer(3,day,hour));//对重置的系统时间进行记录

    day = 0;
    hour = 0;
    isRunning = 0;
}

//输出当前天数
int Timer::output_day()
{
    return day;
}

//输出当前小时数
int Timer::output_hour()
{
    return hour;
}

//计时器开始（继续）运行
void Timer::start()
{
    log_file(KIND_timer,to_logstring_timer(1,day,hour));
    isRunning = 1;
}

//计时器停止（暂时）运行
void Timer::stop()
{
    log_file(KIND_timer,to_logstring_timer(2,day,hour));
    isRunning = 0;
}

//计时器更新当前时间
void Timer::f1(){
    if(isRunning)//当计时器正在运行时才能更新当前时间
    {
        hour++;
        if(hour == 24)//满24进1
        {
            day++;
            hour = 0;
        }
    }
}

/* 主窗口类相关函数 */
//主窗口构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->log_view->document()->setMaximumBlockCount(LogViewCOUNT);//设置log_view最大显示行数

    ui->theMap->installEventFilter(this);//启用主窗口对子类对象theMap的监听

    //设置定时器每100ms触发一次timeout()超时信号，并在触发信号时调用文本框刷新函数textbrowser_refresh()
    ptimer = new QTimer();
    connect(ptimer,SIGNAL(timeout()),this,SLOT(textbrowser_refresh()));
    ptimer->start(100);

    Lcdnumber();//显示当前时间

    //当点击”输入旅客行程按钮时“，调用Dialog_insert_open()
    connect(ui->insert_button, SIGNAL(clicked()), this, SLOT(Dialog_insert_open()));

    //当点击计时器模块的某个按钮时，调用以下对应函数
    connect(ui->start, SIGNAL(clicked()), this, SLOT(Timer_start()));
    connect(ui->stop, SIGNAL(clicked()), this, SLOT(Timer_stop()));
    connect(ui->restart, SIGNAL(clicked()), this, SLOT(Timer_restart()));
}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}

//响应窗口关闭的函数（被保护的重写）
void MainWindow::closeEvent(QCloseEvent *event)
{
    //窗口关闭之前需要的操作
    log_file(KIND_system,to_logstring_system(SystemSTOP));
}


//计时器的显示函数
void MainWindow::Lcdnumber()
{
    ui->day_lcdNumber->display((int)timer.output_day());
    ui->hour_lcdNumber->display((int)timer.output_hour());
}

//日志记录文本框的刷新函数
void MainWindow::textbrowser_refresh()
{
    if(!queue_log_view.empty())
    {
        //从传出队列里取出一条消息
        log_TranstoMView slog_view = queue_log_view.front();
        queue_log_view.pop();

    log_view(slog_view.markind,slog_view.details);
    }
}

//日志记录文本框的显示函数
void MainWindow::log_view(int markkind,QStringList aline)
{
    QString str;
    //对不同种类的记录进行对应的处理
    if(markkind == KIND_system)
    {
        str = QString("程序类记录 %1").arg(aline[1]);
    }
    if(markkind == KIND_input)
    {
        str = QString("输入类记录 姓名：%1 起点：%2 终点：%3 出发时间：%4 day %5 hour 风险模式：%6 %7 %8")
                .arg(aline[0])
                .arg(aline[1])
                .arg(aline[2])
                .arg(aline[3])
                .arg(aline[4])
                .arg(aline[5])
                .arg(aline[6])
                .arg(aline[7]);
    }
    if(markkind == KIND_traveler)
    {
        str = QString("旅客类记录 当前时间：%1 day %2 hour 姓名：%3 状态：%4%5")
                .arg(aline[0])
                .arg(aline[1])
                .arg(aline[2])
                .arg(aline[3])
                .arg(aline[4]);
    }
    if(markkind == KIND_timer)
    {
        str = QString("计时类记录 当前时间：%1 day %2 hour %3")
                .arg(aline[0])
                .arg(aline[1])
                .arg(aline[2]);
    }
    if(markkind == KIND_travelplan)
    {
        str = aline.join("");
        str = QString("计划类记录 %1").arg(str);
    }

    //将字符串显示到文本框中
    ui->log_view->append(str);
}

//监听theMap组件是否有绘图要求的函数
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->theMap && event->type() == QEvent::Paint)
    {
        March_paintTime();
    }
    return true;
}

//theMap组件的绘图函数，即显示当前系统时间的各个旅客的旅行状态
void MainWindow::March_paintTime() {
    QPainter travelers_painter(ui->theMap);

    //打印背景地图
    QImage theMap(":/background/theMap");
    travelers_painter.drawImage(0 ,0 , theMap);

    int sday = timer.output_day();
    int shour = timer.output_hour();

    //如果没有正在进行的旅行，直接返回
    if(travelers_match.empty() || !timer.isRunning)
    {
        return;
    }

    //遍历travelers_match中的所有旅行计划
    for(int i = 0;i < travelers_match.length() && !travelers_match.empty();i++){
        //暂存变量初始化
        int delete_flag = 0;
        int c_cid = travelers_match.at(i).corrent_cid;//当前所在城市在路径里的序列数
        int c_tid = travelers_match.at(i).corrent_tid;//当前所坐班次在路径里的序列数
        long start_trump = travelers_match.at(i).start_day * 24 + travelers_match.at(i).start_hour;//出发时间
        long end_trump = travelers_match.at(i).end_day * 24 + travelers_match.at(i).end_hour;//达到时间
        long stime_trump = sday * 24 + shour - start_trump;//当前时间与出发时间的差，即时间戳

        //每个旅客有三种状态变化
        //变化一：从未出发到出发
        if(sday == travelers_match.at(i).start_day && shour == travelers_match.at(i).start_hour)
        {
            if(travelers_match.at(i).setout_time[0] == travelers_match.at(i).arrive_time[0])//如果出发要做的班次没有等待时间
            {
                //travelers_match[i].corrent_cid与cid是对应的，需要同时变化
                travelers_match[i].corrent_cid++;
                c_cid++;
                //改变自身状态
                travelers_match[i].cid_work = ON_traffic;
            }
            else //如果需要等待
            {
                travelers_match[i].cid_work = ON_await;
            }
        }
        //变化二：到达某个城市
        else if(travelers_match.at(i).arrive_time[c_cid] == stime_trump)
        {
            if(stime_trump + start_trump == end_trump)//特殊处理，如果到达的是终点
            {
                delete_flag = 1;
                travelers_match[i].cid_work = ON_await;
            }
            else if(travelers_match.at(i).setout_time[0] == travelers_match.at(i).arrive_time[0])//据离下一个要坐的班次没有等待时间
            {
                travelers_match[i].corrent_cid++;
                c_cid++;
            }
            else //据离下一个要坐的班次有等待时间
            {
                travelers_match[i].cid_work = ON_await;
                travelers_match[i].corrent_tid++;
            }
        }
        //变化三：到达出发时间
        else if(travelers_match.at(i).setout_time[c_cid] == stime_trump)
        {
            travelers_match[i].corrent_cid++;
            c_cid++;
            travelers_match[i].cid_work = ON_traffic;
        }
        //每个旅客有3种绘图状态
        //状态一：未达到出发时间，不显示
        if(sday < travelers_match.at(i).start_day)//天数未达到
        {
            log_file(KIND_traveler,to_logstring_traveler(travelers_match[i],sday,shour));
            continue;
        }
        else if(sday == travelers_match.at(i).start_day && shour < travelers_match.at(i).start_hour)//天数达到但时间未达到
        {
            log_file(KIND_traveler,to_logstring_traveler(travelers_match[i],sday,shour));
            continue;
        }
        //状态二：在城市中等待
        else if(travelers_match.at(i).cid_work == ON_await)
        {
            QImage person(":/person/person");
            int thecid = travelers_match.at(i).cid[c_cid];
            travelers_painter.drawImage( city_position[thecid].first, city_position[thecid].second, person);
            //打印日志
            log_file(KIND_traveler,to_logstring_traveler(travelers_match[i],sday,shour));
        }
        //状态三：在旅行的路上
        else if(travelers_match.at(i).cid_work == ON_traffic)
        {
            //初始化暂存变量，提高代码可读性
            int thestart_cid = travelers_match.at(i).cid[c_cid-1];
            int theend_cid = travelers_match.at(i).cid[c_cid];
            int thetid = travelers_match.at(i).tid[c_tid];
            int thetrans_kind = city_line[thestart_cid].detail[thetid].transport_kind;

            //计算当前旅行位置
            //变量初始化
            int x_position,y_position;//所求的当前旅行坐标
            int start_xpos,start_ypos;//当前旅行路径的起点城市坐标
            int difference_xpos,difference_ypos;//当前旅行路径终点城市和起点城市的差值
            int thecost_time = city_line[thestart_cid].detail[thetid].timecost;//经历当前路径所花的时间
            int difference_time = stime_trump - travelers_match.at(i).setout_time[c_cid-1];//在这条路径上已经走过的时间
            //这个班次起点城市的坐标
            start_xpos = city_position[thestart_cid].first;
            start_ypos = city_position[thestart_cid].second;
            //两地坐标的差值（终点-起点）
            difference_xpos = city_position[theend_cid].first - start_xpos;
            difference_ypos = city_position[theend_cid].second - start_ypos;
            //利用时间与距离的等比例缩放求得目标坐标
            x_position = (difference_xpos * difference_time / thecost_time) + start_xpos;
            y_position = (difference_ypos * difference_time / thecost_time) + start_ypos;

            //判断交通方式，并引用对应的图片资源,打印输出
            if(thetrans_kind == Car_Kind)
            {
                QImage transport(":/transport/car");
                travelers_painter.drawImage( x_position, y_position, transport);
            }
            else if(thetrans_kind == Train_Kind)
            {
                QImage transport(":/transport/train");
                travelers_painter.drawImage( x_position, y_position, transport);
            }
            else if(thetrans_kind == Plane_Kind)
            {
                QImage transport(":/transport/plane");
                travelers_painter.drawImage( x_position, y_position, transport);
            }
            //打印日志
            log_file(KIND_traveler,to_logstring_traveler(travelers_match[i],sday,shour));
        }

        if(delete_flag){
            //如果已到达终点城市，则删除这个行程计划，则i也需要顺次-1，防止漏掉旅行没有被遍历到
            travelers_match.removeAt(i--);
        }
    }
}

//打开输入数据对话框的函数
void MainWindow::Dialog_insert_open()
{
    qRegisterMetaType<QVariant>("QVariant");//注册元类型
    Timer_stop();//先停止计时器工作
    Dialog_insert dlg;

    connect(&dlg,SIGNAL(offer_data_toMainwindow(QVariant)),this,SLOT(getdata_fromDialog(QVariant)));//绑定对话框窗口向主窗口发送消息的信号与槽

    dlg.exec();//进入对话窗口的事件循环
}

//获取从对话框窗口传递的数据
void MainWindow::getdata_fromDialog(QVariant Datavar)
{
    qRegisterMetaType<QVariant>("QVariant");
    Traveler_plan theplan;
    Traveler_input inputdata = Datavar.value<Traveler_input>();
    //判断输入是否合法
    if(inputdata.name == "")//未输入姓名
    {
        inputdata.legal_flag = 0;//不合法标志改写
        log_file(KIND_input,to_logstring_input(inputdata));//打印日志
        return;//函数提前返回
    }
    else if(inputdata.start_cid == inputdata.end_cid)//起点城市与终点城市重复
    {
        inputdata.legal_flag = 0;
        log_file(KIND_input,to_logstring_input(inputdata));
        return;
    }
    else if(inputdata.day * 24 + inputdata.hour < timer.output_day() * 24 + timer.output_hour())//出发时间小于计时器当前时间
    {
        inputdata.legal_flag = 0;
        log_file(KIND_input,to_logstring_input(inputdata));
        return;
    }

    theplan = cal_traveler_plan(inputdata);
    //判断能否找到解决方案，如果找不到，不会进入模拟显示表
    if(theplan.workin_safemode)
    {
        travelers_match.append(theplan);//有解决方案则将计划添加到链表中
        log_file(KIND_travelplan, to_logstring_travelplan(theplan));
    }
    else
    {
        log_file(KIND_travelplan, to_logstring_travelplan(theplan));
    }
}

//由于调用计时器是信号槽链接机制，无法直接对计时器的功能进行调用，所以要设置槽函数从而间接调用
//调用计时器的start功能
void MainWindow::Timer_start()
{
    timer.start();
}
//调用计时器的stop功能
void MainWindow::Timer_stop()
{
    timer.stop();
}
//调用计时器的restart功能
void MainWindow::Timer_restart()
{
    timer.restart();
    travelers_match.clear();
    ui->theMap->update();
}
//调用计时器的f1功能
void MainWindow::Timer_f1()
{
    timer.f1();
    if(timer.isRunning)
    {
        ui->theMap->update();
    }
}


