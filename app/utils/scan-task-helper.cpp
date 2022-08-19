#include "scan-task-helper.h"

#include <QMap>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QMutex>

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

    ScannerTaskItem* selectTaskByIDV2 (QString taskID);

private:
    static int select_taskid (void* t, int colums, char** val, char** columnName);
    static int select_task_by_id (void* t, int colums, char** val, char** columnName);

public:
    QString                         mDBPath;

    QMap<QString, ScannerTaskItem*> mData;                  // <TaskID, ScannerTaskItem*>

    sqlite3*                        mDB;

    QMutex                          mLocker;

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
}

ScanTaskHelperPrivate::~ScanTaskHelperPrivate()
{
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}
    for (auto m = mData.begin(); m != mData.end(); ++m)    delete m.value();
    mData.clear();
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

ScannerTaskItem *ScanTaskHelperPrivate::selectTaskByIDV2(QString taskID)
{
    ScannerTaskItem* item = new ScannerTaskItem();

    item->setID(taskID);

    QString sql = QString("SELECT `task_status`, `task_start_time`, `task_stop_time`, `task_scan_finished_file_count`,"
                      " `task_scan_file_count`, `task_file_count`, `scan_task_filter_name`, `task_name` "
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
            item->setName(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7))));

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

    int len = strlen("task_id");
    for (int i = 0; i < colums; ++i) {
        if ((strlen(columnName[i]) == len) && (0 == strncpy(columnName[i], "task_id", len))) {
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

    QSet<QString> allT;

    QString sql = QString("SELECT `task_id`, `task_status`, `task_start_time`, `task_stop_time`,"
                          " `task_scan_finished_file_count`, `task_scan_file_count`, `task_file_count`,"
                          " `scan_task_filter_name`, `task_name`, `scan_task_dir`, `scan_task_self_check` "
                          " FROM scan_task;");

    qDebug() << "scan_task sql: '" << sql << "'";

    while (!sqlite_lock());
    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            QString id(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            int taskStatus = sqlite3_column_int(stmt, 1);
            qint64 startTime = sqlite3_column_int64 (stmt, 2);
            qint64 stopTime = sqlite3_column_int64 (stmt, 3);
            int taskScanFinishedFileCount = sqlite3_column_int (stmt, 4);
            int taskScanFileCount = sqlite3_column_int (stmt, 5);
            int taskFileCount = sqlite3_column_int (stmt, 6);
            QString taskFilterName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
            QString taskName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
            QString scanDir(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));
            int scanSelfCheck = sqlite3_column_int (stmt, 10);

            if (nullptr == scanDir || scanDir.isNull () || scanDir.isEmpty () || "" == scanDir) {
                scanDir = "/";
            }

            if (id.isNull () || id.isEmpty () || "" == id)  continue;

            allT.insert (id);

            qInfo() << id;

            if (mData.contains(id)) {
                ScannerTaskItem* item = mData[id];

                if (taskStatus != item->getStatus2 ()
                        || startTime != item->getStartTime2 ()
                        || stopTime != item->getStopTime2 ()
                        || taskScanFinishedFileCount != item->getScanFinishedFileCount ()
                        || taskScanFileCount != item->getScanFileCount ()
                        || taskFileCount != item->getTaskFileCount ()
                        || taskFilterName != item->getFilterName ()
                        || taskName != item->getName ()
                        || scanDir != item->getScanDir2 ()
                        || scanSelfCheck != item->getSelfCheck()) {
                    item->setScanDir (scanDir);
                    item->setStopTime(stopTime);
                    item->setStatus(taskStatus);
                    item->setStartTime(startTime);
                    item->setIsSelfCheck(scanSelfCheck);
                    item->setTaskFileCount(taskFileCount);
                    item->setScanFileCount(taskScanFileCount);
                    item->setScanFinishedFileCount(taskScanFinishedFileCount);
                    qInfo() << "update task id:" << id;
                    Q_EMIT q->updateTask (item);
                }
            } else {
                auto item = new ScannerTaskItem;
                item->setID (id);

                item->setName(taskName);
                item->setScanDir(scanDir);
                item->setStatus(taskStatus);
                item->setStopTime(stopTime);
                item->setStartTime(startTime);
                item->setIsSelfCheck(scanSelfCheck);
                item->setFilterName(taskFilterName);
                item->setTaskFileCount(taskFileCount);
                item->setScanFileCount(taskScanFileCount);
                item->setScanFinishedFileCount(taskScanFinishedFileCount);

                mData[id] = item;

                qInfo() << "new task id:" << id;
                Q_EMIT q->addNewTask (item);
            }
        }
    } else {
        qWarning() << "select: '" << sql << "' error";
    }

    if (stmt)       sqlite3_finalize(stmt);
    while (!sqlite_unlock());

    QSet<QString> delT = mData.keys().toSet () - allT;

    for (auto id : delT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (mData.contains(id)) {
            auto it = mData[id];
            mData.remove(id);
            qInfo () << "delete task '" << id << "'";
            Q_EMIT q->delOldTask(it);
            it->deleteLater();
        }
    }
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

void ScanTaskHelper::testInsertItem()
{
    Q_D(ScanTaskHelper);

    char* errorMsg = nullptr;

    for (int i = 0; i < 1000000; ++i) {
        QString sql = QString("INSERT INTO scan_task (task_id, task_name, scan_interval, scan_task_filter_name,"
                              " scan_task_dir, scan_task_dir_filterout, scan_task_dir_filterout_fileext, "
                              " task_start_time, task_stop_time, task_file_count, task_scan_file_count, task_scan_finished_file_count,"
                              " task_status, scan_result_reuse, scan_task_file_monitor, scan_task_self_check)"
                              " VALUES ('TASK 0011%1', 'name111%1', 004, 'A|B|C|D', '/', '.local', '', 1658558157,"
                              " 1658558157, 1000, 600, 100, 0, 0, 0, 1);").arg (i);

        while (!sqlite_lock());
        int ret = sqlite3_exec(d->mDB, sql.toUtf8().constData(), NULL, NULL, &errorMsg);
        if (SQLITE_OK != ret) {
            qDebug() << "error: " << errorMsg;
            sqlite3_free(errorMsg);
        }

        while (!sqlite_unlock());
    }
}



