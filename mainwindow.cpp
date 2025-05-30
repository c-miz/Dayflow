#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "EditDialog.h"
#include "calendarview.h"
#include "notification.h"
#include <QHeaderView>
#include "filterdialog.h"
#include "schedulereminder.h"

const int PRIORITY_COLUMN=6;  //优先级列
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(new ScheduleModel(this)),proxyModel(new ScheduleFilterProxyModel(this))
{
    ui->setupUi(this);
    proxyModel->setSourceModel(model);  //设置模型代理
    setupTableView();
    setupCalendarView();
    ScheduleReminder* reminder=new ScheduleReminder(model);
    Notification* notify=new Notification;

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEdit);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::onFilter);
    connect(model, &ScheduleModel::modelChanged,m_calendarView, &CalendarView::refresh);
    connect(reminder, &ScheduleReminder::triggerReminder,notify, &Notification::handleReminder);
    proxyModel->sort(PRIORITY_COLUMN,Qt::DescendingOrder); //代理模型排序
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupTableView()
{
    ui->tableView->setModel(proxyModel);  //总览视图改用proxymodel
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::onAdd()
{
    EditDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        ScheduleItem item = dlg.getScheduleItem();
        if (item.isValid())
            model->addItem(item);
    }
}

void MainWindow::onEdit()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    QModelIndex proxyIndex = selected.first();
    QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);  //总览视图中通过代理模型间接访问数据
    int row = sourceIndex.row();
    ScheduleItem original = model->getItem(row);

    EditDialog dlg(this, original);
    if (dlg.exec() == QDialog::Accepted) {
        ScheduleItem modified = dlg.getScheduleItem();
        if (modified.isValid()) {
            model->removeItem(row);
            model->addItem(modified);
        }
    }
}

void MainWindow::onDelete()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if(selected.empty()) return;
    QModelIndex proxyIndex=selected.first();
    QModelIndex sourceIndex=proxyModel->mapToSource(proxyIndex);  //代理模型间接访问
    if (!sourceIndex.isValid()) return;
    model->removeItem(sourceIndex.row());
}
void MainWindow::setupCalendarView()
{
    m_calendarView = new CalendarView(model, ui->monthTab);
    QVBoxLayout *layout = new QVBoxLayout(ui->monthTab);
    layout->addWidget(m_calendarView);
    ui->monthTab->setLayout(layout);
}
// 新增的槽函数，用于处理筛选逻辑
void MainWindow::onFilter()
{
    filterdialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        FilterCriteria criteria = dlg.getCriteria();
        proxyModel->setFilterCriteria(criteria); // 将筛选条件设置到代理模型
    }
}
