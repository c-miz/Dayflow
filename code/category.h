#ifndef CATEGORY_H
#define CATEGORY_H
#include "qstring.h"
#include "QDateTime"
#include "QJsonObject"
class Category
{
public:
    QString name;
    QString storagePath;
    QDateTime createTime;

    // 序列化方法
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["path"] = storagePath;
        obj["time"] = createTime.toString(Qt::ISODate);
        return obj;
    }

    // 反序列化方法
    void fromJson(const QJsonObject &json) {
        name = json["name"].toString();
        storagePath = json["path"].toString();
        createTime = QDateTime::fromString(json["time"].toString(), Qt::ISODate);
    }
    Category()=default;
    bool isEmpty() const {
        return name.isEmpty() && storagePath.isEmpty();
    }


};
struct FileInfo {
    QString fileName;
    QString filePath;
    qint64 fileSize;
    QDateTime uploadTime;
    QString category;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = fileName;
        obj["path"] = filePath;
        obj["size"] = QString::number(fileSize);
        obj["time"] = uploadTime.toString(Qt::ISODate);
        obj["category"] = category;
        return obj;
    }

    static FileInfo fromJson(const QJsonObject &obj) {
        FileInfo info;
        info.fileName = obj["name"].toString();
        info.filePath = obj["path"].toString();
        info.fileSize = obj["size"].toString().toLongLong();
        info.uploadTime = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
        info.category = obj["category"].toString();
        return info;
    }
};

#endif // CATEGORY_H
