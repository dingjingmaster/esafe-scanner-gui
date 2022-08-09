#include "db-manager.h"
#include "../utils/scan-task-helper.h"
#include "../utils/scan-result-helper.h"

#include <QDebug>
#include <QMutex>
#include <QThread>

static QMutex locker;

DBManager* DBManager::gInstance = nullptr;

DBManager* DBManager::instance()
{
    if (!gInstance) {
        locker.lock ();
        if (!gInstance) {
            gInstance = new DBManager;
        }
        locker.unlock ();
    }

    return gInstance;
}

ScanTaskHelper *DBManager::getTaskHelper()
{
    return mScanTask;
}

ScanResultHelper *DBManager::getResultHelper()
{
    return mScanResult;
}

DBManager::DBManager(QObject *parent)
    : QObject{parent}
{
    mScanResult = new ScanResultHelper(DB_PATH);
    mScanResultThread = new QThread;
    mScanResult->moveToThread (mScanResultThread);

    mScanTask = new ScanTaskHelper(DB_PATH);
    mScanTaskThread = new QThread;
    mScanTask->moveToThread (mScanTaskThread);

    mWatcher = new QFileSystemWatcher(this);
    mWatcher->addPath(DB_PATH);

    connect (mWatcher, &QFileSystemWatcher::fileChanged, [&] (QString) {
        // FIXME:// 定时器 1s 更新一次
        qInfo() << "db file changed!";
        if (CUR_RESULT == mPage) {
            if (!mScanResultThread->isRunning ()) {
                mScanResultThread->start();
                mScanResult->loadTaskResult (mTaskName, mTaskFilter);
            }
        } else {
            if (!mScanTaskThread->isRunning ()) {
                mScanTaskThread->start ();
                mScanTask->loadAllTask ();
            }
        }
    });

    // 扫描任务
    connect (mScanTaskThread, &QThread::finished, this, [=] () {
        // 完成
    });
    connect (this, &DBManager::refreshScanTask, this, [=] () {
        mPage = CUR_TASK;
        mScanResultThread->exit (1);
        mScanTaskThread->start ();
        mScanTask->loadAllTask ();
    });


    // 扫描结果
    connect (mScanResultThread, &QThread::finished, this, [=] () {
        // 完成
    });
    connect (this, &DBManager::refreshScanResult, this, [=] (QString taskName, QString filterName) {
        mPage = CUR_RESULT;
        mTaskName = taskName;
        mTaskFilter = filterName;
        mScanTaskThread->exit (1);
        mScanResultThread->start();
        mScanResult->loadTaskResult (taskName, filterName);
    });
}
