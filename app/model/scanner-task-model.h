#ifndef SCANNERTASKMODEL_H
#define SCANNERTASKMODEL_H

#include <QAbstractTableModel>
#include <QtCore/QMutex>

#include "scanner-task-item.h"

class ScanTaskHelper;

class ScannerTaskModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { TaskName = 1, TaskStatus, TaskStartTime, TaskStopTime, TaskScannerProcess, TaskOperation, EnumSize };
public:
    explicit ScannerTaskModel(QObject *parent = nullptr);
    ~ScannerTaskModel();

    QModelIndex getIndexByItem (const ScannerTaskItem* it, int column=0);

private:
    void resetModel ();

Q_SIGNALS:

public Q_SLOTS:
    void addItem (ScannerTaskItem* item);
    void delItem (ScannerTaskItem* item);
    void updateItem (ScannerTaskItem* item);
    void onScrollbarMoved (float ratio);

public:
    int rowCount (const QModelIndex& parent = QModelIndex()) const override;
    int columnCount (const QModelIndex& parent = QModelIndex()) const override;

    QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const override;
    QVariant headerData (int section, Qt::Orientation orentation, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    const int                                           mBackgroundR = 235;
    const int                                           mBackgroundG = 241;
    const int                                           mBackgroundB = 248;

    int                                                 mCurIndex = 0;

    ScanTaskHelper*                                     mScanTaskHelper;
    
    // ScannerxxItem
    QList<ScannerTaskItem*>                             mData;

    QMutex                                              mLocker;
};

#endif // SCANNERTASKMODEL_H
