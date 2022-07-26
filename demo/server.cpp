#include <iostream>

#include <QDebug>
#include <QLocalSocket>
#include <QLocalServer>
#include <QCoreApplication>

#include "../app/utils/message-with-fp.pb.h"

int main (int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    QLocalServer* server = new QLocalServer;
    server->setSocketOptions(QLocalServer::WorldAccessOption);

    server->connect (server, &QLocalServer::newConnection, [server] () {
        QLocalSocket* socket = server->nextPendingConnection();
        socket->connect (socket, &QLocalSocket::readyRead, [=] () {
            if (!socket) {
                return;
            }

            char byte[20480] = {0};
            int size = socket->read(byte, sizeof byte);

            qDebug() << "=====================================";

            using namespace com::esafenet::scanner::client;
            ScannerClientMessage scan;
            if (scan.ParsePartialFromArray(byte, size)) {
                qDebug() << "debug string: " << scan.DebugString().c_str();
                qDebug() << "id: " << scan.id();
                qDebug() << "op: " << scan.operation();
                qDebug() << "file name: " << scan.filename().c_str();
            } else {
                qDebug() << "解析失败!";
            }
            qDebug() << "=====================================\n\n";

        });

        socket->connect(socket, &QLocalSocket::stateChanged, [=] (QLocalSocket::LocalSocketState socketState) {
            if (QLocalSocket::UnconnectedState == socketState || QLocalSocket::ClosingState == socketState) {
                socket->deleteLater();
            }
        });
    });


    server->listen(LOCAL_SOCKET_NAME);

    int ret = app.exec();

    google::protobuf::ShutdownProtobufLibrary();

    return ret;
}
