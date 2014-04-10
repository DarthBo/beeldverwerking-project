#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <sys/stat.h>

// Returns true if file exists
bool exists(const char* name)
{
    struct stat buffer;
    return (stat (name, &buffer) == 0);
}

#endif
