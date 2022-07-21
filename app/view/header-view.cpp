#include "header-view.h"

#include <QCheckBox>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

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

//void HeaderView::onStateChanged(int state)
//{
//    if (Qt::PartiallyChecked == state) {
//        mTristate = true;
//        mNoChange = true;
//    } else {
//        mNoChange = false;
//    }

//    mChecked = (state != Qt::Unchecked);

//    update();
//}

void HeaderView::paintSection(QPainter *p, const QRect &rect, int logicalIndex) const
{
    p->save();
    QHeaderView::paintSection(p, rect, logicalIndex);
    p->restore();

    return;

    if (logicalIndex == 0) {
        QStyleOptionButton op;
        op.initFrom(this);
//        op.rect = QRect(20, 20, 20, 20);

        op.state = QStyle::State_Enabled | QStyle::State_Active;

        if (mChecked) {
            op.state |= QStyle::State_On;
        } else {
            op.state |= QStyle::State_Off;
        }

//        op.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &op, p);

//        if (mChecked) {
//            op.state |= QStyle::State_Sunken;
//        }

//        if (mTristate && mNoChange) {
//            op.state |= QStyle::State_NoChange;
//        } else {
//            op.state |= mChecked ? QStyle::State_On : QStyle::State_Off;
//        }

//        if (testAttribute(Qt::WA_Hover) && underMouse()) {
//            if (mMoving) {
//                op.state |= QStyle::State_MouseOver;
//            } else {
//                op.state &= ~QStyle::State_MouseOver;
//            }
//        }

//        QCheckBox cb;
//        op.iconSize = QSize (20, 20);
//        op.rect = rect;

//        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &op, p, &cb);
    }
}

//bool HeaderView::event(QEvent *e)
//{
//    if (QEvent::Enter == e->type() || QEvent::Leave == e->type()) {
//        QMouseEvent* ee = static_cast<QMouseEvent*>(e);
//        int c = logicalIndexAt(ee->x());
//        if (0 == c) {
//            mMoving = (e->type() == QEvent::Enter);
//            update();

//            return true;
//        }
//    }

//    return QHeaderView::event(e);
//}

void HeaderView::mousePressEvent(QMouseEvent *e)
{
    setChecked(!isChecked());

    Q_EMIT checkBoxClicked(isChecked());
//    int c = logicalIndexAt (e->pos());

//    if ((e->buttons() & Qt::LeftButton) && (0 == c)) {
//        mPressed = true;
//    } else {
//        QHeaderView::mousePressEvent(e);
//    }
}

void HeaderView::redrawCheckBox()
{
    viewport()->update();
}

//void HeaderView::mouseReleaseEvent(QMouseEvent *e)
//{
//    if (mPressed) {
//        if (mTristate && mNoChange) {
//            mChecked = true;
//            mNoChange = false;
//        } else {
//            mChecked = !mChecked;
//        }

//        update ();

//        Qt::CheckState state = mChecked ? Qt::Checked : Qt::Unchecked;

//        Q_EMIT stateChanged(state);
//    } else {
//        QHeaderView::mouseReleaseEvent(e);
//    }

//    mPressed = false;
//}
