#include "scan-task-helper.h"

#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QMap>

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

    qDebug() << "connect to database: " << mDBPath << " successful!";

    // 监控数据库文件
    mWatcher = new QFileSystemWatcher(p);
    mWatcher->addPath(mDBPath);

    q->connect (mWatcher, &QFileSystemWatcher::fileChanged, [&] (QString) {
        qDebug() << "db file changed!";
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
    const char* sql = "SELECT task_id FROM scan_task WHERE scan_task_self_check=0";

    int ret = sqlite3_exec (mDB, sql, select_taskid, this, &errorMsg);
    if (SQLITE_OK != ret) {
        qDebug() << "task data select error: " << errorMsg;
        QMessageBox::warning(nullptr, "数据查询出错", errorMsg, QMessageBox::Ok);
        sqlite3_free(errorMsg);

        return false;
    }

    return true;
}

ScannerTaskItem* ScanTaskHelperPrivate::selectTaskByID(QString taskID)
{
    char* errorMsg = nullptr;

    ScannerTaskItem* item = new ScannerTaskItem();

    item->setName(taskID);

    QString sql = QString("SELECT `task_status`, `task_start_time`, `task_stop_time`, `task_scan_finished_file_count`,"
                      " `task_scan_file_count`, `task_file_count` "
                      " FROM scan_task "
                      " WHERE task_id='%1'").arg(taskID);

    int ret = sqlite3_exec(mDB, sql.toUtf8().constData(), select_task_by_id, item, &errorMsg);
    if (SQLITE_OK != ret) {
        QMessageBox::warning(nullptr, "数据查询出错", errorMsg, QMessageBox::Ok);
        sqlite3_free(errorMsg);
        delete item;
        return nullptr;
    }

    return item;
}

bool ScanTaskHelperPrivate::selectAllTaskIDV2()
{
    int row = 0;
    QString sql = QString("SELECT task_id FROM scan_task WHERE scan_task_self_check=0");

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            ++row;
            const unsigned char* id = sqlite3_column_text(stmt, 0);
            qDebug() << "task id:" << id;
            mNewTaskID += QString(reinterpret_cast<const char*>(id));
        }
    }

    qDebug() << "old TaskID: " << mOldTaskID;
    qDebug() << "new TaskID: " << mNewTaskID;

    if (mNewTaskID.count() == mOldTaskID.count()) {
        goto noChanged;
    }

    if (stmt)       sqlite3_finalize(stmt);

    return true;

noChanged:

    if (stmt)       sqlite3_finalize(stmt);

    return false;
}

ScannerTaskItem *ScanTaskHelperPrivate::selectTaskByIDV2(QString taskID)
{
    ScannerTaskItem* item = new ScannerTaskItem();

    item->setName(taskID);

    QString sql = QString("SELECT `task_status`, `task_start_time`, `task_stop_time`, `task_scan_finished_file_count`,"
                      " `task_scan_file_count`, `task_file_count` "
                      " FROM scan_task "
                      " WHERE task_id='%1'").arg(taskID);

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            item->setStatus(sqlite3_column_int(stmt, 0));
            item->setStartTime(sqlite3_column_int64(stmt, 1));
            item->setStopTime(sqlite3_column_int64(stmt, 2));
            item->setScanFinishedFileCount(sqlite3_column_int64(stmt, 3));
            item->setScanFileCount(sqlite3_column_int64(stmt, 4));
            item->setTaskFileCount(sqlite3_column_int64(stmt, 5));

            qDebug() << "status: " << item->getStatus() << "\n"
                     << "startTime: " << item->getStartTime() << "\n"
                     << "stopTime: " << item->getStopTime() << "\n"
                     << "scan finished file: " << item->getScanFinishedFileCount() << "\n"
                     << "scan file count: " << item->getScanFileCount() << "\n"
                     << "task file count: " << item->getTaskFileCount();
        }
    }

    if (stmt)       sqlite3_finalize(stmt);

    return item;

noChanged:

    if (stmt)       sqlite3_finalize(stmt);

    return nullptr;
}

int ScanTaskHelperPrivate::select_taskid(void *t, int colums, char **val, char **columnName)
{
    ScanTaskHelperPrivate* h = static_cast<ScanTaskHelperPrivate*>(t);
    if (!h) {
        qDebug() << "it's not impossbile!";
        return -1;
    }

    h->mOldTaskID = h->mNewTaskID;
    h->mNewTaskID.clear();
    int len = strlen("task_id");
    for (int i = 0; i < colums; ++i) {
        if ((strlen(columnName[i]) == len) && (0 == strncpy(columnName[i], "task_id", len))) {
            h->mNewTaskID += QString(val[i]);
            qDebug() << "task id:" << QString(val[i]);
        }
    }

    return 0;
}

int ScanTaskHelperPrivate::select_task_by_id(void *t, int colums, char **val, char **columnName)
{
    ScannerTaskItem* h = static_cast<ScannerTaskItem*>(t);
    if (!h) {
        qDebug() << "it's not impossbile!";
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
        qDebug() << "db not change";
        return;
    }

    QSet<QString> delT = mOldTaskID - mNewTaskID;
    QSet<QString> newT = mNewTaskID - mOldTaskID;
    for (auto id : newT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (auto t = selectTaskByIDV2(id)) {
            qDebug() << "add task '" << id << "'";
            Q_EMIT q->addNewTask(t);
            mData[id] = t;
        }
    }

    for (auto id : delT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (mData.contains(id)) {
            auto it = mData[id];
            mData.remove(id);
            qDebug() << "delete task '" << id << "'";
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

void ScanTaskHelper::loadAllTask()
{
    Q_D(ScanTaskHelper);

    d->onDBChanged();
}




