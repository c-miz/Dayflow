#include "ScheduleItem.h"

ScheduleItem::ScheduleItem(QString title, QTime start, QTime end,QDate d, RepeatType repeat)
    : title(title), startTime(start), endTime(end),date(d), repeatType(repeat) {}

bool ScheduleItem::isValid() const {
    return startTime < endTime;
}

