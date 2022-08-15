//
// Created by dingjing on 8/15/22.
//

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "scan-status-helper.h"

#include "tools.h"

#define FILE1           "/dev/shm/task_scanning_info.txt"
#define FILE2           "/usr/local/ultrasec/dat/task_scanning_info.txt"

static bool init = false;

QString ScanStatusHelper::getStatusString()
{
    while (!file_lock());

    if (QFile::exists(FILE1)) {
        QFile f(FILE1);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream io(&f);
        QString str = io.readAll();
        f.close();
        while (!file_unlock());
        return (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) ? str : "";
    } else if (QFile::exists(FILE2)) {
        QFile f(FILE2);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream io(&f);
        QString str = io.readAll();
        f.close();
        while (!file_unlock());
        return (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) ? str : "";
    }

    while (!file_unlock());

    return "";
}
