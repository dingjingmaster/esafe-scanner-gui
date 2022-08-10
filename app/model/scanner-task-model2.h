#ifndef SCANNERTASKMODEL2_H
#define SCANNERTASKMODEL2_H

#include "scanner-task-item.h"

#include <QList>
#include <QObject>
#include <qglobal.h>
#include <QStandardItemModel>


class QTimer;
class QThread;
class QModelIndex;
class ScanTaskHelper;
class ScannerTaskModel2 : public QStandardItemModel
{
    Q_OBJECT
public:
    enum { TaskName = 1, TaskStatus, TaskStartTime, TaskStopTime, TaskScannerProcess, TaskOperation, EnumSize };
public:
    explicit ScannerTaskModel2(QObject *parent = nullptr);
    QModelIndex getIndexByItem(const ScannerTaskItem* item, int column=0);

    QVariant headerData (int section, Qt::Orientation orentation, int role) const override;

Q_SIGNALS:
    void refreshScanTask ();

public Q_SLOTS:
    void updateViewData ();
    void receiveChanged (int row);
    void onVerScrollbar (double value);

    //void addItem (ScannerTaskItem* item);
    //void delItem (ScannerTaskItem* item);
    //void updateItem (ScannerTaskItem* item);

private:
    QVariant getItemInfoByRowAndColumn(int, int);


private:
    int                                     mCurrentRow;
    int                                     mTotalRows = 100;

    const int                               mBackgroundR = 235;
    const int                               mBackgroundG = 241;
    const int                               mBackgroundB = 248;

    QList<ScannerTaskItem*>                 mData;

    QTimer*                                 mTimer;
    QThread*                                mScanTaskThread;
    ScanTaskHelper*                         mScanTask;
};

#endif // SCANNERTASKMODEL2_H
