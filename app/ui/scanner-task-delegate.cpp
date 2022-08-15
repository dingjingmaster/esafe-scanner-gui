#include "scanner-task-delegate.h"

#include <QStyle>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QItemDelegate>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

ScannerTaskDelegate::ScannerTaskDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QSize ScannerTaskDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    return size;
}

void ScannerTaskDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    p->save();

    QString text = index.model()->data(index).toString();
    QRect rect = option.rect;
    Qt::Alignment align = option.displayAlignment;

    switch (index.column()) {
    case 0:
    case 2:
    case 3:
    case 4:
        align |= Qt::AlignHCenter;
        break;
    case 5: {
#if 0
        QStyleOptionViewItemV4 opV4 = option;
        initStyleOption (&opV4, index);
        QStyle* style = opV4.widget ? opV4.widget->style () : QApplication::style();
        QTextDocument doc;
        doc.setHtml(opV4.text);
        opV4.text = QString();
        style->drawControl (QStyle::CE_ItemViewItem, &opV4, p);
        QAbstractTextDocumentLayout::PaintContext ctx;
        if (opV4.state & QStyle::State_Selected)
            ctx.palette.setColor (QPalette::Text, opV4.palette.color (QPalette::Active, QPalette::HighlightedText));
        QRect textRect = style->subElementRect (QStyle::SE_ItemViewItemText, &opV4);
        p->save ();
        p->translate (textRect.topLeft ());
        p->setClipRect (textRect.translated (-textRect.topLeft ()));
        doc.documentLayout ()->draw (p, ctx);
        p->restore ();
        p->restore ();
        return;
#endif
        align |= (Qt::AlignHCenter | Qt::AlignVCenter);
        break;
    }
    case 6:
        p->setPen(Qt::blue);
        align |= Qt::AlignHCenter;
        break;
    default:
        break;
    }

    if (nullptr != text && !text.isNull() && !text.isEmpty())  p->drawText(rect, align, text);

    p->restore();
}
