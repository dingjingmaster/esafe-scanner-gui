#include "scanner-result-item.h"
#include "scanner-result-model.h"

#include <QColor>

ScannerResultModel::ScannerResultModel(QObject* parent)
    : QAbstractTableModel{parent}
{

}

void ScannerResultModel::addItem(ScannerResultItem* item)
{
    if (!item)      return;

    mData.append(item);

    insertRows(mData.count() - 1, 1);
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
        } else if (TaskName == index.column()) {
            return item->getTaskName();
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
        case 1:
            return QString("任务名称");
        case 2:
            return QString("文件名称");
        case 3:
            return QString("处理状态");
        case 4:
            return QString("文件创建时间");
        case 5:
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

    return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
