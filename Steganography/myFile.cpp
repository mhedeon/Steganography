#include "myFile.hpp"

myFile::myFile(const char *filePath, Mode mode): name(0), path(0), file(0), size(0)
{
    if (!myFile::ifFileExist(filePath))
    {
        std::cout << "Cannot create object with file: " << filePath << std::endl;
    }
    else
    {
        path = (char *)malloc(sizeof(char) * (strlen(filePath) + 1));
        strcpy(path, filePath);
        
        // +1 position to avoid '\' at the start of fileName
        name = strrchr(path, '\\') + 1;
        
        if (mode == MODE_READ)
            file = fopen(path, "rb");
        else
            file = fopen(path, "wb");

        fseek(file, 0L, SEEK_END);
        size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        
    }
}

myFile::~myFile()
{
    free(path);
    fclose(file);
}

bool myFile::ifFileExist(const char *filePath)
{
    FILE* fileToCheck;

	if ((fileToCheck = fopen(filePath, "rb")) == NULL) {
		return false;
	}
	fclose(fileToCheck);
	
	return true;
}
