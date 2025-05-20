#include "ScheduleModel.h"
#include <QDate>

ScheduleModel::ScheduleModel(QObject *parent) : QAbstractTableModel(parent) {}

int ScheduleModel::rowCount(const QModelIndex &) const { return items.size(); }

int ScheduleModel::columnCount(const QModelIndex &) const { return 5; }

QVariant ScheduleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const ScheduleItem &item = items[index.row()];
    switch(index.column()) {
    case 0: return item.title;
    case 1: return item.date.toString("yyyy-MM-dd");
    case 2: return item.startTime.toString("HH:mm");
    case 3: return item.endTime.toString("HH:mm");
    case 4:
        switch(item.repeatType) {
        case None: return "无";
        case Daily: return "每天";
        case Weekly: return "每周";
        case Monthly: return "每月";
        }
    }
    return QVariant();
}

QVariant ScheduleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case 0: return "标题";
        case 1: return "日期";
        case 2: return "开始时间";
        case 3: return "结束时间";
        case 4: return "重复类型";
        }
    }
    return QVariant();
}

void ScheduleModel::addItem(const ScheduleItem &item)
{
    beginInsertRows(QModelIndex(), items.size(), items.size());
    items.append(item);
    endInsertRows();
    emit modelChanged();
}

void ScheduleModel::removeItem(int row)
{
    if (row < 0 || row >= items.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    items.removeAt(row);
    endRemoveRows();
}

ScheduleItem ScheduleModel::getItem(int row) const
{
    if (row >=0 && row < items.size())
        return items.at(row);
    return ScheduleItem();
}

QVector<ScheduleItem> ScheduleModel::getSchedulesForDate(const QDate& date) const
{
    QVector<ScheduleItem> result;
    for (const auto& item : items) {
        QDate startDate=item.date;
        QDate maxDate=QDate::currentDate();
        maxDate=maxDate.addYears(10);
        // 处理重复逻辑
        if (item.repeatType == None) {
            if (startDate==date)
                result.append(item);
        } else if(item.repeatType==Daily){

            QDate current = startDate;
            while(current<=maxDate) {
                if (current == date) {
                    result.append(item);
                    break;
                }
                current = current.addDays(1);
            }
        }
        else if(item.repeatType==Weekly){

            QDate current = startDate;
            while(current<=maxDate) {
                if (current == date) {
                    result.append(item);
                    break;
                }
                current = current.addDays(7);
            }
        }
        else if(item.repeatType==Monthly){
            QDate current = startDate;
            while(current<=maxDate) {
                if (current == date) {
                    result.append(item);
                    break;
                }
                current = current.addMonths(1);
            }
        }
    }
    return result;
}


