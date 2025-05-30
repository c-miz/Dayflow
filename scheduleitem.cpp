#include "ScheduleItem.h"


ScheduleItem::ScheduleItem(QString title, QTime start, QTime end,QDate d, RepeatType repeat,int adAm)
    : title(title), startTime(start), endTime(end),date(d), repeatType(repeat),advanceAmount(adAm){
}

bool ScheduleItem::isValid() const {
    return startTime < endTime;
}

