#include "ScheduleItem.h"

ScheduleItem::ScheduleItem(QString title, QTime start, QTime end, QDate d, RepeatType repeat, QStringList tags,int priority,int adAm) // 添加 tags,priority
    : title(title), startTime(start), endTime(end), date(d), repeatType(repeat), tags(tags),priority(priority),advanceAmount(adAm) {} // 初始化 tags,priority

bool ScheduleItem::isValid() const {
    return startTime < endTime;
}
