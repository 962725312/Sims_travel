/* 初始化模块 */
#include <cstdio>
#include "init_data.h"
#include "Core_sims.h"
#include <QFile>
#include <QTextStream>

city_lines city_line[MAX_Citynum];

using namespace std;

//构造函数，创建地图对象实体时的初始化设置
city_map::city_map(void)
{
    //QMap自动根据 key排序
    int cid = 0;
    City_Zone.insert("吉林", cid++);
    City_Zone.insert("长春", cid++);
    City_Zone.insert("齐齐哈尔", cid++);
    City_Zone.insert("哈尔滨", cid++);
    City_Zone.insert("丹东", cid++);
    City_Zone.insert("沈阳", cid++);
    City_Zone.insert("亳州", cid++);
    City_Zone.insert("合肥", cid++);
    City_Zone.insert("北京", cid++);
    City_Zone.insert("山东", cid++);
    City_Zone.insert("厦门", cid++);
    City_Zone.insert("河南", cid++);
    City_Zone.insert("江西", cid++);
}

//构造函数，创建对象实体时的初始化设置
city_lines::city_lines(void)
{
    start_cid = 0;
    lines_num = 0;
}

//构造函数，创建对象实体时的初始化设置
line_detail::line_detail(void)
{
    end_cid = 0;
    start_hour = 0;
    timecost = 0;
    timefre_hour = 0;
    transport_kind = 0;
}

//每个城市的坐标，索引为城市id
QPair<int,int> city_position[MAX_Citynum];

//城市在地图中的左标位置初始化
void position_init(){
    int cid = 0;
    city_position[cid].first = 510;
    city_position[cid++].second = 110;
    city_position[cid].first = 545;
    city_position[cid++].second = 185;
    city_position[cid].first = 240;
    city_position[cid++].second = 55;
    city_position[cid].first = 380;
    city_position[cid++].second = 70;
    city_position[cid].first = 610;
    city_position[cid++].second = 280;
    city_position[cid].first = 500;
    city_position[cid++].second = 305;
    city_position[cid].first = 100;
    city_position[cid++].second = 285;
    city_position[cid].first = 130;
    city_position[cid++].second = 170;
    city_position[cid].first = 415;
    city_position[cid++].second = 205;
    city_position[cid].first = 255;
    city_position[cid++].second = 210;
    city_position[cid].first = 435;
    city_position[cid++].second = 430;
    city_position[cid].first = 280;
    city_position[cid++].second = 320;
    city_position[cid].first = 215;
    city_position[cid++].second = 405;
}


//导入交通班次文件
void lines_load(){
    //暂存变量
    int sstart_id = 0;
    int sline_num = 0;
    line_detail sline_detail;

    //打开文件，以只读模式本文流读取
    QFile file(":/theroad/init_lines_map");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    while(!stream.atEnd()){
        //获取一行文本，并将其分为n个数据放入QStrlinglist对象中
        QString aline = stream.readLine();
        QStringList aline_list = aline.split(" ");

        //将读取的数据放入暂存变量中
        sstart_id = aline_list[0].toInt();
        sline_detail.end_cid = aline_list[1].toInt();
        sline_detail.transport_kind = aline_list[2].toInt();
        sline_detail.start_hour = aline_list[3].toInt();
        sline_detail.timecost = aline_list[4].toInt();
        sline_detail.timefre_hour = aline_list[5].toInt();

        //初始化对应的city_line[]
        city_line[sstart_id].start_cid = sstart_id;
        sline_num = city_line[sstart_id].lines_num;
        city_line[sstart_id].detail[sline_num] = sline_detail;
        city_line[sstart_id].lines_num++;//每读取一条班次信息,起点城市的线路数量就+1
    }
    //关闭文件流
    file.close();
}

