#include "scanner-result-save-thread.h"

#include <QDebug>
#include <QObject>
#include <QApplication>

#include "../utils/tools.h"
#include "../utils/scan-result-helper.h"
#include "../model/scanner-result-model.h"

ScannerResultSaveThread::ScannerResultSaveThread(ScannerResultModel *m, ScanResultHelper* h, QObject *parent)
    : QObject{parent}, mHelper(h), mModel(m)
{
    int rc = sqlite3_open (DB_PATH, &mDB);
    if (SQLITE_OK != rc) {
        qCritical() << "connect to database: " << DB_PATH << " failed!";
        qApp->exit(-1);
    }
}

void ScannerResultSaveThread::misReportByID(QString& id)
{
    QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=6 WHERE ID=%1").arg (id);
    qInfo() << "sql ==> " << sql;

    while (!sqlite_lock());

    char* errMsg = nullptr;
    sqlite3_exec (mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
    if (errMsg) {
        qWarning() << "report error: " << errMsg;
        sqlite3_free(errMsg);
    }

    while (!sqlite_unlock());
}

void ScannerResultSaveThread::deleteItemByID(QString& id)
{
    QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=5 WHERE ID=%1").arg (id);

    while (!sqlite_lock());

    char* errMsg = nullptr;
    sqlite3_exec (mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
    if (errMsg) {
        qWarning() << "report error: " << errMsg;
        sqlite3_free(errMsg);
    } else {
        Q_EMIT mHelper->delOldFile (id);
    }

    while (!sqlite_unlock());
}

void ScannerResultSaveThread::misReportByIDs(QStringList& ids)
{
    for (auto id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=6 WHERE ID=%1").arg (id);
        qInfo() << "sql ==> " << sql;

        char* errMsg = nullptr;
        while (!sqlite_lock());
        sqlite3_exec (mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        while (!sqlite_unlock());
        if (errMsg) {
            qWarning() << "report error: " << errMsg;
            sqlite3_free(errMsg);
        }
        QApplication::processEvents();
    }
}

void ScannerResultSaveThread::deleteItemByIDs(QStringList& ids)
{
    for (auto id : ids) {
        QString sql = QString("UPDATE `scan_result` SET status_reported=1, status=5 WHERE ID=%1").arg (id);
        qDebug() << "sql: " << sql;

        char* errMsg = nullptr;
        while (!sqlite_lock());
        sqlite3_exec (mDB, sql.toUtf8().constData(), nullptr, nullptr, &errMsg);
        while (!sqlite_unlock());
        if (errMsg) {
            qWarning() << "report error: " << errMsg << " DB FILE: " DB_PATH;
            sqlite3_free(errMsg);
        } else {
            Q_EMIT mHelper->delOldFile (id);
        }

        QApplication::processEvents();
    }
}

void ScannerResultSaveThread::onSaveScanResultData()
{
    auto items = mModel->getSaveItems();

    misReportByIDs (items.second);
    deleteItemByIDs (items.first);

    Q_EMIT saveScanResultDataFinished ();
}
