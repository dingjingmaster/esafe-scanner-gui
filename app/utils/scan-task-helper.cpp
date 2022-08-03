#include "scan-task-helper.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>

#include "tools.h"

#include "../model/scanner-task-item.h"

class ScanTaskHelperPrivate
{
public:
    explicit ScanTaskHelperPrivate (QString db, ScanTaskHelper* p);
    ~ScanTaskHelperPrivate();

public:
    void onDBChanged ();

    bool selectAllTaskID ();
    ScannerTaskItem* selectTaskByID (QString taskID);

    /**
     * @brief 遍历整个表，把结果用信号的方式发送出去
     */
    bool selectAllTaskIDV2 ();
    ScannerTaskItem* selectTaskByIDV2 (QString taskID);

private:
    static int select_taskid (void* t, int colums, char** val, char** columnName);
    static int select_task_by_id (void* t, int colums, char** val, char** columnName);

public:
    QString                         mDBPath;

    QSet<QString>                   mOldTaskID;
    QSet<QString>                   mNewTaskID;

    QMap<QString, ScannerTaskItem*> mData;                  // <TaskID, ScannerTaskItem*>

    sqlite3*                        mDB;
    QFileSystemWatcher*             mWatcher;


    // const
    ScanTaskHelper*                 q_ptr;
    Q_DECLARE_PUBLIC(ScanTaskHelper);
};

ScanTaskHelperPrivate::ScanTaskHelperPrivate(QString db, ScanTaskHelper *p)
    : q_ptr(p)
{
    Q_Q(ScanTaskHelper);

    mDBPath = db;

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to database: " << mDBPath << " failed!";
        QMessageBox::warning(nullptr, "数据库错误", "连接数据库出错", QMessageBox::Ok);
        qApp->exit(-1);
    }

    qInfo() << "connect to database: " << mDBPath << " successful!";

    // 监控数据库文件
    mWatcher = new QFileSystemWatcher(p);
    mWatcher->addPath(mDBPath);

    q->connect (mWatcher, &QFileSystemWatcher::fileChanged, [&] (QString) {
        qInfo() << "db file changed!";
        onDBChanged();
    });
}

ScanTaskHelperPrivate::~ScanTaskHelperPrivate()
{
    if (mWatcher)       delete mWatcher;
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}
    for (auto m = mData.begin(); m != mData.end(); ++m)    delete m.value();
    mData.clear();
    mOldTaskID.clear();
    mNewTaskID.clear();
}

bool ScanTaskHelperPrivate::selectAllTaskID()
{
    char* errorMsg = nullptr;
    const char* sql = "SELECT task_id, task_start_time, task_stop_time,"
                      " task_file_count, task_scan_file_count,"
                      " task_scan_finished_file_count, task_status FROM scan_task WHERE scan_task_self_check=1";
    while (!sqlite_lock());
    int ret = sqlite3_exec (mDB, sql, select_taskid, this, &errorMsg);
    if (SQLITE_OK != ret) {
        qInfo() << "task data select error: " << errorMsg;
        QMessageBox::warning(nullptr, "数据查询出错", errorMsg, QMessageBox::Ok);
        sqlite3_free(errorMsg);
        goto error;
    }

    while (!sqlite_unlock());

    return true;

error:

    while (!sqlite_unlock());

    return false;
}

ScannerTaskItem* ScanTaskHelperPrivate::selectTaskByID(QString taskID)
{
    char* errorMsg = nullptr;

    ScannerTaskItem* item = new ScannerTaskItem();

    item->setName(taskID);

    QString sql = QString("SELECT `task_status`, `task_start_time`, `task_stop_time`, `task_scan_finished_file_count`,"
                      " `task_scan_file_count`, `task_file_count`, `scan_task_filter_name` "
                      " FROM scan_task "
                      " WHERE task_id='%1'").arg(taskID);

    while (!sqlite_lock());
    int ret = sqlite3_exec(mDB, sql.toUtf8().constData(), select_task_by_id, item, &errorMsg);
    if (SQLITE_OK != ret) {
        QMessageBox::warning(nullptr, "数据查询出错", errorMsg, QMessageBox::Ok);
        sqlite3_free(errorMsg);
        delete item;
        goto error;
    }

    while (!sqlite_unlock());

    return item;

error:

    while (!sqlite_unlock());

    return nullptr;
}

bool ScanTaskHelperPrivate::selectAllTaskIDV2()
{
    Q_Q(ScanTaskHelper);

    QString sql = "SELECT task_id, task_start_time, task_stop_time,"
                      " task_file_count, task_scan_file_count,"
                      " task_scan_finished_file_count, task_status FROM scan_task WHERE scan_task_self_check=1";

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    while (!sqlite_lock());
    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            QString id(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            qint64 startTime = sqlite3_column_int64 (stmt, 1);
            qint64 stopTime = sqlite3_column_int64 (stmt, 2);
            int taskFileCount = sqlite3_column_int (stmt, 3);
            int taskScanFileCount = sqlite3_column_int (stmt, 4);
            int taskScanFinishedFileCount = sqlite3_column_int (stmt, 5);
            int taskStatus = sqlite3_column_int (stmt, 6);

            if (mData.contains(id)) {
                ScannerTaskItem* item = mData[id];
                item->setStopTime(stopTime);
                item->setStatus(taskStatus);
                item->setStartTime(startTime);
                item->setTaskFileCount(taskFileCount);
                item->setScanFileCount(taskScanFileCount);
                item->setScanFinishedFileCount(taskScanFinishedFileCount);
                Q_EMIT q->updateTask (item);
            }

            qInfo() << "task id:" << id;

            mNewTaskID += id;
        }
    }
    if (stmt)       sqlite3_finalize(stmt);
    while (!sqlite_unlock());

    qInfo() << "old TaskID: " << mOldTaskID;
    qInfo() << "new TaskID: " << mNewTaskID;

    if (mNewTaskID.count() == mOldTaskID.count()) {
        goto noChanged;
    }

    return true;

noChanged:

    return false;
}

ScannerTaskItem *ScanTaskHelperPrivate::selectTaskByIDV2(QString taskID)
{
    ScannerTaskItem* item = new ScannerTaskItem();

    item->setName(taskID);

    QString sql = QString("SELECT `task_status`, `task_start_time`, `task_stop_time`, `task_scan_finished_file_count`,"
                      " `task_scan_file_count`, `task_file_count`, `scan_task_filter_name` "
                      " FROM scan_task "
                      " WHERE task_id='%1'").arg(taskID);

    sqlite3_stmt* stmt = NULL;
    while (!sqlite_lock());
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            item->setStatus(sqlite3_column_int(stmt, 0));
            item->setStartTime(sqlite3_column_int64(stmt, 1));
            item->setStopTime(sqlite3_column_int64(stmt, 2));
            item->setScanFinishedFileCount(sqlite3_column_int64(stmt, 3));
            item->setScanFileCount(sqlite3_column_int64(stmt, 4));
            item->setTaskFileCount(sqlite3_column_int64(stmt, 5));
            item->setFilterName(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6))));

            qInfo () << "status: " << item->getStatus() << "\n"
                     << "startTime: " << item->getStartTime() << "\n"
                     << "stopTime: " << item->getStopTime() << "\n"
                     << "scan finished file: " << item->getScanFinishedFileCount() << "\n"
                     << "scan file count: " << item->getScanFileCount() << "\n"
                     << "task file count: " << item->getTaskFileCount() << "\n"
                     << "filter name: " << item->getFilterName() << "\n\n\n";
        }
    } else {
        goto noChanged;
    }

    if (stmt)       sqlite3_finalize(stmt);
    while (!sqlite_unlock());

    return item;

noChanged:

    while (!sqlite_unlock());
    if (stmt)       sqlite3_finalize(stmt);

    return nullptr;
}

int ScanTaskHelperPrivate::select_taskid(void *t, int colums, char **val, char **columnName)
{
    ScanTaskHelperPrivate* h = static_cast<ScanTaskHelperPrivate*>(t);
    if (!h) {
        qInfo() << "it's not impossbile!";
        return -1;
    }

    h->mOldTaskID = h->mNewTaskID;
    h->mNewTaskID.clear();
    int len = strlen("task_id");
    for (int i = 0; i < colums; ++i) {
        if ((strlen(columnName[i]) == len) && (0 == strncpy(columnName[i], "task_id", len))) {
            h->mNewTaskID += QString(val[i]);
            qInfo() << "task id:" << QString(val[i]);
        }
    }

    return 0;
}

int ScanTaskHelperPrivate::select_task_by_id(void *t, int colums, char **val, char **columnName)
{
    ScannerTaskItem* h = static_cast<ScannerTaskItem*>(t);
    if (!h) {
        qInfo() << "it's not impossbile!";
        return -1;
    }

    for (int i = 0; i < colums; ++i) {
        if (QString(columnName[i]) == "task_status") {
            switch (QString(columnName[i]).toInt()) {
            case ScannerTaskItem::Scanning:
                h->setStatus(ScannerTaskItem::Scanning);
            case ScannerTaskItem::Finish:
                h->setStatus(ScannerTaskItem::Finish);
            case ScannerTaskItem::Suspended:
                h->setStatus(ScannerTaskItem::Suspended);
            case ScannerTaskItem::Unknow:
            default:
                h->setStatus(ScannerTaskItem::Unknow);
            }
        } else if (QString(columnName[i]) == "task_start_time") {
            h->setStartTime(QString(columnName[i]).toInt());
        } else if (QString(columnName[i]) == "task_stop_time") {
            h->setStopTime(QString(columnName[i]).toInt());
        } else if (QString(columnName[i]) == "task_scan_finished_file_count") {
            h->setScanFinishedFileCount(QString(columnName[i]).toInt());
        } else if (QString(columnName[i]) == "task_scan_file_count") {
            h->setScanFileCount(QString(columnName[i]).toInt());
        } else if (QString(columnName[i]) == "task_file_count") {
            h->setTaskFileCount(QString(columnName[i]).toInt());
        }
    }

    return 0;
}

void ScanTaskHelperPrivate::onDBChanged()
{
    Q_Q(ScanTaskHelper);

    if (!selectAllTaskIDV2()) {
        qInfo () << "db not change";
        return;
    }

    QSet<QString> delT = mOldTaskID - mNewTaskID;
    QSet<QString> newT = mNewTaskID - mOldTaskID;
    QStringList deltSorted = delT.toList();
    deltSorted.sort();

    QStringList newtSorted = newT.toList();
    newtSorted.sort();

    for (auto id : newtSorted) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (auto t = selectTaskByIDV2(id)) {
            qInfo () << "add task '" << id << "'";
            Q_EMIT q->addNewTask(t);
            mData[id] = t;
        }
    }

    for (auto id : deltSorted) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (mData.contains(id)) {
            auto it = mData[id];
            mData.remove(id);
            qInfo () << "delete task '" << id << "'";
            Q_EMIT q->delOldTask(it);
            delete it;
        }
    }

    mOldTaskID = mNewTaskID;
}


ScanTaskHelper::ScanTaskHelper(QString dbPath, QObject* parent)
    : QObject(parent), d_ptr(new ScanTaskHelperPrivate(dbPath, this))
{

}

ScanTaskHelper::~ScanTaskHelper()
{
    if (d_ptr)          delete d_ptr;
}

void ScanTaskHelper::resetTask()
{
    Q_D(ScanTaskHelper);

    auto val = d->mData.values ();

    for (auto ik : val) {
        Q_EMIT addNewTask (ik);
    }
}

void ScanTaskHelper::loadAllTask()
{
    Q_D(ScanTaskHelper);

    d->onDBChanged();
}



