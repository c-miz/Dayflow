#ifndef SCHEDULEITEM_H
#define SCHEDULEITEM_H

#include <QTime>
#include <QString>
#include <QDate>
#include <QStringList>

enum RepeatType { None, Daily, Weekly, Monthly };

class ScheduleItem
{
public:
    ScheduleItem(QString title = "",
                 QTime start = QTime::currentTime(),
                 QTime end = QTime::currentTime().addSecs(3600),
                 QDate d = QDate::currentDate(),
                 RepeatType repeat = None,
                 QStringList tags = QStringList(),
                 int priority=0,
                 int advanceAmount=5); // 添加 tags,priority参数

    QString title;
    QTime startTime;
    QTime endTime;
    QDate date;
    RepeatType repeatType;
    QStringList tags;
    int priority;// 新增 tags,priority 成员
    int advanceAmount;

    // 验证时间有效性
    bool isValid() const;
};

#endif // SCHEDULEITEM_H
