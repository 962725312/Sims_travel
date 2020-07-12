/* 日志文件处理模块 */
#include "Log.h"
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <mainwindow.h>

extern std::queue<log_TranstoMView> queue_log_view;//以队列作为将日志记录传输给主窗口模块的日志显示窗口模块的通道

/*对不同种类日志记录进行字符串表转换的函数*/

//系统状态
QStringList to_logstring_system(int state)
{
    QStringList strlist;

    if(state == SystemSTART)
    {
        strlist << "---------------------------------------------------------\n";
        strlist << "程序开始运行";
        strlist << "---------------------------------------------------------\n";
    }
    else if(state == SystemSTOP)
    {
        strlist << "---------------------------------------------------------\n";
        strlist << "程序运行结束";
        strlist << "---------------------------------------------------------\n";
    }
    else
    {
        strlist << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        strlist << "程序返回异常";
        strlist << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    }
    return strlist;
}

//用户输入
QStringList to_logstring_input(Traveler_input input_detail)
{
    QStringList strlist;

    strlist << input_detail.name;
    strlist << QString::number(input_detail.start_cid);
    strlist << QString::number(input_detail.end_cid);
    strlist << QString::number(input_detail.day);
    strlist << QString::number(input_detail.hour);
    if(input_detail.legal_flag == 0)
    {
        strlist << "输入不合法" << " 本条记录被抛弃 ";
    }
    else if(input_detail.safe_mode == LeastRisk_Mode)
    {
        strlist << "风险最低模式" << " 无时间限制 ";
    }
    else if(input_detail.safe_mode == TimeLimited_Mode)
    {
        strlist << "限定时间风险最低模式,时间限制为(单位：hour)：" << QString::number(input_detail.time_limit);
    }

    if(input_detail.transport_consider == Considertrans_Yes)
    {
        strlist << " 考虑交通风险";
    }
    else if(input_detail.transport_consider == Considertrans_No)
    {
        strlist << " 不考虑交通风险";
    }
    return strlist;
}

//旅行状态
QStringList to_logstring_traveler(Traveler_plan plan_detail,int day,int hour)
{
    QStringList strlist;
    int travel_over;
    travel_over = (plan_detail.end_day - day) * 24 + plan_detail.end_hour - hour;

    strlist << QString::number(day) << QString::number(hour);
    strlist << plan_detail.name;

    if(!plan_detail.workin_safemode)
    {
        strlist << "在限时模式下找不到解决方案 " << "本条记录被抛弃";
    }
    else if(!travel_over)
    {
        strlist << "旅客已到达,终点城市为" << QString::number(plan_detail.cid[plan_detail.corrent_cid]);
    }
    else
    {
        if(day * 24 +hour < plan_detail.start_day * 24 + plan_detail.start_hour)
        {
            strlist << "当前时间小于旅客出发时间 " << "旅客尚未出发";
        }
        else if(plan_detail.cid_work == ON_await)
        {
            strlist << "旅客正在等待,所在城市：" << QString::number(plan_detail.cid[plan_detail.corrent_cid]);
        }
        else if(plan_detail.cid_work == ON_traffic)
        {
            strlist << "旅客正在路上,下一目的地：" << QString::number(plan_detail.cid[plan_detail.corrent_cid]);
        }
    }
    return strlist;
}


//旅行计划
QStringList to_logstring_travelplan(Traveler_plan plan_detail){
    QStringList strlist;
    if(!plan_detail.workin_safemode)
    {
        strlist << "姓名为：" <<plan_detail.name << " ";
        strlist << "在限时模式下找不到解决方案 " << "本条记录被抛弃";
    }
    else{
        strlist << "姓名为：" <<plan_detail.name << " ";
        strlist << "出发时间为:" << QString::number(plan_detail.start_day) << "day" << QString::number(plan_detail.start_hour) << "hour ";
        strlist << "到达时间为:" << QString::number(plan_detail.end_day) << "day" << QString::number(plan_detail.end_hour) << "hour ";
        strlist << "旅行路线为:";
        for(int i = 0; i < plan_detail.all_citynum;i++){
            strlist << QString::number(plan_detail.cid[i]) << "=";
            strlist << QString::number(plan_detail.setout_time[i]);
            if(city_line[plan_detail.cid[i]].detail[plan_detail.tid[i]].transport_kind == Car_Kind)
            {
                strlist << "-汽车-";
            }
            else if(city_line[plan_detail.cid[i]].detail[plan_detail.tid[i]].transport_kind == Train_Kind)
            {
                strlist << "-火车-";
            }
            else if(city_line[plan_detail.cid[i]].detail[plan_detail.tid[i]].transport_kind == Plane_Kind)
            {
                strlist << "-飞机-";
            }
            strlist << QString::number(plan_detail.arrive_time[i+1]) << "=";
        }
        strlist << QString::number(plan_detail.cid[plan_detail.all_citynum]);
    }
    return strlist;
}

//计时器状态
QStringList to_logstring_timer(int timer_state,int day,int hour)
{
    QStringList strlist;

    strlist << QString::number(day) << QString::number(hour);

    if(timer_state == 1){
        strlist << "计时器开始运行";
    }
    else if(timer_state == 2){
        strlist << "计时器已暂停";
    }
    else if(timer_state == 3){
        strlist << "计时器已清零并停止,系统状态重置";
    }

    return strlist;
}

/*将得到处理后的字符串列表根据不同种类日志记录的规则进行输出*/
void log_file(int markkind, QStringList aline){
    //将即将输入到日志文件里的记录保存到主窗口日志滚动窗的传送队列中
    log_TranstoMView slog_view;
    slog_view.markind = markkind;
    slog_view.details = aline;
    queue_log_view.push(slog_view);

    //打开日志文件，以只写模式非覆盖形式写入新的日志记录
    QFile file("log_file.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream stream(&file);
    stream.setCodec("utf-8");

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");

    if(markkind == KIND_system)
    {
        stream << aline[0];
        stream << current_date << QString(" 程序类记录 ") << aline[1] << "\n";
        stream << aline[2];
    }
    if(markkind == KIND_input)
    {
        stream << current_date << QString(" 输入类记录 ");
        stream << QString("姓名：") << aline[0];
        stream << QString(" 起点：") << aline[1] << QString(" 终点：") << aline[2];
        stream << QString(" 出发时间：") << aline[3] << QString(" day") << aline[4] << QString(" hour");
        stream << QString(" 风险模式：") << aline[5] << aline[6] << aline[7] <<"\n";
    }
    if(markkind == KIND_traveler)
    {
        stream << current_date << QString(" 旅客类记录 ");
        stream << QString("当前时间：") << aline[0] << QString(" day") << aline[1] << QString(" hour");
        stream << QString(" 姓名：") << aline[2];
        stream << QString(" 状态：") << aline[3] << aline[4] << "\n";
    }
    if(markkind == KIND_timer)
    {
        stream << current_date << QString(" 计时类记录 ");
        stream << QString("当前时间：") << aline[0] << QString(" day") << aline[1] << QString(" hour ");
        stream << aline[2] << "\n";
    }
    if(markkind == KIND_travelplan)
    {
        stream << current_date << QString(" 计划类记录 ");
        for(int i = 0; i < aline.length();i++){
            stream << aline[i];
        }
        stream << "\n";
    }

    file.close();
}
