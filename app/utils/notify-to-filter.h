#ifndef NOTIFYTOFILTER_H
#define NOTIFYTOFILTER_H

#include <QObject>

class QLocalSocket;

class NotifyToFilter : public QObject
{
    Q_OBJECT
public:
    static NotifyToFilter* getInstance ();

    bool sendData (std::string data);

private:
    explicit NotifyToFilter (QObject *parent = nullptr);
    NotifyToFilter (NotifyToFilter&) = delete;
    ~NotifyToFilter ();


private:
    static NotifyToFilter*          gInstance;
    QLocalSocket*                   mSocket;
};

#endif // NOTIFYTOFILTER_H
