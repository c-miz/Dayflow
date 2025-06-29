// filterdialog.cpp
#include "filterdialog.h"
#include "ui_filterdialog.h"

filterdialog::filterdialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::filterdialog)
{
    ui->setupUi(this);

    // 初始化标签下拉框选项，与 EditDialog 保持一致性
    ui->comboBox->addItems({"无", "1", "2", "3"});
    ui->comboBox_2->addItems({"无", "4", "5", "6"});
    ui->comboBox_3->addItems({"无", "7", "8", "9"});

    // 为 dateEdit 设置一个默认日期，例如当天
    ui->dateEdit->setDate(QDate::currentDate());
    ui->spinBox->setMaximum(5);
    ui->spinBox->setValue(0);   // 默认优先级
}

filterdialog::~filterdialog()
{
    delete ui;
}

FilterCriteria filterdialog::getCriteria() const
{
    FilterCriteria criteria;
    criteria.tag1 = ui->comboBox->currentText();
    criteria.tag2 = ui->comboBox_2->currentText();
    criteria.tag3 = ui->comboBox_3->currentText();
    criteria.date = ui->dateEdit->date();
    criteria.priority = ui->spinBox->value();
    return criteria;
}
