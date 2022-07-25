#include "header-view.h"

#include <QDebug>
#include <QPainter>
#include <QCheckBox>
#include <QMouseEvent>
#include <QApplication>

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    setHighlightSections(false);
    setMouseTracking(true);

    setSectionsClickable(true);

    setSectionResizeMode(QHeaderView::Stretch);
}

bool HeaderView::isChecked() const
{
    return mChecked;
}

void HeaderView::setChecked(bool c)
{
    if (c != mChecked) {
        mChecked = c;
        redrawCheckBox();
    }
}

void HeaderView::paintSection(QPainter *p, const QRect &rect, int logicalIndex) const
{
    p->save();
    QHeaderView::paintSection(p, rect, logicalIndex);
    p->restore();

    if (logicalIndex == 0) {
        QRect rectCB(rect.left() + (rect.width() - 20) / 2, rect.top() + (rect.height() - 20) / 2, 20, 20);
        QStyleOptionButton cbOp;
        cbOp.initFrom(this);
        cbOp.state |= QStyle::State_Enabled;
        cbOp.state |= mChecked ? QStyle::State_On : QStyle::State_Off;
        cbOp.rect = rectCB;
        QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbOp, p);
    }
}

void HeaderView::mousePressEvent(QMouseEvent *e)
{
    setChecked(!isChecked());

    Q_EMIT checkBoxClicked(isChecked());
}

void HeaderView::redrawCheckBox()
{
    viewport()->update();
}
