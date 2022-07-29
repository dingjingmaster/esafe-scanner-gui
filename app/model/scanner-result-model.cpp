#include "scanner-result-item.h"
#include "scanner-result-model.h"

#include <QList>
#include <QDebug>
#include <QColor>

#include "../utils/scan-result-helper.h"

ScannerResultModel::ScannerResultModel(QObject* parent)
    : QAbstractTableModel{parent}, mScanResultHelper(new ScanResultHelper(QString(DB_PATH), this))
{
    // 数据库与model连接
    connect(mScanResultHelper, &ScanResultHelper::addNewFile, this, &ScannerResultModel::addItem);
    connect(mScanResultHelper, &ScanResultHelper::delOldFile, this, &ScannerResultModel::delItem);

    // 清空数据 showData (QString TaskName, QString filterName);
    connect(this, &ScannerResultModel::clearData, mScanResultHelper, &ScanResultHelper::clearData);
    connect(this, &ScannerResultModel::showData, mScanResultHelper, &ScanResultHelper::loadTaskResult);

    connect(this, &ScannerResultModel::clearData, this, [=] () { beginResetModel(); mData.clear(); endResetModel(); mScanResultHelper->clearData(); });
    
    
    void fixChanged (int);
    void delChanged (int);
    void misReportChanged (int);

    mScanResultHelper->loadTaskResult();
}

bool ScannerResultModel::hasChecked()
{
    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (i.i->t()->getChecked()) {
            return true;
        }
    }
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

    mData.append(item);
    insertRows(mData.count() - 1, 1);
    
    switch (item->getStatus2 ()) {
    case ScannerResultItem::MisReport:
        ++mMisReport;
        break;
    case ScannerResultItem::Deleted:
        ++mDelete;
        break;
    default:
        ++mNoFix;
        break;
    }
    
    QModelIndex idx = getIndexByItem(item);
    
    Q_EMIT dataChanged (idx, idx);
}

void ScannerResultModel::delItem(ScannerResultItem *item)
{
    if (!item)      return;
    
    QModelIndex idx = getIndexByItem(item);
    
    if (!idx.isValid())     return;
    
    if (mData.contains (item))  mData.removeOne (item);
    
    removeRow (idx.row());
    
    switch (item->getStatus2 ()) {
    case ScannerResultItem::MisReport:
        --mMisReport;
        break;
    case ScannerResultItem::Deleted:
        --mDelete;
        break;
    default:
        --mNoFix;
        break;
    }
    
    Q_EMIT dataChanged (idx, idx);
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

QList<const ScannerResultItem *> ScannerResultModel::getSelectedItem()
{
    QList<const ScannerResultItem *> ls;

    for (auto i = mData.constBegin(); i != mData.constEnd(); ++i) {
        if (i.i->t()->getChecked()) {
            ls.append(i.i->t());
        }
    }

    return ls;
}

void ScannerResultModel::selectAll(bool s)
{
    int all = rowCount();
    for (auto i = 0; i < all; ++i) {
        QModelIndex idx = index(i, 0);
        if (!idx.isValid()) continue;
        static_cast<ScannerResultItem*>(idx.internalPointer())->setChecked(s);
        Q_EMIT dataChanged(idx, idx);

    }
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

    return QVariant();
}

bool ScannerResultModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << index;
    
    if (!index.isValid ())          return false;
    
    ScannerResultItem* item = static_cast<ScannerResultItem*> (index.internalPointer ());
    switch (index.column ()) {
    case 2:
        item->setStatus (value.toString ());
        Q_EMIT dataChanged (index, index);
        return true;
    default:
        break;
    }
    
    return false;
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
