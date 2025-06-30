#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ScheduleModel.h"
#include <QLabel>
#include <QPushButton>
#include "calendarview.h"
#include "category.h"
#include "schedulefilterproxymodel.h"
#include "QTableWidget"

namespace Ui {
class MainWindow;
}

enum class Theme { Light, Dark };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    static QList<Category> loadCategoriesFromJson();
    ~MainWindow();

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onFilter();
    void onToggleTheme();
    void onBackgroundImageChanged(const QString& imagePath);
private:
    Ui::MainWindow *ui;
    ScheduleModel *model;
    ScheduleFilterProxyModel *proxyModel; //代理模型，用于总览界面的排序
    void setupTableView();
    void setupCalendarView();
    void setupcategoryPage();
    void initConfigFile();
    static void saveCategoryToJson(const Category &category);
    static QJsonObject loadJsonConfig();
    static QString getConfigPath();
    bool categoryExists(const QString &name);
    Category findCategoryByName(const QString &name);
    void updateCategoryInJson(const Category &updatedCat, const QString &oldName);
    void removeCategoryFromJson(const QString &categoryName);
    void setupFileManagementPage();
    void saveFileMetadata(const FileInfo &info);
    void refreshFiles(const QString &category, QTableWidget *table);
    void removeFileMetadata(const QString &filePath);
    void refreshFileManagementCategories();
    void showScheduleDetail(const ScheduleItem &item);
    QString getSchedulePath();
    QList<ScheduleItem> loadAllSchedules();
    void saveAllSchedules(const QList<ScheduleItem> &items);

    CalendarView *m_calendarView;

    QLabel *monthLabel;
    QPushButton *prevMonthBtn;
    QPushButton *nextMonthBtn;
    void setupTheme();
    void applyTheme(Theme theme);
    void saveThemePreference(Theme theme) const;
    Theme loadThemePreference() const;
    Theme m_currentTheme;
    static const QString CONFIG_FILENAME;
};

#endif // MAINWINDOW_H
