#include "model-item-sort.h"
#include "scanner-task-model.h"
#include "scanner-result-model.h"

#include <QDebug>

ModelItemSort::ModelItemSort(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool ModelItemSort::lessThan(const QModelIndex &sLeft, const QModelIndex &sRight) const
{
    auto lItem = sLeft.internalPointer();
    auto rItem = sRight.internalPointer();
    if (nullptr == lItem)       return true;

    QString className = static_cast<QObject*>(lItem)->metaObject()->className();

    if ("ScannerTaskItem" == className) {
        return static_cast<ScannerTaskItem*>(lItem)->getName() < static_cast<ScannerTaskItem*>(rItem)->getName();
    } else if ("ScannerResultItem" == className) {

    }


    return true;
}
