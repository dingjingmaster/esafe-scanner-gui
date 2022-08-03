#include "tools.h"
#include <stdio.h>

#include <QDebug>

#define LOCK_FILE   "/usr/local/ultrasec/dat/.est.lock"

static FILE* fl = NULL;

void init_lock_file ();

bool sqlite_lock()
{
    init_lock_file ();

    if (!fl) {
        fl = fopen (LOCK_FILE, "a+");
        if (!fl) {
            qDebug() << "fopen error!";
            exit(-1);
        }
    }

    if (flock (fl->_fileno, LOCK_EX | LOCK_NB) == 0) {
        qDebug() << "locked";
        return true;
    }

    return false;
}


bool sqlite_unlock()
{
    init_lock_file ();

    if (fl) {
        if (flock (fl->_fileno, LOCK_UN) == 0) {
            qDebug() << "unlocked\n";
            return true;
        }
    } else {
        return true;
    }

    return false;
}

void init_lock_file ()
{
    if (access (LOCK_FILE, F_OK)) {
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

