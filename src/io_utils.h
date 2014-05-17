#ifndef IO_UTILS_H
#define IO_UTILS_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

// Returns true if file exists
bool file_exists(const char* name)
{
    struct stat buffer;
    return (stat (name, &buffer) == 0);
}

// Execute first parameter as a command and returns the captured stdout as an std::string.
// If second parameter equals true stderr will also be captured, this effectively appends " 2>&1" to the command.
std::string execute(const std::string& cmd, bool getStdErr = true) {
    std::string toExec = cmd;
    std::string data;
    FILE * stream;
    const int bufferSize = 256;
    char buffer[bufferSize];
    if(getStdErr){
        toExec.append(" 2>&1");
    }
    stream = popen(toExec.c_str(), "r");
    if (stream) {
        while (!feof(stream)){
            if (fgets(buffer, bufferSize, stream) != NULL){
                data.append(buffer);
            }
        }
        pclose(stream);
    }
    return data;
}

#endif
