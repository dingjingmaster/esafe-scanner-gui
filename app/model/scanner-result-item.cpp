#include "scanner-result-item.h"

#include <QDebug>
#include <QDateTime>

ScannerResultItem::ScannerResultItem(QObject *parent)
    : QObject{parent}
{

}

ScannerResultItem::ScannerResultItem(QString taskName, QString fileName, Status status, qint64 createTime, qint64 modifyTime, QObject *parent)
    : QObject{parent}
{
    mTaskName = taskName;
    mFileName = fileName;
    mStatus = status;
    mFileCreateTime = createTime;
    mFileModifyTime = modifyTime;
}

void ScannerResultItem::setID(qint64 id)
{
    mID = id;
}

void ScannerResultItem::setChecked(bool checked)
{
    mChecked = checked;
}

void ScannerResultItem::setTaskName(QString name)
{
    mTaskName = name;
}

void ScannerResultItem::setFileName(QString fileName)
{
    mFileName = fileName;
}

void ScannerResultItem::setStatus(int status)
{
    switch (status) {
    case MisReport:
        mStatus = MisReport;
        break;
    case Deleted:
        mStatus = Deleted;
        break;
    case Untreated:
    default:
        mStatus = Untreated;
        break;
    }
}

void ScannerResultItem::setStatus(Status status)
{
    mStatus = status;
}

void ScannerResultItem::setStatus(QString status)
{
    if ("误报" == status) {
        mStatus = MisReport;
    } else if ("删除" == status) {
        mStatus = Deleted;
    } else {
        mStatus = Untreated;
    }
}

void ScannerResultItem::setFileCreateTime(QString time)
{
    qInfo() << "result create time: " << time;
    mFileCreateTime = QDateTime::fromString (time, "yyyy-MM-dd hh:mm:ss").toSecsSinceEpoch ();
}

void ScannerResultItem::setFileCreateTime(qint64 time)
{
    mFileCreateTime = time;
}

void ScannerResultItem::setFileModifyTime(qint64 time)
{
    mFileModifyTime = time;
}

bool ScannerResultItem::canUntreated()
{
    return mCanUntreated;
}

void ScannerResultItem::setCanUntreated(bool f)
{
    mCanUntreated = f;
}

qint64 ScannerResultItem::getID()
{
    return mID;
}

bool ScannerResultItem::getChecked()
{
    return mChecked;
}

QString ScannerResultItem::getTaskName()
{
    return (nullptr == mTaskName || mTaskName.isEmpty()) ? "" : mTaskName;
}

QString ScannerResultItem::getFileName()
{
    return (nullptr == mFileName || mFileName.isEmpty()) ? "" : mFileName;
}

QString ScannerResultItem::getStatus()
{
    switch (mStatus) {
    case MisReport:
        return "误报";
    case Deleted:
        return "删除";
    default:
        break;
    }
    
    return "未处理";
}

ScannerResultItem::Status ScannerResultItem::getStatus2()
{
    return mStatus;
}

int ScannerResultItem::getStatus(QString status)
{
    if ("误报" == status) {
        return static_cast<int>(MisReport);
    } else if ("删除" == status) {
        return static_cast<int>(Deleted);
    }
    
    return static_cast<int>(Untreated);
}

QString ScannerResultItem::getFileCreateTime()
{
    return mFileCreateTime <= 0 ? "" : QDateTime::fromSecsSinceEpoch(mFileCreateTime).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString ScannerResultItem::getFileModifyTime()
{
    return mFileModifyTime <= 0 ? "" : QDateTime::fromSecsSinceEpoch(mFileModifyTime).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString ScannerResultItem::getFilterName()
{
    return mFilterName;
}
