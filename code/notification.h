// Notification.h
#include <QObject>
#include <QMessageBox>

class Notification : public QObject
{
    Q_OBJECT
public:
    explicit Notification(QObject *parent = nullptr);

public slots:
    void handleReminder(const QString &message);

private:
    void showPopup(const QString &msg);
};
