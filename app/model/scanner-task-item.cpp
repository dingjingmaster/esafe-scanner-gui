#include "scanner-task-item.h"

#include <QDebug>
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
    case NoBegin:
        mStatus = NoBegin;
        break;
    case Scanning:
        mStatus = Scanning;
        break;
    case Stop:
        mStatus = Stop;
        break;
    case Finish:
        mStatus = Finish;
        break;
    case Suspended:
        mStatus = Suspended;
        break;
    case Error:
        mStatus = Error;
        break;
    case Unknow:
    default:
        mStatus = Unknow;
        break;
    }
}

void ScannerTaskItem::setID(QString id)
{
    mID = id;
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

QString ScannerTaskItem::getScanDir2()
{
    return mScanDir;
}

QStringList ScannerTaskItem::getScanDir()
{
    return mScanDir.split ("|").toSet ().toList ();
}

void ScannerTaskItem::setScanDir(QString scanDir)
{
    mScanDir = scanDir;
}

void ScannerTaskItem::setTaskFileCount(qint64 count)
{
    mTaskFileCount = count;
}

void ScannerTaskItem::setScanFileCount(qint64 count)
{
    mScanFileCount = count;
}

void ScannerTaskItem::setFilterName(QString filterName)
{
    mFilterName = filterName;
}

void ScannerTaskItem::setScanFinishedFileCount(qint64 count)
{
    mScanFinishedFileCount = count;
}

QString ScannerTaskItem::getID()
{
    return (nullptr == mID || mID.isNull() || mID.isEmpty()) ? " " : mID;
}

QString ScannerTaskItem::getName()
{
    return (nullptr == mName || mName.isNull() || mName.isEmpty()) ? " " : mName;
}

int ScannerTaskItem::getStatus2()
{
    return mStatus;
}

/**
 * @brief 任务状态：未开始、扫描中、已停止、已完成、已暂停、扫描发生错误
 *  Unknow = 0, NoBegin, Scanning, Stop, Finish, Suspended, Error
 */
QString ScannerTaskItem::getStatus()
{
    switch (mStatus) {
    case NoBegin:
        return tr("未开始");
    case Scanning:
        return tr("扫描中");
    case Stop:
        return tr("已停止");
    case Finish:
        return tr("已完成");
    case Suspended:
        return tr("已暂停");
    case Error:
        return tr("扫描发生错误");
    case Unknow:
    default:
        break;
    }

    return tr("未知");
}

qint64 ScannerTaskItem::getStartTime2()
{
    return mStartTime;
}

QString ScannerTaskItem::getStartTime()
{
    qInfo() << "start time:" << mStopTime;
    return mStartTime <= 0 ? "" : QDateTime::fromSecsSinceEpoch(mStartTime).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

qint64 ScannerTaskItem::getStopTime2()
{
    return mStopTime;
}

QString ScannerTaskItem::getStopTime()
{
    QDateTime tim = QDateTime::fromSecsSinceEpoch (mStopTime);

    qInfo() << "stop time:" << mStopTime;

    return mStopTime <= 0 ? "" : QDateTime::fromSecsSinceEpoch(mStopTime).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString ScannerTaskItem::getFilterName()
{
    return (nullptr != mFilterName && !mFilterName.isNull() && !mFilterName.isEmpty()) ? mFilterName : "";
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
    return QString("已经扫描文件总数: %1\n"
                   "本次扫描文件总数: %2\n"
                   "扫描范围文件总数: %3\n")
            .arg(getScanFinishedFileCount())
            .arg(getScanFileCount())
            .arg(getTaskFileCount());
}

QString ScannerTaskItem::getOperation()
{
    return tr("查看结果");
}
