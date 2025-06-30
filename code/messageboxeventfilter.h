#ifndef MESSAGEBOXEVENTFILTER_H
#define MESSAGEBOXEVENTFILTER_H

#include <QObject>
#include <QDebug>
#include <QMessageBox> // 需要这个头文件来识别 QMessageBox 类
#include <QLabel> // 可能需要识别 QLabel
#include <QDialogButtonBox> // 可能需要识别 QDialogButtonBox
#include <QPushButton> // 可能需要识别 QPushButton
#include <QFrame> // 可能会遇到 QFrame
#include <QWidget> // 重要的通用 Widget
#include <QEvent>

class MessageBoxEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MessageBoxEventFilter(QObject *parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        // 过滤 ChildAdded 事件，因为 QMessageBox 会动态创建其子控件
        if (event->type() == QEvent::ChildAdded) {
            QChildEvent *childEvent = static_cast<QChildEvent*>(event);
            QObject *child = childEvent->child();

            // 尝试识别 QMessage 的弹窗本身
            QMessageBox *msgBoxParent = qobject_cast<QMessageBox*>(obj);
            if (msgBoxParent) {
                // 如果当前 obj 是一个 QMessageBox 实例
                // 并且它的窗口标题是你的日程弹窗标题，那么我们正在观察正确的 QMessageBox
                // 这是一个更精确的判断，防止误伤其他 QMessageBox
                // 根据你的代码，标题是 date.toString("yyyy-MM-dd")+"日程"
                // 你可能需要一个更通用的匹配，或者在测试时点击一个固定日期的日程
                if (msgBoxParent->windowTitle().contains("日程")) { // 简化判断，实际应更精确
                    qDebug() << "  -> QMessageBox Child Added:";
                    qDebug() << "     Parent (QMessageBox): " << msgBoxParent->metaObject()->className() << " Name: " << msgBoxParent->objectName();
                    qDebug() << "     Child: " << child->metaObject()->className() << " Name: " << child->objectName();

                    // 进一步尝试获取 QWidget 指针，以便将来设置样式
                    QWidget *childWidget = qobject_cast<QWidget*>(child);
                    if (childWidget) {
                        qDebug() << "     Child is QWidget. Geometry: " << childWidget->geometry();
                        // !!! 关键调试点：在这里临时为子Widget设置一个醒目的背景色 !!!
                        // 这将帮助你视觉上定位哪个组件是黑色的。
                        // childWidget->setStyleSheet("background-color: red; border: 2px solid yellow;");
                        // 暂时不要 uncomment 上面这行，先看输出
                    }
                    qDebug() << "---------------------------------------";
                }
            }
        }
        // 对于其他事件，传递给默认处理器
        return QObject::eventFilter(obj, event);
    }
};

#endif // MESSAGEBOXEVENTFILTER_H
