#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    enum Status { Scanning, Finish }; Q_ENUM(Status)
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);
    ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QString progress, QObject *parent = nullptr);

    void setName (QString name);
    void setStatus (Status tatus);
    void setStopTime (qint64 stopTime);
    void setStartTime (qint64 startTime);
    void setProgress (QString progress);

    QString getName ();
    QString getStatus ();
    QString getStartTime ();
    QString getStopTime ();
    QString getProgress ();
    QString getOperation ();

private:
    QString                 mName;
    Status                  mStatus;
    qint64                  mStartTime;
    qint64                  mStopTime;
    QString                 mProgress;
    QString                 mOperation;
};

#endif // SCANNERTASKITEM_H
