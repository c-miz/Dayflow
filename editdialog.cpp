#include "EditDialog.h"
#include "ui_EditDialog.h"
#include <QPushButton>

EditDialog::EditDialog(QWidget *parent, const ScheduleItem &item)
    : QDialog(parent), ui(new Ui::EditDialog)
{
    ui->setupUi(this);

    // 初始化控件
    ui->titleEdit->setText(item.title);
    ui->startTimeEdit->setTime(item.startTime);
    ui->endTimeEdit->setTime(item.endTime);
    ui->dateEdit->setDate(item.date);
    ui->repeatCombo->addItems({"无重复", "每天", "每周", "每月"});
    ui->repeatCombo->setCurrentIndex(item.repeatType);
    ui->remindTime->setValue(item.advanceAmount);

    // 连接时间验证信号
    connect(ui->startTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EditDialog::validateTimes);
    connect(ui->endTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EditDialog::validateTimes);
}

EditDialog::~EditDialog() { delete ui; }

ScheduleItem EditDialog::getScheduleItem() const
{
    ScheduleItem item;
    item.title = ui->titleEdit->text();
    item.startTime = ui->startTimeEdit->time();
    item.endTime = ui->endTimeEdit->time();
    item.date=ui->dateEdit->date();
    item.repeatType = static_cast<RepeatType>(ui->repeatCombo->currentIndex());
    item.advanceAmount=ui->remindTime->value();
    return item;
}

void EditDialog::validateTimes()
{
    bool valid = ui->startTimeEdit->dateTime() < ui->endTimeEdit->dateTime();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
