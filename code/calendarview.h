#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QWidget>
#include <QPixmap>
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
    void setBackgroundImage(const QString& imagePath);
signals:
    void themeChangeRequested();
    void backgroundImageChanged(const QString& imagePath);
protected: // <-- 新增
    void paintEvent(QPaintEvent *event) override;
private slots:
    void updateMonth(int year, int month);
    void handleDateClick(const QDate &date);
    void onSetBackgroundClicked();
    void onClearBackgroundClicked();
private:
    void paintEventMarkers();

    ScheduleModel *m_model;
    QCalendarWidget *m_calendar;
    QLabel *m_monthLabel;
    QPixmap m_backgroundImage;
    QMap<QDate, int> m_eventMarkers;
};

#endif // CALENDARVIEW_H
