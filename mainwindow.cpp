#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "EditDialog.h"
#include "calendarview.h"
#include "notification.h"
#include "schedulereminder.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(new ScheduleModel(this))
{
    ui->setupUi(this);
    setupTableView();
    setupCalendarView();
    ScheduleReminder* reminder=new ScheduleReminder(model);
    Notification* notify=new Notification;

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEdit);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(model, &ScheduleModel::modelChanged,
            m_calendarView, &CalendarView::refresh);
    connect(reminder, &ScheduleReminder::triggerReminder,
            notify, &Notification::handleReminder);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupTableView()
{
    ui->tableView->setModel(model);
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

    int row = selected.first().row();
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
    if (!selected.isEmpty())
        model->removeItem(selected.first().row());
}
void MainWindow::setupCalendarView()
{
    m_calendarView = new CalendarView(model, ui->monthTab);
    QVBoxLayout *layout = new QVBoxLayout(ui->monthTab);
    layout->addWidget(m_calendarView);
    ui->monthTab->setLayout(layout);
}
