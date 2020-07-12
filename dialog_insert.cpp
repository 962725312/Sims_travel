/* 对话框窗口模块 */
#include "dialog_insert.h"
#include "ui_dialog_insert.h"
#include "init_data.h"
#include "Log.h"

using namespace std;

//对话框窗口构造函数
Dialog_insert::Dialog_insert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_insert)
{
    ui->setupUi(this);
    //初始化具有自定义数据的comboBox-start_place_edit,end_place_edit
    city_map citymap;
    ui->start_place_edit->clear();
    ui->end_place_edit->clear();
    ui->riskymode_edit->clear();
    foreach(const QString &str,citymap.City_Zone.keys())
        ui->start_place_edit->addItem(str,citymap.City_Zone.value(str));
    foreach(const QString &str,citymap.City_Zone.keys())
        ui->end_place_edit->addItem(str,citymap.City_Zone.value(str));

    //初始化具有自定义数据的combobox-riskymode_edit
    QString str1 = "风险最低";
    QString str2 = "限定时间风险最低";
    ui->riskymode_edit->addItem(str1,1);
    ui->riskymode_edit->addItem(str2,2);

    //初始化具有自定义数据的combobox-transport_consider
    QString str3 = "考虑";
    QString str4 = "不考虑";
    ui->transport_consider->addItem(str3,1);
    ui->transport_consider->addItem(str4,0);

    connect(ui->ensure_button,SIGNAL(clicked()),this,SLOT(offer_data()));//当点击确认按钮时，调用提供数据的函数
    connect(ui->ensure_button,SIGNAL(clicked()),this,SLOT(close()));//当点击确认按钮时，并完成数据传送后，关闭此窗口
}

//析构函数
Dialog_insert::~Dialog_insert()
{
    delete ui;
}

//传输数据给主窗口的函数
void Dialog_insert::offer_data(){
    Traveler_input straveler_input;    //暂存变量
    //保存从窗口获得的数据
    straveler_input.name = ui->nam_edit->text();
    straveler_input.start_cid = ui->start_place_edit->currentData().toInt();
    straveler_input.end_cid = ui->end_place_edit->currentData().toInt();
    straveler_input.day = ui->day_spinbox->value();
    straveler_input.hour = ui->hour_spinbox->value();
    straveler_input.safe_mode = ui->riskymode_edit->currentData().toInt();
    straveler_input.time_limit = ui->timelimit_spinBox->value();
    straveler_input.transport_consider = ui->transport_consider->currentData().toInt();
    straveler_input.legal_flag = 1;

    //打印日志
    log_file(KIND_input,to_logstring_input(straveler_input));

    //将此函数与offer_data_toMainwindow信号函数相连接，每当调用此函数时，都会触发offer_data_toMainwindow信号
    qRegisterMetaType<QVariant>("QVariant");
    QVariant DataVar;
    DataVar.setValue(straveler_input);//将要传递的数据包装成通用类型
    emit offer_data_toMainwindow(DataVar);
}
