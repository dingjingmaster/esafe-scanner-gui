#ifndef SCANNERRESULTITEM_H
#define SCANNERRESULTITEM_H

#include <QObject>

class ScannerResultItem : public QObject
{
    Q_OBJECT
public:
    enum Status { Untreated, Fix }; Q_ENUM(Status)
public:
    explicit ScannerResultItem(QObject *parent = nullptr);
    ScannerResultItem(QString taskName, QString fileName, Status status, qint64 createTime, qint64 modifyTime, QObject *parent = nullptr);

    void setTaskName (QString name);
    void setFileName (QString fileName);
    void setStatus (Status status);
    void setFileCreateTime (qint64 time);
    void setFileModifyTime (qint64 time);

    QString getTaskName ();
    QString getFileName ();
    QString getStatus ();
    QString getFileCreateTime ();
    QString getFileModifyTime ();

private:
    QString             mTaskName;
    QString             mFileName;
    QString             mStatus;
    qint64              mFileCreateTime;
    qint64              mFileModifyTime;

};

#endif // SCANNERRESULTITEM_H
