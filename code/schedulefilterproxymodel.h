// ScheduleFilterProxyModel.h
#ifndef SCHEDULEFILTERPROXYMODEL_H
#define SCHEDULEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>
#include "filterdialog.h"
#include "ScheduleItem.h"

class ScheduleFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit ScheduleFilterProxyModel(QObject *parent = nullptr);
    void setFilterCriteria(const FilterCriteria &criteria);
    const FilterCriteria& getCurrentCriteria() const { return m_criteria; }
    bool isFilterActive() const { return m_filterActive; }


protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    bool dateMatches(const ScheduleItem& item, const QDate& filterDate) const;

    FilterCriteria m_criteria;
    bool m_filterActive = false;
};

#endif // SCHEDULEFILTERPROXYMODEL_H
