//
// Created by dingjing on 23-6-5.
//

#include "configure.h"

#include <glib.h>

#include <QSettings>

#define ULTRASEC_CONFIG_DAEMON      "/usr/local/ultrasec/dat/daemon_conf.ini"

Configure* Configure::gInstance = nullptr;

Configure *Configure::getInstance()
{
    static gsize iv = 0;

    if (g_once_init_enter(&iv)) {
        gInstance = new Configure;
        g_once_init_leave(&iv, 1);
    }

    return gInstance;
}

QString Configure::misreportPrompt()
{
    QString str = "是否确定为例外文件？";

    QSettings set(ULTRASEC_CONFIG_DAEMON, QSettings::IniFormat);

    const char* key = "Login_Setting/clientExceptionPrompt";

    if (set.contains (key)) {
        QString sk = set.value (key).toString();
        if (!sk.isNull() && !sk.isEmpty()) {
            str = sk;
        }
    }

    return str;
}

QString Configure::dsmPrompt()
{
    QString str = "是否确定制作授权加密文件？";

    QSettings set(ULTRASEC_CONFIG_DAEMON, QSettings::IniFormat);

    const char* key = "Login_Setting/clientEncryptionPrompt";

    if (set.contains (key)) {
        QString sk = set.value (key).toString();
        if (!sk.isNull() && !sk.isEmpty()) {
            str = sk;
        }
    }

    return str;
}
