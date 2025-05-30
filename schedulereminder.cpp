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
    const QDateTime current = QDateTime::currentDateTime();
    for(int i = 0; i < m_model->rowCount(); ++i) {
        const ScheduleItem &item = m_model->getItem(i);
        QDateTime scheduletime(item.date,item.startTime);
        scheduletime=scheduletime.addSecs(-60*item.advanceAmount);

        if( current >= scheduletime&&current<=scheduletime.addSecs(60)) {
            // 触发提醒
            QString msg = QString("日程提醒：%1 即将在%2分钟后开始")
                              .arg(item.title)
                              .arg(item.advanceAmount);
            std::cout<<"remindertriggered";

            emit triggerReminder(msg);
        }
    }
}
