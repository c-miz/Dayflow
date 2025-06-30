#include "calendarview.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCalendarWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QLabel>
#include "eventcalendar.h"
#include <QSpacerItem>
#include <QFileDialog>

CalendarView::CalendarView(ScheduleModel *model, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    this->setObjectName("calendarViewWidget");
    this->setAutoFillBackground(true);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    //头部导航栏
    QHBoxLayout *navLayout = new QHBoxLayout;

    m_monthLabel = new QLabel(this);
    m_monthLabel->setObjectName("monthLabel");

    QPushButton *prevMonthBtn = new QPushButton("◀", this);
    QPushButton *nextMonthBtn = new QPushButton("▶", this);
    prevMonthBtn->setObjectName("navButton");
    nextMonthBtn->setObjectName("navButton");

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QPushButton *addBtn = new QPushButton("+", this);
    addBtn->setObjectName("flatIconButton");
    QPushButton *menuBtn = new QPushButton("切换", this);
    menuBtn->setObjectName("flatIconButton");
    QPushButton *clearBgBtn = new QPushButton("-", this);
    clearBgBtn->setObjectName("flatIconButton");

    //修改布局顺序
    navLayout->addWidget(m_monthLabel);
    navLayout->addWidget(prevMonthBtn);
    navLayout->addWidget(nextMonthBtn);
    navLayout->addSpacerItem(spacer);
    navLayout->addWidget(addBtn);
    navLayout->addWidget(clearBgBtn);
    navLayout->addWidget(menuBtn);

    //(不变)
    m_calendar = new EventCalendar(this);
    m_calendar->findChild<QWidget*>("qt_calendar_navigationbar")->setVisible(false);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    m_calendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);

    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(m_calendar);
    setLayout(mainLayout);

    //信号连接
    connect(prevMonthBtn, &QPushButton::clicked, m_calendar, &QCalendarWidget::showPreviousMonth);
    connect(nextMonthBtn, &QPushButton::clicked, m_calendar, &QCalendarWidget::showNextMonth);
    connect(m_calendar, &QCalendarWidget::clicked, this, &CalendarView::handleDateClick);
    connect(m_calendar, &QCalendarWidget::currentPageChanged, this, &CalendarView::updateMonth);
    connect(menuBtn, &QPushButton::clicked, this, &CalendarView::themeChangeRequested);
    connect(addBtn, &QPushButton::clicked, this, &CalendarView::onSetBackgroundClicked);
    connect(clearBgBtn, &QPushButton::clicked, this, &CalendarView::onClearBackgroundClicked);

    // 初始化
    updateMonth(QDate::currentDate().year(), QDate::currentDate().month());
    refresh();
}
void CalendarView::refresh()
{
    paintEventMarkers();
    static_cast<EventCalendar*>(m_calendar)->refreshView();
}

void CalendarView::updateMonth(int year, int month)
{
    QDate date(year, month, 1);
    // 修改标签文本格式为 "YYYY / M"
    m_monthLabel->setText(date.toString("yyyy / M"));
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
        int add_month=0;
        while (current <= maxDate) {
            if (current >= monthStart) {
                dateEvents[current]++;
            }

            switch(item.repeatType) {
            case Daily: current = current.addDays(1); break;
            case Weekly: current = current.addDays(7); break;
            case Monthly: {
                add_month++;
                current=item.date.addMonths(add_month);
                break;
            }
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
    QMessageBox msgBox(this);
    msgBox.setObjectName("infoMessageBox");
    msgBox.setWindowTitle(date.toString("yyyy-MM-dd") + "日程");
    msgBox.setText(eventDetails.join("\n"));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok); // 只保留“OK”按钮

    //调整OK按钮
    QAbstractButton *okButton = msgBox.button(QMessageBox::Ok);
    if(okButton) {
        okButton->setMinimumWidth(100);
    }

    msgBox.exec();
}
void CalendarView::onSetBackgroundClicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this, "选择背景图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp)");
    if (!imagePath.isEmpty()) {
        setBackgroundImage(imagePath);
        emit backgroundImageChanged(imagePath); // 发出信号，通知主窗口保存路径
    }
}

// 设置背景图片的核心
void CalendarView::setBackgroundImage(const QString& imagePath)
{
    if (imagePath.isEmpty() || !m_backgroundImage.load(imagePath)) {
        // 如果路径为空，或者加载图片失败
        m_backgroundImage = QPixmap(); // 清空图片
    }
    update();
}
void CalendarView::onClearBackgroundClicked()
{
    setBackgroundImage(""); // 调用 setBackgroundImage 并传入空路径，即可清除背景
    emit backgroundImageChanged(""); // 发出信号，通知主窗口保存空路径，即清除设置
}
void CalendarView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 首先检查背景图片是否存在且有效
    if (!m_backgroundImage.isNull()) {
        // 将图片绘制到整个区域
        painter.drawPixmap(this->rect(), m_backgroundImage);
    }
    QWidget::paintEvent(event);
}
