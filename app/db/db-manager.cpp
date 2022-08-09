#include "db-manager.h"
#include "../utils/scan-task-helper.h"
#include "../utils/scan-result-helper.h"

#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QTimer>

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
    mScanResultThread = new QThread(this);
    mScanResult->moveToThread (mScanResultThread);

    mScanTask = new ScanTaskHelper(DB_PATH);
    mScanTaskThread = new QThread(this);
    mScanTask->moveToThread (mScanTaskThread);

    mWatcher = new QFileSystemWatcher(this);
    mWatcher->addPath(DB_PATH);

    mTimer = new QTimer;
    mTimer->setSingleShot(true);

    connect(mTimer, &QTimer::timeout, this, [=] () {
        qInfo() << "db file changed!";
        if (CUR_RESULT == mPage) {
            if (!mScanResultThread->isRunning ()) {
                mScanResultThread->start();
                //Q_EMIT refresh();
            }
        } else {
            if (!mScanTaskThread->isRunning ()) {
                qInfo() << "scan task";
                Q_EMIT refreshScanTask ();
            }
        }
    });

    connect (mWatcher, &QFileSystemWatcher::fileChanged, this, [&] (QString) {
        // FIXME:// 定时器 1s 更新一次
        if (mTimer->isActive ()) {
            return;
        }
        mTimer->start (3 * 1000);
    });

    // 扫描任务
    connect (mScanTaskThread, &QThread::finished, this, [=] () {
        // 完成
    });

    connect (this, &DBManager::refreshScanTask, mScanTask, &ScanTaskHelper::loadAllTask);

    // 扫描结果
    connect (mScanResultThread, &QThread::finished, this, [=] () {
        // 完成
    });
    connect (this, &DBManager::refreshScanResult, mScanResult, &ScanResultHelper::loadTaskResult);

    mScanTaskThread->start ();
    mScanResultThread->start ();
}
