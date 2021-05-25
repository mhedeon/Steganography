#pragma once

#include <cstdint>

#include <wtypes.h>

class WavFile {
private:
public:
	// properties...
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
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;

	// ASCII stuffable character in bitmap...
	long bmTotalStuffchar;
	unsigned long wavFileSize;

	// methods
	WavFile(const char* filename);										// constructor
	int isFileExist(const char* filename);								// check file existence
	int readHeader(const char* filename);									// read WAVE Header
	int printFileInfo();											// print header variables
	int checkFilesForHiding(char* parentfile, char* childfile);		// check files
	int hide(char* parentfile, char* childfile, char* outputfile); 	// hide into parent
	int unhide(char* parentfile, char* txtfile);					// unhide txt
};