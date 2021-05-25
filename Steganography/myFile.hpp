#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <stdbool.h>

enum Mode
{
    MODE_READ = 0,
    MODE_WRITE
};

class myFile
{
private:
    char *name;
    char *path;
    FILE *file;
    uint32_t size;
    
public:
    myFile(const char *filePath, Mode mode);
    ~myFile();
    
    static bool ifFileExist(const char *filePath);
};

