#ifndef COURSESCHEDULE_H
#define COURSESCHEDULE_H

#include <QMainWindow>
#include <QJsonArray>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QLineEdit;
class QComboBox;
class QTimeEdit;
class QPushButton;
class QLabel;
class QStatusBar;
QT_END_NAMESPACE

class CourseSchedule : public QMainWindow
{
    Q_OBJECT

public:
    explicit CourseSchedule(QWidget *parent = nullptr);

private slots:
    void addCourse();
    void editCourse();
    void deleteCourse();
    void saveCourses();
    void loadCourses();
    void cellDoubleClicked(int row, int column);
    void showContextMenu(const QPoint &pos);

private:
    void setupUI();
    void setupTable();
    void clearInputs();
    int findInsertionRow(int day, const QTime &startTime);
    void populateFormFromSelection();
    bool validateCourseInput();
    void updateCourseInTable(int row, int col);
    void createTimeSlots();

    // UI 组件
    QTableWidget *scheduleTable;
    QLineEdit *courseNameEdit;
    QComboBox *dayComboBox;
    QTimeEdit *startTimeEdit;
    QTimeEdit *endTimeEdit;
    QLineEdit *locationEdit;
    QLineEdit *instructorEdit;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QStatusBar *statusBar;

    // 数据存储
    QJsonArray courses;
    QList<QTime> timeSlots;
    int selectedRow = -1;
    int selectedCol = -1;
};

#endif // COURSESCHEDULE_H
