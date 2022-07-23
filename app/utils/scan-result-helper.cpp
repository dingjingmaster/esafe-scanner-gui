#include "scan-result-helper.h"

#include "../model/scanner-result-item.h"

#include <QMap>
#include <QSet>
#include <QDebug>
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

    bool selectFileMD5ByFilterName ();
    ScannerResultItem* selectFileByFileMD5 (QString fileMD5);

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
        qDebug() << "db file changed!";
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

    if (!selectFileMD5ByFilterName()) {
        qDebug() << "db not change";
        return;
    }

    //
    QSet<QString> delT = mOldTaskID - mNewTaskID;
    QSet<QString> newT = mNewTaskID - mOldTaskID;
    for (auto id : newT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (auto t = selectFileByFileMD5(id)) {
            qDebug() << "add file '" << id << "'";
            Q_EMIT q->addNewFile(t);
            mData[id] = t;
        }
    }

    for (auto id : delT) {
        if (nullptr == id || id.isNull() || id.isEmpty() || "" == id)   continue;
        if (mData.contains(id)) {
            auto it = mData[id];
            mData.remove(id);
            qDebug() << "delete file '" << id << "'";
            Q_EMIT q->delOldFile(it);
            delete it;
        }
    }

    mOldTaskID = mNewTaskID;
}

bool ScanResultHelperPrivate::selectFileMD5ByFilterName()
{
    QStringList k = mTaskFilter.split(";");
    if (k.count() <= 0)     return false;

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    sqlite3_stmt* stmt = NULL;

    for (auto ik : k) {
        qDebug() << "filter name --> " << ik;
        if (nullptr == ik || ik.isNull() || ik.isEmpty() || "" == ik)   continue;
        QString sql = QString("SELECT scan_file_name_md5 FROM scan_result WHERE status=0 AND filter_name LIKE '%") + ik + "%'";
        qDebug() << "sql ==> " << sql;
        int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
        if (SQLITE_OK == ret) {
            while (SQLITE_DONE != sqlite3_step(stmt)) {
                const unsigned char* id = sqlite3_column_text(stmt, 0);
                qDebug() << "task id:" << id;
                mNewTaskID += QString(reinterpret_cast<const char*>(id));
            }
        }
        if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}
    }

    qDebug() << "old fileMD5: " << mOldTaskID;
    qDebug() << "new fileMD5: " << mNewTaskID;

    if (mNewTaskID.count() == mOldTaskID.count()) {
        goto noChanged;
    }

    if (stmt)           { sqlite3_finalize(stmt); stmt = nullptr;}

    return true;

noChanged:

    if (stmt)       sqlite3_finalize(stmt);

    return false;
}

ScannerResultItem *ScanResultHelperPrivate::selectFileByFileMD5(QString fileMD5)
{
    ScannerResultItem* item = new ScannerResultItem;
    item->setTaskName(mTaskName);

    QString sql = QString("SELECT `scan_file_name`, `status`, `file_create_time`, `file_modify_time` "
                      " FROM scan_result "
                      " WHERE scan_file_name_md5='%1'").arg(fileMD5);

    mOldTaskID = mNewTaskID;
    mNewTaskID.clear();

    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(mDB, sql.toUtf8().constData(), -1, &stmt, nullptr);
    if (SQLITE_OK == ret) {
        while (SQLITE_DONE != sqlite3_step(stmt)) {
            item->setFileName(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
            item->setStatus(sqlite3_column_int(stmt, 1));
            item->setFileCreateTime(sqlite3_column_int(stmt, 2));
            item->setFileModifyTime(sqlite3_column_int(stmt, 3));

            qDebug() << "file name: " << item->getFileName() << "\n"
                     << "status: " << item->getStatus() << "\n"
                     << "create time: " << item->getFileCreateTime() << "\n"
                     << "modify time: " << item->getFileModifyTime() << "\n"
                     << "filter name: " << item->getFilterName() << "\n\n\n";
        }
    }

    if (stmt)       sqlite3_finalize(stmt);

    return item;

noChanged:

    if (stmt)       sqlite3_finalize(stmt);

    return nullptr;

}
