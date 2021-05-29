#include <iostream>
#include <cstdlib>
#include <cstring>
#include "WavFile.hpp"
#include "myFile.hpp"
#include "logger.hpp"

WavFile::WavFile(const char* filePath, uint16_t key): internalError(false), possibleBytesToHideCount(0), step(0), lfsr(NULL)
{
	memset(&wavHeader, 0, sizeof(s_wavHeader));
	memset(&myHeader, 0, sizeof(s_myHeader));

	if (!myFile::ifFileExist(filePath))
	{
		char buff[100] = { 0 };

        snprintf(buff, sizeof(buff), "Cannot create WavFile obj correctly. File \"%s\" doesn't exist", filePath);
        LOG(LOG_ERR, buff);

        internalError = true;
	}
	else if (readWavHeader(filePath) != WAV_SUCCESS)
	{
		LOG(LOG_ERR, "Cannot create WavFile obj correctly. Internal ERROR in readWavHeader()");

		internalError = true;
	}
	
	lfsr = new LFSR(key);
}

WavFile::~WavFile()
{
	free(myHeader.name);
	delete lfsr;
}

int WavFile::readWavHeader(const char* wavFileName)
{
	int res = WAV_ERROR;

	FILE* wavFile = fopen(wavFileName, "rb");

	if (fread((char*)&(wavHeader.ChunkID), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint32_t*)&(wavHeader.ChunkSize), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((char*)&(wavHeader.Format), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((char*)&(wavHeader.Subchunk1ID), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint32_t*)&(wavHeader.Subchunk1Size), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint16_t*)&(wavHeader.AudioFormat), 2, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint16_t*)&(wavHeader.NumChannels), 2, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint32_t*)&(wavHeader.SampleRate), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint32_t*)&(wavHeader.ByteRate), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint16_t*)&(wavHeader.BlockAlign), 2, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint16_t*)&(wavHeader.BitsPerSample), 2, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((char*)&(wavHeader.Subchunk2ID), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
	if (fread((uint32_t*)&(wavHeader.Subchunk2Size), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};

	if (!strncmp(wavHeader.Subchunk2ID, "LIST", 4))
	{
		wavHeader.listSize = wavHeader.Subchunk2Size;
		fseek(wavFile, wavHeader.listSize, SEEK_CUR);
		wavHeader.listSize += 8; //adding lost SubChunkID and SubChunkSize size

		if (fread((char*)&(wavHeader.Subchunk2ID), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};
		if (fread((uint32_t*)&(wavHeader.Subchunk2Size), 4, 1, wavFile) != 1 || ferror(wavFile)) {LOG(LOG_WARN, "qwe"); goto error;};

		if (strncmp(wavHeader.Subchunk2ID, "data", 4))
		{
			wavHeader.Subchunk2Size = 0;
			char buff[100] = { 0 };

			snprintf(buff, sizeof(buff), "Unknown DATA subchunk(%.4s)", wavHeader.Subchunk2ID);
			LOG(LOG_ERR, buff);

			internalError = true;

			res = WAV_ERROR;
		}
	}

	res = WAV_SUCCESS;
	fclose(wavFile);

	return res;

error:
	LOG(LOG_ERR, "InternalError while reading .wav header");
	perror("WavFile::readWavHeader() ERROR");
	internalError = true;
	fclose(wavFile);

	return WAV_ERROR;
}

void WavFile::printFileInfo()
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return;
	}

	std::cout << "RIFF             : " << wavHeader.ChunkID[0] << wavHeader.ChunkID[1] << wavHeader.ChunkID[2] << wavHeader.ChunkID[3] << std::endl;
	std::cout << "Riff Chuck       : " << wavHeader.ChunkSize << std::endl;
	std::cout << "Format           : " << wavHeader.Format[0] << wavHeader.Format[1] << wavHeader.Format[2] << wavHeader.Format[3] << std::endl;
	std::cout << "Fmt              : " << wavHeader.Subchunk1ID[0] << wavHeader.Subchunk1ID[1] << wavHeader.Subchunk1ID[2] << wavHeader.Subchunk1ID[3] << std::endl;
	std::cout << "Fmt chunk        : " << wavHeader.Subchunk1Size << std::endl;
	std::cout << "Audio format(PCM): " << wavHeader.AudioFormat << std::endl;
	std::cout << "Channels         : " << wavHeader.NumChannels << std::endl;
	std::cout << "Sample Rate      : " << wavHeader.SampleRate << std::endl;
	std::cout << "Bytes per sec    : " << wavHeader.ByteRate << std::endl;
	std::cout << "Bytes block      : " << wavHeader.BlockAlign << std::endl;
	std::cout << "Bits per sample  : " << wavHeader.BitsPerSample << std::endl;
	if (!!wavHeader.listSize)
	{
		std::cout << "List chunk       : LIST" << std::endl;
		std::cout << "Chunk size       : " << wavHeader.listSize << std::endl;
	}
	std::cout << "Data chuck       : " << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << std::endl;
	std::cout << "Chunk size       : " << wavHeader.Subchunk2Size << std::endl;

}

int WavFile::checkFilesForHiding(char* wavFilePath, char* binFilePath)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return WAV_ERROR;
	}

	FILE* wavFile = NULL, * binFile = NULL;
	uint32_t wavFileSize = 0, binFileSize = 0;

	if (!myFile::ifFileExist(wavFilePath))
	{
		char buff[100] = { 0 };

        snprintf(buff, sizeof(buff), "File check failed. File \"%s\" doesn't exist", wavFilePath);
        LOG(LOG_ERR, buff);

        internalError = true;

		return WAV_ERROR;
	}

	if (!myFile::ifFileExist(binFilePath))
	{
		char buff[100] = { 0 };

        snprintf(buff, sizeof(buff), "File check failed. File \"%s\" doesn't exist", binFilePath);
        LOG(LOG_ERR, buff);

        internalError = true;

		return WAV_ERROR;
	}

	prepareHeader(binFilePath);

	binFileSize = myFile::getFileSize(binFilePath);

	possibleBytesToHideCount = (wavHeader.Subchunk2Size - (myHeader.headerSize * wavHeader.BitsPerSample)) /
														wavHeader.BitsPerSample;

	if (possibleBytesToHideCount <= binFileSize)
	{
		char buff[100] = { 0 };

        snprintf(buff, sizeof(buff), "Binary file's size is: (%d) bytes, but you can hide (%d) bytes", binFileSize, possibleBytesToHideCount);
		LOG(LOG_WARN, "Binary file is too big to hide it into current .wav file");
        LOG(LOG_WARN, buff);

		internalError = true;
		return WAV_ERROR;
	}
	
	step = possibleBytesToHideCount / binFileSize;
	
	LOG(LOG_INF, wavHeader.Subchunk2Size);
	LOG(LOG_INF, myHeader.headerSize);
	LOG(LOG_INF, wavHeader.BitsPerSample);
	LOG(LOG_INF, possibleBytesToHideCount);
	LOG(LOG_INF, step);

	return WAV_SUCCESS;
}

void WavFile::prepareHeader(char* childfile)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return;
	}

	myFile temp(childfile, MODE_READ);
	
	myHeader.fileSize = temp.getSize();
	myHeader.nameSize = strlen(temp.getName()) + 1;
	std::cout << __FUNCTION__ << "(): " << myHeader.nameSize << "|" << std::endl;
	myHeader.name = (char *)malloc(myHeader.nameSize);
	memset(myHeader.name, 0, myHeader.nameSize);
	strcpy(myHeader.name, temp.getName());
	std::cout << "|" << myHeader.name << "|" << std::endl;

	myHeader.headerSize = sizeof(myHeader.fileSize) + sizeof(myHeader.nameSize) + myHeader.nameSize;

	std::cout << "fileSize: " << myHeader.fileSize << std::endl;
	std::cout << "headerSize: " << myHeader.headerSize << std::endl;
}

void WavFile::writeHeader(FILE **parFile, FILE **outFile)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return;
	}

	// temporary just to test enc/dec with header and size info
	for (uint32_t i = 0; i < sizeof(uint32_t); i++)
	{
		char *headerByte = (char *)&(myHeader.fileSize) + i;
		hideByte(parFile, outFile, *headerByte);
	}
	for (uint32_t i = 0; i < sizeof(uint32_t); i++)
	{
		char *headerByte = (char *)&(myHeader.nameSize) + i;
		hideByte(parFile, outFile, *headerByte);
	}

	for (uint32_t i = 0; i < myHeader.nameSize; i++)
	{
		hideByte(parFile, outFile, myHeader.name[i]);
	}
}

void WavFile::readHeader(FILE **parFile)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return;
	}

	uint32_t temp = 0;
	
	for (uint32_t i = 0; i < sizeof(uint32_t); i++)
	{
		((char *)&(myHeader.fileSize))[i] = readHiddenByte(parFile);
	}
	for (uint32_t i = 0; i < sizeof(uint32_t); i++)
	{
		((char *)&(myHeader.nameSize))[i] = readHiddenByte(parFile);
	}

	std::cout << __FUNCTION__ << "(): " << myHeader.nameSize << "|" << std::endl;

	myHeader.name = (char *)malloc(myHeader.nameSize);
	memset(myHeader.name, 0, myHeader.nameSize);
	for (uint32_t i = 0; i < myHeader.nameSize; i++)
	{
		myHeader.name[i] = readHiddenByte(parFile);
	}
	std::cout << "|" << myHeader.name << "|" << std::endl;

	std::cout << "myHeader.fileSize dec: " << myHeader.fileSize << std::endl;

	myHeader.headerSize = sizeof(myHeader.fileSize) + sizeof(myHeader.nameSize) + myHeader.nameSize;

	possibleBytesToHideCount = (wavHeader.Subchunk2Size - (myHeader.headerSize * wavHeader.BitsPerSample)) /
														wavHeader.BitsPerSample;

	if (possibleBytesToHideCount <= myHeader.fileSize || !possibleBytesToHideCount || !myHeader.fileSize)
	{
		LOG(LOG_WARN, "InternalError. Something wrong with parameters");

		internalError = true;
	}
	
	step = possibleBytesToHideCount / myHeader.fileSize;
	LOG(LOG_INF, wavHeader.Subchunk2Size);
	LOG(LOG_INF, myHeader.headerSize);
	LOG(LOG_INF, wavHeader.BitsPerSample);
	LOG(LOG_INF, possibleBytesToHideCount);
	LOG(LOG_INF, step);
}

char WavFile::readHiddenByte(FILE **file)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return 0;
	}

	char byte = 0, temp = 0;

	for (int i = 0; i < 8; i++, temp = 0)
	{
		temp = fgetc(*file);
		temp = lfsr->getByte(temp);
		byte |= ((temp & 0x1) << i);

		for (int skipBytes = 0; skipBytes < wavHeader.BitsPerSample / 8 - 1; skipBytes++)
		{
			fgetc(*file);
		}
	}
	
	return byte;
}

void WavFile::hideByte(FILE **fileContainer, FILE **fileResult, char byte)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return;
	}

	char fileByte = 0;
	for (int bit = 0; bit < 8; bit++)
	{
		fileByte = fgetc(*fileContainer);
		fileByte = (fileByte & 0xFE) | ((char)((byte >> bit) & 1));
		fputc(lfsr->getByte(fileByte), *fileResult);

		for (int skipBytes = 0; skipBytes < wavHeader.BitsPerSample / 8 - 1; skipBytes++)
		{
			fputc(fgetc(*fileContainer), *fileResult);
		}
	}
}

int WavFile::encryptFile(char* contFilePath, char* binFilePath, char* outFilePath)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return WAV_ERROR;
	}

	FILE *contFile = NULL, *binFile = NULL, *outFile = NULL;
	unsigned char header[WAV_HEADER_SIZE];

	// check and Initialize parent & txt files...
	if (checkFilesForHiding(contFilePath, binFilePath) != WAV_SUCCESS)
	{
		LOG(LOG_ERR, "InternalError");
		return WAV_ERROR;
	}

	contFile = fopen(contFilePath, "rb");
	binFile = fopen(binFilePath, "rb");
	outFile = fopen(outFilePath, "w+b");

	fread(header, WAV_HEADER_SIZE, 1, contFile);		// read WAV header
	fwrite(header, WAV_HEADER_SIZE, 1, outFile);		// write WAV header
	
	//for case of LIST presence
	for (uint32_t i = 0; i < wavHeader.listSize; i++)
	{
		fputc(fgetc(contFile), outFile);
	}

	writeHeader(&contFile, &outFile);

	// main hiding process
	while (!feof(binFile))
	{
		hideByte(&contFile, &outFile, fgetc(binFile));
		if (!feof(binFile))
		{
			for (uint32_t skipByte = 0; skipByte < step * (wavHeader.BitsPerSample / 8); skipByte++)
			{
				fputc(fgetc(contFile), outFile);
			}
		}
	}

	// write remaing wav bytes into the new file.
	while (!feof(contFile))
	{
		fputc(fgetc(contFile), outFile);
	}

	// close all file handlers
	fclose(contFile);
	fclose(binFile);
	fclose(outFile);

	return WAV_SUCCESS;
}

int WavFile::decryptFile(char* encFilePath)
{
	if (internalError)
	{
		LOG(LOG_ERR, "InternalError");
		return WAV_ERROR;
	}

	FILE *encFile = NULL, *binFile = NULL;

	encFile = fopen(encFilePath, "rb");

	// skip WAV header
	fseek(encFile, WAV_HEADER_SIZE + wavHeader.listSize, SEEK_SET);

	readHeader(&encFile);
	binFile = fopen(myHeader.name, "w+b");

	uint32_t i = 0;
	while (!feof(encFile) && i < myHeader.fileSize)
	{
		fputc(readHiddenByte(&encFile), binFile);
		i++;
		for (uint32_t skipByte = 0; skipByte < step * (wavHeader.BitsPerSample / 8); skipByte++)
		{
			fgetc(encFile);
		}
	}

	fclose(encFile);
	fclose(binFile);

	return WAV_SUCCESS;
}
