#include "scanner-task-delegate.h"

#include <QDebug>
#include <QItemDelegate>
#include <QPainter>

ScannerTaskDelegate::ScannerTaskDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerTaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    qDebug() << "delegate size hint";

    return size;
}

void ScannerTaskDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    qDebug() << "delegate paint";

    QString text = index.model()->data(index).toString();
    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;

    switch (index.column()) {
    case 0:
    case 1:
    case 2:
    case 6:
        align |= Qt::AlignHCenter;
        break;
    default:
        break;
    }

    p->drawText(rect, align, text);
}
