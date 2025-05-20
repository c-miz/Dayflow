#ifndef SCHEDULEITEM_H
#define SCHEDULEITEM_H

#include <QTime>
#include <QString>


enum RepeatType { None, Daily, Weekly, Monthly };

class ScheduleItem
{
public:
    ScheduleItem(QString title = "",
                 QTime start = QTime::currentTime(),
                 QTime end = QTime::currentTime().addSecs(3600),
                 QDate d=QDate::currentDate(),
                 RepeatType repeat = None);

    QString title;
    QTime startTime;
    QTime endTime;
    QDate date;
    RepeatType repeatType;

    // 验证时间有效性
    bool isValid() const;
};

#endif // SCHEDULEITEM_H
