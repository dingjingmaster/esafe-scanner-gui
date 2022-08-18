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

    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);

    connect(mTimer, &QTimer::timeout, this, &DBManager::updateModel);

    connect (mWatcher, &QFileSystemWatcher::fileChanged, this, [&] (const QString&) {
        qDebug() << "file changed!";
        // FIXME:// 定时器 5s 更新一次
        if (mTimer->isActive ()) {
            return;
        }
        mTimer->start (5 * 1000);
    });

    // 扫描任务
    connect (this, &DBManager::refreshScanTask, mScanTask, &ScanTaskHelper::loadAllTask);

    // 扫描结果
    connect (this, &DBManager::refreshScanResult2, mScanResult, &ScanResultHelper::refresResult);
    connect (this, &DBManager::refreshScanResult, mScanResult, &ScanResultHelper::loadTaskResult);

#if 0
    // db monitor
    connect (this, &DBManager::stopDBMonitor, this, [=] () {mWatcher->removePath(DB_PATH);});
    connect (this, &DBManager::startDBMonitor, this, [=] () {
        mWatcher->addPath(DB_PATH);
        updateModel();
    });
#endif

    mScanTaskThread->start ();
    mScanResultThread->start ();
}

void DBManager::updateModel()
{
    qInfo() << "db file changed!";
    if (CUR_RESULT == mPage) {
        qDebug() << "scan result db changed";
        Q_EMIT refreshScanResult2 ();
    } else {
        qDebug() << "scan task db changed";
        Q_EMIT refreshScanTask ();
    }
}

void DBManager::setCurPage(CurPage p)
{
    mPage = p;
}
