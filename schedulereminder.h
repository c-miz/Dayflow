#ifndef SCHEDULEREMINDER_H
#define SCHEDULEREMINDER_H
#include <QObject>
#include <QDateTime>
#include <QTimer>
#include "schedulemodel.h"

class ScheduleReminder:public QObject
{
    Q_OBJECT;
public:
    explicit ScheduleReminder(ScheduleModel *model,QObject *parent = nullptr);
signals:
    void triggerReminder(const QString &message);

private:
    QTimer checkTimer;
    ScheduleModel* m_model;
private slots:
    void checkSchedules();
};

#endif // SCHEDULEREMINDER_H
