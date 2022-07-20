#ifndef SCANNERTASKDELEGATE_H
#define SCANNERTASKDELEGATE_H

#include <QStyledItemDelegate>

class QPainter;

class ScannerTaskDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ScannerTaskDelegate(QObject *parent = nullptr);

public:
    QSize sizeHint (const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint (QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // SCANNERPROGRESSDELEGATE_H
