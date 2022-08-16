#include "../db/db-manager.h"
#include "scanner-result-item.h"
#include "scanner-result-model.h"

#include <QList>
#include <QDebug>
#include <QColor>

#include "../utils/scan-result-helper.h"

ScannerResultModel::ScannerResultModel(QObject* parent)
    : QAbstractTableModel{parent}, mScanResultHelper(DBManager::instance()->getResultHelper())
{
    // 数据库与model连接
    connect(mScanResultHelper, &ScanResultHelper::addNewFile, this, &ScannerResultModel::addItem);
    connect(mScanResultHelper, qOverload<ScannerResultItem*>(&ScanResultHelper::delOldFile), this, &ScannerResultModel::delItem);

    connect(mScanResultHelper, &ScanResultHelper::detailOne, this, [=] () {
        int cur = (++mCur <= mTotal) ? mCur : mTotal;
        progress(cur, mTotal);
    });

    // 清空数据 showData (QString TaskName, QString filterName);
    //connect(this, &ScannerResultModel::clearData, mScanResultHelper, &ScanResultHelper::clearData);
    connect(this, &ScannerResultModel::showData, this, [=] (QString taskName, QString filterName, QStringList scanDir) {
        mScanResultHelper->loadTaskResult(taskName, filterName, scanDir);
    });

    connect(this, &ScannerResultModel::clearData, this, [=] () {
        beginResetModel();
        mData.clear();
        endResetModel();
        mScanResultHelper->clearData();
        mDelete = 0;
        mNoFix = 0;
        mMisReport = 0;
        mChangedItem.clear ();
    });
    

    //mThread = new QThread(this);
    //connect(this, &ScannerResultModel::showData, mScanResultHelper, &ScanResultHelper::loadTaskResult);
    //moveToThread(mThread);

    //mThread->start();
    DBManager::instance()->refreshScanResult ();
    //mScanResultHelper->loadTaskResult();
}

bool ScannerResultModel::hasChanged()
{
    return mChangedItem.count () > 0;
}

bool ScannerResultModel::hasChecked()
{
    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (i.i->t()->getChecked()) {
            return true;
        }
    }

    return false;
}

bool ScannerResultModel::isCheckAllItems()
{
    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (!i.i->t()->getChecked()) {
            return false;
        }
    }

    return true;
}

void ScannerResultModel::addItem(ScannerResultItem* item)
{
    if (!item)      return;

    mLocker.lock();

    mData.append(item);
    insertRows(mData.count() - 1, 1);
    changeItemCount(item->getStatus2 ());

    if ((mCurIndex - 10 <= 0) || (rowCount() < mCurIndex + 30)) {
        QModelIndex idx = getIndexByItem(item);
        Q_EMIT dataChanged (idx, idx);
    }

    mLocker.unlock();
}

void ScannerResultModel::delItem(ScannerResultItem *item)
{
    if (!item)      return;

    mLocker.lock();

    QModelIndex idx = getIndexByItem(item);

    if (!idx.isValid())     return;
    
    if (mData.contains (item))  mData.removeOne (item);
    removeRow (idx.row());
    changeItemCount(item->getStatus2 (), false);

    if (mCurIndex < 30 || rowCount() < mCurIndex + 30) {
        Q_EMIT dataChanged (idx, idx);
    }

    mLocker.unlock();
}

int ScannerResultModel::getNoFixCount() 
{
    return mNoFix; 
}

int ScannerResultModel::getDeleteCount() 
{
    return mDelete; 
}

int ScannerResultModel::getMisReportCount() 
{
    return mMisReport;
}

int ScannerResultModel::getAllCount()
{
    return rowCount();
}

QList<const ScannerResultItem *> ScannerResultModel::getChangedItem()
{
    return mChangedItem.keys ();
}

QModelIndex ScannerResultModel::getIndexByItem(const ScannerResultItem *item, int column)
{
    if (!item) {
        qDebug() << "item is null";
        return QModelIndex();
    }
    
    int rows = rowCount();
    for (auto i = 0; i < rows; ++i) {
        QModelIndex ii = index(i, column);
        const ScannerResultItem* it = static_cast <const ScannerResultItem*> (ii.internalPointer());
        if (it == item) {
            return ii;
        }
    }
    
    qDebug() << "item not found!";
        
    return QModelIndex();
}

#if 0
void ScannerResultModel::saveResult()
{
    // FIXME:// 此处需要注意，result model数据不能被修改
    mLocker.lock();

    QStringList del;
    QStringList misReport;
    auto ls = getChangedItem ();

    for (auto l : ls) {
        ScannerResultItem* item = const_cast<ScannerResultItem*>(l);
        QString fileName = QString("%1").arg(item->getID ());

        int status = item->getStatus2 ();
        if (ScannerResultItem::MisReport == status) {
            misReport << fileName;
        } else if (ScannerResultItem::Deleted == status) {
            del << fileName;
        } else {
            qDebug() << "not apply: " << fileName;
            continue;
        }
    }
    mChangedItem.clear();
    mLocker.unlock();

    qDebug() << "delete item: " << del.size();
    qDebug() << "misreport item: " << misReport.size();

    mScanResultHelper->deleteItemByName (del);
    mScanResultHelper->misReportByName (misReport);
}
#endif

QPair<QStringList, QStringList> ScannerResultModel::getSaveItems()
{
    mLocker.lock();

    QStringList del;
    QStringList misReport;
    auto ls = getChangedItem ();

    for (auto l : ls) {
        ScannerResultItem* item = const_cast<ScannerResultItem*>(l);
        QString fileID = QString("%1").arg(item->getID ());

        int status = item->getStatus2 ();
        if (ScannerResultItem::MisReport == status) {
            misReport << fileID;
        } else if (ScannerResultItem::Deleted == status) {
            del << fileID;
        } else {
            qDebug() << "not apply: " << fileID;
            continue;
        }
    }
    mChangedItem.clear();
    mLocker.unlock();

    return {del, misReport};// QPair<QStringList, QStringList>;
}

QList<ScannerResultItem *> ScannerResultModel::getSelectedItem()
{
    QList<ScannerResultItem *> ls;

    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (i.i->t()->getChecked()) {
            ls.append(i.i->t());
        }
    }

    return ls;
}

void ScannerResultModel::changeItemCount(int status, bool isAdd)
{
    switch (status) {
    case ScannerResultItem::MisReport:
        if (isAdd) ++mMisReport; else --mMisReport;
        break;
    case ScannerResultItem::Deleted:
        if (isAdd) ++mDelete; else --mDelete;
        break;
    case ScannerResultItem::Untreated:
        if (isAdd) ++mNoFix; else --mNoFix;
        break;
    default:
        break;
    }
}

void ScannerResultModel::selectAll(bool s)
{
    int all = rowCount();
    for (auto i = 0; i < all; ++i) {
        QModelIndex idx = index(i, 0);
        if (!idx.isValid()) continue;
        static_cast<ScannerResultItem*>(idx.internalPointer())->setChecked(s);
    }
    lazyUpdateView();
}

int ScannerResultModel::rowCount(const QModelIndex &parent) const
{
    return mData.count();
}

int ScannerResultModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant ScannerResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return QVariant();

    auto item = static_cast<ScannerResultItem*>(index.internalPointer());

    if (Qt::DisplayRole == role) {
        if (0 == index.column()) {
            return item->getChecked();
//        } else if (TaskName == index.column()) {
//            return item->getTaskName();
        } else if (FileName == index.column()) {
            return item->getFileName();
        } else if (Status == index.column()) {
            return item->getStatus();
        } else if (CreateTime == index.column()) {
            return item->getFileCreateTime();
        } else if (ModifyTime == index.column()) {
            return item->getFileModifyTime();
        }

    } else if (Qt::BackgroundRole == role) {
        if (0 == index.column()) {
            return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
        }
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

QVariant ScannerResultModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        switch (section) {
        case 0:
            return QString("");
//        case 1:
//            return QString("任务名称");
        case 1:
            return QString("文件名称");
        case 2:
            return QString("处理方式");
        case 3:
            return QString("文件扫描时间");
        case 4:
            return QString("文件修改时间");
        default:
            break;
        }

    } else if (Qt::BackgroundRole == role) {
        return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

bool ScannerResultModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << index;
    
    if (!index.isValid ())          return false;
    
    ScannerResultItem* item = static_cast<ScannerResultItem*> (index.internalPointer ());
    auto toChange = ScannerResultItem::getStatus(value.toString());

    switch (index.column ()) {
    case 2: {
        //ScannerResultItem
        itemStatusChanged(item, static_cast<ScannerResultItem::Status>(toChange));

        // 更新当前 model 里 status 状态
        item->setStatus (value.toString ());
        Q_EMIT dataChanged (index, index);
        
        return true;
    }
    default:
        break;
    }
    
    return false;
}

bool ScannerResultModel::setData(ScannerResultItem &index, const QVariant &value, int role)
{
    auto toChange = ScannerResultItem::getStatus(value.toString());

    //ScannerResultItem
    itemStatusChanged(&index, static_cast<ScannerResultItem::Status>(toChange));

    // 更新当前 model 里 status 状态
    index.setStatus (value.toString ());

    return true;
}

QModelIndex ScannerResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mData.count() - 1) {
            return QModelIndex();
        }

        return createIndex(row, column, mData.at(row));
    }

    return QModelIndex();
}

Qt::ItemFlags ScannerResultModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    Qt::ItemFlags flags = Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    switch (index.column ()) {
    case 2:
        flags |= Qt::ItemIsEditable;
        break;
    default:
        break;
    }

    return flags;
}

bool ScannerResultModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool ScannerResultModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}

void ScannerResultModel::updateCount()
{
    int     tNoFix = 0;
    int     tDelete = 0;
    int     tMisReport = 0;

    for (auto it : mData) {
        switch (it->getStatus2()) {
            case 6: {
                ++tMisReport;
                break;
            }
            case 5: {
                ++tDelete;
                break;
            }
            case 0:
            default: {
                ++tNoFix;
                break;
            }
        }
    }

    mNoFix = tNoFix;
    mDelete = tDelete;
    mMisReport = tMisReport;
}

void ScannerResultModel::setSelectedItemStatus(ScannerResultItem::Status status)
{
    auto selectedItem = getSelectedItem();

    for (auto it : selectedItem) {
        itemStatusChanged(it, status);
    }
}

void ScannerResultModel::itemStatusChanged(ScannerResultItem *item, ScannerResultItem::Status toChange)
{
    if (mChangedItem.contains(item)) {
        auto kv = mChangedItem[item];
        auto savedStatus = kv.first;
        auto lastStatus = kv.second;

        // 修改计数
        changeItemCount(toChange);
        changeItemCount(lastStatus, false);

        // 说明未变
        if (savedStatus == toChange) {
            mChangedItem.remove (item);
        } else {
            QPair<int, int> changedKV(savedStatus, toChange);
            mChangedItem[item] = changedKV;
        }
    } else {
        QPair<int, int> kv(item->getStatus2 (), toChange);
        mChangedItem[item] = kv;
        changeItemCount(toChange);
        changeItemCount(item->getStatus2 (), false);
    }
}

void ScannerResultModel::onScrollbarMoved(float ratio)
{
    mLocker.lock();

    mCurIndex = ratio * rowCount() + 1;

    mLocker.unlock();
}

void ScannerResultModel::applyData()
{
    auto items = getSaveItems();

    mCur = 0;
    mTotal =  items.first.size() + items.second.size();
    Q_EMIT progress(0, mTotal);

    mScanResultHelper->misReportByIDs (items.second);
    mScanResultHelper->deleteItemByIDs (items.first);
}





