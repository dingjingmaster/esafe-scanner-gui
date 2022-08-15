//
// Created by dingjing on 8/15/22.
//

#include <QResizeEvent>
#include <QtCore/QEvent>
#include <QtGui/QPainter>


#include "progress.h"

Progress::Progress(QWidget *parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void Progress::newParent()
{
    if (parent()) {
        parent()->installEventFilter(this);
        raise();
    }
}

bool Progress::eventFilter(QObject *obj, QEvent *ev)
{
    if (parent() == obj) {
        if (QEvent::Resize == ev->type()) {
            resize(static_cast<QResizeEvent*>(ev)->size());
        } else if (QEvent::ChildAdded == ev->type()) {
            raise();
        }
    }

    return QWidget::eventFilter(obj, ev);
}

bool Progress::event(QEvent *ev)
{
    if (QEvent::ParentAboutToChange == ev->type()) {
        if (parent()) {
            parent()->removeEventFilter(this);
        } else if (QEvent::ParentChange == ev->type()) {
            newParent();
        }
    }

    return QWidget::event(ev);
}

void Progress::paintEvent(QPaintEvent *)
{
    QPainter p (this);

    p.fillRect (rect(), {100, 100, 100, 128});
    p.setPen({200, 200, 255});
    p.drawText(rect(), "Loading..........");
}
