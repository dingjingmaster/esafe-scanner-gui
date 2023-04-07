#include "tools.h"
#include <stdio.h>

#include <QDebug>
#include <QLockFile>
#include <QtWidgets/QApplication>

#define LOCK_FILE1      "/dev/shm/.est.db.lock"
#define LOCK_FILE2      "/usr/local/ultrasec/dat/.est.db.lock"
static QLockFile* f1 = nullptr;
//static FILE* fl = NULL;
const char* lockFile1 = NULL;

#define LOCK_FILE3      "/dev/shm/.est.runingTask.lock"
#define LOCK_FILE4      "/usr/local/ultrasec/dat/.est.runingTask.lock"
//static FILE* f2 = NULL;
static QLockFile* f2 = nullptr;
const char* lockFile2 = NULL;

void init_lock_file ();
void init_lock_file2 ();

bool sqlite_lock()
{
    init_lock_file ();

    if (!f1) {
        if (lockFile1) {
            f1 = new QLockFile(lockFile1);
            if (!f1) {
                qDebug() << "fopen error!";
                exit(-1);
            }
        }
    }

    if (f1 && f1->lock()) {
        return true;
    }

    return false;
}

bool sqlite_unlock()
{
    init_lock_file ();

    if (!f1) return true;

    if (f1 && f1->isLocked()) {
        f1->unlock();
        return true;
    }

    return false;
}

bool file_lock()
{
    init_lock_file2 ();

    if (!lockFile2) {
        qWarning() << "logfile error";
        exit(-1);
    }

    if (!f2) {
        f2 = new QLockFile(lockFile2);
        if (!f2) {
            qDebug() << "fopen error!";
            exit(-1);
        }
    }

    if (f2->lock()) {
        return true;
    }

    return false;
}

bool file_unlock()
{
    init_lock_file2 ();

    if (!f2) return true;

    if (f2->isLocked()) {
        f2->unlock();
        return true;
    }

    return false;
}

void init_lock_file ()
{
    if (!access (LOCK_FILE1, F_OK)) {
        lockFile1 = LOCK_FILE1;
    } else if (!access(LOCK_FILE2, F_OK)) {
        lockFile1 = LOCK_FILE2;
    } else {
        qDebug() << "lock file not exists!";
        exit (-1);
#if 0
        if (creat (LOCK_FILE, S_IRWXU | S_IRWXG | S_IRWXO)) {
            qDebug() << "creat()failed!";
            exit(-1);
        }
#endif
    }
}

void init_lock_file2 ()
{
    if (!access (LOCK_FILE3, F_OK)) {
        lockFile2 = LOCK_FILE3;
    } else if (!access(LOCK_FILE4, F_OK)) {
        lockFile2 = LOCK_FILE4;
    } else {
        qDebug() << "lock file not exists!";
        exit (-1);
#if 0
        if (creat (LOCK_FILE, S_IRWXU | S_IRWXG | S_IRWXO)) {
            qDebug() << "creat()failed!";
            exit(-1);
        }
#endif
    }
}
