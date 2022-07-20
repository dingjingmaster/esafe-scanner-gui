#include "scanner-result-delegate.h"

#include <QDebug>
#include <QPainter>

ScannerResultDelegate::ScannerResultDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerResultDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    qDebug() << "delegate size hint";

    return size;
}

void ScannerResultDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    qDebug() << "delegate paint";

    QString text = index.model()->data(index).toString();
    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;

    switch (index.column()) {
    case 0:
    case 1:
    case 3:
        align |= Qt::AlignHCenter;
        break;
    default:
        break;
    }

    p->drawText(rect, align, text);
}
