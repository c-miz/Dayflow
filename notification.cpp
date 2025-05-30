#include "notification.h"

#include <QObject>
#include <QMessageBox>

Notification::Notification(QObject *parent)
    : QObject(parent)
{
}
void Notification::handleReminder(const QString &message)
{
    showPopup(message);
}

void Notification::showPopup(const QString &msg)
{
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("日程提醒"));
    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setWindowFlags(Qt::WindowStaysOnTopHint);
    msgBox->show();
}
