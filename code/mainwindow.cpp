#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "EditDialog.h"
#include "calendarview.h"
#include "notification.h"
#include <QHeaderView>
#include "filterdialog.h"
#include "schedulereminder.h"
#include "courseschedule.h"
#include "QLineEdit"
#include "category.h"
#include "QStandardPaths"
#include "QDir"
#include "QJsonObject"
#include "QFormLayout"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include "QListWidget"
#include "qdesktopservices.h"
#include "QFileDialog"
#include "QTreeWidget"

const QString MainWindow::CONFIG_FILENAME = "categories.json";
const int PRIORITY_COLUMN=6;  //优先级列
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(new ScheduleModel(this)),proxyModel(new ScheduleFilterProxyModel(this))
{
    ui->setupUi(this);
    proxyModel->setSourceModel(model);  //设置模型代理
    setupTableView();
    setupCalendarView();
    setupcategoryPage();
    setupFileManagementPage();
    ScheduleReminder* reminder=new ScheduleReminder(model);
    Notification* notify=new Notification;
    model->loadFromJson(); // 启动时加载数据
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEdit);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::onFilter);
    connect(model, &ScheduleModel::modelChanged,m_calendarView, &CalendarView::refresh);
    connect(reminder, &ScheduleReminder::triggerReminder,notify, &Notification::handleReminder);
    connect(ui->tabWidget, &QTabWidget::currentChanged, [=](int index){
        if(ui->tabWidget->tabText(index) == "文件管理") {
            refreshFileManagementCategories();
        }
    });
    proxyModel->sort(PRIORITY_COLUMN,Qt::DescendingOrder); //代理模型排序
    // 实现新页面：相对独立的课程表,记录了课程信息
    QWidget * filepage = new CourseSchedule();
    ui->tabWidget->addTab(filepage, "课程表");
}
void MainWindow::initConfigFile()
{
    QJsonObject root = loadJsonConfig();
    if(!root.contains("categories")){
        root["categories"] = QJsonArray();
        QFile file(getConfigPath());
        if(file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(root).toJson());
        }
    }
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
QString MainWindow::getConfigPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/" + CONFIG_FILENAME;
}
void MainWindow::saveCategoryToJson(const Category &cat)
{
    QJsonObject root = loadJsonConfig();
    QJsonArray categoriesArray = root["categories"].toArray();

    // 查找是否已存在
    bool exists = false;
    for(int i=0; i<categoriesArray.size(); ++i){
        QJsonObject obj = categoriesArray[i].toObject();
        if(obj["name"].toString() == cat.name){
            exists = true;
            break;
        }
    }

    if(!exists){
        categoriesArray.append(cat.toJson());
        root["categories"] = categoriesArray;

        QFile file(getConfigPath());
        if(file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(root).toJson());
        }
    }
}


QJsonObject MainWindow::loadJsonConfig()
{
    QFile file(getConfigPath());
    if(!file.open(QIODevice::ReadOnly))
        return QJsonObject();

    return QJsonDocument::fromJson(file.readAll()).object();
}

QList<Category> MainWindow::loadCategoriesFromJson()
{
    QList<Category> categories;
    QJsonObject root = loadJsonConfig();
    QJsonArray categoriesArray = root["categories"].toArray();

    foreach(const QJsonValue &val, categoriesArray){
        Category cat;
        cat.name = val.toObject()["name"].toString();
        cat.storagePath = val.toObject()["path"].toString();
        cat.createTime = QDateTime::fromString(
            val.toObject()["time"].toString(), Qt::ISODate);
        categories.append(cat);
    }

    return categories;
}

// 检查类别是否存在
bool MainWindow::categoryExists(const QString &name)
{
    QList<Category> categories = loadCategoriesFromJson();
    foreach(const Category &cat, categories) {
        if(cat.name.toLower() == name.toLower().trimmed()) {
            return true;
        }
    }
    return false;
}

// 通过名称查找类别
Category MainWindow::findCategoryByName(const QString &name)
{
    QList<Category> categories = loadCategoriesFromJson();
    foreach(const Category &cat, categories) {
        if(cat.name == name) {
            return cat;
        }
    }
    return Category(); // 返回空对象
}

// 更新JSON中的类别信息
void MainWindow::updateCategoryInJson(const Category &updatedCat, const QString &oldName)
{
    QJsonObject root = loadJsonConfig();
    QJsonArray categoriesArray = root["categories"].toArray();

    // 遍历查找旧名称
    bool found = false;
    for(int i = 0; i < categoriesArray.size(); ++i) {
        QJsonObject obj = categoriesArray[i].toObject();
        if(obj["name"].toString() == oldName) { // 根据旧名称匹配
            categoriesArray.replace(i, updatedCat.toJson());
            found = true;
            break;
        }
    }

    if(found) {
        root["categories"] = categoriesArray;
        QFile file(getConfigPath());
        if(file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(root).toJson());
        }
    }
}

// 从JSON中移除类别
void MainWindow::removeCategoryFromJson(const QString &categoryName)
{
    QJsonObject root = loadJsonConfig();
    QJsonArray categoriesArray = root["categories"].toArray();

    // 创建新数组过滤要删除的条目
    QJsonArray newArray;
    foreach(const QJsonValue &val, categoriesArray) {
        QJsonObject obj = val.toObject();
        if(obj["name"].toString() != categoryName) {
            newArray.append(obj);
        }
    }
    QList<ScheduleItem> items = loadAllSchedules();
    for(auto &item : items) {
        if(item.category == categoryName) {
            item.category.clear();
        }
    }
    saveAllSchedules(items);

    // 更新根对象并保存
    root["categories"] = newArray;
    QFile file(getConfigPath());
    if(file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}
//文件管理
void MainWindow::setupFileManagementPage()
{
    QWidget *filePage = new QWidget();
    ui->tabWidget->addTab(filePage, "文件管理");


    QHBoxLayout *mainLayout = new QHBoxLayout(filePage);

    // 左侧类别面板
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QListWidget *categoryList = new QListWidget();
    categoryList->setObjectName("fileCategoryList");
    leftLayout->addWidget(new QLabel("所有类别："));
    leftLayout->addWidget(categoryList);

    // 右侧文件操作面板
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QTableWidget *fileTable = new QTableWidget();
    fileTable->setColumnCount(4);
    fileTable->setHorizontalHeaderLabels({"文件名", "大小", "上传时间", "操作"});
    fileTable->horizontalHeader()->setStretchLastSection(true);

    QPushButton *uploadBtn = new QPushButton("上传文件");
    rightLayout->addWidget(uploadBtn);
    rightLayout->addWidget(fileTable);

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 3);

    // 初始化类别列表
    auto refreshCategories = [=](){
        categoryList->clear();
        QList<Category> categories = loadCategoriesFromJson();
        foreach(const Category &cat, categories) {
            categoryList->addItem(cat.name);
        }
    };
    refreshCategories();

    // 文件上传功能
    connect(uploadBtn, &QPushButton::clicked, [=](){
        QList<QListWidgetItem*> selected = categoryList->selectedItems();
        if(selected.isEmpty()) {
            QMessageBox::warning(this, "错误", "请先选择类别");
            return;
        }

        QString category = selected.first()->text();
        QString filePath = QFileDialog::getOpenFileName(this, "选择文件");
        if(filePath.isEmpty()) return;

        QFileInfo srcInfo(filePath);
        Category cat = findCategoryByName(category);
        if(cat.name.isEmpty()) return;

        // 创建目标路径
        QString destPath = cat.storagePath + "/" + srcInfo.fileName();
        if(QFile::exists(destPath)) {
            if(QMessageBox::question(this, "文件存在", "文件已存在，是否覆盖？")
                == QMessageBox::No) return;
        }

        // 复制文件
        if(!QFile::copy(filePath, destPath)) {
            QMessageBox::critical(this, "错误", "文件上传失败");
            return;
        }

        // 保存元数据
        FileInfo info;
        info.fileName = srcInfo.fileName();
        info.filePath = destPath;
        info.fileSize = srcInfo.size();
        info.uploadTime = QDateTime::currentDateTime();
        info.category = category;
        saveFileMetadata(info);

        refreshFiles(category, fileTable);
    });

    // 类别选择变化事件
    connect(categoryList, &QListWidget::itemSelectionChanged, [=](){
        QList<QListWidgetItem*> selected = categoryList->selectedItems();
        if(!selected.isEmpty()) {
            refreshFiles(selected.first()->text(), fileTable);
        }
    });
}

// 文件元数据存储
void MainWindow::saveFileMetadata(const FileInfo &info)
{
    QJsonObject root = loadJsonConfig();
    QJsonArray filesArray = root["files"].toArray();

    // 移除旧记录（如果存在）
    for(int i=0; i<filesArray.size(); ++i) {
        QJsonObject obj = filesArray[i].toObject();
        if(obj["path"].toString() == info.filePath) {
            filesArray.removeAt(i);
            break;
        }
    }

    filesArray.append(info.toJson());
    root["files"] = filesArray;

    QFile file(getConfigPath());
    if(file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

void MainWindow::refreshFileManagementCategories()
{
    // 查找文件管理页面的QListWidget
    QListWidget *categoryList = findChild<QListWidget*>("fileCategoryList"); // 需要给控件设置objectName

    if(categoryList) {
        categoryList->clear();
        QList<Category> categories = loadCategoriesFromJson();
        foreach(const Category &cat, categories) {
            categoryList->addItem(cat.name);
        }
    }
}

// 刷新文件列表
void MainWindow::refreshFiles(const QString &category, QTableWidget *table)
{
    table->clearContents();
    table->setRowCount(0);

    QJsonObject root = loadJsonConfig();
    QJsonArray filesArray = root["files"].toArray();

    int row = 0;
    foreach(const QJsonValue &val, filesArray) {
        FileInfo info = FileInfo::fromJson(val.toObject());
        if(info.category != category) continue;

        table->insertRow(row);

        // 文件名
        QTableWidgetItem *nameItem = new QTableWidgetItem(info.fileName);
        nameItem->setData(Qt::UserRole, info.filePath);
        table->setItem(row, 0, nameItem);

        // 文件大小
        QString sizeStr = QString("%1 MB").arg(info.fileSize/1024.0/1024, 0, 'f', 2);
        table->setItem(row, 1, new QTableWidgetItem(sizeStr));

        // 上传时间
        table->setItem(row, 2, new QTableWidgetItem(info.uploadTime.toString("yyyy-MM-dd HH:mm")));

        // 操作按钮
        QWidget *btnWidget = new QWidget();
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);

        QPushButton *openBtn = new QPushButton("打开");
        QPushButton *deleteBtn = new QPushButton("删除");

        btnLayout->addWidget(openBtn);
        btnLayout->addWidget(deleteBtn);
        btnLayout->setContentsMargins(0,0,0,0);

        table->setCellWidget(row, 3, btnWidget);

        // 打开按钮事件
        connect(openBtn, &QPushButton::clicked, [=](){
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.filePath));
        });

        // 删除按钮事件
        connect(deleteBtn, &QPushButton::clicked, [=](){
            if(QMessageBox::question(this, "确认删除", "确定要删除该文件吗？")
                == QMessageBox::Yes)
            {
                if(QFile::remove(info.filePath)) {
                    removeFileMetadata(info.filePath);
                    refreshFiles(category, table);
                } else {
                    QMessageBox::critical(this, "错误", "文件删除失败");
                }
            }
        });

        row++;
    }
}

// 删除文件元数据
void MainWindow::removeFileMetadata(const QString &filePath)
{
    QJsonObject root = loadJsonConfig();
    QJsonArray filesArray = root["files"].toArray();

    QJsonArray newArray;
    foreach(const QJsonValue &val, filesArray) {
        if(val.toObject()["path"].toString() != filePath) {
            newArray.append(val);
        }
    }

    root["files"] = newArray;
    QFile file(getConfigPath());
    if(file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
    }
}

// 日程详情弹窗
void MainWindow::showScheduleDetail(const ScheduleItem &item)
{
    QDialog dialog(this);
    QFormLayout layout(&dialog);

    // 显示详细信息
    layout.addRow("标题:", new QLabel(item.title));
    layout.addRow("时间:", new QLabel(item.date.toString("yyyy-MM-dd") + " " +
                                      item.startTime.toString("hh:mm") + " - " +
                                      item.endTime.toString("hh:mm")));
    layout.addRow("类别:", new QLabel(item.category.isEmpty() ? "未分类" : item.category));

    dialog.exec();
}
// 日程存储路径
QString MainWindow::getSchedulePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/schedules.json";
}
QList<ScheduleItem> MainWindow::loadAllSchedules()
{
    QList<ScheduleItem> items;
    QFile file(getSchedulePath());
    if(file.open(QIODevice::ReadOnly)) {
        QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
        foreach(const QJsonValue &val, array) {
            items.append(ScheduleItem::fromJson(val.toObject()));
        }
    }
    return items;
}
// 保存所有日程
void MainWindow::saveAllSchedules(const QList<ScheduleItem> &items)
{
    QJsonArray array;
    foreach(const ScheduleItem &item, items) {
        array.append(item.toJson());
    }

    QFile file(getSchedulePath());
    if(file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(array).toJson());
    }
}

void MainWindow::setupcategoryPage()
{
    QWidget *categoryPage = new QWidget();
    ui->tabWidget->addTab(categoryPage, "类别管理");
    QVBoxLayout *mainLayout = new QVBoxLayout(categoryPage);
    QTabWidget *funcTabs = new QTabWidget();
    mainLayout->addWidget(funcTabs);

    // 创建子功能页
    QWidget *createPage = new QWidget();
    funcTabs->addTab(createPage, "新建类别");
    QFormLayout *createLayout = new QFormLayout(createPage);
    QLineEdit *nameInput = new QLineEdit();
    createLayout->addRow("类别名称：",nameInput);
    QPushButton *createBtn = new QPushButton("创建");
    connect(createBtn, &QPushButton::clicked, [=](){
        if(nameInput->text().isEmpty()) {
            QMessageBox::warning(this, "错误", "类别名称不能为空");
            return;
        }

        Category newCat;
        newCat.name = nameInput->text();
        newCat.createTime = QDateTime::currentDateTime();

        // 创建存储目录
        QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        newCat.storagePath = basePath + "/" + newCat.name;
        QDir().mkpath(newCat.storagePath);

        // 保存到配置文件
        saveCategoryToJson(newCat);
        if(funcTabs->count() > 1) { // 确保编辑页面已存在
            QWidget *editPage = funcTabs->widget(1); // 编辑页是第2个tab
            QListWidget *list = editPage->findChild<QListWidget*>();
            if(list) {
                list->clear();
                QList<Category> categories = loadCategoriesFromJson();
                foreach(const Category &cat, categories) {
                    list->addItem(cat.name);
                }
            }
        }

        nameInput->clear(); // 清空输入框
        QMessageBox::warning(this, "成功", "创建完成");
    });
    createLayout->addWidget(createBtn);

    // 创建逻辑


    // 编辑类别页
    QWidget *editPage = new QWidget();
    QVBoxLayout *mainEditLayout = new QVBoxLayout(editPage);

    // 类别列表
    QListWidget *categoryList = new QListWidget();
    categoryList->setMinimumHeight(200);
    mainEditLayout->addWidget(new QLabel("现有类别列表："));
    mainEditLayout->addWidget(categoryList);

    // 编辑区域
    QHBoxLayout *editControls = new QHBoxLayout();
    QLineEdit *nameEdit = new QLineEdit();
    QPushButton *updateBtn = new QPushButton("更新名称");
    QPushButton *deleteBtn = new QPushButton("删除类别");
    editControls->addWidget(nameEdit);
    editControls->addWidget(updateBtn);
    editControls->addWidget(deleteBtn);

    mainEditLayout->addLayout(editControls);

    // 初始化加载数据
    auto refreshList = [=](){
        categoryList->clear();
        QList<Category> categories = loadCategoriesFromJson();
        foreach(const Category &cat, categories) {
            categoryList->addItem(cat.name);
        }
    };
    refreshList(); // 首次加载

    // 列表项选择事件
    QObject::connect(categoryList, &QListWidget::currentItemChanged, [=](QListWidgetItem *item){
        if(item) {
            nameEdit->setText(item->text());
        }
    });

    // 更新按钮点击
    QObject::connect(updateBtn, &QPushButton::clicked, [=](){
        QListWidgetItem *currentItem = categoryList->currentItem();
        if(!currentItem) {
            QMessageBox::warning(this, "错误", "请先选择要修改的类别");
            return;
        }

        QString newName = nameEdit->text().trimmed();
        if(newName.isEmpty()) {
            QMessageBox::warning(this, "错误", "类别名称不能为空");
            return;
        }

        // 获取原数据
        Category oldCat = findCategoryByName(currentItem->text());
        if(oldCat.name.isEmpty()) {
            QMessageBox::warning(this, "错误", "找不到原类别数据");
            return;
        }

        // 检查名称重复
        if(newName != oldCat.name && categoryExists(newName)) {
            QMessageBox::warning(this, "错误", "该类别名称已存在");
            return;
        }

        // 更新数据
        oldCat.name = newName;
        updateCategoryInJson(oldCat, currentItem->text());

        refreshList();
        QMessageBox::information(this, "成功", "类别已更新");
    });

    // 删除按钮点击
    QObject::connect(deleteBtn, &QPushButton::clicked, [=](){
        QListWidgetItem *currentItem = categoryList->currentItem();
        if(!currentItem) {
            QMessageBox::warning(this, "错误", "请先选择要删除的类别");
            return;
        }

        if(QMessageBox::question(this, "确认删除",
                                  "确定要删除该类别吗？相关文件将永久丢失！",
                                  QMessageBox::Yes|QMessageBox::No) == QMessageBox::No) {
            return;
        }

        Category delCat = findCategoryByName(currentItem->text());
        if(delCat.name.isEmpty()) return;

        // 删除存储目录
        QDir dir(delCat.storagePath);
        if(dir.exists()) {
            if(!dir.removeRecursively()) {
                QMessageBox::critical(this, "错误", "目录删除失败");
                return;
            }
        }

        // 从配置移除
        removeCategoryFromJson(delCat.name);
        refreshList();
        QMessageBox::information(this, "成功", "类别已删除");
    });

    funcTabs->addTab(editPage, "编辑类别");
    //日程分类管理
    QWidget *scheduleMgmtPage = new QWidget();
    funcTabs->addTab(scheduleMgmtPage, "日程管理");
    QVBoxLayout *mainLayout_1 = new QVBoxLayout(scheduleMgmtPage);

    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels({"类别", "日程数量", "最近日程时间"});
    mainLayout_1->addWidget(treeWidget);

    // 刷新数据按钮
    QPushButton *refreshBtn = new QPushButton("刷新");
    mainLayout_1->addWidget(refreshBtn);

    // 初始化刷新
    auto refreshScheduleView = [=](){
        treeWidget->clear();

        // 加载所有类别和日程
        QList<Category> categories = loadCategoriesFromJson();
        QList<ScheduleItem> allSchedules = loadAllSchedules();

        // 按类别分组
        QMap<QString, QList<ScheduleItem>> categoryMap;
        foreach(const ScheduleItem &item, allSchedules) {
            QString catName = item.category.isEmpty() ? "未分类" : item.category;
            categoryMap[catName].append(item);
        }

        // 构建树形结构
        foreach(const Category &cat, categories) {
            QTreeWidgetItem *catItem = new QTreeWidgetItem(treeWidget);
            catItem->setText(0, cat.name);

            QList<ScheduleItem> items = categoryMap.value(cat.name, {});
            catItem->setText(1, QString::number(items.size()));
            if(!items.isEmpty()) {
                auto latest = std::max_element(items.begin(), items.end(),
                                               [](const ScheduleItem &a, const ScheduleItem &b){
                                                   return a.date < b.date;
                                               });
                catItem->setText(2, latest->date.toString("yyyy-MM-dd"));
            }

            // 添加子项显示日程详情
            foreach(const ScheduleItem &item, items) {
                QTreeWidgetItem *child = new QTreeWidgetItem(catItem);
                child->setText(0, item.title);
                child->setText(1, item.date.toString("MM-dd") + " " +
                                      item.startTime.toString("hh:mm") + "-" +
                                      item.endTime.toString("hh:mm"));
                child->setData(0, Qt::UserRole, QVariant::fromValue(item));
            }
        }

        // 添加未分类项
        QList<ScheduleItem> uncategorized = categoryMap.value("未分类", {});
        if(!uncategorized.isEmpty()) {
            QTreeWidgetItem *uncatItem = new QTreeWidgetItem(treeWidget);
            uncatItem->setText(0, "未分类");
            uncatItem->setText(1, QString::number(uncategorized.size()));
            // ...类似添加子项...
        }
    };

    connect(refreshBtn, &QPushButton::clicked, refreshScheduleView);
    refreshScheduleView();

    // 双击查看详情
    connect(treeWidget, &QTreeWidget::itemDoubleClicked, [=](QTreeWidgetItem *item, int column){
        if(item->parent()) { // 子项（具体日程）
            ScheduleItem si = item->data(0, Qt::UserRole).value<ScheduleItem>();
            showScheduleDetail(si);
        }
    });
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
