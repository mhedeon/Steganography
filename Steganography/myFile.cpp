#include "myFile.hpp"
#include "logger.hpp"

myFile::myFile(const char *filePath, Mode mode): internalError(false), name(0), path(0), size(0)
{
    if (!myFile::ifFileExist(filePath))
    {
        char buff[100] = { 0 };

        snprintf(buff, sizeof(buff), "Cannot create myFile obj correctly. File \"%s\" doesn't exist", filePath);
        LOG(LOG_ERR, buff);

        internalError = true;
    }
    else
    {
        FILE *file;

        path = (char *)malloc(sizeof(char) * (strlen(filePath) + 1));
        strcpy(path, filePath);
        
        // +1 position to avoid '\' at the start of fileName
        name = strrchr(path, '\\') + 1;

        // +1 position to avoid '.' at the start of fileName
        ext = strrchr(path, '.') + 1;
        
        if (mode == MODE_READ)
            file = fopen(path, "rb");
        else
            file = fopen(path, "wb");

        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        fclose(file);
    }
}

myFile::~myFile()
{
    free(path);
}

char * myFile::getExt() const
{
    if (internalError)
    {
        LOG(LOG_ERR, "InternalError");
    }

    return ext;
}

char * myFile::getName() const
{
    if (internalError)
    {
        LOG(LOG_ERR, "InternalError");
    }

    return name;
}

char * myFile::getPath() const
{
    if (internalError)
    {
        LOG(LOG_ERR, "InternalError");
    }

    return path;
}

uint32_t myFile::getSize() const
{
    if (internalError)
    {
        LOG(LOG_ERR, "InternalError");
    }

    return size;
}


bool myFile::ifFileExist(const char *filePath)
{
    FILE* fileToCheck = NULL;

	if ((fileToCheck = fopen(filePath, "rb")) == NULL) {
		return false;
	}
	fclose(fileToCheck);
	
	return true;
}

uint32_t myFile::getFileSize(const char *filePath)
{
    FILE* fileToCheck = NULL;
    uint32_t fileToCheckSize = 0;

    if (myFile::ifFileExist(filePath))
    {
        fileToCheck = fopen(filePath, "rb");

        fseek(fileToCheck, 0L, SEEK_END);
        fileToCheckSize = ftell(fileToCheck);
        fseek(fileToCheck, 0L, SEEK_SET);

        fclose(fileToCheck);
    }

    return fileToCheckSize;
}
