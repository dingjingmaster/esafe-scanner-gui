#ifndef CTOOLS_H
#define CTOOLS_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

bool sqlite_lock();
bool sqlite_unlock();

// file
bool file_lock ();
bool file_unlock ();


#endif // CTOOLS_H
