#include "scanner-task-item.h"

#include <QDateTime>

ScannerTaskItem::ScannerTaskItem(QObject *parent)
    : QObject(parent)
{

}

ScannerTaskItem::ScannerTaskItem(QString name, Status status, qint64 startTime, qint64 stopTime, QObject* parent)
    : QObject(parent)
{
    mName = name;
    mStatus = status;
    mStartTime = startTime;
    mStopTime = stopTime;
}

void ScannerTaskItem::setStatus(int status)
{
    switch (status) {
    case Scanning:
        mStatus = Scanning;
        break;
    case Finish:
        mStatus = Finish;
        break;
    case Suspended:
        mStatus = Suspended;
        break;
    case Unknow:
    default:
        mStatus = Unknow;
        break;
    }
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

void ScannerTaskItem::setTaskFileCount(qint64 count)
{
    mTaskFileCount = count;
}

void ScannerTaskItem::setScanFileCount(qint64 count)
{
    mScanFileCount = count;
}

void ScannerTaskItem::setScanFinishedFileCount(qint64 count)
{
    mScanFinishedFileCount = count;
}

QString ScannerTaskItem::getName()
{
    return (nullptr == mName || mName.isNull() || mName.isEmpty()) ? " " : mName;
}

QString ScannerTaskItem::getStatus()
{
    switch (mStatus) {
    case Scanning:
        return tr("进行中");
    case Finish:
        return tr("已完成");
    case Suspended:
        return tr("暂停");
    case Unknow:
    default:
        break;
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

qint64 ScannerTaskItem::getTaskFileCount()
{
    return mTaskFileCount > 0 ? mTaskFileCount : 0;
}

qint64 ScannerTaskItem::getScanFileCount()
{
    return mScanFileCount > 0 ? mScanFileCount : 0;
}

qint64 ScannerTaskItem::getScanFinishedFileCount()
{
    return mScanFinishedFileCount > 0 ? mScanFinishedFileCount : 0;
}

QString ScannerTaskItem::getProgress()
{
    return QString("已扫描文件数: %1\n"
                   "文件总数: %2\n"
                   "终端文件数: %3\n")
            .arg(getScanFinishedFileCount())
            .arg(getScanFileCount())
            .arg(getTaskFileCount());
}

QString ScannerTaskItem::getOperation()
{
    return tr("查看结果");
}
