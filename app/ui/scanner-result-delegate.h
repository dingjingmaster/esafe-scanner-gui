#ifndef SCANNERRESULTDELEGATE_H
#define SCANNERRESULTDELEGATE_H

#include <QStyledItemDelegate>

class QPainter;

class ScannerResultDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ScannerResultDelegate(QObject *parent = nullptr);

public:
    QSize sizeHint (const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint (QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
private:
    QObject*        mObj;
};

#endif // SCANNERRESULTDELEGATE_H
