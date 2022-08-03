#ifndef CTOOLS_H
#define CTOOLS_H

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/file.h>

bool sqlite_lock();
bool sqlite_unlock();


#endif // CTOOLS_H
