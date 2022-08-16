//
// Created by dingjing on 8/16/22.
//

#ifndef DSIP_SIT_EVENTLOOP_H
#define DSIP_SIT_EVENTLOOP_H

#include <QEvent>
#include <QThread>
#include <QEventLoop>

class EventLoop : public QEventLoop
{
    Q_OBJECT
public:
    explicit EventLoop (QObject* parent = nullptr);
    void deleteLater();

    int exec();

    bool eventFilter (QObject* watched, QEvent* event) override;

private:
    bool        mUIThread = false;
};


#endif //DSIP_SIT_EVENTLOOP_H
