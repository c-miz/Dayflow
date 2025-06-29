#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "ScheduleItem.h"
#include "mainwindow.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr, const ScheduleItem &item = ScheduleItem());
    ~EditDialog();

    ScheduleItem getScheduleItem() const;

private slots:
    void validateTimes();

private:
    Ui::EditDialog *ui;
};

#endif // EDITDIALOG_H
