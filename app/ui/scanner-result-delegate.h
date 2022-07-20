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
};

#endif // SCANNERRESULTDELEGATE_H
