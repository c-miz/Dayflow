#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ScheduleModel.h"
#include <QLabel>
#include <QPushButton>
#include "calendarview.h"

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

private:
    Ui::MainWindow *ui;
    ScheduleModel *model;
    void setupTableView();
    void initMonthView();
    void updateMonthLabel();
    void setupCalendarView();
    CalendarView *m_calendarView;

    QLabel *monthLabel;
    QPushButton *prevMonthBtn;
    QPushButton *nextMonthBtn;
};

#endif // MAINWINDOW_H
