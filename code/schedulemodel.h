#ifndef SCHEDULEMODEL_H
#define SCHEDULEMODEL_H

#include <QAbstractTableModel>
#include "ScheduleItem.h"

class ScheduleModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ScheduleModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addItem(const ScheduleItem &item);
    void removeItem(int row);
    ScheduleItem getItem(int row) const;
    QVector<ScheduleItem> getSchedulesForDate(const QDate& date) const;
    void saveToJson() const;
    void loadFromJson();


private:
    QList<ScheduleItem> items; QString getStoragePath() const;

signals:
    void modelChanged();
};

#endif // SCHEDULEMODEL_H
