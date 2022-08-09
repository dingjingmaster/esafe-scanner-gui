#include "scanner-task-model.h"

#include <QSize>
#include <QDebug>
#include <QColor>

#include "../db/db-manager.h"
#include "../utils/scan-task-helper.h"

ScannerTaskModel::ScannerTaskModel(QObject *parent)
    : QAbstractTableModel{parent}, mScanTaskHelper(DBManager::instance ()->getTaskHelper ())
{
    // 数据库与model连接
    connect(mScanTaskHelper, &ScanTaskHelper::addNewTask, this, &ScannerTaskModel::addItem);
    connect(mScanTaskHelper, &ScanTaskHelper::delOldTask, this, &ScannerTaskModel::delItem);
    connect(mScanTaskHelper, &ScanTaskHelper::updateTask, this, &ScannerTaskModel::updateItem);

    mScanTaskHelper->loadAllTask();
}

ScannerTaskModel::~ScannerTaskModel()
{
    mData.clear();
}

QModelIndex ScannerTaskModel::getIndexByItem(const ScannerTaskItem* item, int column)
{
    if (!item) {
        qDebug() << "item is null";
        return QModelIndex();
    }

    int rows = rowCount();
    for (auto i = 0; i < rows; ++i) {
        QModelIndex ii = index(i, column);
        const ScannerTaskItem* it = static_cast <const ScannerTaskItem*> (ii.internalPointer());
        if (it == item) {
            return ii;
        }
    }

    qDebug() << "item not found!";

    return QModelIndex();
}

void ScannerTaskModel::resetModel()
{
    beginResetModel ();

    mData.clear ();

    endResetModel ();

    mScanTaskHelper->resetTask ();
}

void ScannerTaskModel::addItem(ScannerTaskItem* item)
{
    if (!item)      return;

    qDebug() << "add task item: " << item->getName();

    mData.append(item);

    insertRows(mData.count() - 1, 1);
}

void ScannerTaskModel::delItem(ScannerTaskItem *item)
{
    if (!item)      return;

    qInfo() << "delete task: " << item->getName();

    mData.removeOne (item);

    QModelIndex idx = getIndexByItem (item);
    if (idx.isValid ()) {
        removeRow (idx.row ());
    }
}

void ScannerTaskModel::updateItem(ScannerTaskItem *item)
{
    if (!item)      return;

    qInfo() << "update task: " << item->getName();

    QModelIndex idx = getIndexByItem (item);
    if (idx.isValid ()) {
        QModelIndex idx1 = index (idx.row (), (int)(EnumSize) - 1);
        Q_EMIT dataChanged (idx, idx1);
    }
}

int ScannerTaskModel::rowCount(const QModelIndex &parent) const
{
    return mData.count();
}

int ScannerTaskModel::columnCount(const QModelIndex &parent) const
{
    return EnumSize;
}

QVariant ScannerTaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())       return QVariant();

    auto item = static_cast<ScannerTaskItem*>(index.internalPointer());

    if (Qt::DisplayRole == role) {
        if (0 == index.column()) {
            return QString("%1").arg(index.row() + 1);
        } else if (TaskName == index.column()) {
            return item->getName();
        } else if (TaskStatus == index.column()) {
            return item->getStatus();
        } else if (TaskStartTime == index.column()) {
            return item->getStartTime();
        } else if (TaskStopTime == index.column()) {
            return item->getStopTime();
        } else if (TaskScannerProcess == index.column()) {
            return item->getProgress();
        } else if (TaskOperation == index.column()) {
            return item->getOperation();
        }
    } else if (Qt::BackgroundRole == role) {
        if (0 == index.column()) {
            return QColor::fromRgb(mBackgroundR, mBackgroundG, mBackgroundB);
        }
    } else if (Qt::TextAlignmentRole == role) {
        return Qt::AlignCenter;
    } else if (Qt::ForegroundRole == role) {
        switch (index.column()) {
        case 5: {
            return QColor(0, 0, 255);
            break;
        }
        default:
            break;
        }
    }

    return QVariant();
}

QVariant ScannerTaskModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        switch (section) {
        case 0:
            return QString("序号");
        case 1:
            return QString("任务名称");
        case 2:
            return QString("任务状态");
        case 3:
            return QString("开始时间");
        case 4:
            return QString("结束时间");
        case 5:
            return QString("扫描进度");
        case 6:
            return QString("扫描结果");
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

QModelIndex ScannerTaskModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (row < 0 || row > mData.count() - 1) {
            return QModelIndex();
        }

        return createIndex(row, column, mData.at(row));
    }

    return QModelIndex();
}

Qt::ItemFlags ScannerTaskModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ScannerTaskModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();

    return true;
}

bool ScannerTaskModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();

    return true;
}
