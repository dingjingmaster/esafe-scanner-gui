#ifndef SCANNERRESULTITEM_H
#define SCANNERRESULTITEM_H

#include <QObject>

class ScannerResultItem : public QObject
{
    Q_OBJECT
public:
    // 任务处理状态：未处理、误报、删除
    enum Status { Untreated, MisReport, Deleted }; Q_ENUM(Status)
public:
    explicit ScannerResultItem(QObject *parent = nullptr);
    ScannerResultItem(QString taskName, QString fileName, Status status, qint64 createTime, qint64 modifyTime, QObject *parent = nullptr);

    void setID(qint64 id);
    void setID(QString id);
    void setChecked (bool);
    void setStatus (int status);
    void setStatus (Status status);
    void setStatus (QString status);
    void setTaskName (QString name);
    void setFileName (QString fileName);
    void setFileCreateTime (QString time);
    void setFileCreateTime (qint64 time);
    void setFileModifyTime (qint64 time);

    // 是否可以执行未处理操作？
    // 误报、删除不可执行未处理
    bool canUntreated ();
    void setCanUntreated (bool f);

    qint64 getID();

    bool getChecked ();
    QString getTaskName ();
    QString getFileName ();
    QString getFilterName ();

    qint64 getFileCreateTime2 ();
    QString getFileCreateTime ();
    QString getFileModifyTime ();
    
    QString getStatus ();
    Status getStatus2 ();
    static int getStatus (QString status);

private:
    bool                mChecked = false;
    bool                mCanUntreated = true;
    
    qint64              mID;
    Status              mStatus;
    QString             mTaskName;
    QString             mFileName;
    QString             mFilterName;
    qint64              mFileCreateTime;
    qint64              mFileModifyTime;
};

#endif // SCANNERRESULTITEM_H
