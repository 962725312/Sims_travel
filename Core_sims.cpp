/* 旅行路线设计模块 */
#include "Log.h"
#include "init_data.h"
#include <QMap>

using namespace std;

int city_num = 13;//地图中的城市总量
QMap<int,int> city_safety;//每个城市的安全程度，以id为key查询

//城市安全度初始化
void safety_init(){
    int cid = 0;
    city_safety.insert(cid++,LOW_safety);
    city_safety.insert(cid++,MID_safety);
    city_safety.insert(cid++,HIGH_safety);
    city_safety.insert(cid++,LOW_safety);
    city_safety.insert(cid++,MID_safety);
    city_safety.insert(cid++,HIGH_safety);
    city_safety.insert(cid++,LOW_safety);
    city_safety.insert(cid++,MID_safety);
    city_safety.insert(cid++,HIGH_safety);
    city_safety.insert(cid++,LOW_safety);
    city_safety.insert(cid++,MID_safety);
    city_safety.insert(cid++,HIGH_safety);
    city_safety.insert(cid++,MID_safety);
}

//根据旅客输入的数据计算最低风险行程
Traveler_plan cal_traveler_plan(Traveler_input tra_input){
    Traveler_plan tra_plan;//输出的暂存变量
    tra_plan.workin_safemode = 1;//初始化默认有解决方案
    tra_plan.cid_work = ON_await;
    long start_timetrump = tra_input.day * 24 + tra_input.hour;//出发时间的时间戳
    int start = tra_input.start_cid;//旅程起点
    int end = tra_input.end_cid;//旅程终点
    int trans_risk = tra_input.transport_consider;//是否考虑交通风险
    int risk[4] = {0,20,50,90};//每种交通工具对应风险度*10

    //最低风险模式的寻路
    if(tra_input.safe_mode == LeastRisk_Mode){

        //最低风险模式的特殊变量（可以将其考虑为无限时间的的限定时间最低风险模式，即可以直接使用限定时间最低风险模式的一部分，但考虑到代码可读性和耦合性，将其分开设置）
        long distance[city_num];//用于储存每个点到起点的距离
        int line_num[city_num];//用于储存从起点到每个点需要走过的路径数
        int time_trump[city_num];//用于储存每个点到起点的时间间隔
        int former_gotime[city_num];//用于储存到达一个点时，它前一个点的出发时间与最初时间的间隔
        int seen[city_num];//访问标记，记录是否已经找到从起点到达该点的最短路径
        int former_city[city_num];//key为当前节点，value为走最短路径到达当前节点的上一个节点
        int former_line[city_num];//key为当前节点，value为走最短路径到达当前节点的上一个班次

        //变量初始化
        for(int i = 0; i < city_num; i++){
            distance[i] = MAX_distance;
            seen[i] = 0;
        }
        distance[start] = 0;
        time_trump[start] = 0;
        former_gotime[start] = -1;
        former_city[start] = -1;
        former_line[start] = -1;
        line_num[start] = 0;

        //遍历city_num-1次；每次找出一个顶点的最短路径
        for(int i = 1; i < city_num; i++){
            int mindist = MAX_distance;// 当前最小的路径距离
            int mindist_city = 0;
            // 寻找当前最小的路径；
            // 即，在未获取最短路径的顶点中，找到离vs最近的顶点(mindist_city)
            for(int j = 0; j < city_num; j++){
                if(distance[j] < mindist && !seen[j])
                {
                    mindist = distance[j];
                    mindist_city = j;
                }
            }
            //标记"顶点mindist_city"为已经获取到最短路径
            seen[mindist_city] = 1;

            // 修正当前最短路径和前驱顶点
            // 即，当已经获取"顶点k的最短路径"之后，更新"未获取最短路径的顶点的最短路径和前驱顶点"
            long sday = ( start_timetrump + time_trump[mindist_city] ) / 24;
            long shour = ( start_timetrump + time_trump[mindist_city] ) % 24;
            for(int j = 0; j < city_line[mindist_city].lines_num; j++){
                long wait_time;//坐j班次的等待时间
                int end_cid = city_line[mindist_city].detail[j].end_cid;
                int timefre_day = ( city_line[mindist_city].detail[j].timefre_hour) / 24;
                //计算等待时间
                //判断当天是否有此班次
                if(sday % timefre_day == 0)//如果有此班次的情况
                {
                    //判断当天班次是否赶得上
                    if(shour < city_line[mindist_city].detail[j].start_hour)//如果赶得上的话
                    {
                        wait_time = city_line[mindist_city].detail[j].start_hour - shour;
                    }
                    else//如果赶不上的话，就要坐本班次的下一班
                    {
                        wait_time = city_line[mindist_city].detail[j].timefre_hour + city_line[mindist_city].detail[j].start_hour - shour;
                    }
                }
                else//如果当天没有此班次
                {
                    wait_time = city_line[mindist_city].detail[j].start_hour - (sday % timefre_day)*24 + city_line[mindist_city].detail[j].timefre_hour - shour;
                }

                //计算风险值(即路的权值)
                long thedist_trans = trans_risk * (city_safety.value(mindist_city) * risk[city_line[mindist_city].detail[j].transport_kind] * city_line[mindist_city].detail[j].timecost);
                long thedist = wait_time * city_safety.value(mindist_city) + city_safety.value(mindist_city) + thedist_trans;
                //更新
                if(mindist + thedist < distance[end_cid] && !seen[end_cid])
                {
                    distance[end_cid] = mindist + thedist;
                    time_trump[end_cid] = time_trump[mindist_city] + wait_time + city_line[mindist_city].detail[j].timecost;
                    former_gotime[end_cid] = time_trump[mindist_city] + wait_time;
                    former_city[end_cid] = mindist_city;
                    former_line[end_cid] = j;
                    line_num[end_cid] = line_num[mindist_city] + 1;
                }
            }
        }
        //将所求得的结果用tra_plan暂存起来
        tra_plan.name = tra_input.name;
        tra_plan.start_day = tra_input.day;
        tra_plan.start_hour = tra_input.hour;
        tra_plan.safe_mode = tra_input.safe_mode;
        tra_plan.end_day = (start_timetrump + time_trump[end]) / 24;
        tra_plan.end_hour = (start_timetrump + time_trump[end]) % 24;
        tra_plan.all_citynum = line_num[end];
        tra_plan.corrent_cid = 0;
        tra_plan.corrent_tid = 0;
        for(int i = line_num[end], thecid = end; i >= 1 ;i--){
            tra_plan.cid[i] = thecid;//从i到1
            tra_plan.tid[i-1] = former_line[thecid];//从i-1到0
            tra_plan.arrive_time[i] = time_trump[thecid];//从i到1
            tra_plan.setout_time[i-1] = former_gotime[thecid];//从i-1到0
            thecid = former_city[thecid];
        }
        //tra_plan中的的特殊处理
        tra_plan.cid[0] = start;
        tra_plan.arrive_time[0] = 0;
        tra_plan.transport_consider = trans_risk;
    }
    else if(tra_input.safe_mode == TimeLimited_Mode){
        int rest_time = tra_input.time_limit;
        //限定时间最低风险的特殊变量,二维数组储存，xxxx[i][j]，i表示城市序号，j表示旅行的剩余时间
        long distance[city_num][rest_time + 1];//用于储存每个点到起点的最短距离
        int line_num[city_num][rest_time + 1];//用于储存从起点到每个点需要走过的路径数
        int time_trump[city_num][rest_time + 1];//用于储存到达一个点时，它前一个点的到达时间与最初时间的间隔
        int former_gotime[city_num][rest_time + 1];//用于储存到达一个点时，它前一个点的出发时间与最初时间的间隔
        int seen[city_num][rest_time + 1];//访问标记，记录是否已经找到从起点到达该点的最短路径
        int former_city[city_num][rest_time + 1];//key为当前节点，value为走最短路径到达当前节点的上一个节点
        int former_line[city_num][rest_time + 1];//key为当前节点，value为走最短路径到达当前节点的上一个班次

        //变量初始化
        for(int i = 0; i < city_num; i++){
            for(int j = 0; j <= rest_time;j++){
                distance[i][j] = MAX_distance;
                seen[i][j] = 0;
            }
        }
        distance[start][rest_time] = 0;
        time_trump[start][rest_time] = -1;
        former_gotime[start][rest_time] = -1;
        former_city[start][rest_time] = -1;
        former_line[start][rest_time] = -1;
        line_num[start][rest_time] = 0;

        while(true){
            int mindist = MAX_distance;// 当前最小的路径距离
            int mindist_city,mindist_time;
            // 寻找当前最小的路径；
            // 即，在未获取最短路径的顶点中，找到离vs最近的顶点(mindist_city)
            for(int i = 0; i < city_num; i++){
                for(int j = 0; j <= rest_time; j++){
                    if(distance[i][j] < mindist && !seen[i][j])
                    {
                        mindist = distance[i][j];
                        mindist_city = i;
                        mindist_time = j;
                    }
                }
            }
            //如果所有可达的distance[i][j]都已经标记完,就结束寻找
            if(mindist == MAX_distance)
            {
                break;
            }
            //标记"顶点mindist_city"在剩余时间为"mindist_time"已经获取到最短路径
            for(int i = 0; i <= mindist_time;i++){
                seen[mindist_city][i] = 1;
            }

            // 修正当前最短路径和前驱顶点
            // 即，当已经获取"顶点k的最短路径"之后，更新"未获取最短路径的顶点的最短路径和前驱顶点"
            for(int j = 0; j < city_line[mindist_city].lines_num; j++){
                long wait_time;//坐j班次的等待时间
                int theline_time = city_line[mindist_city].detail[j].timecost;
                int end_cid = city_line[mindist_city].detail[j].end_cid;
                int timefre_day = ( city_line[mindist_city].detail[j].timefre_hour) / 24;
                //计算等待时间
                //判断当天是否有此班次
                long sday = ( start_timetrump + rest_time - mindist_time ) / 24;
                long shour = ( start_timetrump + rest_time - mindist_time ) % 24;
                if(sday % timefre_day == 0)//如果有此班次的情况
                {
                    //判断当天班次是否赶得上
                    if(shour < city_line[mindist_city].detail[j].start_hour)//如果赶得上的话
                    {
                        wait_time = city_line[mindist_city].detail[j].start_hour - shour;
                    }
                    else//如果赶不上的话，就要坐本班次的下一班
                    {
                        wait_time = city_line[mindist_city].detail[j].timefre_hour + city_line[mindist_city].detail[j].start_hour - shour;
                    }
                }
                else//如果当天没有此班次
                {
                    wait_time = city_line[mindist_city].detail[j].start_hour - (sday % timefre_day)*24 + city_line[mindist_city].detail[j].timefre_hour - shour;
                }

                //计算风险值(即路的权值)
                long thedist_trans = trans_risk * (city_safety.value(mindist_city) * risk[city_line[mindist_city].detail[j].transport_kind] * city_line[mindist_city].detail[j].timecost);
                long thedist = wait_time * city_safety.value(mindist_city) + city_safety.value(mindist_city) + thedist_trans;
                //更新
                int all_costtime = wait_time + theline_time;
                if(mindist_time - all_costtime >=0 && mindist + thedist < distance[end_cid][mindist_time - all_costtime] && !seen[end_cid][mindist_time - all_costtime])
                {
                    distance[end_cid][mindist_time - all_costtime] = mindist + thedist;
                    time_trump[end_cid][mindist_time - all_costtime] = rest_time - mindist_time;
                    former_gotime[end_cid][mindist_time - all_costtime] = rest_time - mindist_time + wait_time;
                    former_city[end_cid][mindist_time - all_costtime] = mindist_city;
                    former_line[end_cid][mindist_time - all_costtime] = j;
                    line_num[end_cid][mindist_time - all_costtime] = line_num[mindist_city][mindist_time] + 1;
                }

            }
        }

        //将所求得的结果用tra_plan暂存起来
        tra_plan.name = tra_input.name;
        tra_plan.start_day = tra_input.day;
        tra_plan.start_hour = tra_input.hour;
        tra_plan.safe_mode = tra_input.safe_mode;

        //找出distance[end_cid][j>=0]中最小的
        int mindist = MAX_distance;// 当前最小的路径距离
        int mindist_time;
        for(int i = 0; i <= rest_time; i++){
            if(distance[end][i] < mindist)
            {
                mindist = distance[end][i];
                mindist_time = i;
            }
        }
        //找不到可以在限定时间内的解决方案
        if(mindist == MAX_distance)
        {
            tra_plan.workin_safemode = 0;
            return tra_plan;
        }

        //将所求得的结果用tra_plan暂存起来
        tra_plan.end_day = (start_timetrump + rest_time - mindist_time) / 24;
        tra_plan.end_hour = (start_timetrump + rest_time - mindist_time) % 24;
        tra_plan.all_citynum = line_num[end][mindist_time];
        tra_plan.corrent_cid = 0;
        tra_plan.corrent_tid = 0;
        for(int i = line_num[end][mindist_time], thecid = end; i >= 1 ;i--){
            tra_plan.cid[i] = thecid;//从i到1
            tra_plan.tid[i-1] = former_line[thecid][mindist_time];//从i-1到0
            tra_plan.arrive_time[i] = rest_time - mindist_time;//从i到1
            tra_plan.setout_time[i-1] = former_gotime[thecid][mindist_time];//从i-1到0
            int old_mindist_time = mindist_time;
            mindist_time = rest_time - time_trump[thecid][mindist_time];//更新为前一个点的剩余时间
            thecid = former_city[thecid][old_mindist_time];//更新为前一个点的cid
        }
        //tra_plan的特殊处理
        tra_plan.cid[0] = start;
        tra_plan.arrive_time[0] = 0;
        tra_plan.transport_consider = trans_risk;
    }
    return tra_plan;
}



