#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ScheduleModel.h"
#include <QLabel>
#include <QPushButton>
#include "calendarview.h"
#include "schedulefilterproxymodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onFilter();
private:
    Ui::MainWindow *ui;
    ScheduleModel *model;
    ScheduleFilterProxyModel *proxyModel; //代理模型，用于总览界面的排序
    void setupTableView();
    void setupCalendarView();
    CalendarView *m_calendarView;

    QLabel *monthLabel;
    QPushButton *prevMonthBtn;
    QPushButton *nextMonthBtn;
};

#endif // MAINWINDOW_H
