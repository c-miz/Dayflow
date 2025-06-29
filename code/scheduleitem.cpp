#include "ScheduleItem.h"
#include <QJsonArray>
#include "QJsonObject"

ScheduleItem::ScheduleItem(QString title, QTime start, QTime end, QDate d, RepeatType repeat, QStringList tags,int priority,int adAm,QString cat) // 添加 tags,priority
    : title(title), startTime(start), endTime(end), date(d), repeatType(repeat), tags(tags),priority(priority),advanceAmount(adAm) ,category(cat){} // 初始化 tags,priority

bool ScheduleItem::isValid() const {
    return startTime < endTime;
}

QJsonObject ScheduleItem::toJson() const
{
    QJsonObject obj;
    obj["title"] = title;
    obj["start"] = startTime.toString("hh:mm:ss");
    obj["end"] = endTime.toString("hh:mm:ss");
    obj["date"] = date.toString("yyyy-MM-dd");
    obj["repeat"] = static_cast<int>(repeatType);

    QJsonArray tagsArray;
    foreach(const QString &tag, tags) {
        tagsArray.append(tag);
    }
    obj["tags"] = tagsArray;

    obj["priority"] = priority;
    obj["advance"] = advanceAmount;
    obj["category"] = category;
    return obj;
}

ScheduleItem ScheduleItem::fromJson(const QJsonObject &json)
{
    ScheduleItem item;

    item.title = json["title"].toString();
    item.startTime = QTime::fromString(json["start"].toString(), "hh:mm:ss");
    item.endTime = QTime::fromString(json["end"].toString(), "hh:mm:ss");
    item.date = QDate::fromString(json["date"].toString(), "yyyy-MM-dd");
    item.repeatType = static_cast<RepeatType>(json["repeat"].toInt(0));

    QJsonArray tagsArray = json["tags"].toArray();
    foreach(const QJsonValue &val, tagsArray) {
        item.tags.append(val.toString());
    }

    item.priority = json["priority"].toInt(0);
    item.advanceAmount = json["advance"].toInt(5);
    item.category = json["category"].toString();

    // 处理可能存在的无效时间
    if(!item.startTime.isValid()) item.startTime = QTime::currentTime();
    if(!item.endTime.isValid()) item.endTime = item.startTime.addSecs(3600);
    if(!item.date.isValid()) item.date = QDate::currentDate();

    return item;
}
