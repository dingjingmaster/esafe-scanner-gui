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
#include <QMutex>

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

    QStringList                         mScanDir;
    QString                             mTaskName;
    QString                             mTaskFilter;
    QString                             mFilterOutDir;

    QMap<QString, ScannerResultItem*>   mData;                  // <FileMD5, ScannerResultItem*>

    sqlite3*                            mDB;
    QFileSystemWatcher*                 mWatcher;

    QMutex                              mLocker;

    // const
    ScanResultHelper*                   q_ptr;
    Q_DECLARE_PUBLIC(ScanResultHelper);
};

ScanResultHelper::ScanResultHelper(QString dbPath, QObject *parent)
    : QObject{parent}, d_ptr(new ScanResultHelperPrivate(dbPath, this))
{
    Q_D(ScanResultHelper);

    connect (this, qOverload<QString&>(&ScanResultHelper::delOldFile), this, &ScanResultHelper::onItemDeleted, Qt::UniqueConnection);
}

ScanResultHelper::~ScanResultHelper()
{
    delete d_ptr;
}

void ScanResultHelper::testInsertItem()
{
    Q_D(ScanResultHelper);

    char* errorMsg = nullptr;

    for (int i = 0; i < 1000000; ++i) {
        QString sql = QString("INSERT INTO scan_result (scan_file_name, policy_id, action_id, status, scan_finished_time, detect_result, file_size, file_type)"
                              "VALUES ('/tmp/%1', 'A', 'A', 0, 1658558157, '', '', '');").arg (i);

        while (!sqlite_lock());
        int ret = sqlite3_exec(d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errorMsg);
        if (SQLITE_OK != ret) {
            qDebug() << "error: " << errorMsg;
            sqlite3_free(errorMsg);
        }
        while (!sqlite_unlock());
        usleep(300);
    }
}

void ScanResultHelper::clearData()
{
    Q_D(ScanResultHelper);

    d->mLocker.lock();

    d->mTaskName.clear();
    d->mTaskFilter.clear();

    for (auto i = d->mData.begin(); i != d->mData.end(); ++i) {
        delete i.value();
    }
    d->mData.clear();

    d->mLocker.unlock();
}

void ScanResultHelper::refresResult()
{
    Q_D(ScanResultHelper);

    if ((nullptr == d->mTaskName) || d->mTaskName.isNull () || d->mTaskName.isEmpty () || d->mTaskName == ""
        || (nullptr == d->mTaskFilter) || d->mTaskFilter.isNull () || d->mTaskFilter.isEmpty () || d->mTaskFilter == "") {
        return;
    }

    loadTaskResult (d->mTaskName, d->mTaskFilter, d->mScanDir, d->mFilterOutDir);
}

void ScanResultHelper::onItemDeleted(QString& id)
{
    Q_D (ScanResultHelper);

    //d->mLocker.lock();
    if (d->mData.contains(id)) {
        auto item = d->mData[id];
        Q_EMIT delOldFile (item);
        // QMap 的 remove 里做了资源释放操作
        d->mData.remove(id);
        //item ->deleteLater();
    }
    //d->mLocker.unlock();
}

void ScanResultHelper::loadTaskResult(QString taskName, QString taskFilter, QStringList scanDir, QString filterOutDir)
{
    Q_D(ScanResultHelper);

    // 此处需要修改

    d->mScanDir = scanDir;
    d->mTaskName = taskName;
    d->mTaskFilter = taskFilter;
    d->mFilterOutDir = filterOutDir;

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
}

ScanResultHelperPrivate::~ScanResultHelperPrivate()
{
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}
    mLocker.lock();
    for (auto m = mData.begin(); m != mData.end(); ++m)    delete m.value();
    mData.clear();
    mLocker.unlock();
}

void ScanResultHelperPrivate::onDBChanged()
{
    Q_Q(ScanResultHelper);

    QStringList k = mTaskFilter.split("|");
    QStringList od = mFilterOutDir.split("|");
    if (k.count() <= 0)     return;

    QStringList policyIDs;
    for (const auto& ii : k) {
        if (nullptr == ii || ii.isNull() || ii.isEmpty() || "" == ii)   continue;
        policyIDs += ("'" + ii + "'");
    }

    QString policy = policyIDs.join(",");

    QSet<QString> allItem;
    sqlite3_stmt* stmt = nullptr;

    if (policy.isNull() || policy.isEmpty() || "" == policy) {
        qDebug() << "filter is null";
        return;
    }

    QString sql = QString("SELECT `ID`, `scan_file_name`, `status`, `scan_finished_time`"
                          " FROM scan_result WHERE status!=5 AND policy_id IN (%1)").arg (policy);
    qInfo() << "sql ==> " << sql;
    while (!sqlite_lock()); // {if (++ev % 10) QApplication::processEvents(); usleep(1000);};
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            QApplication::processEvents();
            QString id = QString("%1").arg(sqlite3_column_int(stmt, 0));
            QString fileName = QString(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            int status = sqlite3_column_int(stmt, 2);   // 状态不更新，只有客户端会改
            int finishedTime = sqlite3_column_int(stmt, 3);

            if (!QFile::exists(fileName) || nullptr == id || id.isNull() || id.isEmpty() || "" == id
                || nullptr == fileName || fileName.isNull() || fileName.isEmpty() || "" == fileName) {
                qWarning() << "file not exists or id、file name is empty";
                continue;
            }

            // scan directory
            for (const auto& s : mScanDir) {
                if (fileName.startsWith(s)) {

                    bool filterOut = false;

                    // filter out dir
                    if (od.empty()) {
                        for (const auto& d : od) {
                            if (nullptr == d || "" == d || d.isEmpty()) continue;
                            if (d.startsWith("/")) {
                                if (fileName.startsWith(d)) {
                                    filterOut = true;
                                    break;
                                }
                            } else {
                                if (fileName.contains(d)) {
                                    filterOut = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (filterOut) break;

                    //mLocker.lock();
                    auto item = (mData.contains(id)) ? mData[id] : nullptr;
                    //mLocker.unlock();
                    if (item) {
                        if (finishedTime != item->getFileCreateTime()) {
                            item->setFileCreateTime(finishedTime);
                            //item->setStatus(status);
                            Q_EMIT q->updateFile(item);
                        }
                    } else {
                        auto item = new ScannerResultItem;
                        item->setTaskName(mTaskName);

                        item->setID(id);
                        item->setStatus(status);
                        item->setFileName(fileName);
                        item->setFileCreateTime(finishedTime);
                        item->setCanUntreated((item->getStatus2() == ScannerResultItem::MisReport) ? true : false);

                        QFileInfo file(item->getFileName());
                        if (file.exists()) {
                            item->setFileModifyTime(file.metadataChangeTime().toSecsSinceEpoch());
                        }

                        mLocker.lock();
                        mData[id] = item;
                        mLocker.unlock();
                        Q_EMIT q->addNewFile(item);
                    }
                    qInfo() << "task id:" << id;
                    allItem += id;
                    QApplication::processEvents();
                    break;
                }
            }
        }
        if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    } else {
        qWarning() << "sql execute error: " << sqlite3_errmsg(mDB);
    }

    if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    while (!sqlite_unlock());

    mLocker.lock();
    QSet<QString> nowItem = mData.keys().toSet();
    mLocker.unlock();

    auto delItem = allItem - nowItem;

    for (auto id : delItem) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        // 线程安全的
        Q_EMIT q->delOldFile(id);
    }

    Q_EMIT q->allItemsUpdated();
}

bool ScanResultHelperPrivate::selectIDByFilterName()
{
    QStringList k = mTaskFilter.split("|");
    if (k.count() <= 0)     return false;

    sqlite3_stmt* stmt = nullptr;

    while (!sqlite_lock());
    for (const auto& ik : k) {
        qInfo() << "filter name --> " << ik;
        if (nullptr == ik || ik.isNull() || ik.isEmpty() || "" == ik)   continue;
        QString sql = QString("SELECT ID FROM scan_result WHERE status!=5 AND policy_id LIKE '%") + ik + "%'";
        qInfo() << "sql ==> " << sql;
        int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
        if (SQLITE_OK == ret) {
            while (SQLITE_DONE != sqlite3_step(stmt)) {
                QString id = QString("%1").arg(sqlite3_column_int(stmt, 0));
                qInfo() << "task id:" << id;
                //QString(reinterpret_cast<const char*>(id));
            }
        }
        if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    }
    if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    while (!sqlite_unlock());

    return true;

noChanged:

    return false;
}

ScannerResultItem *ScanResultHelperPrivate::selectFileByID (QString id)
{
    auto* item = new ScannerResultItem;
    item->setTaskName(mTaskName);

    QString sql = QString("SELECT `ID`, `scan_file_name`, `status`, `scan_finished_time` "
                      " FROM scan_result "
                      " WHERE ID='%1'").arg(id);

    while (!sqlite_lock());
    sqlite3_stmt* stmt = nullptr;
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

void ScanResultHelper::misReportByIDs(QStringList& ids)
{
    Q_D(ScanResultHelper);

    for (const auto& id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status=6 WHERE ID=%1").arg(id);
        qInfo() << "sql ==> " << sql;

        char *errMsg = nullptr;
        while (!sqlite_lock());
        sqlite3_exec(d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        while (!sqlite_unlock());
        if (errMsg) {
            qWarning() << "report error: " << errMsg;
            sqlite3_free(errMsg);
        }
        Q_EMIT detailOne();
        QApplication::processEvents();
    }
}

void ScanResultHelper::deleteItemByIDs(QStringList& ids)
{
    Q_D(ScanResultHelper);

    for (auto id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status=5 WHERE ID=%1").arg (id);
        qDebug() << "sql: " << sql;

        char* errMsg = nullptr;
        while (!sqlite_lock());
        sqlite3_exec (d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        while (!sqlite_unlock());
        if (errMsg) {
            qWarning() << "report error: " << errMsg << " DB FILE: " DB_PATH;
            sqlite3_free(errMsg);
        } else {
            Q_EMIT delOldFile (id);
        }
        Q_EMIT detailOne();
        QApplication::processEvents();
    }
}
