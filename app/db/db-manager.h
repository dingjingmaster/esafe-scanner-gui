#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QMutex>
#include <QEventLoop>
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
    enum CurPage { CUR_STOP, CUR_TASK, CUR_RESULT }; Q_ENUM(CurPage)

public:
    static DBManager* instance();

    ScanTaskHelper* getTaskHelper();
    ScanResultHelper* getResultHelper();

    CurPage getCurPage ();
    void setCurPage (CurPage page);

Q_SIGNALS:
    void startLoadResult ();            // 信号暂未用到
    void stopLoadResult ();             // 信号发到界面
    void cancelLoadResult ();           // 信号由界面来

    void loadTaskStart();
    void loadTaskStop();

    void loadTaskResultStart();
    void loadTaskResultStop();


    void stopDBMonitor();
    void startDBMonitor();
    void refreshScanTask ();
    void refreshScanResult2 ();
    void refreshScanResult (const QString taskName="", const QString taskFilter="", const QStringList scanDir=QStringList(), const QString filterOut="", const QString fileType="", const QString fileTypeOut="");

private Q_SLOTS:
    void updateModel();


private:
    explicit DBManager(QObject *parent = nullptr);
    //~DBManager();

private:
    CurPage                 mPage;
    QMutex                  mPageLock;
    QEventLoop              mPageChangeExec;


    QTimer*                 mTimer;

    static DBManager*       gInstance;

    ScanResultHelper*       mScanResult;
    QThread*                mScanResultThread;

    ScanTaskHelper*         mScanTask;
    QThread*                mScanTaskThread;

    QFileSystemWatcher*     mWatcher;
};



#endif // DBMANAGER_H
