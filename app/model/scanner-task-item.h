#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    enum Status { Unknow, Scanning, Suspended, Finish }; Q_ENUM(Status)
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);
    ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QObject *parent = nullptr);

    void setName (QString name);
    void setStatus (Status tatus);
    void setStopTime (qint64 stopTime);
    void setStartTime (qint64 startTime);

    void setTaskFileCount(qint64 count);
    void setScanFileCount(qint64 count);
    void setScanFinishedFileCount(qint64 count);

    QString getName ();
    QString getStatus ();
    QString getStartTime ();
    QString getStopTime ();
    qint64 getTaskFileCount();
    qint64 getScanFileCount();
    qint64 getScanFinishedFileCount();
    QString getProgress ();
    QString getOperation ();

private:
    QString                 mName;
    Status                  mStatus;
    qint64                  mStartTime;
    qint64                  mStopTime;

    qint64                  mTaskFileCount;
    qint64                  mScanFileCount;
    qint64                  mScanFinishedFileCount;

    QString                 mOperation;
};

#endif // SCANNERTASKITEM_H
