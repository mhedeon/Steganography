#include "header.hpp"
#include "WavFile.hpp"

#ifndef __WavFile
#define __WavFile
#endif

//class contructor
WavFile::WavFile(const char* filename)
{
	if (isFileExist(filename))
	{
		readWavHeader(filename);
	}
}

WavFile::~WavFile()
{

}

//checks for existence of file, returns 0 if ok, returns -1 on error
bool WavFile::isFileExist(const char* filename)
{
	FILE* wavFilename;

	if ((wavFilename = fopen(filename, "rb")) == NULL) {
		return false;
	}
	fclose(wavFilename);
	
	return true;
}

int WavFile::readWavHeader(const char* filename)
{

	if (isFileExist(filename)) {
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
	cout << "Data chuck       : " << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
	cout << "Length of chunk  : " << wavHeader.Subchunk2Size << endl;

	return 0;
}

int WavFile::checkFilesForHiding(char* parentfile, char* childfile)
{
	FILE* tfile, * pfile;
	unsigned long t = 0, p = 0;

	// check and Initialize files...
	// read txt file and read wav file length...
	if (!isFileExist(parentfile))
	{
		throw("WAV file doesn't exist");
		return -1;
	}

	if (!isFileExist(childfile))
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

	wavDataSize = p - 44;
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

			wavDataSize-=2;
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
		wavDataSize-=2;
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
