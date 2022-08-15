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
    static QTextStream* io = nullptr;
    if (!init) {
        const char* filePath = nullptr;
        if (QFile::exists(FILE1)) {
            filePath = FILE1;
        } else if (QFile::exists(FILE2)) {
            filePath = FILE2;
        }

        if (filePath) {
            io = new QTextStream(filePath);
            init = true;
        }
    }

    while (!file_lock());
    if (io) {
        QString str = io->readAll();
        while (!file_unlock());
        return (nullptr != str && !str.isNull() && !str.isEmpty() && "" != str) ? str : "";
    }
    while (!file_unlock());

    return "";
}
