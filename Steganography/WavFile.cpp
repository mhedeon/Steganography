#include "header.hpp"
#include "WavFile.hpp"
#include "myFile.hpp"

#ifndef __WavFile
#define __WavFile
#endif

//class contructor
WavFile::WavFile(const char* filename)
{
	if (myFile::ifFileExist(filename))
	{
		readWavHeader(filename);
	}
}

WavFile::~WavFile()
{

}

int WavFile::readWavHeader(const char* filename)
{
	int res = WAV_ERROR;

	if (myFile::ifFileExist(filename)) {
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
			std::cout << wavHeader.Subchunk2ID << std::endl;
			std::cout << wavHeader.Subchunk2Size << std::endl;
			wavHeader.listSize = wavHeader.Subchunk2Size;
			std::cout << wavHeader.listSize << std::endl;
			fseek(wavFilename, wavHeader.listSize, SEEK_CUR);
			wavHeader.listSize += 8; //adding lost SubChunkID and SubChunkSize size
			std::cout << wavHeader.listSize << std::endl;

			fread((char*)&(wavHeader.Subchunk2ID), 4, 1, wavFilename);
			fread((uint32_t*)&(wavHeader.Subchunk2Size), 4, 1, wavFilename);
			if (strncmp(wavHeader.Subchunk2ID, "data", 4))
			{
				wavHeader.Subchunk2Size = 0;
				std::cout << __FUNCTION__ << "(): " << __LINE__ << ": " << "Unknown DATA subchunk(" << wavHeader.Subchunk2ID << ")." << std::endl;
			}
		}
		
		fclose(wavFilename);
		return 0;
	}
	return -1;
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

	pfile = fopen(parentfile, "rb");
	while (!feof(pfile))					// read the length of wav file...
	{
		fgetc(pfile);
		p++;
	}

	readWavHeader(parentfile);				// read wav header info...

	wavDataSize = p - WAV_HEADER_SIZE;
	if (t > wavDataSize)
	{
		throw "TXT filesize is greater than WAVE file...";
		return -1;
	}

	//if(waChannel != 1)
//		{
//			throw ("Only mono WAVE files can be used...");
//			return -1;
//		}

	return 0;
}

void WavFile::prepareHeader(char* childfile)
{
	myFile temp(childfile, MODE_READ);
	
	myHeader.fileSize = temp.size;
	myHeader.headerSize = sizeof(myHeader.fileSize);

	std::cout << "fileSize: " << myHeader.fileSize << std::endl;
}

void WavFile::writeHeader(FILE **parFile, FILE **outFile)
{
	// temporary just to test enc/dec with header and size info
	for (uint32_t i = 0; i < 4; i++)
	{
		char *headerByte = (char *)&(myHeader.fileSize) + i;
		hideByte(parFile, outFile, *headerByte);
	}
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
		fileByte &= 0xFE;				//FE, to make sure LSB is always zero
		fileByte |= (char)((byte >> bit) & 1);
		fputc(fileByte, *fileResult);

		for (int skipBytes = 0; skipBytes < wavHeader.BitsPerSample / 8 - 1; skipBytes++)
		{
			fputc(fgetc(*fileContainer), *fileResult);
			wavDataSize-=1;
		}

		wavDataSize-=1;
	}
}

void WavFile::readHeader(FILE **parFile)
{
	uint32_t temp = 0;
	
	for (int i = 0; i < 4; i++)
	{
		((char *)&(myHeader.fileSize))[i] = readHiddenByte(parFile);
	}
	std::cout << "myHeader.fileSize dec: " << myHeader.fileSize << std::endl;
}

int WavFile::hide(char* parentfile, char* childfile, char* outputfile)
{
	FILE* wfile, * tfile, * ofile;
	unsigned char header[WAV_HEADER_SIZE];

	// check and Initialize parent & txt files...
	if (checkFilesForHiding(parentfile, childfile) == -1)
	{
		throw ("error!, initialization failed...");
		
		return -1;
	}

	wfile = fopen(parentfile, "rb");
	tfile = fopen(childfile, "rb");
	ofile = fopen(outputfile, "w+b");

	fread(header, WAV_HEADER_SIZE, 1, wfile);		// read WAV header
	fwrite(header, WAV_HEADER_SIZE, 1, ofile);		// write WAV header
	
	//for case of LIST presence. loop here, bcs I don't know length of LIST chunks
	for (uint32_t i = 0; i < wavHeader.listSize; i++)
	{
		fputc(fgetc(wfile), ofile);
		wavDataSize-=1;
	}
	
	prepareHeader(childfile);
	writeHeader(&wfile, &ofile);

	// main hiding/encoding process
	while (!feof(tfile))
	{
		hideByte(&wfile, &ofile, fgetc(tfile));
	}

	// write remaing wav bytes into the new file.
	if (wavDataSize > 0)
	{
		while (!feof(wfile)) {
			fputc(fgetc(wfile), ofile);
		}
	}

	// close all file handlers
	fclose(wfile);
	fclose(tfile);
	fclose(ofile);

	return 0;
}

int WavFile::unhide(char* parentfile, char* txtfile)
{
	if (readWavHeader(parentfile) == -1) {
		return -1;
	}

	FILE* bfile, * tfile;
	char ch = 0;
	bfile = fopen(parentfile, "rb");
	tfile = fopen(txtfile, "w+b");

	// skip WAV header
	fseek(bfile, WAV_HEADER_SIZE + wavHeader.listSize, SEEK_SET);

	readHeader(&bfile);

	int i = 0;
	while (!feof(bfile) && i < myHeader.fileSize)
	{
		ch = readHiddenByte(&bfile);

		fputc(ch, tfile);
		i++;
	}

	fclose(bfile);
	fclose(tfile);
	return 0;
}
