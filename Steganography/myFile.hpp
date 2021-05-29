#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <stdbool.h>
#include <string>
#include "logger.hpp"

enum Mode
{
    MODE_READ = 0,
    MODE_WRITE
};

class myFile
{
private:
    bool internalError;
public:
    char *ext;
    char *name;
    char *path;
    uint32_t size;
    

    myFile(const char *filePath, Mode mode);
    ~myFile();
    
    static bool ifFileExist(const char *filePath);
};

