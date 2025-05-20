// eventcalendar.h
#pragma once
#include <QCalendarWidget>
#include <QMap>

class EventCalendar : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit EventCalendar(QWidget *parent = nullptr);

    void refreshView() {
        updateCells();
    }

    void setEvents(const QMap<QDate, int>& events);

    void setMarkerColor(const QColor &color) {
        m_markerColor = color;
        updateCells();
    }

    void setTextColor(const QColor &color) {
        m_textColor = color;
        updateCells();
    }

protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private:
    QMap<QDate, int> m_events;
    QColor m_markerColor = QColor(255, 200, 200);
    QColor m_textColor = Qt::black;

};
