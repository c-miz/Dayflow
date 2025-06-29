// eventcalendar.cpp
#include "eventcalendar.h"
#include <QPainter>

EventCalendar::EventCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}

void EventCalendar::setEvents(const QMap<QDate, int>& events)
{
    m_events = events;
    refreshView();
}

void EventCalendar::paintCell(QPainter *painter, const QRect &rect, const QDate date) const
{
    // 先调用基类绘制基础内容
    QCalendarWidget::paintCell(painter, rect, date);
    // 背景色渐变处理
    if (m_events.contains(date)) {
        painter->save();

        // 创建半透明渐变背景
        QLinearGradient grad(rect.topLeft(), rect.bottomRight());
        grad.setColorAt(0, QColor(255, 230, 230, 150)); // 浅红色渐变
        grad.setColorAt(1, QColor(255, 180, 180, 200));

        // 绘制背景
        painter->fillRect(rect.adjusted(1,1,-1,-1), grad);

        // 调整文字颜色确保可见性
        painter->setPen(Qt::darkRed);
        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

        painter->restore();
    }
}
