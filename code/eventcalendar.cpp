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
    // 如果日期不属于当前显示的月份，则完全不绘制
    if (date.month() != monthShown()) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    if (date == QDate::currentDate()) {
        // 如果是今天，绘制蓝色实心圆角矩形背景
        painter->setBrush(QColor("#1875F0")); // 主题蓝色
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 8.0, 8.0);
    }
    else if (m_events.contains(date)) {
        // 如果不是今天，但有事件，绘制淡蓝色圆形背景
        QRectF pillRect(0, 0, 28, 28); // 定义一个固定大小的背景
        pillRect.moveCenter(rect.center()); // 将其移动到单元格中心

        painter->setBrush(QColor("#D6EFFF")); // 淡蓝色
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(pillRect, 14.0, 14.0);
    }

    // 绘制日期数字
    QFont font = painter->font();
    font.setPointSizeF(10.5);
    font.setWeight(QFont::Medium);
    painter->setFont(font);

    // 根据不同情况设置文字颜色
    if (date == QDate::currentDate()) {
        painter->setPen(Qt::white);         // 今天：白色文字
    } else if (m_events.contains(date)) {
        painter->setPen(QColor("#1875F0"));  // 有事件：蓝色文字
    } else if (date.dayOfWeek() == 6 || date.dayOfWeek() == 7) {
        painter->setPen(Qt::lightGray);     // 周末：浅灰色文字
    } else {
        painter->setPen(Qt::black);         // 工作日：黑色文字
    }

    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

    painter->restore();
}
