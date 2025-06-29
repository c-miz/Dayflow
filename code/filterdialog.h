// filterdialog.h
#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QString>
#include <QDate>
#include <QStringList>

// 定义筛选条件结构体
struct FilterCriteria {
    QString tag1;
    QString tag2;
    QString tag3;
    QDate date;
    int priority;
};

namespace Ui {
class filterdialog;
}

class filterdialog : public QDialog
{
    Q_OBJECT

public:
    explicit filterdialog(QWidget *parent = nullptr);
    ~filterdialog();

    FilterCriteria getCriteria() const; // 获取筛选条件的方法

private:
    Ui::filterdialog *ui;
};

#endif // FILTERDIALOG_H
