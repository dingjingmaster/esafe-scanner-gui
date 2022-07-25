#include "scanner-result-delegate.h"

#include <QDebug>
#include <QPainter>
#include <QApplication>

ScannerResultDelegate::ScannerResultDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerResultDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    return size;
}

void ScannerResultDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    p->save();

    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;
    QPalette pal;

    switch (index.column()) {
    case 0: {
        QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.state |= QStyle::State_Enabled;
        cbOp.state |= index.model()->data(index).toBool() ? QStyle::State_On : QStyle::State_Off;
        cbOp.rect = rectCB;
        p->setBrush(pal.windowText());
        p->setPen(pal.windowText().color());
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);
        break;
    }
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: {
        QString text = index.model()->data(index).toString();
        align |= Qt::AlignHCenter;
        p->setBrush(pal.windowText());
        p->drawText(rect, align, text);
        break;
    }
    default:
        break;
    }

    p->restore();
}
