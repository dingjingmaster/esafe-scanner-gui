#include "scanner-result-item.h"

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
    mStatus = Untreated;
}

void ScannerResultItem::setStatus(Status status)
{
    mStatus = status;
}

void ScannerResultItem::setFileCreateTime(qint64 time)
{
    mFileCreateTime = time;
}

void ScannerResultItem::setFileModifyTime(qint64 time)
{
    mFileModifyTime = time;
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
    return "未处理";
}

QString ScannerResultItem::getFileCreateTime()
{
    return QDateTime::fromMSecsSinceEpoch(mFileCreateTime).toLocalTime().toString("yyyy-MM-dd_hh:mm:ss");
}

QString ScannerResultItem::getFileModifyTime()
{
    return QDateTime::fromMSecsSinceEpoch(mFileModifyTime).toLocalTime().toString("yyyy-MM-dd_hh:mm:ss");
}

QString ScannerResultItem::getFilterName()
{
    return mFilterName;
}
