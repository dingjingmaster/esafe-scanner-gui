#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include "../utils/scan-result-helper.h"


class QTimer;
class QThread;
class ScanTaskHelper;
class ScanResultHelper;
class QFileSystemWatcher;

class DBManager : public QObject
{
    Q_OBJECT
public:
    enum CurPage { CUR_TASK, CUR_RESULT }; Q_ENUM(CurPage)

public:
    static DBManager* instance();

    ScanTaskHelper* getTaskHelper();
    ScanResultHelper* getResultHelper();

    void setCurPage (CurPage page);

Q_SIGNALS:
    void stopDBMonitor();
    void startDBMonitor();
    void refreshScanTask ();
    void refreshScanResult2 ();
    void refreshScanResult (QString taskName="", QString taskFilter="", QStringList scanDir=QStringList());

private Q_SLOTS:
    void updateModel();


private:
    explicit DBManager(QObject *parent = nullptr);
    //~DBManager();

private:
    CurPage                 mPage;

    QTimer*                 mTimer;

    static DBManager*       gInstance;

    ScanResultHelper*       mScanResult;
    QThread*                mScanResultThread;

    ScanTaskHelper*         mScanTask;
    QThread*                mScanTaskThread;

    QFileSystemWatcher*     mWatcher;
};



#endif // DBMANAGER_H
