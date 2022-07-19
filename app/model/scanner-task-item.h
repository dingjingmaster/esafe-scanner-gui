#ifndef SCANNERTASKITEM_H
#define SCANNERTASKITEM_H

#include <QObject>

class ScannerTaskItem : public QObject
{
    Q_OBJECT
public:
    explicit ScannerTaskItem(QObject *parent = nullptr);

public:
    QString                 mName;
    QString                 mStatus;
    qint64                  mStartTime;
    qint64                  mStopTime;
    QString                 mProcess;
    QString                 mOperation;
};

#endif // SCANNERTASKITEM_H
