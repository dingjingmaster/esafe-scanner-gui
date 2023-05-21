#include "notify-to-filter.h"

#include <QLocalSocket>

void notify_policy_filter (ScannerResultItem::Status status)
{
    typedef struct _Message
    {
        unsigned long messageType;        // 即消息索引号
        unsigned long messageLength;             // 消息头后面消息体长度
        char data[12];
    } Message;

    static const char* socket = "/usr/local/ultrasec/start/ultrasec_pf.sock";

    QLocalSocket sock;
    Message msg = {
        .messageType = 10039,
        .messageLength = (unsigned long)((status == ScannerResultItem::Deleted) ? 6 : 9),
    };

    if (ScannerResultItem::Deleted == status) {
        strncpy (msg.data, "delete", 6);
    } else {
        strncpy (msg.data, "misreport", 9);
    }

    for (int i = 3; i > 0; --i) {
        sock.connectToServer (socket);
        if (sock.waitForConnected (100)) {
            if (!sock.isValid()) {
                qWarning() << "error: " << sock.error();
            } else {
                qWarning() << "Start write";
                sock.write ((const char*) &msg, sizeof (Message));
                sock.flush();
                qWarning() << "Start end";
                break;
            }
        }
        else {
            qWarning() << "timeout";
        }
    }

    sock.close();
}
