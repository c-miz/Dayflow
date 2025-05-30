// ScheduleFilterProxyModel.cpp
#include "ScheduleFilterProxyModel.h"
#include "ScheduleModel.h"
#include "filterdialog.h"

ScheduleFilterProxyModel::ScheduleFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {}

void ScheduleFilterProxyModel::setFilterCriteria(const FilterCriteria &criteria) {
    m_criteria = criteria;
    m_filterActive = true;
    invalidateFilter();  //对所有行决定是否显示
}

bool ScheduleFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (!m_filterActive) {
        return true; //如果不进行筛选，则显示所有事务
    }
    //获取原模型
    ScheduleModel *sModel = qobject_cast<ScheduleModel*>(sourceModel());
    if (!sModel) {
        return false;
    }
    ScheduleItem item = sModel->getItem(sourceRow);

    // 日期匹配
    if (!dateMatches(item, m_criteria.date)) {
        return false;
    }

    //Tag匹配
    if (m_criteria.tag1 != "无" && !item.tags.contains(m_criteria.tag1)) {
        return false;
    }
    if (m_criteria.tag2 != "无" && !item.tags.contains(m_criteria.tag2)) {
        return false;
    }
    if (m_criteria.tag3 != "无" && !item.tags.contains(m_criteria.tag3)) {
        return false;
    }

    // Priority匹配
    if (item.priority != m_criteria.priority) {
        return false;
    }

    return true;
}

//检查日期匹配的辅助函数
bool ScheduleFilterProxyModel::dateMatches(const ScheduleItem& item, const QDate& filterDate) const {
    QDate current = item.date;
    if (item.repeatType == RepeatType::None) {
        return current == filterDate;
    }
    while (current <= filterDate) {
        if (current == filterDate) {
            return true;
        }
        QDate nextCurrent = current;
        switch (item.repeatType) {
        case RepeatType::Daily:
            nextCurrent = current.addDays(1);
            break;
        case RepeatType::Weekly:
            nextCurrent = current.addDays(7);
            break;
        case RepeatType::Monthly:
            nextCurrent = current.addMonths(1);
            break;
        case RepeatType::None:
            return false;
        }
        if (nextCurrent <= current && item.repeatType != RepeatType::None) {
            return false;
        }
        current = nextCurrent;
    }
    return false;
}
