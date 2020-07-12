#ifndef INIT_DATA_H
#define INIT_DATA_H
#define MAX_aCityLines 60 //每个城市的最大班次数量限制
#include <QMap>

class city_map
{
public:
    QMap<QString, int> City_Zone;//城市名称与id的对应
    city_map();
};

class line_detail
{
public:
    int end_cid;//终点城市
    int start_hour;//出发时间
    int timecost;//班次花费时间
    int timefre_hour;//班次间隔时间
    int transport_kind;//班次交通工具种类
    line_detail();//构造函数
};

class city_lines
{
public:
    int start_cid;//城市id
    int lines_num;//以该城市为起点的班次数量
    line_detail detail[MAX_aCityLines];//每个班次的细节
    city_lines();//构造函数
};

extern city_lines city_line[];
extern QPair<int,int> city_position[];

void lines_load();
void position_init();

#endif // INIT_DATA_H
