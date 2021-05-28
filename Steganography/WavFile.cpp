#include "header.hpp"
#include "WavFile.hpp"
#include "myFile.hpp"

#ifndef __WavFile
#define __WavFile
#endif

//class contructor
WavFile::WavFile(const char* filename)
{
	memset(&wavHeader, 0, sizeof(s_wavHeader));
	memset(&myHeader, 0, sizeof(s_myHeader));

	if (myFile::ifFileExist(filename))
	{
		readWavHeader(filename);
	}
}

WavFile::~WavFile()
{
	free(myHeader.name);
}

int WavFile::readWavHeader(const char* filename)
{
	int res = WAV_ERROR;

	FILE* wavFilename = fopen(filename, "rb");

	fread((char*)&(wavHeader.ChunkID), 4, 1, wavFilename);
	fread((uint32_t*)&(wavHeader.ChunkSize), 4, 1, wavFilename);
	fread((char*)&(wavHeader.Format), 4, 1, wavFilename);
	fread((char*)&(wavHeader.Subchunk1ID), 4, 1, wavFilename);
	fread((uint32_t*)&(wavHeader.Subchunk1Size), 4, 1, wavFilename);
	fread((uint16_t*)&(wavHeader.AudioFormat), 2, 1, wavFilename);
	fread((uint16_t*)&(wavHeader.NumChannels), 2, 1, wavFilename);
	fread((uint32_t*)&(wavHeader.SampleRate), 4, 1, wavFilename);
	fread((uint32_t*)&(wavHeader.ByteRate), 4, 1, wavFilename);
	fread((uint16_t*)&(wavHeader.BlockAlign), 2, 1, wavFilename);
	fread((uint16_t*)&(wavHeader.BitsPerSample), 2, 1, wavFilename);
	fread((char*)&(wavHeader.Subchunk2ID), 4, 1, wavFilename);
	fread((uint32_t*)&(wavHeader.Subchunk2Size), 4, 1, wavFilename);
	if (!strncmp(wavHeader.Subchunk2ID, "LIST", 4))
	{
		wavHeader.listSize = wavHeader.Subchunk2Size;
		fseek(wavFilename, wavHeader.listSize, SEEK_CUR);
		wavHeader.listSize += 8; //adding lost SubChunkID and SubChunkSize size

		fread((char*)&(wavHeader.Subchunk2ID), 4, 1, wavFilename);
		fread((uint32_t*)&(wavHeader.Subchunk2Size), 4, 1, wavFilename);
		if (strncmp(wavHeader.Subchunk2ID, "data", 4))
		{
			wavHeader.Subchunk2Size = 0;
			std::cout << __FUNCTION__ << "(): " << __LINE__ << ": " << "Unknown DATA subchunk(" << wavHeader.Subchunk2ID << ")." << std::endl;
			
			res = WAV_ERROR;
		}
	}

	res = WAV_SUCCESS;
	fclose(wavFilename);

	return res; 
}

int WavFile::printFileInfo()
{
	cout << "RIFF             : " << wavHeader.ChunkID[0] << wavHeader.ChunkID[1] << wavHeader.ChunkID[2] << wavHeader.ChunkID[3] << endl;
	cout << "Riff Chuck       : " << wavHeader.ChunkSize << endl;
	cout << "Format           : " << wavHeader.Format[0] << wavHeader.Format[1] << wavHeader.Format[2] << wavHeader.Format[3] << endl;
	cout << "Fmt              : " << wavHeader.Subchunk1ID[0] << wavHeader.Subchunk1ID[1] << wavHeader.Subchunk1ID[2] << wavHeader.Subchunk1ID[3] << endl;
	cout << "Fmt chunk        : " << wavHeader.Subchunk1Size << endl;
	cout << "Audio format(PCM): " << wavHeader.AudioFormat << endl;
	cout << "Channels         : " << wavHeader.NumChannels << endl;
	cout << "Sample Rate      : " << wavHeader.SampleRate << endl;
	cout << "Bytes per sec    : " << wavHeader.ByteRate << endl;
	cout << "Bytes block      : " << wavHeader.BlockAlign << endl;
	cout << "Bits per sample  : " << wavHeader.BitsPerSample << endl;
	if (!!wavHeader.listSize)
	{
		cout << "List chunk       : LIST" << std::endl;
		cout << "Chunk size       : " << wavHeader.listSize << std::endl;
	}
	cout << "Data chuck       : " << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
	cout << "Chunk size       : " << wavHeader.Subchunk2Size << endl;

	return 0;
}

int WavFile::checkFilesForHiding(char* parentfile, char* childfile)
{
	FILE* tfile, * pfile;
	unsigned long t = 0, p = 0;

	// check and Initialize files...
	// read txt file and read wav file length...
	if (!myFile::ifFileExist(parentfile))
	{
		throw("WAV file doesn't exist");
		return -1;
	}

	if (!myFile::ifFileExist(childfile))
	{
		throw("txt file doesn't exist");
		return -1;
	}

	tfile = fopen(childfile, "rb");
	while (!feof(tfile))					// read the length of txt file...
	{
		fgetc(tfile);
		t++;
	}
	fclose(tfile);

	pfile = fopen(parentfile, "rb");
	while (!feof(pfile))					// read the length of wav file...
	{
		fgetc(pfile);
		p++;
	}
	fclose(pfile);

	wavDataSize = p - WAV_HEADER_SIZE;
	if (t > wavDataSize)
	{
		throw "TXT filesize is greater than WAVE file...";
		return -1;
	}

	return 0;
}

void WavFile::prepareHeader(char* childfile)
{
	myFile temp(childfile, MODE_READ);
	
	myHeader.fileSize = temp.size;
	myHeader.nameSize = strlen(temp.name) + 1;
	std::cout << __FUNCTION__ << "(): " << myHeader.nameSize << "|" << std::endl;
	myHeader.name = (char *)malloc(myHeader.nameSize);
	memset(myHeader.name, 0, myHeader.nameSize);
	strcpy(myHeader.name, temp.name);
	std::cout << "|" << myHeader.name << "|" << std::endl;

	// myHeader.headerSize = sizeof(myHeader.fileSize);

	std::cout << "fileSize: " << myHeader.fileSize << std::endl;
}

void WavFile::writeHeader(FILE **parFile, FILE **outFile)
{
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
}

char WavFile::readHiddenByte(FILE **file)
{
	char byte = 0, temp = 0;

	for (int i = 0; i < 8; i++, temp = 0)
	{
		temp = fgetc(*file);
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
	char fileByte = 0;
	for (int bit = 0; bit < 8; bit++)
	{
		fileByte = fgetc(*fileContainer);
		fileByte = (fileByte & 0xFE) | ((char)((byte >> bit) & 1));
		fputc(fileByte, *fileResult);

		for (int skipBytes = 0; skipBytes < wavHeader.BitsPerSample / 8 - 1; skipBytes++)
		{
			fputc(fgetc(*fileContainer), *fileResult);
		}
	}
}

int WavFile::encryptFile(char* contFilePath, char* binFilePath, char* outFilePath)
{
	FILE *contFile = NULL, *binFile = NULL, *outFile = NULL;
	unsigned char header[WAV_HEADER_SIZE];

	// check and Initialize parent & txt files...
	if (checkFilesForHiding(contFilePath, binFilePath) == -1)
	{
		throw ("error!, initialization failed...");
		return -1;
	}

	contFile = fopen(contFilePath, "rb");
	binFile = fopen(binFilePath, "rb");
	outFile = fopen(outFilePath, "w+b");

	fread(header, WAV_HEADER_SIZE, 1, contFile);		// read WAV header
	fwrite(header, WAV_HEADER_SIZE, 1, outFile);		// write WAV header
	
	//for case of LIST presence. loop here, bcs I don't know length of LIST chunks
	for (uint32_t i = 0; i < wavHeader.listSize; i++)
	{
		fputc(fgetc(contFile), outFile);
	}

	prepareHeader(binFilePath);
	writeHeader(&contFile, &outFile);

	// main hiding process
	while (!feof(binFile))
	{
		hideByte(&contFile, &outFile, fgetc(binFile));
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

	return 0;
}

int WavFile::decryptFile(char* encFilePath)
{
	FILE *encFile = NULL, *tfile = NULL;

	encFile = fopen(encFilePath, "rb");

	// skip WAV header
	fseek(encFile, WAV_HEADER_SIZE + wavHeader.listSize, SEEK_SET);

	readHeader(&encFile);
	tfile = fopen(myHeader.name, "w+b");

	uint32_t i = 0;
	while (!feof(encFile) && i < myHeader.fileSize)
	{
		fputc(readHiddenByte(&encFile), tfile);
		i++;
	}

	fclose(encFile);
	fclose(tfile);
	return 0;
}
