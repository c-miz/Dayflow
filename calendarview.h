#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QMap>
#include "ScheduleModel.h"

class QLabel;
class QCalendarWidget;

class CalendarView : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarView(ScheduleModel *model, QWidget *parent = nullptr);
    void refresh();

private slots:
    void updateMonth(int year, int month);
    void handleDateClick(const QDate &date);

private:
    void paintEventMarkers();
    void setupNavigation();

    ScheduleModel *m_model;
    QCalendarWidget *m_calendar;
    QLabel *m_monthLabel;
    QMap<QDate, int> m_eventMarkers;
};

#endif // CALENDARVIEW_H
