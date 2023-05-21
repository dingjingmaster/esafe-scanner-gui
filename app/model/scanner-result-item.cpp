#include "scanner-result-item.h"

#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <QItemSelectionModel>

ScannerResultItem::ScannerResultItem(QObject *parent)
    : QObject{parent}
{
    mTaskName = "";
    mFileName = "";
    mStatus = Untreated;
    mFileCreateTime = 0;
    mFileModifyTime = 0;
}

ScannerResultItem::ScannerResultItem(QString taskName, QString fileName, Status status, qint64 createTime, qint64 modifyTime, QObject *parent)
    : QObject{parent}
{
    mTaskName = std::move(taskName);
    mFileName = std::move(fileName);
    mStatus = status;
    mFileCreateTime = createTime;
    mFileModifyTime = modifyTime;
}

void ScannerResultItem::setID(qint64 id)
{
    mID = id;
}

void ScannerResultItem::setID(QString id)
{
    if (nullptr == id || id.isNull () || id.isEmpty () || "" == id) {
        return;
    }

    mID = id.toInt ();
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
    case 6:
        mStatus = MisReport;
        break;
    case 5:
        mStatus = Deleted;
        break;
    case 0:
    default:
        mStatus = Untreated;
        break;
    }
}

void ScannerResultItem::setStatus(Status status)
{
    mStatus = status;
}

void ScannerResultItem::setStatus(const QString& status)
{
    if ("例外文件" == status) {
        mStatus = MisReport;
    } else if ("删除" == status) {
        mStatus = Deleted;
    } else {
        mStatus = Untreated;
    }
}

void ScannerResultItem::setFileCreateTime(const QString& time)
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
    return (nullptr == mTaskName || mFileName.isNull() || mTaskName.isEmpty()) ? "" : mTaskName;
}

QString ScannerResultItem::getFileName() const
{
    return (nullptr == mFileName || mFileName.isNull() || mFileName.isEmpty()) ? "" : mFileName;
}

QString ScannerResultItem::getStatus()
{
    switch (mStatus) {
    case MisReport:
        return "例外文件";
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

int ScannerResultItem::getStatus(const QString& status)
{
    if ("例外文件" == status) {
        return static_cast<int>(MisReport);
    } else if ("删除" == status) {
        return static_cast<int>(Deleted);
    }
    
    return static_cast<int>(Untreated);
}

QString ScannerResultItem::getFileCreateTime() const
{
    return mFileCreateTime <= 0 ? "" : QDateTime::fromSecsSinceEpoch(mFileCreateTime).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString ScannerResultItem::getFileModifyTime()
{
    qint64 tim = mFileModifyTime;
    if (!tim && QFileInfo::exists(mFileName)) {
        QFileInfo fi(mFileName);
        tim = fi.lastModified().toSecsSinceEpoch();
    }

    return tim <= 0 ? "" : QDateTime::fromSecsSinceEpoch(tim).toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString ScannerResultItem::getFilterName() const
{
    return mFilterName;
}

qint64 ScannerResultItem::getFileCreateTime2()
{
    return mFileCreateTime;
}

void ScannerResultItem::setFileType(const QString &fileType)
{
    mFileType = fileType;
}

QString ScannerResultItem::getFileType()
{
    return mFileType;
}
