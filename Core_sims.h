#ifndef CORE_SIMS_H
#define CORE_SIMS_H

//风险模式
#define LeastRisk_Mode 1
#define TimeLimited_Mode 2

#define NameLen_Limit 20 //姓名长度限制
#define MAX_Citynum 20 //最大城市数限制

//交通工具种类
#define Car_Kind 1
#define Train_Kind 2
#define Plane_Kind 3

//城市风险度
#define LOW_safety 2
#define MID_safety 5
#define HIGH_safety 9

//旅客所处状态
#define ON_traffic 1
#define ON_await 0

//风险度计算是否考虑交通工具的风险
#define Considertrans_Yes 1
#define Considertrans_No  0

//最大风险度
#define MAX_distance 999999

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <QString>
#include <queue>
#include <QMetaType>

class Traveler_input
{
public:
    QString name;//旅行者姓名
    int start_cid;//出发城市的id
    int end_cid;//目的城市的id
    int day,hour;//出发时间
    int safe_mode;//旅行安全策略
    int time_limit;//限时风险策略的时间限制
    int transport_consider;//是否考虑交通工具风险
    int legal_flag;//判断输入是否合法的标志
};

Q_DECLARE_METATYPE(Traveler_input);//注册Traveler_input为元类型

class Traveler_plan
{
public:
    QString name;//旅行者姓名
    int start_day,start_hour;//出发时间
    int end_day,end_hour;//到达时间
    int safe_mode;//旅行安全策略
    int workin_safemode;//判断在限时模式下能否找到方案
    int transport_consider;//是否考虑交通工具风险

    int corrent_cid;//当前所在城市在cid[]中的序号
    int corrent_tid;//当前所乘交通工具在tid[]中的序号   
    int cid_work;//判断是在等待班次还是在交通班次上

    int all_citynum;//旅客完成本次旅行需要走过的路径数（即城市数-1）
    //cid[i]与tid[i]一一对应,即在cid[i]的城市，下一个要做的交通工具为tid[i],i <= all_citynum
    int cid[MAX_Citynum];//储存计算所得的路线经过的城市序列
    int tid[MAX_Citynum];//储存计算所得的路线乘坐的的交通序列
    long setout_time[MAX_Citynum];//从i出发的时间戳
    long arrive_time[MAX_Citynum];//到达i的时间戳
};

void safety_init();

Traveler_plan cal_traveler_plan(Traveler_input tra_input);

#endif // CORE_SIMS_H
