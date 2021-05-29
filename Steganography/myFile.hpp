#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <stdbool.h>
#include <string>

enum Mode
{
    MODE_READ = 0,
    MODE_WRITE
};

class myFile
{
private:
    bool internalError;
    char *ext;
    char *name;
    char *path;
    uint32_t size;
public:
    myFile(const char *filePath, Mode mode);
    ~myFile();
 
    char * getExt() const;
    char * getName() const;
    char * getPath() const;
    uint32_t getSize() const;
    
    static bool ifFileExist(const char *filePath);
    static uint32_t getFileSize(const char *filePath);
};

