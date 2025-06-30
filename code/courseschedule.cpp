#include "courseschedule.h"
#include <QApplication>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTimeEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QTime>
#include <QScrollBar>

CourseSchedule::CourseSchedule(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadCourses();
}

void CourseSchedule::setupUI()
{
    // 创建中央部件
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    // 创建输入控件
    courseNameEdit = new QLineEdit;
    courseNameEdit->setPlaceholderText(tr("例如：高等数学"));

    dayComboBox = new QComboBox;
    QStringList days = {tr("星期一"), tr("星期二"), tr("星期三"),
                        tr("星期四"), tr("星期五"), tr("星期六"), tr("星期日")};
    dayComboBox->addItems(days);

    startTimeEdit = new QTimeEdit(QTime(8, 0));
    endTimeEdit = new QTimeEdit(QTime(9, 40));

    locationEdit = new QLineEdit;
    locationEdit->setPlaceholderText(tr("例如：A栋301"));

    instructorEdit = new QLineEdit;
    instructorEdit->setPlaceholderText(tr("例如：张教授"));

    // 创建按钮
    addButton = new QPushButton(tr("添加课程"));
    editButton = new QPushButton(tr("编辑课程"));
    deleteButton = new QPushButton(tr("删除课程"));
    saveButton = new QPushButton(tr("保存数据"));

    // 禁用编辑和删除按钮直到选中课程
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);

    // 创建输入表单
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("课程名称:"), courseNameEdit);
    formLayout->addRow(tr("上课日期:"), dayComboBox);
    formLayout->addRow(tr("开始时间:"), startTimeEdit);
    formLayout->addRow(tr("结束时间:"), endTimeEdit);
    formLayout->addRow(tr("上课地点:"), locationEdit);
    formLayout->addRow(tr("授课教师:"), instructorEdit);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addStretch();

    // 创建课程表格
    scheduleTable = new QTableWidget(0, 7);
    setupTable();

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(scheduleTable);

    // 创建状态栏
    statusBar = new QStatusBar;
    setStatusBar(statusBar);

    // 连接信号槽
    connect(addButton, &QPushButton::clicked, this, &CourseSchedule::addCourse);
    connect(editButton, &QPushButton::clicked, this, &CourseSchedule::editCourse);
    connect(deleteButton, &QPushButton::clicked, this, &CourseSchedule::deleteCourse);
    connect(saveButton, &QPushButton::clicked, this, &CourseSchedule::saveCourses);
    connect(scheduleTable, &QTableWidget::cellDoubleClicked,
            this, &CourseSchedule::cellDoubleClicked);
    connect(scheduleTable, &QTableWidget::itemSelectionChanged,
            this, &CourseSchedule::populateFormFromSelection);
    scheduleTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(scheduleTable, &QTableWidget::customContextMenuRequested,
            this, &CourseSchedule::showContextMenu);

    // 设置窗口属性
    setWindowTitle(tr("学生课程管理系统"));
    resize(900, 650);
}

void CourseSchedule::setupTable()
{
    // 设置表格列标题（星期）
    QStringList headers = {tr("星期一"), tr("星期二"), tr("星期三"),
                           tr("星期四"), tr("星期五"), tr("星期六"), tr("星期日")};
    scheduleTable->setHorizontalHeaderLabels(headers);

    // 设置表格属性
    scheduleTable->verticalHeader()->setVisible(true);
    scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    scheduleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    scheduleTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    scheduleTable->setAlternatingRowColors(true);
    scheduleTable->setStyleSheet(
        "QTableWidget { gridline-color: #d0d0d0; }"
        "QHeaderView::section { background-color: #f0f0f0; }"
        );

    // 创建时间槽
    createTimeSlots();
}

void CourseSchedule::createTimeSlots()
{
    // 创建时间槽（从8:00到21:00，每30分钟一个槽位）
    timeSlots.clear();
    for (int hour = 8; hour <= 21; hour++) {
        timeSlots.append(QTime(hour, 0));
        if (hour < 21) timeSlots.append(QTime(hour, 30));
    }

    // 设置表格行数
    scheduleTable->setRowCount(timeSlots.size());

    // 设置垂直表头（时间）
    for (int i = 0; i < timeSlots.size(); i++) {
        QString timeLabel = timeSlots[i].toString("hh:mm");
        if (i % 2 == 0) {
            // 整点时间显示为粗体
            scheduleTable->setVerticalHeaderItem(i, new QTableWidgetItem(timeLabel));
            scheduleTable->verticalHeaderItem(i)->setFont(QFont("Arial", 9, QFont::Bold));
        } else {
            scheduleTable->setVerticalHeaderItem(i, new QTableWidgetItem(""));
        }
    }
}

int CourseSchedule::findInsertionRow(int day, const QTime &startTime)
{
    // 查找课程应该插入的行（按时间排序）
    for (int row = 0; row < timeSlots.size(); row++) {
        if (startTime <= timeSlots[row]) {
            return row;
        }
    }
    return timeSlots.size() - 1;
}

bool CourseSchedule::validateCourseInput()
{
    // 验证课程输入是否有效
    if (courseNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("输入错误"), tr("课程名称不能为空"));
        courseNameEdit->setFocus();
        return false;
    }

    if (startTimeEdit->time() >= endTimeEdit->time()) {
        QMessageBox::warning(this, tr("输入错误"), tr("结束时间必须晚于开始时间"));
        startTimeEdit->setFocus();
        return false;
    }

    return true;
}

void CourseSchedule::addCourse()
{
    if (!validateCourseInput()) return;

    // 创建课程JSON对象
    QJsonObject course;
    course["name"] = courseNameEdit->text().trimmed();
    course["day"] = dayComboBox->currentText();
    course["start"] = startTimeEdit->time().toString("hh:mm");
    course["end"] = endTimeEdit->time().toString("hh:mm");
    course["location"] = locationEdit->text().trimmed();
    course["instructor"] = instructorEdit->text().trimmed();

    // 添加到课程数组
    courses.append(course);

    // 更新表格
    int dayIndex = dayComboBox->currentIndex();
    int row = findInsertionRow(dayIndex, startTimeEdit->time());
    updateCourseInTable(row, dayIndex);

    // 清空输入框
    clearInputs();
    statusBar->showMessage(tr("课程添加成功"), 3000);
}

void CourseSchedule::editCourse()
{
    if (selectedRow == -1 || selectedCol == -1) {
        QMessageBox::warning(this, tr("编辑错误"), tr("请先选择要编辑的课程"));
        return;
    }

    if (!validateCourseInput()) return;

    // 获取选中的课程索引
    QTableWidgetItem *item = scheduleTable->item(selectedRow, selectedCol);
    int courseIndex = item->data(Qt::UserRole).toInt();

    if (courseIndex >= 0 && courseIndex < courses.size()) {
        // 更新课程JSON对象
        QJsonObject course;
        course["name"] = courseNameEdit->text().trimmed();
        course["day"] = dayComboBox->currentText();
        course["start"] = startTimeEdit->time().toString("hh:mm");
        course["end"] = endTimeEdit->time().toString("hh:mm");
        course["location"] = locationEdit->text().trimmed();
        course["instructor"] = instructorEdit->text().trimmed();

        courses[courseIndex] = course;

        // 计算新的位置
        int newDayIndex = dayComboBox->currentIndex();
        int newRow = findInsertionRow(newDayIndex, startTimeEdit->time());

        // 如果位置发生变化，清除原位置内容
        if (newDayIndex != selectedCol || newRow != selectedRow) {
            delete scheduleTable->takeItem(selectedRow, selectedCol);
        }

        // 更新课程到新位置
        updateCourseInTable(newRow, newDayIndex);

        // 更新选中位置
        selectedRow = newRow;
        selectedCol = newDayIndex;

        statusBar->showMessage(tr("课程更新成功"), 3000);
    }
}

void CourseSchedule::deleteCourse()
{
    if (selectedRow == -1 || selectedCol == -1) {
        QMessageBox::warning(this, tr("删除错误"), tr("请先选择要删除的课程"));
        return;
    }

    QTableWidgetItem *item = scheduleTable->item(selectedRow, selectedCol);
    if (!item) return;

    int courseIndex = item->data(Qt::UserRole).toInt();

    if (courseIndex >= 0 && courseIndex < courses.size()) {
        int ret = QMessageBox::question(this, tr("确认删除"),
                                        tr("确定要删除选中的课程吗？"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            // 从课程数组中移除
            courses.removeAt(courseIndex);

            // 从表格中移除
            delete scheduleTable->takeItem(selectedRow, selectedCol);

            // 更新剩余项的索引
            for (int r = 0; r < scheduleTable->rowCount(); r++) {
                for (int c = 0; c < scheduleTable->columnCount(); c++) {
                    QTableWidgetItem *otherItem = scheduleTable->item(r, c);
                    if (otherItem) {
                        int idx = otherItem->data(Qt::UserRole).toInt();
                        if (idx > courseIndex) {
                            otherItem->setData(Qt::UserRole, idx - 1);
                        }
                    }
                }
            }

            // 重置选中状态
            selectedRow = -1;
            selectedCol = -1;
            editButton->setEnabled(false);
            deleteButton->setEnabled(false);

            statusBar->showMessage(tr("课程已删除"), 3000);
        }
    }
}

void CourseSchedule::updateCourseInTable(int row, int col)
{
    // 获取最后添加的课程
    QJsonObject course = courses.last().toObject();

    // 创建课程信息字符串
    QString info = QString("%1\n%2 - %3\n%4\n%5")
                       .arg(course["name"].toString(),
                            course["start"].toString(),
                            course["end"].toString(),
                            course["location"].toString(),
                            course["instructor"].toString());

    // 创建表格项
    QTableWidgetItem *item = new QTableWidgetItem(info);
    item->setData(Qt::UserRole, courses.size() - 1); // 存储课程索引
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(QColor(220, 240, 255)); // 浅蓝色背景

    // 设置到表格
    scheduleTable->setItem(row, col, item);
}

void CourseSchedule::populateFormFromSelection()
{
    // 获取选中的单元格
    QList<QTableWidgetItem*> selected = scheduleTable->selectedItems();
    if (selected.isEmpty()) {
        selectedRow = -1;
        selectedCol = -1;
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        return;
    }

    QTableWidgetItem *item = selected.first();
    selectedRow = item->row();
    selectedCol = item->column();

    int courseIndex = item->data(Qt::UserRole).toInt();

    if (courseIndex >= 0 && courseIndex < courses.size()) {
        QJsonObject course = courses[courseIndex].toObject();

        // 填充表单
        courseNameEdit->setText(course["name"].toString());
        dayComboBox->setCurrentText(course["day"].toString());
        startTimeEdit->setTime(QTime::fromString(course["start"].toString(), "hh:mm"));
        endTimeEdit->setTime(QTime::fromString(course["end"].toString(), "hh:mm"));
        locationEdit->setText(course["location"].toString());
        instructorEdit->setText(course["instructor"].toString());

        editButton->setEnabled(true);
        deleteButton->setEnabled(true);
    }
}

void CourseSchedule::cellDoubleClicked(int row, int column)
{
    // 双击单元格时选中该单元格
    scheduleTable->setCurrentCell(row, column);
    populateFormFromSelection();
}

void CourseSchedule::showContextMenu(const QPoint &pos)
{
    // 创建上下文菜单
    QMenu menu(this);
    QAction *editAction = menu.addAction(tr("编辑课程"));
    QAction *deleteAction = menu.addAction(tr("删除课程"));

    // 检查是否有选中的课程
    QTableWidgetItem *item = scheduleTable->itemAt(pos);
    if (!item) {
        editAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }

    // 执行菜单操作
    QAction *selectedAction = menu.exec(scheduleTable->viewport()->mapToGlobal(pos));
    if (selectedAction == editAction) {
        editCourse();
    } else if (selectedAction == deleteAction) {
        deleteCourse();
    }
}

void CourseSchedule::saveCourses()
{
    // 保存课程到JSON文件
    QJsonDocument doc(courses);
    QFile file("courses.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        statusBar->showMessage(tr("课程数据已保存"), 3000);
    } else {
        statusBar->showMessage(tr("保存失败: ") + file.errorString(), 5000);
    }
}

void CourseSchedule::loadCourses()
{
    // 从JSON文件加载课程
    QFile file("courses.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        courses = doc.array();
        file.close();

        // 清空表格
        for (int r = 0; r < scheduleTable->rowCount(); r++) {
            for (int c = 0; c < scheduleTable->columnCount(); c++) {
                if (scheduleTable->item(r, c)) {
                    delete scheduleTable->takeItem(r, c);
                }
            }
        }

        // 重新加载课程
        for (int i = 0; i < courses.size(); i++) {
            QJsonObject course = courses[i].toObject();
            int dayIndex = dayComboBox->findText(course["day"].toString());
            if (dayIndex < 0) continue;

            int row = findInsertionRow(dayIndex,
                                       QTime::fromString(course["start"].toString(), "hh:mm"));

            // 创建课程信息字符串
            QString info = QString("%1\n%2 - %3\n%4\n%5")
                               .arg(course["name"].toString(),
                                    course["start"].toString(),
                                    course["end"].toString(),
                                    course["location"].toString(),
                                    course["instructor"].toString());

            // 创建表格项
            QTableWidgetItem *item = new QTableWidgetItem(info);
            item->setData(Qt::UserRole, i);
            item->setTextAlignment(Qt::AlignCenter);
            item->setBackground(QColor(220, 240, 255));
            scheduleTable->setItem(row, dayIndex, item);
        }

        statusBar->showMessage(tr("课程数据已加载"), 3000);
    }
}

void CourseSchedule::clearInputs()
{
    // 清空输入表单
    courseNameEdit->clear();
    locationEdit->clear();
    instructorEdit->clear();
    dayComboBox->setCurrentIndex(0);
    startTimeEdit->setTime(QTime(8, 0));
    endTimeEdit->setTime(QTime(9, 40));
    courseNameEdit->setFocus();
}
