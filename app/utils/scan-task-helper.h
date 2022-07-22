#ifndef SCANTASKHELPER_H
#define SCANTASKHELPER_H

#include <QSet>
#include <QObject>
#include <QFileSystemWatcher>

#include <sqlite3.h>

class ScannerTaskItem;
class ScanTaskHelperPrivate;

/**
 * @brief 用于监控数据库改变，每次数据库有改变则获取所有任务ID，并构建 ModelItem 给 Model 层
 *      它由 Model 层实例化
 */
class ScanTaskHelper : public QObject
{
    Q_OBJECT
public:
    explicit ScanTaskHelper(QString dbPath, QObject *parent = nullptr);
    ~ScanTaskHelper();

private:

Q_SIGNALS:
    void addNewTask (ScannerTaskItem* it);
    void delOldTask (ScannerTaskItem* it);

public Q_SLOTS:
    void onDBChanged ();


private:
    ScanTaskHelperPrivate*  d_ptr;

    Q_DISABLE_COPY(ScanTaskHelper)
    Q_DECLARE_PRIVATE(ScanTaskHelper)
};

#endif // SCANTASKHELPER_H
