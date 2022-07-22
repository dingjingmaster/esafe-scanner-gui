#include "scan-task-helper.h"

#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QMap>

#include <model/scanner-task-item.h>

class ScanTaskHelperPrivate
{
public:
    explicit ScanTaskHelperPrivate (QString db, ScanTaskHelper* p);
    ~ScanTaskHelperPrivate();

public:
    bool selectAllTaskID ();

    ScannerTaskItem* selectTaskByID (QString taskID);

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
};

ScanTaskHelperPrivate::ScanTaskHelperPrivate(QString db, ScanTaskHelper *p)
{
    mDBPath = db;

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to database: " << mDBPath << " failed!";
        QMessageBox::warning(nullptr, "数据库错误", "连接数据库出错", QMessageBox::Ok);
        qApp->exit(-1);
    }

    qDebug() << "connect to database: " << mDBPath << " successful!";
}

ScanTaskHelperPrivate::~ScanTaskHelperPrivate()
{
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}
}

bool ScanTaskHelperPrivate::selectAllTaskID()
{
    char* errorMsg = nullptr;
    const char* sql = "SELECT task_id FROM scan_task WHERE scan_task_self_check='0'";

    int ret = sqlite3_exec(mDB, sql, select_taskid, this, &errorMsg);
    if (SQLITE_OK != ret) {
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

int ScanTaskHelperPrivate::select_taskid(void *t, int colums, char **val, char **columnName)
{
    ScanTaskHelper* h = static_cast<ScanTaskHelper*>(t);
    if (!h) {
        qDebug() << "it's not impossbile!";
        return -1;
    }

    h->d_ptr->mNewTaskID.clear();
    int len = strlen("task_id");
    for (int i = 0; i < colums; ++i) {
        if ((strlen(columnName[i]) == len) && (0 == strncpy(columnName[i], "task_id", len))) {
            h->d_ptr->mNewTaskID += QString(val[i]);
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


ScanTaskHelper::ScanTaskHelper(QString dbPath, QObject *parent)
    : QObject{parent}, d_ptr(new ScanTaskHelperPrivate(dbPath, this))
{

}

ScanTaskHelper::~ScanTaskHelper()
{
    if (d_ptr)          delete d_ptr;
}

void ScanTaskHelper::onDBChanged()
{
    Q_D(ScanTaskHelper);

    d->selectAllTaskID();

    QSet<QString> delT = d->mOldTaskID - d->mNewTaskID;
    QSet<QString> newT = d->mNewTaskID - d->mOldTaskID;
    for (auto id : newT) {
        if (auto t = d->selectTaskByID(id)) {
            Q_EMIT addNewTask(t);
            d->mData[id] = t;
        }
    }

    for (auto id : delT) {
        if (d->mData.contains(id)) {
            auto it = d->mData[id];
            d->mData.remove(id);
            Q_EMIT delOldTask(it);
            delete it;
        }
    }

    d->mOldTaskID = d->mNewTaskID;
}



