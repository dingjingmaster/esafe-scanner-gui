#ifndef SCANNERRESULTMODEL_H
#define SCANNERRESULTMODEL_H
#include <QMap>
#include <QPair>
#include <QThread>
#include <QAbstractTableModel>
#include <QtCore/QMutex>
#include "scanner-result-item.h"

class ScanResultHelper;

class ScannerResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { /*TaskName = 1,*/ FileName = 1, Status, CreateTime, ModifyTime, EnumSize };
public:
    explicit ScannerResultModel(QObject *parent = nullptr);

    bool hasChanged ();
    bool hasChecked ();
    bool isCheckAllItems ();
    void addItem (ScannerResultItem* item);
    void delItem (ScannerResultItem* item);
    
    int getAllCount ();
    int getNoFixCount ();
    int getDeleteCount ();
    int getMisReportCount ();

    QList<const ScannerResultItem*> getChangedItem ();
    QList<ScannerResultItem*> getSelectedItem ();
    QModelIndex getIndexByItem (const ScannerResultItem* it, int column=0);

    void saveResult();

    void setSelectedItemStatus (ScannerResultItem::Status status);

private:
    void changeItemCount(int status, bool isAdd=true);

public Q_SLOTS:
    void selectAll (bool s);
    void onScrollbarMoved (float ratio);

Q_SIGNALS:
    void clearData ();
    void lazyUpdateView ();
    void showData (QString TaskName, QString filterName, QStringList scanDir);

public:
    int rowCount (const QModelIndex& parent = QModelIndex()) const override;
    int columnCount (const QModelIndex& parent = QModelIndex()) const override;

    QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const override;
    QVariant headerData (int section, Qt::Orientation orentation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setData(ScannerResultItem &index, const QVariant &value, int role = Qt::EditRole);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    void updateCount ();
    void itemStatusChanged (ScannerResultItem* it, ScannerResultItem::Status s);

private:
    const int                                           mBackgroundR = 235;
    const int                                           mBackgroundG = 241;
    const int                                           mBackgroundB = 248;

    // 当前界面显示的数据index
    int                                                 mCurIndex = 0;

    // 数据状态记录
    //"任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%5), 删除: (%5)");
    int                                                 mNoFix = 0;
    int                                                 mDelete = 0;
    int                                                 mMisReport = 0;

    ScanResultHelper*                                   mScanResultHelper;
    
    // FIXME:// 不应该释放Item内存，这块需要用 智能指针 优化
    QList<ScannerResultItem*>                           mData;
    QMap<const ScannerResultItem*, QPair<int, int>>     mChangedItem;

    // 数据库改变、加载数据线程
    // NOTE:// 暂时没有使用，把 model 放到线程里，图形界面更新会有问题
    QThread*                                            mThread;

    // NOTE:// 暂时没有生效
    // 锁定 mData、mChangedItem、mNoFix、mDelete、mMisReport
    QMutex                                              mLocker;
};

#endif // SCANNERRESULTMODEL_H
