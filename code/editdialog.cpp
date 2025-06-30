#include "EditDialog.h"
#include "ui_EditDialog.h"
#include <QPushButton>
#include "mainwindow.h"

EditDialog::EditDialog(QWidget *parent, const ScheduleItem &item)
    : QDialog(parent), ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    QComboBox *categoryCombo=new QComboBox();
    categoryCombo->setObjectName("categoryCombo");

    // 初始化控件
    ui->titleEdit->setText(item.title);
    ui->startTimeEdit->setTime(item.startTime);
    ui->endTimeEdit->setTime(item.endTime);
    ui->dateEdit->setDate(item.date); // 确保item.date是有效的
    ui->repeatCombo->addItems({"无重复", "每天", "每周", "每月"});
    ui->repeatCombo->setCurrentIndex(item.repeatType);
    ui->remindTime->setValue(item.advanceAmount);
    ui->formLayout->insertRow(3, "所属类别:", categoryCombo);


    // 初始化标签输入框
    ui->tag1Edit->addItems({"无","1","2","3"});
    ui->tag2Edit->addItems({"无","4","5","6"});
    ui->tag3Edit->addItems({"无","7","8","9"});
    ui->priorbox->setValue(item.priority);
    ui->priorbox->setMaximum(5);

    // 连接时间验证信号
    connect(ui->startTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EditDialog::validateTimes);
    connect(ui->endTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EditDialog::validateTimes);
    for (const Category &cat : MainWindow::loadCategoriesFromJson()) {
        categoryCombo->addItem(cat.name);
    }

    categoryCombo->addItem("未分类");
    categoryCombo->setCurrentText(item.category.isEmpty() ? "未分类" : item.category);
}

EditDialog::~EditDialog() { delete ui; }

ScheduleItem EditDialog::getScheduleItem() const
{
    ScheduleItem item;
    item.title = ui->titleEdit->text();
    item.startTime = ui->startTimeEdit->time();
    item.endTime = ui->endTimeEdit->time();
    item.date = ui->dateEdit->date();
    item.repeatType = static_cast<RepeatType>(ui->repeatCombo->currentIndex());
    item.advanceAmount=ui->remindTime->value();

    // 收集标签
    item.tags.clear(); // 清空旧标签
    QString tag1 = ui->tag1Edit->currentText().trimmed();
    QString tag2 = ui->tag2Edit->currentText().trimmed();
    QString tag3 = ui->tag3Edit->currentText().trimmed();

    if (!tag1.isEmpty()) item.tags.append(tag1);
    if (!tag2.isEmpty()) item.tags.append(tag2);
    if (!tag3.isEmpty()) item.tags.append(tag3);
    item.priority=ui->priorbox->value();

    QComboBox *combo = findChild<QComboBox*>("categoryCombo");
    QString category = combo ? combo->currentText() : "";
    item.category = (category == "未分类") ? "" : category;

    return item;
}

void EditDialog::validateTimes()
{
    bool valid = ui->startTimeEdit->dateTime() < ui->endTimeEdit->dateTime();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
