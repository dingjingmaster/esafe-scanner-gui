#ifndef SCANNERRESULTITEM_H
#define SCANNERRESULTITEM_H

#include <QObject>

class ScannerResultItem : public QObject
{
    Q_OBJECT
public:
    explicit ScannerResultItem(QObject *parent = nullptr);

public:
    QString             mTaskName;
    QString             mFileName;
    QString             mStatus;
    QString             mFileCreateTime;
    QString             mFileModifyTime;

};

#endif // SCANNERRESULTITEM_H
