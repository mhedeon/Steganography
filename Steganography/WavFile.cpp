#include "header.hpp"
#include "WavFile.hpp"

#ifndef __WavFile
#define __WavFile
#endif

//checks for existence of file, returns 0 if ok, returns -1 on error
int WavFile::isFileExist(const char* filename)
{
	FILE* wavFilename;
	wavFilename = fopen(filename, "rb");
	if (wavFilename == NULL) {
		throw("File doesn't exits or wrong Filename...\n");
		exit(1);
		return (-1);
	}
	fclose(wavFilename);
	return 0;
}

int WavFile::readHeader(const char* filename)
{

	if (isFileExist(filename) == 0) {
		FILE* wavFilename = fopen(filename, "rb");

		fread((char*)&ChunkID, 4, 1, wavFilename);
		fread((uint32_t*)&ChunkSize, 4, 1, wavFilename);
		fread((char*)&Format, 4, 1, wavFilename);
		fread((char*)&Subchunk1ID, 4, 1, wavFilename);
		fread((uint32_t*)&Subchunk1Size, 4, 1, wavFilename);
		fread((uint16_t*) & AudioFormat, 2, 1, wavFilename);
		fread((uint16_t*) & NumChannels, 2, 1, wavFilename);
		fread((uint32_t*)&SampleRate, 4, 1, wavFilename);
		fread((uint32_t*)&ByteRate, 4, 1, wavFilename);
		fread((uint16_t*)&BlockAlign, 2, 1, wavFilename);
		fread((uint16_t*)&BitsPerSample, 2, 1, wavFilename);
		fread((char*)&Subchunk2ID, 4, 1, wavFilename);
		fread((uint32_t*)&Subchunk2Size, 4, 1, wavFilename);
		
		fclose(wavFilename);
		return 0;
	}
	return -1;
}

//class contructor
WavFile::WavFile(const char* filename)
{
	if (isFileExist(filename) == 0)
	{
		readHeader(filename);
	}
}

int WavFile::printFileInfo()
{
	cout << "RIFF             : " << ChunkID[0] << ChunkID[1] << ChunkID[2] << ChunkID[3] << endl;
	cout << "Riff Chuck       : " << ChunkSize << endl;
	cout << "Wave             : " << Format[0] << Format[1] << Format[2] << Format[3] << endl;
	cout << "Fmt              : " << Subchunk1ID[0] << Subchunk1ID[1] << Subchunk1ID[2] << Subchunk1ID[3] << endl;
	cout << "Fmt chunk        : " << Subchunk1Size << endl;
	cout << "Mono Or Stero    : " << AudioFormat << endl;
	cout << "Channel          : " << NumChannels << endl;
	cout << "Sample Rate      : " << SampleRate << endl;
	cout << "Bytes per sec    : " << ByteRate << endl;
	cout << "Bytes Per Sample : " << BlockAlign << endl;
	cout << "Bits per sample  : " << BitsPerSample << endl;
	cout << "Data chuck       : " << Subchunk2ID[0] << Subchunk2ID[1] << Subchunk2ID[2] << Subchunk2ID[3] << endl;
	cout << "Length of chunk  : " << Subchunk2Size << endl;

	return 0;
}

int WavFile::checkFilesForHiding(char* parentfile, char* childfile)
{
	FILE* tfile, * pfile;
	unsigned long t = 0, p = 0;

	// check and Initialize files...
	// read txt file and read wav file length...
	if (isFileExist(parentfile) == -1)
	{
		throw("WAV file doesn't exist");
		return -1;
	}

	if (isFileExist(childfile) == -1)
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

	readHeader(parentfile);				// read wav header info...

	wavFileSize = p - 44;
	if (t > wavFileSize)
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

int WavFile::hide(char* parentfile, char* childfile, char* outputfile)
{
	FILE* wfile, * tfile, * ofile;
	unsigned char header[44];
	char wavbuffer, txtbuffer;
	int i;
	char txtTerminatorIndicator = '*';

	// check and Initialize parent & txt files...
	if (checkFilesForHiding(parentfile, childfile) == -1)
	{
		throw ("error!, initialization failed...");
		return -1;
	}

	wfile = fopen(parentfile, "rb");
	tfile = fopen(childfile, "rb");
	ofile = fopen(outputfile, "w+b");

	fread(header, 44, 1, wfile);		// read WAV header
	fwrite(header, 44, 1, ofile);		// write WAV header

	// main hiding/encoding process
	while (!feof(tfile))
	{
		txtbuffer = fgetc(tfile);
		for (i = 0; i < 8; i++)
		{
			wavbuffer = fgetc(wfile);
			wavbuffer &= 0xFE;				//FE, to make sure LSB is always zero
			wavbuffer |= (char)((txtbuffer >> i) & 1);
			fputc(wavbuffer, ofile);
			wavbuffer = fgetc(wfile);
			fputc(wavbuffer, ofile);

			wavFileSize-=2;
		}
	}

	// stuffing txt terminator indicator.
	for (i = 0; i < 8; i++)
	{
		wavbuffer = fgetc(wfile);
		wavbuffer &= 0xFE;
		wavbuffer |= (char)((txtTerminatorIndicator >> i) & 1);
		fputc(wavbuffer, ofile);
		wavbuffer = fgetc(wfile);
		fputc(wavbuffer, ofile);
		wavFileSize-=2;
	}

	// write remaing wav bytes into the new file.
	if (wavFileSize > 0)
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
	if (readHeader(parentfile) == -1) {
		return -1;
	}

	FILE* bfile, * tfile;
	char ch, bmpBuffer[8];
	int i;
	bfile = fopen(parentfile, "rb");
	tfile = fopen(txtfile, "w+b");

	fseek(bfile, 44, SEEK_SET);				//skip the BMP header part
	ch = 0;

	while (!feof(bfile))
	{
		// read the last bit from BMP file
		ch = 0;
		for (i = 0; i <= 7; i++) {
			bmpBuffer[i] = fgetc(bfile);
			fgetc(bfile);
		}

		for (i = 7; i >= 0; i--) {
			ch += (bmpBuffer[i] & 1);
			if (i != 0)
				ch <<= 1;
		}

		if (ch == EOF || ch == '*') {
			break;
		}
		else {
			fputc(ch, tfile);
		}
	}

	fclose(bfile);
	fclose(tfile);
	return 0;
}