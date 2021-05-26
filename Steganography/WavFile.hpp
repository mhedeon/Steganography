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
	
	/*  temporary not used
	uint32_t nameSize; // in bytes
	char *name;
	*/
	uint32_t headerSize;
};

class WavFile {
private:
	struct s_wavHeader wavHeader = { 0 };
	struct s_myHeader myHeader = { 0 };
	
	//to be deleted
	uint32_t wavDataSize;

	char readHiddenByte(FILE **file);
	void hideByte(FILE **fileContainer, FILE **fileResult, char byte);
public:
	WavFile(const char* filename);
	~WavFile();


	void prepareHeader(char* childfile);
	void writeHeader(FILE **parFile, FILE **outFile);
	void readHeader(FILE **parFile);


	int readWavHeader(const char* filename);									// read WAVE Header
	int printFileInfo();											// print header variables
	int checkFilesForHiding(char* parentfile, char* childfile);		// check files
	int hide(char* parentfile, char* childfile, char* outputfile); 	// hide into parent
	int unhide(char* parentfile, char* txtfile);					// unhide txt
};
