#include "../db/db-manager.h"
#include "scanner-result-item.h"
#include "scanner-result-model.h"

#include <QList>
#include <QDebug>
#include <QColor>

#include "../utils/notify-to-filter.h"
#include "../utils/scan-result-helper.h"

ScannerResultModel::ScannerResultModel(QObject* parent)
    : QAbstractTableModel{parent}, mScanResultHelper(DBManager::instance()->getResultHelper())
{
    qRegisterMetaType<QList<QSharedPointer<ScannerResultItem>>>("QList<QSharedPointer<ScannerResultItem>>");

    // 数据库与model连接
    connect(mScanResultHelper, qOverload<const QSharedPointer<ScannerResultItem>&>(&ScanResultHelper::addNewFile), this, qOverload<const QSharedPointer<ScannerResultItem>&>(&ScannerResultModel::addItem));
    connect(mScanResultHelper, qOverload<const QSharedPointer<ScannerResultItem>&>(&ScanResultHelper::delOldFile), this, qOverload<const QSharedPointer<ScannerResultItem>&>(&ScannerResultModel::delItem));

    connect(mScanResultHelper, qOverload<const QList<QSharedPointer<ScannerResultItem>>&>(&ScanResultHelper::addNewFile), this, qOverload<const QList<QSharedPointer<ScannerResultItem>>&>(&ScannerResultModel::addItem));
    connect(mScanResultHelper, qOverload<const QList<QSharedPointer<ScannerResultItem>>&>(&ScanResultHelper::delOldFile), this, qOverload<const QList<QSharedPointer<ScannerResultItem>>&>(&ScannerResultModel::delItem));

    connect (this, qOverload<const QString&>(&ScannerResultModel::deleteItem), mScanResultHelper, &ScanResultHelper::onItemDeleted, Qt::UniqueConnection);

    connect(mScanResultHelper, &ScanResultHelper::detailOne, this, [=] () {
        int cur = (++mCur <= mTotal) ? mCur : mTotal;
        progress(cur, mTotal);
    });

    // 这里不能打开，否则在用户选中数据后，数据库自动更新时候，统计不准确
    //connect(mScanResultHelper, &ScanResultHelper::allItemsUpdated, this, &ScannerResultModel::updateCount);

    // 清空数据 showData (QString TaskName, QString filterName);
    //connect(this, &ScannerResultModel::clearData, mScanResultHelper, &ScanResultHelper::clearData);
    connect(this, qOverload<const QString&, const QString&, const QStringList&, const QString&>(&ScannerResultModel::showData), this,
            [=] (const QString& taskName, const QString& filterName, const QStringList& scanDir, const QString& filterOutDir) {
        mScanResultHelper->loadTaskResult(taskName, filterName, scanDir, filterOutDir);
    });

    connect(this, &ScannerResultModel::clearData, this, [=] () {
        beginResetModel();
        mData.clear();
        mChangedItem.clear ();
        mScanResultHelper->reset();
        endResetModel();
        mNoFix = 0;
        mDelete = 0;
        mMisReport = 0;
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

void ScannerResultModel::addItem(const QSharedPointer<ScannerResultItem>& item)
{
    if (!item)      return;

    mLocker.lock();
    mData.append(item);
    insertRows(mData.count() - 1, 1);
    mLocker.unlock();

    changeItemCount(item->getStatus2 ());

    QModelIndex idx = getIndexByItem(item);
    if ((mCurIndex - 10 <= idx.row()) && (idx.row() <= mCurIndex + 30)) {
        Q_EMIT dataChanged (idx, idx);
    }

    Q_EMIT dataStatueChanged();
}

void ScannerResultModel::addItem(const QList<QSharedPointer<ScannerResultItem>>& item)
{
    mLocker.lock();
    mData.append(item);
    insertRows(mData.count() - 1, item.count());
    mLocker.unlock();

    qDebug() << "add item: " << item.count();

    for (auto& i : item) {
        if (nullptr == i) {
            continue;
        }
        changeItemCount(i->getStatus2 ());
    }

    for (auto& i : item) {
        QModelIndex idx = getIndexByItem(i);
        if (!idx.isValid()) {
            continue;
        }
//        qDebug() << "idx: " << idx;
        if ((mCurIndex - 10 <= idx.row()) && (idx.row() <= mCurIndex + 30)) {
            Q_EMIT dataChanged (idx, idx);
        }
        else {
            break;
        }
    }

    Q_EMIT dataStatueChanged();
}

void ScannerResultModel::delItem(const QSharedPointer<ScannerResultItem>& item)
{
    if (!item)      return;

    mLocker.lock();

    QModelIndex idx = getIndexByItem(item);

    if (!idx.isValid()) {
        mLocker.unlock();
        return;
    }

    if (mData.contains (item))  mData.removeOne (item);
    mLocker.unlock();

    removeRow (idx.row());
    changeItemCount(item->getStatus2 (), false);

    if (mCurIndex < 30 || rowCount() < mCurIndex + 30) {
        Q_EMIT dataChanged (idx, idx);
    }

    Q_EMIT dataStatueChanged();
}


void ScannerResultModel::delItem(const QList<QSharedPointer<ScannerResultItem>>& item)
{
    qDebug() << "delete item: " << item.count();

    mLocker.lock();

    for (auto& i : item) {
        QModelIndex idx = getIndexByItem(i);
        if (!idx.isValid()) {
            continue;
        }
        if (mData.contains (i))  mData.removeOne (i);
        removeRow (idx.row());

        if (mCurIndex < 30 || rowCount() < mCurIndex + 30) {
            Q_EMIT dataChanged (idx, idx);
        }
        else {
            break;
        }
    }

    for (auto& i : item) {
        if (nullptr == i) {
            continue;
        }
        changeItemCount(i->getStatus2 (), false);
        Q_EMIT deleteItem (i->getFileName());
    }

    mLocker.unlock();

    Q_EMIT dataStatueChanged();
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

QList<QSharedPointer<ScannerResultItem>> ScannerResultModel::getChangedItem()
{
    return mChangedItem.keys ();
}

QModelIndex ScannerResultModel::getIndexByItem(const QSharedPointer<ScannerResultItem>& item, int column) const
{
    if (!item) {
        qDebug() << "item is null";
        return {};
    }
    
    int rows = rowCount();
    for (auto i = 0; i < rows; ++i) {
        QModelIndex ii = index(i, column);
        const auto it = static_cast <const ScannerResultItem*> (ii.internalPointer());
        if (it == item) {
            return ii;
        }
    }
    
    qDebug() << "item not found!";
        
    return {};
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

    for (auto& item : ls) {
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

QList<QSharedPointer<ScannerResultItem>> ScannerResultModel::getSelectedItem()
{
    mLocker.lock();
    QList<QSharedPointer<ScannerResultItem>> ls;

    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (i.i->t()->getChecked()) {
            ls.append(i.i->t());
        }
    }
    mLocker.unlock();

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
    if (!index.isValid())       return {};

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
    } else if (Qt::CheckStateRole == role) {
        return item->getChecked() ? Qt::Checked : Qt::Unchecked;
    }
    else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    }

    return {};
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
            return QString("处理状态");
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

    return {};
}

bool ScannerResultModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << index;
    
    if (!index.isValid ())          return false;
    
    auto item = static_cast<ScannerResultItem*> (index.internalPointer ());
    auto toChange = ScannerResultItem::getStatus(value.toString());

    switch (index.column ()) {
    case 2: {
        //ScannerResultItem
        itemStatusChanged(QSharedPointer<ScannerResultItem>(item), static_cast<ScannerResultItem::Status>(toChange));

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
    itemStatusChanged(QSharedPointer<ScannerResultItem>(&index), static_cast<ScannerResultItem::Status>(toChange));

    // 更新当前 model 里 status 状态
    index.setStatus (value.toString ());

    return true;
}

QModelIndex ScannerResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mData.count() - 1) {
            return {};
        }

        return createIndex(row, column, mData.at(row).data());
    }

    return {};
}

Qt::ItemFlags ScannerResultModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    Qt::ItemFlags flags = Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    switch (index.column ()) {
    case 2:
        //flags |= Qt::ItemIsEditable;
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

    mLocker.lock();
    for (auto it : mData) {
        switch (it->getStatus2()) {
            case ScannerResultItem::MisReport: {
                ++tMisReport;
                break;
            }
            case ScannerResultItem::Deleted: {
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
    mLocker.unlock();

    mNoFix = tNoFix;
    mDelete = tDelete;
    mMisReport = tMisReport;

    Q_EMIT dataStatueChanged();
}

void ScannerResultModel::setSelectedItemStatus(ScannerResultItem::Status status)
{
    auto selectedItem = getSelectedItem();

    for (auto it : selectedItem) {
        itemStatusChanged(it, status);
    }
}

void ScannerResultModel::itemStatusChanged(QSharedPointer<ScannerResultItem> item, ScannerResultItem::Status toChange)
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
    mCurIndex = ratio * rowCount() + 1;
}

void ScannerResultModel::applyData()
{
    //auto items = getSaveItems();
    auto items = getSaveItemPointAndIds();

    mCur = 0;
    mTotal =  items.first.second.size() + items.second.second.size();
    Q_EMIT progress(0, mTotal);

    mScanResultHelper->misReportByIDs (items.second.second);
    mScanResultHelper->deleteItemByIDs (items.first.second);

    if (!items.first.first.isEmpty()) {
        for (auto it : items.first.first) {
            delItem (it);
        }
    }
}

void ScannerResultModel::applyMisReportData(const QModelIndex &idx)
{
    if (!idx.isValid()) { return; }

    auto item = static_cast<ScannerResultItem*>(idx.internalPointer());
    if (!item) { return;}
    item->setStatus (ScannerResultItem::MisReport);
    mScanResultHelper->misReportByIDs ((QStringList() << QString("%1").arg(item->getID())));
    updateCount();
    notify_policy_filter (ScannerResultItem::MisReport);
}

void ScannerResultModel::applyDelData(const QModelIndex& idx)
{
    if (!idx.isValid()) { return; }

    auto item = QSharedPointer<ScannerResultItem>(static_cast<ScannerResultItem*>(idx.internalPointer()));
    if (!item) { return;}
    mScanResultHelper->deleteItemByIDs ((QStringList() << QString("%1").arg(item->getID())));
    delItem (item);
    updateCount();
    notify_policy_filter (ScannerResultItem::Deleted);
}

QPair<QList<QSharedPointer<ScannerResultItem>>, QList<QSharedPointer<ScannerResultItem>>> ScannerResultModel::getSaveItemPoints()
{
    mLocker.lock();

    QList<QSharedPointer<ScannerResultItem>> del;
    QList<QSharedPointer<ScannerResultItem>> misReport;
    auto ls = getChangedItem ();

    for (auto& item : ls) {
        int status = item->getStatus2 ();
        if (ScannerResultItem::MisReport == status) {
            misReport << item;
        } else if (ScannerResultItem::Deleted == status) {
            del << item;
        } else {
            continue;
        }
    }
    mChangedItem.clear();
    mLocker.unlock();

    return {del, misReport};// QPair<QStringList, QStringList>;
}

QPair<QPair<QList<QSharedPointer<ScannerResultItem>>, QStringList>, QPair<QList<QSharedPointer<ScannerResultItem>>, QStringList>>
ScannerResultModel::getSaveItemPointAndIds()
{
    mLocker.lock();

    QStringList delIds;
    QList<QSharedPointer<ScannerResultItem>> del;
    QList<QSharedPointer<ScannerResultItem>> misReport;
    QStringList misReportIds;

    auto ls = getChangedItem ();

    for (auto& item : ls) {
        QString fileID = QString("%1").arg(item->getID ());
        int status = item->getStatus2 ();
        if (ScannerResultItem::MisReport == status) {
            misReport << item;
            misReportIds << fileID;
        } else if (ScannerResultItem::Deleted == status) {
            del << item;
            delIds << fileID;
        } else {
            continue;
        }
    }
    mChangedItem.clear();
    mLocker.unlock();

    return {{del, delIds}, {misReport, misReportIds}};// QPair<QStringList, QStringList>;
}





