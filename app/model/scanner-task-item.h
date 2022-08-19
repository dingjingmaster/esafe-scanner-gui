#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    // 未知 -- 未开始、扫描中、已停止、已完成、已暂停、扫描发生错误
    // -- 任务状态：0 - 未开始，1 - 扫描中，2 - 已停止，3 - 已完成, 4 - 已暂停, 5 - 继续扫描, 13 - 扫描发生错误
    enum Status { Unknow = -1, NoBegin = 0, Scanning, Stop, Finish, Suspended = 4, Continue = 5, Error = 13 }; Q_ENUM(Status)
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);
    ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QObject *parent = nullptr);

    void setStatus (int tatus);
    void setID (QString id);
    void setName (QString name);
    void setStatus (Status tatus);
    void setStopTime (qint64 stopTime);
    void setStartTime (qint64 startTime);

    void setIsSelfCheck (int s);
    void setIsSelfCheck (bool s);

    QString getScanDir2 ();
    QStringList getScanDir ();
    void setScanDir (QString scanDir);

    void setTaskFileCount(qint64 count);
    void setScanFileCount(qint64 count);
    void setFilterName (QString filterName);
    void setScanFinishedFileCount(qint64 count);

    QString getID ();
    QString getName ();

    bool getSelfCheck();
    int getStatus2 ();
    QString getStatus ();

    qint64 getStartTime2 ();
    QString getStartTime ();

    qint64 getStopTime2 ();
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
    QString                 mScanDir;
    QString                 mOperation;
    QString                 mFilterName;

    bool                    mSelfCheck;                     // 是否自查任务

    Status                  mStatus;
    qint64                  mStopTime;
    qint64                  mStartTime;

    qint64                  mTaskFileCount;
    qint64                  mScanFileCount;
    qint64                  mScanFinishedFileCount;
};

#endif // SCANNERTASKITEM_H
