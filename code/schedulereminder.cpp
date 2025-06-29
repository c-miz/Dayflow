#include <Qobject>
#include <QDateTime>
#include <QTimer>
#include "schedulemodel.h"
#include "schedulereminder.h"
#include <iostream>

ScheduleReminder::ScheduleReminder(ScheduleModel* model,QObject *parent) : QObject(parent),m_model(model){
    checkTimer.setInterval(60000); // 60秒
    connect(&checkTimer, &QTimer::timeout, this, &ScheduleReminder::checkSchedules);
    checkTimer.start();
}
void ScheduleReminder::checkSchedules(){
    const QTime currentTime = QTime::currentTime();
    const QDate currentDate=QDate::currentDate();
    for(int i = 0; i < m_model->rowCount(); ++i) {
        const ScheduleItem &item = m_model->getItem(i);
        QTime scheduletime=item.startTime.addSecs(-60*item.advanceAmount);
        QDate scheduleDate=item.date;
        bool checkDate=false;
        if(currentDate==scheduleDate){checkDate=true;}
        else if(item.repeatType!=None){
            if(scheduleDate<currentDate){
                if(item.repeatType==Daily){checkDate=true;}

                else if(item.repeatType==Weekly){
                    if(scheduleDate.daysTo(currentDate)%7==0){
                        checkDate=true;
                    }
                }

                else if(item.repeatType==Monthly){
                    if(scheduleDate.day()==currentDate.day()){
                        checkDate=true;
                    }
                }
            }
        }

        if( checkDate==true&&currentTime >= scheduletime.addSecs(-60)&&currentTime<scheduletime) {
            // 触发提醒
            QString msg = QString("日程提醒：%1 即将在%2分钟后开始")
                              .arg(item.title)
                              .arg(item.advanceAmount);
            std::cout<<"remindertriggered";

            emit triggerReminder(msg);
        }
    }
}
