//
// Created by dingjing on 8/16/22.
//

#include "event-loop.h"

#include <QApplication>

int EventLoop::exec()
{
    if (qApp->thread() == QThread::currentThread()) {
        qApp->installEventFilter(this);
        mUIThread = true;
    }

    return QEventLoop::exec(QEventLoop::AllEvents);
}

bool EventLoop::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress
        || QEvent::MouseButtonRelease == event->type()
        || QEvent::MouseButtonDblClick == event->type()) {
        return true;
    }

    return false;
}

EventLoop::EventLoop(QObject *parent)
    : QEventLoop(parent)
{


}

void EventLoop::deleteLater()
{
    if (mUIThread) {
        qApp->removeEventFilter(this);
    }

    return QEventLoop::deleteLater();
}
