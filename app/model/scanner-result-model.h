#ifndef SCANNERRESULTMODEL_H
#define SCANNERRESULTMODEL_H

#include <QAbstractTableModel>

class ScanResultHelper;
class ScannerResultItem;

class ScannerResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { /*TaskName = 1,*/ FileName = 1, Status, CreateTime, ModifyTime, EnumSize };
public:
    explicit ScannerResultModel(QObject *parent = nullptr);

    bool hasChecked ();
    bool isCheckAllItems ();
    void addItem (ScannerResultItem* item);
    void delItem (ScannerResultItem* item);
    
    int getNoFixCount ();
    int getDeleteCount ();
    int getMisReportCount ();
    int getAllCount ();
    
    QModelIndex getIndexByItem (const ScannerResultItem* it);
    
    QList<const ScannerResultItem*> getSelectedItem ();

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

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags (const QModelIndex& index) const override;

    bool insertRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows (int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    const int                   mBackgroundR = 235;
    const int                   mBackgroundG = 241;
    const int                   mBackgroundB = 248;
    
    // 数据状态记录
    //"任务名称: (%1), 总条数: (%2), 未处理: (%3), 误报: (%5), 删除: (%5)");
    int                         mNoFix = 0;
    int                         mDelete = 0;
    int                         mMisReport = 0;

    //
    QList<ScannerResultItem*>   mData;
    ScanResultHelper*           mScanResultHelper;
};

#endif // SCANNERRESULTMODEL_H
