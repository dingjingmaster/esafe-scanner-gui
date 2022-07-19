#include "scanner-task-model.h"

ScannerTaskModel::ScannerTaskModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int ScannerTaskModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

int ScannerTaskModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant ScannerTaskModel::data(const QModelIndex &index, int role) const
{
    if (Qt::DisplayRole == role) {
        if (0 == index.column()) {
            return QString("%1").arg(index.row() + 1);
        }
        return QString ("R%1, C%2").arg(index.row() + 1).arg(index.column() + 1);
    }

    return QVariant();
}

QVariant ScannerTaskModel::headerData(int section, Qt::Orientation orentation, int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orentation) {
        if (Qt::Horizontal == orentation) {
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
                return QString("操作");
            default:
                break;
            }
        } else {
            return QString("%1").arg(section + 1);
        }

    }

    return QVariant();
}
