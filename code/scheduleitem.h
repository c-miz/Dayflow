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
    QString category;
    ScheduleItem(QString title = "",
                 QTime start = QTime::currentTime(),
                 QTime end = QTime::currentTime().addSecs(3600),
                 QDate d = QDate::currentDate(),
                 RepeatType repeat = None,
                 QStringList tags = QStringList(),
                 int priority=0,
                 int advanceAmount=5,
                 QString cat="");

    QString title;
    QTime startTime;
    QTime endTime;
    QDate date;
    RepeatType repeatType;
    QStringList tags;
    int priority;
    int advanceAmount;

    // 验证时间有效性
    bool isValid() const;
    QJsonObject toJson() const;
    static ScheduleItem fromJson(const QJsonObject &json);
};

#endif // SCHEDULEITEM_H
