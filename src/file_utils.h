#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// Returns true if file exists
bool exists(const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

#endif
