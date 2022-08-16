#ifndef SCANNERRESULTSAVETHREAD_H
#define SCANNERRESULTSAVETHREAD_H

#include <QObject>
#include <QStringList>
#include <sqlite3.h>

class ScanResultHelper;
class ScannerResultModel;
class ScannerResultSaveThread : public QObject
{
    Q_OBJECT
public:
    explicit ScannerResultSaveThread(ScannerResultModel* m, ScanResultHelper* h, QObject *parent = nullptr);


    void misReportByID(QString&);
    void deleteItemByID(QString&);

    void misReportByIDs(QStringList&);
    void deleteItemByIDs(QStringList&);


Q_SIGNALS:
    void saveScanResultDataFinished ();

public Q_SLOTS:
    void onSaveScanResultData ();

private:
    // 不要释放， 后续优化 使用智能指针
    ScannerResultModel*             mModel;
    ScanResultHelper*               mHelper;

    sqlite3*                        mDB;
};

#endif // SCANNERRESULTSAVETHREAD_H
