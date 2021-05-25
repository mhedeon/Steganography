#pragma once

#include <cstdint>
#include <stdbool.h>

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
	char Subchunk2ID[4];
	uint32_t Subchunk2Size;
};

struct s_myHeader {
	uint32_t fileSize; // in bytes
	uint32_t nameSize; // in bytes
	char *name;
};

class WavFile {
private:
	struct s_wavHeader wavHeader = { 0 };
	struct s_myHeader myHeader = { 0 };
	uint32_t wavDataSize;

public:
	WavFile(const char* filename);
	~WavFile();

	bool isFileExist(const char* filename);								// check file existence
	int readWavHeader(const char* filename);									// read WAVE Header
	int printFileInfo();											// print header variables
	int checkFilesForHiding(char* parentfile, char* childfile);		// check files
	int hide(char* parentfile, char* childfile, char* outputfile); 	// hide into parent
	int unhide(char* parentfile, char* txtfile);					// unhide txt
};
