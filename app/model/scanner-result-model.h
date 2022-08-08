#ifndef SCANNERRESULTMODEL_H
#define SCANNERRESULTMODEL_H
#include <QMap>
#include <QPair>
#include <QAbstractTableModel>
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
    QList<const ScannerResultItem*> getSelectedItem ();
    QModelIndex getIndexByItem (const ScannerResultItem* it, int column=0);

    void saveResult();

private:
    void changeItemCount(int status, bool isAdd=true);

public Q_SLOTS:
    void selectAll (bool s);

Q_SIGNALS:
    void clearData ();
    void showData (QString TaskName, QString filterName);

public:
    int rowCount (const QModelIndex& parent = QModelIndex()) const override;
    int columnCount (const QModelIndex& parent = QModelIndex()) const override;

    QVariant data (const QModelIndex& index, int role=Qt::DisplayRole) const override;
    QVariant headerData (int section, Qt::Orientation orentation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    const int                                           mBackgroundR = 235;
    const int                                           mBackgroundG = 241;
    const int                                           mBackgroundB = 248;
    
    // 数据状态记录
    //"任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%5), 删除: (%5)");
    int                                                 mNoFix = 0;
    int                                                 mDelete = 0;
    int                                                 mMisReport = 0;

    ScanResultHelper*                                   mScanResultHelper;
    
    // FIXME:// 不应该释放Item内存，这块需要用 智能指针 优化
    QList<ScannerResultItem*>                           mData;
    QMap<const ScannerResultItem*, QPair<int, int>>     mChangedItem;
};

#endif // SCANNERRESULTMODEL_H
