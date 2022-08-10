#include "scanner-task-model2.h"
#include "../utils/scan-task-helper.h"

#include <QDebug>
#include <QTimer>
#include <QThread>


ScannerTaskModel2::ScannerTaskModel2(QObject *parent)
    : QStandardItemModel {parent}, mCurrentRow(0)
{
    mScanTask = new ScanTaskHelper(DB_PATH);
    mScanTaskThread = new QThread(this);
    mScanTask->moveToThread (mScanTaskThread);

    connect(mScanTask, &ScanTaskHelper::addNewTask, this, [=] (ScannerTaskItem* it) {
        if (!it)        return;

        mData.append (it);

        updateViewData();
    });

    connect(mScanTask, &ScanTaskHelper::delOldTask, this, [=] (ScannerTaskItem* it) {
        if (!it)        return;

        mData.removeOne (it);

        updateViewData();

        #if 0
        QModelIndex idx = getIndexByItem (item);
        if (idx.isValid ()) {
            removeRow (idx.row ());
        }
        #endif
    });
    connect(mScanTask, &ScanTaskHelper::updateTask, this, [=] (ScannerTaskItem* it) {
        if (!it)        return;

        qInfo() << "update task: " << it->getName();

        updateViewData();

        #if 0
        QModelIndex idx = getIndexByItem (it);
        if (idx.isValid ()) {
            QModelIndex idx1 = index (idx.row (), (int)(EnumSize) - 1);
            Q_EMIT dataChanged (idx, idx1);
        }
        #endif
    });

    mTimer = new QTimer;

    //connect(mTimer, &QTimer::timeout, this, [=] () {
    //    if (!mScanTaskThread->isRunning ()) {
    //        qInfo() << "scan task";
    //        Q_EMIT refreshScanTask ();
    //    }
    //});

    //connect (mWatcher, &QFileSystemWatcher::fileChanged, this, [&] (QString) {
    //    // FIXME:// 定时器 1s 更新一次
    //    if (mTimer->isActive ()) {
    //        return;
    //    }
    //    mTimer->start (3 * 1000);
    //});

    // 扫描任务
    connect (mScanTaskThread, &QThread::finished, this, [=] () {
        // 完成
    });

    connect (this, &ScannerTaskModel2::refreshScanTask, mScanTask, &ScanTaskHelper::loadAllTask);

    mScanTaskThread->start ();
}

QModelIndex ScannerTaskModel2::getIndexByItem(const ScannerTaskItem* item, int column)
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

QVariant ScannerTaskModel2::headerData(int section, Qt::Orientation orentation, int role) const
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

void ScannerTaskModel2::updateViewData()
{
    removeRows (0, rowCount ());

    for (int i = mCurrentRow, j = 0; i < mCurrentRow + mTotalRows; ++i) {
        if (i >= mData.size ())     break;

        setItem(j, 0, new QStandardItem(getItemInfoByRowAndColumn (j, 0).toString ()));
        setItem(j, 1, new QStandardItem(getItemInfoByRowAndColumn (j, TaskName).toString ()));
        setItem(j, 2, new QStandardItem(getItemInfoByRowAndColumn (j, TaskStatus).toString ()));
        setItem(j, 3, new QStandardItem(getItemInfoByRowAndColumn (j, TaskStartTime).toString ()));
        setItem(j, 4, new QStandardItem(getItemInfoByRowAndColumn (j, TaskStopTime).toString ()));
        setItem(j, 5, new QStandardItem(getItemInfoByRowAndColumn (j, TaskScannerProcess).toString ()));
        setItem(j, 6, new QStandardItem(getItemInfoByRowAndColumn (j, TaskOperation).toString ()));
        ++j;
    }
}

void ScannerTaskModel2::receiveChanged(int row)
{
    mCurrentRow += row;

    if (mCurrentRow < 0)                mCurrentRow = 0;
    if (mCurrentRow > mData.size ())    mCurrentRow = mData.size ();

    qInfo () << "pos :" << mCurrentRow;

    //if (mCurrentRow < 0 || mCurrentRow > mData.size () - 50) {
    //    mCurrentRow -= row;
    //    return;
    //}

    updateViewData ();
}

void ScannerTaskModel2::onVerScrollbar(double value)
{
    mCurrentRow = value * mData.size();

    if (mCurrentRow >= mData.size ()) {
        return;
    }

    updateViewData();
}

QVariant ScannerTaskModel2::getItemInfoByRowAndColumn(int row, int col)
{
    if (row < 0 || row >= mData.size ()) {
        QVariant();
    }

    qDebug() << "data size: " << mData.size ();

    ScannerTaskItem* it = mData.at (row);

    if (nullptr == it) return QVariant();

    if (0 == col) {
        return QString("%1").arg(row + 1);
    } else if (TaskName == col) {
        return it->getName();
    } else if (TaskStatus == col) {
        return it->getStatus();
    } else if (TaskStartTime == col) {
        return it->getStartTime();
    } else if (TaskStopTime == col) {
        return it->getStopTime();
    } else if (TaskScannerProcess == col) {
        return it->getProgress();
    } else if (TaskOperation == col) {
        return it->getOperation();
    }

    return QVariant();
}
