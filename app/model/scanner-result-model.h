#ifndef SCANNERRESULTMODEL_H
#define SCANNERRESULTMODEL_H

#include <QAbstractTableModel>

class ScanResultHelper;
class ScannerResultItem;

class ScannerResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { TaskName = 1, FileName, Status, CreateTime, ModifyTime, EnumSize };
public:
    explicit ScannerResultModel(QObject *parent = nullptr);

    void addItem (ScannerResultItem* item);
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

    //
    QList<ScannerResultItem*>   mData;
    ScanResultHelper*           mScanResultHelper;
};

#endif // SCANNERRESULTMODEL_H
