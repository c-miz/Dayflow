#include "calendarview.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCalendarWidget>
#include <QPainter>
#include <QMessageBox>
#include <QLabel>
#include "eventcalendar.h"

CalendarView::CalendarView(ScheduleModel *model, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 导航栏
    QHBoxLayout *navLayout = new QHBoxLayout;
    QPushButton *prevBtn = new QPushButton("◀", this);
    QPushButton *nextBtn = new QPushButton("▶", this);
    m_monthLabel = new QLabel(this);

    navLayout->addWidget(prevBtn);
    navLayout->addWidget(m_monthLabel);
    navLayout->addWidget(nextBtn);

    // 日历控件
    m_calendar = new EventCalendar(this);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(m_calendar);

    // 信号连接
    connect(prevBtn, &QPushButton::clicked,
            m_calendar, &QCalendarWidget::showPreviousMonth);
    connect(nextBtn, &QPushButton::clicked,
            m_calendar, &QCalendarWidget::showNextMonth);
    connect(m_calendar, &QCalendarWidget::clicked,
            this, &CalendarView::handleDateClick);
    connect(m_calendar, &QCalendarWidget::currentPageChanged,
            this, &CalendarView::updateMonth);

    updateMonth(QDate::currentDate().year(),
                QDate::currentDate().month());

}

void CalendarView::refresh()
{
    paintEventMarkers();
    static_cast<EventCalendar*>(m_calendar)->refreshView();
}

void CalendarView::updateMonth(int year, int month)
{
    QDate date(year, month, 1);
    m_monthLabel->setText(date.toString("yyyy年MM月"));
    paintEventMarkers();
}


void CalendarView::paintEventMarkers()
{
    QMap<QDate, int> dateEvents;

    QDate monthStart = m_calendar->selectedDate().addDays(
        -m_calendar->selectedDate().day() + 1);
    QDate monthEnd = monthStart.addMonths(1).addDays(-1);

    // 遍历所有日程项
    for (int i = 0; i < m_model->rowCount(); ++i) {
        const ScheduleItem &item = m_model->getItem(i);

        // 扩展重复事件
        QDate current = item.date;
        QDate maxDate=current.addYears(10);
        while (current <= maxDate) {
            if (current >= monthStart) {
                dateEvents[current]++;
            }

            switch(item.repeatType) {
            case Daily: current = current.addDays(1); break;
            case Weekly: current = current.addDays(7); break;
            case Monthly: current = current.addMonths(1); break;
            default: break;
            }

            if (item.repeatType == RepeatType::None) break;
        }
    }

    // 传递给日历控件
    static_cast<EventCalendar*>(m_calendar)->setEvents(dateEvents);
}

void CalendarView::handleDateClick(const QDate &date)
{
    QVector<ScheduleItem> events = m_model->getSchedulesForDate(date);
    QStringList eventDetails;

    if(events.empty()){
        eventDetails<<"当天没有日程";
    }
    else{
        for(const auto &item : events) {
            QString detail = QString("%1-%2 %3")
            .arg(item.startTime.toString("HH:mm"))
                .arg(item.endTime.toString("HH:mm"))
                .arg(item.title);
            if(!item.tags.isEmpty()){
                detail.append(QString(QString(" [%1]").arg(item.tags.join(", ")))); //tag项
            }
            eventDetails<<detail;
        }
    }

    QMessageBox::information(this, date.toString("yyyy-MM-dd")+"日程",
                             eventDetails.join("\n"));
}
