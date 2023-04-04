#include "db-manager.h"
#include "../utils/scan-task-helper.h"
#include "../utils/scan-result-helper.h"

#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QThread>
#include <QDateTime>

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
//    mTimer->setSingleShot (true);

    // 此处仅仅用于更新
    connect(mTimer, &QTimer::timeout, this, &DBManager::updateModel);

//    connect (mWatcher, &QFileSystemWatcher::fileChanged, this, [&] (const QString&) {
//        qDebug() << "file changed!";
////        static unsigned int lastTime = 0;
////        unsigned int curTim = QDateTime::currentDateTime().toSecsSinceEpoch();
//        // FIXME:// 定时器 5s 更新一次
//        if (mTimer->isActive ()) {
//            return;
//        }
//        mTimer->start(5 * 1000);
//    });
    restartTimer (1);

    // 扫描任务
    connect (this, &DBManager::refreshScanTask, mScanTask, &ScanTaskHelper::loadAllTask);

    // 扫描结果
    connect (this, &DBManager::refreshScanResult2, mScanResult, &ScanResultHelper::refreshResult);
    connect (this, &DBManager::refreshScanResult, mScanResult, &ScanResultHelper::loadTaskResult);

#if 0
    // db monitor
    connect (this, &DBManager::stopDBMonitor, this, [=] () {mWatcher->removePath(DB_PATH);});
    connect (this, &DBManager::startDBMonitor, this, [=] () {
        mWatcher->addPath(DB_PATH);
        updateModel();
    });
#endif

    connect (mScanTask, &ScanTaskHelper::loadFinished, this, &DBManager::loadTaskStop);
    connect (mScanResult, &ScanResultHelper::loadFinished, this, &DBManager::loadTaskResultStop);

    mScanTaskThread->start ();
    mScanResultThread->start ();
}

void DBManager::updateModel()
{
    if (CUR_RESULT == getCurPage()) {
        qDebug() << "scan result db changed";
        if (!mScanResult->isRunning()) {
            qDebug() << "result start ...";
            Q_EMIT refreshScanResult2 ();
        }
    } else if (CUR_TASK == getCurPage()){
        qDebug() << "scan task db changed";
        if (!mScanTask->isRunning()) {
            qDebug() << "task start ...";
            Q_EMIT refreshScanTask ();
        }
    }
    else if (CUR_STOP == getCurPage()) {
        mPageChangeExec.quit();
    }
}

void DBManager::setCurPage(CurPage p)
{
    mPageLock.lock();
    mPage = p;
    mPageLock.unlock();

    if (CUR_STOP == mPage) {
        mScanTask->cancel();
        mScanResult->cancel();
        mPageChangeExec.exec ();
    }
}

DBManager::CurPage DBManager::getCurPage()
{
    mPageLock.lock();

    auto page = mPage;

    mPageLock.unlock();

    return page;
}

void DBManager::restartTimer(int32_t sec)
{
    mTimer->stop();
    mTimer->setInterval(sec * 1000);
    mTimer->start();
}
