#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    // 未知 -- 未开始、扫描中、已停止、已完成、已暂停、扫描发生错误
    // -- 任务状态：0 - 未开始，1 - 扫描中，2 - 已停止，3 - 已完成, 5 - 已暂停, 13 - 扫描发生错误
    enum Status { Unknow = -1, NoBegin = 0, Scanning, Stop, Finish, Suspended = 5, Error = 13 }; Q_ENUM(Status)
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);
    ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QObject *parent = nullptr);

    void setStatus (int tatus);
    void setID (QString id);
    void setName (QString name);
    void setStatus (Status tatus);
    void setStopTime (qint64 stopTime);
    void setStartTime (qint64 startTime);

    void setTaskFileCount(qint64 count);
    void setScanFileCount(qint64 count);
    void setFilterName (QString filterName);
    void setScanFinishedFileCount(qint64 count);

    QString getID ();
    QString getName ();
    QString getStatus ();
    QString getStartTime ();
    QString getStopTime ();
    QString getFilterName ();
    qint64 getTaskFileCount();
    qint64 getScanFileCount();
    qint64 getScanFinishedFileCount();
    QString getProgress ();
    QString getOperation ();

private:
    QString                 mID;
    QString                 mName;
    Status                  mStatus;
    qint64                  mStartTime;
    qint64                  mStopTime;

    qint64                  mTaskFileCount;
    qint64                  mScanFileCount;
    qint64                  mScanFinishedFileCount;

    QString                 mFilterName;
    QString                 mOperation;
};

#endif // SCANNERTASKITEM_H
