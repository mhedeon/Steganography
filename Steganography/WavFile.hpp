#pragma once

#include <cstdint>
#include <stdbool.h>

#define WAV_SUCCESS 0
#define WAV_ERROR 1
#define WAV_HEADER_SIZE 44

struct s_wavHeader {
	char ChunkID[4];
	uint32_t ChunkSize;
	char Format[4];
	char Subchunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	uint32_t listSize; //if it present in header - add +8 bytes to skip whole LIST subchunk
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;
};

struct s_myHeader {
	uint32_t fileSize; // in bytes
	uint32_t nameSize; // in bytes
	char *name;

	// Doesn't write into file.
	// Need just for calculates while hiding.
	// (sizeof(fileSize) + sizeof(nameSize) + nameSize)
	uint32_t headerSize;
};

class WavFile {
private:
	struct s_wavHeader wavHeader = { 0 };
	struct s_myHeader myHeader = { 0 };
	
	bool internalError;
	
	uint32_t possibleBytesToHideCount;
	uint32_t step;

	char readHiddenByte(FILE **file);
	void hideByte(FILE **fileContainer, FILE **fileResult, char byte);
	void prepareHeader(char* childfile);
	void writeHeader(FILE **parFile, FILE **outFile);
	void readHeader(FILE **parFile);
	int readWavHeader(const char* filename);									// read WAVE Header
	int checkFilesForHiding(char* parentfile, char* childfile);		// check files
public:
	WavFile(const char* filename);
	~WavFile();

	void printFileInfo();											// print header variables
	int encryptFile(char* parentfile, char* childfile, char* outputfile);
	int decryptFile(char* encFilePath);
};
