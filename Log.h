#ifndef LOG_H
#define LOG_H

//程序运行状态的标志
#define SystemSTART 1
#define SystemSTOP  0

//日志记录的种类
#define KIND_system     0
#define KIND_input      1
#define KIND_traveler   2
#define KIND_timer      3
#define KIND_travelplan 4

#include "Core_sims.h"
#include <QTimer>
#include <QTime>
#include <QStringList>
#include <QTextBrowser>

QStringList to_logstring_system(int state);
QStringList to_logstring_input(Traveler_input input_detail);
QStringList to_logstring_travelplan(Traveler_plan plan_detail);
QStringList to_logstring_traveler(Traveler_plan plan_detail,int day,int hour);
QStringList to_logstring_timer(int timer_state,int day,int hour);

void log_file(int markkind, QStringList aline);
#endif // LOG_H
