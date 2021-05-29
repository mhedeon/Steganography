#include "header.hpp"
#include "WavFile.hpp"
#include "myFile.hpp"
#include "logger.hpp"

void usage(void);
int wavInfoHandler(char *);
int wavEncoderHandler(char *, char *);
int wavEncoderHandler(char *, char *, char *);
int wavDecoderHandler(char *);

int main(int argc, char* argv[]) {
	// myFile q((char *)"data\\Polishchuk_magistr.pdf", MODE_READ);
	// std::cout << q.getPath() << std::endl << q.getName() << std::endl << q.getExt() << std::endl;
	// myFile w((char *)"data\\Polishchuk_magistr.", MODE_READ);
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	switch (argc)
	{
	case 1:
	case 2:
		usage();
		// exit(0);
		break;

	case 3:
		if (strcmp(argv[1], "-i") == 0)
		{
			wavInfoHandler(argv[2]);
		}
		else if (strcmp(argv[1], "-d") == 0)
		{
			wavDecoderHandler(argv[2]);
		}
		else
		{
			usage();
		}
		break;

	case 4:
		if (strcmp(argv[1], "-e") == 0)
		{
			wavEncoderHandler(argv[2], argv[3]);
		}
		else
		{
			usage();
		}
		break;
	case 5:
		if (strcmp(argv[1], "-e") == 0)
		{
			wavEncoderHandler(argv[2], argv[3], argv[4]);
		}
		else
		{
			usage();
		}
		break;
	default:
		usage();
	}

	return 0;
}

void usage(void)
{
	std::cout << "usage  : wav -<option> <argumets>\n" <<
			"Options:\n" <<
			"-i     : wav file's information\n" <<
			"-e     : hide binary file into wav file\n" <<
			"-d     : unhide binary file from wav file\n" <<
			"Example: \n" <<
			"wav -i <wav_file>\n" <<
			"wav -e <input_wav_file> <input_binary_file> <output_wav_file(optional)>\n" <<
			"wav -d <input_wav_file>\n" <<
	std::endl;
}

//function definition
int wavInfoHandler(char* wavFilePath)
{
	WavFile wavFile(wavFilePath);

	wavFile.printFileInfo();

	return 0;
};

int wavEncoderHandler(char *wavFilePath, char *wavBinaryPath)
{
	return wavEncoderHandler(wavFilePath, wavBinaryPath, (char *)"output_file.wav");
}

int wavEncoderHandler(char* wavFilePath, char* wavBinaryPath, char* outFilePath)
{
	WavFile wavFile(wavFilePath);
	if (wavFile.encryptFile(wavFilePath, wavBinaryPath, outFilePath) == -1)
		return -1;

	std::cout << "encoding done" << std::endl;
	return 0;
};

int wavDecoderHandler(char* wavFilePath)
{
	WavFile wavFile(wavFilePath);
	if (wavFile.decryptFile(wavFilePath) == -1)
		return -1;

	std::cout << "decoding done" << std::endl;
	return 0;
};
