#include "message-with-fp.pb.h"
#include "notify-to-filter.h"

#include <QLocalSocket>
#include <QMutex>

NotifyToFilter* NotifyToFilter::gInstance = nullptr;

NotifyToFilter *NotifyToFilter::getInstance()
{
    static QMutex lock;

    if (!NotifyToFilter::gInstance) {
        lock.lock();
        if (!NotifyToFilter::gInstance) {
            gInstance = new NotifyToFilter;
        }
        lock.unlock();
    }

    return gInstance;
}

bool NotifyToFilter::sendData(std::string data)
{
    if (QLocalSocket::UnconnectedState == mSocket->state()) {
        qDebug() << "server not connect & connting ...";
        mSocket->connectToServer(LOCAL_SOCKET_NAME);
    }

    bool ret = false;

#if 1
    com::esafenet::scanner::client::ScannerClientMessage msg;
    msg.ParseFromArray(data.c_str(), data.length());
    qDebug() << "debug, send string: " << msg.DebugString().c_str();
#endif

    if (mSocket->waitForConnected()) {
        ret = mSocket->write(data.c_str(), data.length()) > 0;
        mSocket->flush();
        mSocket->close();
    } else {
        qDebug() << "write to server timeout!";
    }

    return ret;
}

NotifyToFilter::NotifyToFilter(QObject *parent)
    : QObject{parent}, mSocket(new QLocalSocket(this))
{
    mSocket->abort();
    mSocket->connectToServer(LOCAL_SOCKET_NAME, QLocalSocket::WriteOnly);
}

NotifyToFilter::~NotifyToFilter()
{
    if (mSocket) {
        mSocket->disconnectFromServer();
        mSocket->deleteLater();
    }

}
