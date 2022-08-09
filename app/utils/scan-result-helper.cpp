#include "scan-result-helper.h"

#include "tools.h"
#include "../model/scanner-result-item.h"

#include <QMap>
#include <QSet>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QFileSystemWatcher>

#include <sqlite3.h>

class ScanResultHelperPrivate
{
public:
    explicit ScanResultHelperPrivate (QString db, ScanResultHelper* p);
    ~ScanResultHelperPrivate();

public:
    void onDBChanged ();

    bool selectIDByFilterName ();
    ScannerResultItem* selectFileByID (QString id);

public:
    QString                             mDBPath;

    QString                             mTaskName;
    QString                             mTaskFilter;

    QSet<QString>                       mOldTaskID;
    QSet<QString>                       mNewTaskID;

    QMap<QString, ScannerResultItem*>   mData;                  // <FileMD5, ScannerResultItem*>

    sqlite3*                            mDB;
    QFileSystemWatcher*                 mWatcher;

    // const
    ScanResultHelper*                   q_ptr;
    Q_DECLARE_PUBLIC(ScanResultHelper);
};

ScanResultHelper::ScanResultHelper(QString dbPath, QObject *parent)
    : QObject{parent}, d_ptr(new ScanResultHelperPrivate(dbPath, this))
{

}

ScanResultHelper::~ScanResultHelper()
{
    if (d_ptr)          delete d_ptr;
}

void ScanResultHelper::misReportByName(QString name)
{
    Q_D(ScanResultHelper);

    if (!(d->mData.contains (name))) {
        qWarning() << "item: '" << name << "' not existed";
        return;
    }

    ScannerResultItem* item = d->mData[name];

    QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=6 WHERE ID=%1").arg (item->getID ());
    qInfo() << "sql ==> " << sql;

    while (!sqlite_lock());

    char* errMsg = NULL;
    sqlite3_exec (d->mDB, sql.toUtf8().constData(), NULL, NULL, &errMsg);
    if (errMsg) {
        qWarning() << "report error: " << errMsg;
        sqlite3_free(errMsg);
    }

    while (!sqlite_unlock());
}

void ScanResultHelper::deleteItemByName(QString name)
{
    Q_D(ScanResultHelper);

    if (!(d->mData.contains (name))) {
        qWarning() << "item: '" << name << "' not existed";
        return;
    }

    ScannerResultItem* item = d->mData[name];

    QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=5 WHERE ID=%1").arg (item->getID ());

    while (!sqlite_lock());

    char* errMsg = NULL;
    sqlite3_exec (d->mDB, sql.toUtf8().constData(), NULL, NULL, &errMsg);
    if (errMsg) {
        qWarning() << "report error: " << errMsg;
        sqlite3_free(errMsg);
    } else {
        d->mData.remove (name);
        Q_EMIT delOldFile (item);
        delete item;
    }

    while (!sqlite_unlock());
}

void ScanResultHelper::clearData()
{
    Q_D(ScanResultHelper);

    d->mNewTaskID.clear();
    d->mOldTaskID.clear();
    d->mTaskName.clear();
    d->mTaskFilter.clear();

    for (auto i = d->mData.begin(); i != d->mData.end(); ++i) {
        i.value()->deleteLater();
    }
    d->mData.clear();
}

void ScanResultHelper::loadTaskResult(QString taskName, QString taskFilter)
{
    Q_D(ScanResultHelper);

    d->mTaskName = taskName;
    d->mTaskFilter = taskFilter;

    d->onDBChanged();
}

ScanResultHelperPrivate::ScanResultHelperPrivate(QString db, ScanResultHelper *p)
    : q_ptr(p)
{
    Q_Q(ScanResultHelper);

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
        qInfo() << "db file changed!";
        onDBChanged();
    });
}

ScanResultHelperPrivate::~ScanResultHelperPrivate()
{
    if (mWatcher)       delete mWatcher;
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}
    for (auto m = mData.begin(); m != mData.end(); ++m)    delete m.value();
    mData.clear();
    mOldTaskID.clear();
    mNewTaskID.clear();
}

void ScanResultHelperPrivate::onDBChanged()
{
    Q_Q(ScanResultHelper);

    if (nullptr == mTaskName || mTaskName.isNull() || mTaskName.isEmpty() || "" == mTaskName) return;

    if (!selectIDByFilterName()) {
        qInfo() << "db not change";
        return;
    }

    //
    QSet<QString> delT = mOldTaskID - mNewTaskID;
    QSet<QString> newT = mNewTaskID - mOldTaskID;
    for (auto id : newT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (auto t = selectFileByID (id)) {
            qInfo() << "add file '" << id << "'";
            Q_EMIT q->addNewFile(t);
            mData[id] = t;
        }
    }

    for (auto id : delT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (mData.contains(id)) {
            auto it = mData[id];
            mData.remove(id);
            qInfo() << "delete file '" << id << "'";
            Q_EMIT q->delOldFile(it);
            delete it;
        }
    }

    mOldTaskID = mNewTaskID;
}

bool ScanResultHelperPrivate::selectIDByFilterName()
{
    QStringList k = mTaskFilter.split(";");
    if (k.count() <= 0)     return false;

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    sqlite3_stmt* stmt = NULL;

    while (!sqlite_lock());
    for (auto ik : k) {
        qInfo() << "filter name --> " << ik;
        if (nullptr == ik || ik.isNull() || ik.isEmpty() || "" == ik)   continue;
        QString sql = QString("SELECT ID FROM scan_result WHERE status!=5 AND policy_id LIKE '%") + ik + "%'";
        qInfo() << "sql ==> " << sql;
        int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
        if (SQLITE_OK == ret) {
            while (SQLITE_DONE != sqlite3_step(stmt)) {
                QString id = QString("%1").arg(sqlite3_column_int(stmt, 0));
                qInfo() << "task id:" << id;
                mNewTaskID += id;
                //QString(reinterpret_cast<const char*>(id));
            }
        }
        if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    }
    if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    while (!sqlite_unlock());

    qInfo () << "old fileMD5: " << mOldTaskID;
    qInfo () << "new fileMD5: " << mNewTaskID;

    if (mNewTaskID.count() == mOldTaskID.count()) {
        goto noChanged;
    }

    return true;

noChanged:

    return false;
}

ScannerResultItem *ScanResultHelperPrivate::selectFileByID (QString id)
{
    ScannerResultItem* item = new ScannerResultItem;
    item->setTaskName(mTaskName);

    QString sql = QString("SELECT `ID`, `scan_file_name`, `status`, `scan_finished_time` "
                      " FROM scan_result "
                      " WHERE ID='%1'").arg(id);

    while (!sqlite_lock());
    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            item->setID (sqlite3_column_int(stmt, 0));
            item->setFileName(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
            item->setStatus(sqlite3_column_int(stmt, 2));
            item->setFileCreateTime (QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))));

            item->setCanUntreated((item->getStatus2() == ScannerResultItem::MisReport) ? true : false);
            //item->setFileCreateTime(sqlite3_column_int(stmt, 3));
            // modify time
            QFileInfo file (item->getFileName ());
            if (file.exists ()) {
                item->setFileModifyTime (file.metadataChangeTime ().toSecsSinceEpoch ());
            }

            qInfo () << "file name: " << item->getFileName() << "\n"
                     << "status: " << item->getStatus() << "\n"
                     << "create time: " << item->getFileCreateTime() << "\n"
                     << "modify time: " << item->getFileModifyTime() << "\n"
                     << "filter name: " << item->getFilterName() << "\n\n\n";
        }
    } else {
        goto noChanged;
    }
    while (!sqlite_unlock());

    if (stmt)       sqlite3_finalize(stmt);

    return item;

noChanged:
    while (!sqlite_unlock());

    if (stmt)       sqlite3_finalize(stmt);

    return nullptr;

}
