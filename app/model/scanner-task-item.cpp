#include "scanner-task-item.h"

#include <QDateTime>

ScannerTaskItem::ScannerTaskItem(QObject *parent)
    : QObject{parent}
{

}

ScannerTaskItem::ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QString progress)
{
    mName = name;
    mStatus = status;
    mStartTime = startTime;
    mStopTime = stopTime;
    mProgress = progress;
}

void ScannerTaskItem::setName(QString name)
{
    mName = name;
}

void ScannerTaskItem::setStatus(Status status)
{
    mStatus = status;
}

void ScannerTaskItem::setStopTime(qint64 stopTime)
{
    mStopTime = stopTime;
}

void ScannerTaskItem::setStartTime(qint64 startTime)
{
    mStartTime = startTime;
}

void ScannerTaskItem::setProgress(QString progress)
{
    mProgress = progress;
}

QString ScannerTaskItem::getName()
{
    return (mName == nullptr || mName.isEmpty()) ? "" : mName;
}

QString ScannerTaskItem::getStatus()
{
    switch (mStatus) {
    case Scanning:
        return tr("进行中");
    case Finish:
        return tr("已完成");
    }

    return tr("未知");
}

QString ScannerTaskItem::getStartTime()
{
    return QDateTime::fromMSecsSinceEpoch(mStartTime).toLocalTime().toString("yyyy-MM-dd_hh:mm:ss");
}

QString ScannerTaskItem::getStopTime()
{
    return QDateTime::fromMSecsSinceEpoch(mStartTime).toLocalTime().toString("yyyy-MM-dd_hh:mm:ss");
}

QString ScannerTaskItem::getProgress()
{
    return mProgress;
}

QString ScannerTaskItem::getOperation()
{
    return tr("查看结果");
}
