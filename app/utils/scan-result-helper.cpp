#include "scan-result-helper.h"

#include "tools.h"
#include "../model/scanner-result-item.h"

#include <QMap>
#include <QSet>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QFileSystemWatcher>

#include <gio/gio.h>
#include <sqlite3.h>

static void doDeleteLater(ScannerResultItem* obj);

class ScanResultHelperPrivate
{
public:
    explicit ScanResultHelperPrivate (QString& db, ScanResultHelper* p);
    ~ScanResultHelperPrivate();

    void open ();
    void close ();
public:
    bool isCanceled() const;
    bool isRunning();
    void setRunning(bool r);

public:
    bool checkRunning();
    void onDBChanged ();

    bool selectIDByFilterName ();
    ScannerResultItem* selectFileByID (QString id);

private:
    bool isInScanDir(const QString& path);
    bool isInScanOutDir(const QString& path) const;
    bool isInScanFileType(const QString& fileType) const;
    bool isInScanFileOutType(const QString& fileName, const QString& fileType) const;

public:
    QString                                     mDBPath;

    QStringList                                 mScanDir;
    QString                                     mTaskName;
    QString                                     mTaskFilter;
    QString                                     mFilterOutDir;

    QString                                     mFileType;
    QString                                     mFileTypeOut;

    QMap<QString, ScannerResultItem*>           mData;                  // <FileMD5, ScannerResultItem*>
    QMutex                                      mLocker;

    sqlite3*                                    mDB;
    QMutex                                      mDBLocker;
    QFileSystemWatcher*                         mWatcher;

    GCancellable*                               mCancel;                // 取消操作
    bool                                        mIsRunning = false;
    QMutex                                      mIsRunningLocker;

    // const
    ScanResultHelper*                           q_ptr;
    Q_DECLARE_PUBLIC(ScanResultHelper);
};

ScanResultHelper::ScanResultHelper(QString dbPath, QObject *parent)
    : QObject{parent}, d_ptr(new ScanResultHelperPrivate(dbPath, this))
{
    Q_D(ScanResultHelper);
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
        QString sql = QString("INSERT INTO scan_result (scan_file_name, policy_id, action_id, status, scan_finished_time, detect_result, file_size, file_type, scan_path, exec_path, scan_file_type, exec_type)"
                              "VALUES ('/tmp/最奥大苏打地区党伽倻过渡期一个肚脐眼过渡期一个对齐我有个对齐过渡期为过渡期蔓延到股权五斗柜趣味有多高趣味-%1', 'A', 'A', 0, 1658558157, '', '', '', '', '', '', '');").arg (i);

        while (!sqlite_lock());
        d->open ();
        int ret = sqlite3_exec(d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errorMsg);
        if (SQLITE_OK != ret) {
            qDebug() << "error: " << errorMsg;
            sqlite3_free(errorMsg);
        }
        d->close();
        while (!sqlite_unlock());
        usleep(300);
    }
}

void ScanResultHelper::reset ()
{
    Q_D(ScanResultHelper);

    g_cancellable_cancel (d->mCancel);

    d->mLocker.lock();
    for (auto& i : d->mData) {
        delete i;
    }
    d->mData.clear();
    d->mLocker.unlock();

    g_cancellable_reset (d->mCancel);
}

void ScanResultHelper::refreshResult()
{
    Q_D(ScanResultHelper);

    if ((nullptr == d->mTaskName) || d->mTaskName.isNull () || d->mTaskName.isEmpty () || d->mTaskName == ""
        || (nullptr == d->mTaskFilter) || d->mTaskFilter.isNull () || d->mTaskFilter.isEmpty () || d->mTaskFilter == "") {
        return;
    }

    loadTaskResult (d->mTaskName, d->mTaskFilter, d->mScanDir, d->mFilterOutDir, d->mFileType, d->mFileTypeOut);
}

void ScanResultHelper::onItemDeleted(const QString& id)
{
    Q_D (ScanResultHelper);

    d->mLocker.lock();
    if (d->mData.contains(id)) {
        auto item = d->mData[id];
        Q_EMIT delOldFile (item);
        // QMap 的 remove 里做了资源释放操作
        d->mData.remove(id);
    }
    d->mLocker.unlock();
}

void ScanResultHelper::loadTaskResult (const QString& taskName, const QString& taskFilter, const QStringList& scanDir, const QString& filterOutDir, const QString& fileType, const QString& fileTypeOut)
{
    Q_D(ScanResultHelper);

    // 此处需要修改
    d->mScanDir = scanDir;
    d->mTaskName = taskName;
    d->mTaskFilter = taskFilter;
    d->mFilterOutDir = filterOutDir;
    d->mFileType = fileType;
    d->mFileTypeOut = fileTypeOut;

    d->onDBChanged();
}

ScanResultHelperPrivate::ScanResultHelperPrivate(QString& db, ScanResultHelper *p)
    : q_ptr(p), mDB(nullptr), mWatcher(nullptr)
{
    Q_Q(ScanResultHelper);

    mCancel = g_cancellable_new();

    mDBPath = std::move(db);

    qDebug() << "connect to database: " << mDBPath << " successful!";
}

ScanResultHelperPrivate::~ScanResultHelperPrivate()
{
    if (mCancel)        { g_object_unref (mCancel); mCancel = nullptr;}
    if (mDB)            { sqlite3_close(mDB); mDB = nullptr;}

    mLocker.lock();
    for (auto & m : mData) {
        delete m;
    }
    mData.clear();
    mLocker.unlock();
}

void ScanResultHelperPrivate::onDBChanged()
{
    Q_Q(ScanResultHelper);

    g_return_if_fail(!isCanceled());

    setRunning (true);

    QSet<QString> allItem;
    QList <ScannerResultItem*> addItem;
    QList <ScannerResultItem*> delItem;
    QList <ScannerResultItem*> updateItem;

    QStringList k = mTaskFilter.split("|");
    QStringList od = mFilterOutDir.split("|");
    if (k.count() <= 0)     return;

    QStringList policyIDs;
    for (const auto& ii : k) {
        if (nullptr == ii || ii.isNull() || ii.isEmpty() || "" == ii)   continue;
        policyIDs += ("'" + ii + "'");
    }

    QString policy = policyIDs.join(",");

    sqlite3_stmt* stmt = nullptr;
    if (policy.isNull() || policy.isEmpty() || "" == policy) {
        qDebug() << "filter is null";
        return;
    }

    QString sql = QString("SELECT `ID`, `scan_file_name`, `status`, `scan_finished_time`, `file_type`, `change_time`"
                          " FROM scan_result WHERE status!=5 AND policy_id IN (%1)").arg (policy);
//    qInfo() << "sql ==> " << sql;
    g_return_if_fail(!isCanceled());
    while (!sqlite_lock()); // {if (++ev % 10) QApplication::processEvents(); usleep(1000);};
    open();
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            if (isCanceled()) { qDebug() << "canceled!"; QApplication::processEvents(); break;}
            QApplication::processEvents();
            QString id = QString("%1").arg(sqlite3_column_int(stmt, 0));
            QString fileName = QString(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            int status = sqlite3_column_int(stmt, 2);   // 状态不更新，只有客户端会改
            int finishedTime = sqlite3_column_int(stmt, 3);
            QString fileType (reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            gint64 modifyTime = sqlite3_column_int(stmt, 5);

            // 取消
            if (isCanceled()) {
                qWarning () << "canceled";
                break;
            }

            if (/*!QFile::exists(fileName) ||*/ nullptr == id || id.isNull() || id.isEmpty() || "" == id
                || nullptr == fileName || fileName.isNull() || fileName.isEmpty() || "" == fileName) {
                qWarning() << "file not exists or id、file name is empty";
                continue;
            }

            if (isInScanDir(fileName) && !isInScanOutDir(fileName) && isInScanFileType(fileType) && !isInScanFileOutType(fileName, fileType)) {
                if (mData.contains (id)) {
                    auto item = mData[id];
                    if (finishedTime != item->getFileCreateTime()) {
                        item->setFileModifyTime(modifyTime);
                        item->setFileCreateTime(finishedTime);
                        updateItem += item;
                    }
                }
                else {
                    auto item = new ScannerResultItem;
                    item->setTaskName(mTaskName);
                    item->setID(id);
                    item->setStatus(status);
                    item->setFileName(fileName);
                    item->setFileType(fileType);
                    item->setFileModifyTime(modifyTime);
                    item->setFileCreateTime(finishedTime);
                    item->setCanUntreated(item->getStatus2 () == ScannerResultItem::MisReport);

                    QFileInfo file(item->getFileName());
                    if (file.exists()) {
                        item->setFileModifyTime(file.lastModified().toSecsSinceEpoch());
                    }
                    mLocker.lock();
                    mData[id] = item;
                    mLocker.unlock();
                    addItem += item;
                }
                allItem += id;
            }
//            QApplication::processEvents();
        }
        if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    }
    else {
        qWarning() << "sql execute error: " << sqlite3_errmsg(mDB);
        goto out;
    }

    // 是否是取消操作
    if (!isCanceled()) {
        mLocker.lock();
        auto delItemT = mData.keys().toSet() - allItem;
        mLocker.unlock();

        for (auto& id : delItemT) {
            if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
            delItem += mData[id];
        }

        Q_EMIT q->addNewFile (addItem);
        Q_EMIT q->delOldFile (delItem);
        Q_EMIT q->updateFile (updateItem);

        qDebug() << "is running..." << "all: " << allItem.count() << "del: " << delItem.count();

        Q_EMIT q->allItemsUpdated();
    }
    else {
        qInfo () << "取消";
    }

out:
    if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    close();
    while (!sqlite_unlock());

    Q_EMIT q_ptr->loadFinished();

    setRunning (false);

    qDebug() << "query db ok!!";
}

bool ScanResultHelperPrivate::selectIDByFilterName()
{
    QStringList k = mTaskFilter.split("|");
    if (k.count() <= 0)     return false;

    sqlite3_stmt* stmt = nullptr;

    while (!sqlite_lock());
    open();
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
    close();
    while (!sqlite_unlock());

    return true;
}

ScannerResultItem* ScanResultHelperPrivate::selectFileByID (QString id)
{
    auto* item = new ScannerResultItem;
    item->setTaskName(mTaskName);

    QString sql = QString("SELECT `ID`, `scan_file_name`, `status`, `scan_finished_time` "
                      " FROM scan_result "
                      " WHERE ID='%1'").arg(id);

    while (!sqlite_lock());
    open();
    sqlite3_stmt* stmt = nullptr;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            item->setID (sqlite3_column_int(stmt, 0));
            item->setFileName(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
            item->setStatus(sqlite3_column_int(stmt, 2));
            item->setFileCreateTime (QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))));

            item->setCanUntreated((item->getStatus2 () == ScannerResultItem::MisReport));
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
    close();
    while (!sqlite_unlock());

    if (stmt)       sqlite3_finalize(stmt);

    return item;

noChanged:
    close();
    while (!sqlite_unlock());

    if (stmt)       sqlite3_finalize(stmt);

    return nullptr;

}

bool ScanResultHelperPrivate::isInScanDir(const QString &path)
{
    if (mScanDir.isEmpty()) {
        return true;
    }

    if (std::any_of (mScanDir.begin(), mScanDir.end(), [=] (const QString& s) -> bool {
        if ("/" == s || "*" == s) {
            return true;
        }
        auto& ss = (!s.endsWith ("/") ? (s + "/") : s);
        if (path.startsWith (ss)) {
            return true;
        }
        return false;
    })) return true;

    return false;
}

bool ScanResultHelperPrivate::isInScanOutDir(const QString &path) const
{
    QStringList od = mFilterOutDir.split("|").toSet().toList();
    if (od.isEmpty()) {
        return false;
    }
//    qDebug() << "out dir: " << od;

    if (std::any_of (od.begin(), od.end(), [=] (const QString& s) -> bool {
        if ("/" == s || "*" == s) {
            return true;
        }
        else if ("" == s) {
            return false;
        }
        auto& ss = (!s.endsWith ("/") ? (s + "/") : s);
        if (path.startsWith (ss)) {
            return true;
        }
        return false;
    })) return true;

    return false;
}

bool ScanResultHelperPrivate::isInScanFileType(const QString& fileType) const
{
    QStringList ft = mFileType.split("|").toSet().toList();
    if (ft.isEmpty()) {
        return true;
    }
//    qDebug() << "file type: " << fileType << "all file type:" << ft;

    if (std::any_of (ft.begin(), ft.end(), [=] (const QString& s) -> bool {
        if ("" == s || "*" == s) {
            return true;
        }
        return s == fileType;
    })) return true;

    return false;
}

bool ScanResultHelperPrivate::isInScanFileOutType(const QString& fileName, const QString &fileType) const
{
    QStringList ft = mFileTypeOut.split("#").toSet().toList();
    if (ft.isEmpty()) {
        return false;
    }

    QStringList fileExtT = fileName.split(".");
    fileExtT.pop_front();
    QString fileExt = fileExtT.join (".");

//    qDebug()
//        << "\nfile name: " << fileName
//        << "\nfile type: " << fileType
//        << "\nfile extend name: " << fileExt
//        << "\nscan file out type: " << mFileTypeOut
//        ;

//    qDebug() << "out file type: " << ft;

    if (std::any_of (ft.begin(), ft.end(), [=] (const QString& s) -> bool {
        if ("*" == s) {
            return true;
        }
        else if ("" == s) {
            return false;
        }

        QStringList ls = s.split (":");

        g_return_val_if_fail(ls.length() == 2, false);

        auto filterOutType = ls.takeFirst().split ("|").toSet();
        auto filterExtName = ls.takeFirst().split ("&").toSet();

//        qDebug()
//            << "\nfilter out type: " << filterOutType
//            << "\nfilter out extend name: " << filterExtName
//            ;

        if (!filterOutType.contains (fileType)) { return false; }

        if (filterExtName.isEmpty()) { return true; }

        for (auto& n : filterExtName) {
            if (n == fileExt) {
                return true;
            }
            else if (n.length() == fileExt.length()) {
                auto ssk = n.split ("?").toSet();
                if (std::all_of (ssk.begin(), ssk.end(), [&] (const QString& ass) -> bool {
                    if ("" == ass || ass.isNull() || ass.isEmpty()) return true;
                    return fileExt.contains (ass);
                })) return true;
            }
        }
        return false;
    })) return true;

    return false;
}

bool ScanResultHelperPrivate::checkRunning()
{
    return !g_cancellable_is_cancelled (mCancel);
}

bool ScanResultHelperPrivate::isRunning()
{
    mIsRunningLocker.lock();
    bool r = mIsRunning;
    mIsRunningLocker.unlock();

    return r;
}

bool ScanResultHelperPrivate::isCanceled() const
{
    return g_cancellable_is_cancelled (mCancel);
}

void ScanResultHelperPrivate::setRunning(bool r)
{
    mIsRunningLocker.lock();
    mIsRunning = r;
    mIsRunningLocker.unlock();
}

void ScanResultHelperPrivate::open()
{
    mDBLocker.lock();

    int rc = sqlite3_open (mDBPath.toUtf8().constData(), &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to database: " << mDBPath << " failed!";
        QMessageBox::warning(nullptr, "数据库错误", "连接数据库出错", QMessageBox::Ok);
        qApp->exit(-1);
    }

    mDBLocker.unlock();
}

void ScanResultHelperPrivate::close()
{
    mDBLocker.lock();

    if (mDB) {
        sqlite3_close (mDB);
        mDB = nullptr;
    }

    mDBLocker.unlock();
}

void ScanResultHelper::misReportByIDs(const QStringList& ids)
{
    Q_D(ScanResultHelper);

    for (const auto& id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status=6, status_reported=0 WHERE ID=%1 AND status!=6").arg(id);
        qInfo() << "sql ==> " << sql;

        char *errMsg = nullptr;
        while (!sqlite_lock());
        d->open ();
        sqlite3_exec(d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        d->close();
        while (!sqlite_unlock());
        if (errMsg) {
            qWarning() << "report error: " << errMsg;
            sqlite3_free(errMsg);
        }
        Q_EMIT detailOne();
        QApplication::processEvents();
    }
}

void ScanResultHelper::deleteItemByIDs(const QStringList& ids)
{
    Q_D(ScanResultHelper);

    for (auto id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status=5, status_reported=0 WHERE ID=%1 AND status != 5").arg (id);
        qDebug() << "sql: " << sql;

        char* errMsg = nullptr;
        while (!sqlite_lock());
        d->open();
        sqlite3_exec (d->mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        d->close();
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

void ScanResultHelper::cancel()
{
    Q_D(ScanResultHelper);

    g_cancellable_cancel (d->mCancel);
}

bool ScanResultHelper::isRunning()
{
    Q_D(ScanResultHelper);

    return d->isRunning();
}

static void doDeleteLater(ScannerResultItem* obj)
{
    if (obj) {
        obj->deleteLater();
    }
}
