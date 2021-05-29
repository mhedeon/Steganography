#include "header.hpp"
#include "WavFile.hpp"
#include "myFile.hpp"
#include "logger.hpp"

void usage(void);
int wavInfoHandler(char *);
int wavEncoderHandler(char *, char *, uint16_t);
int wavEncoderHandler(char *, char *, char *, uint16_t);
int wavDecoderHandler(char *, uint16_t);

int main(int argc, char* argv[]) {

	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	switch (argc)
	{
	case 1:
	case 2:
		usage();
		break;

	case 3:
		if (strcmp(argv[1], "-i") == 0)
		{
			wavInfoHandler(argv[2]);
		}
		else
		{
			usage();
			break;
		}
		break;

	case 4:
		usage();
		break;
	case 5:
		if (strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-k") == 0)
		{
			uint16_t key = (uint16_t)atoi(argv[4]);
			if (key < 1 || key > 65000)
			{
				LOG(LOG_ERR, "Key must be [1; 65 000]");
				usage();
				break;
			}
			wavDecoderHandler(argv[2], key);
		}
		else
		{
			usage();
			break;
		}
		break;
	case 6:
		if (strcmp(argv[1], "-e") == 0 && strcmp(argv[4], "-k") == 0)
		{
			uint16_t key = (uint16_t)atoi(argv[5]);
			if (key < 1 || key > 65000)
			{
				LOG(LOG_ERR, "Key must be [1; 65 000]");
				usage();
				break;
			}
			wavEncoderHandler(argv[2], argv[3], key);
		}
		else
		{
			usage();
			break;
		}
		break;
	case 7:
		if (strcmp(argv[1], "-e") == 0 && strcmp(argv[5], "-k") == 0)
		{
			uint16_t key = (uint16_t)atoi(argv[6]);
			if (key < 1 || key > 65000)
			{
				LOG(LOG_ERR, "Key must be [1; 65 000]");
				usage();
				break;
			}
			wavEncoderHandler(argv[2], argv[3], argv[4], key);
		}
		else
		{
			usage();
			break;
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
			"-k     : key for operations. Available range is [1; 65 000]\n" <<
			"Example: \n" <<
			"wav -i <wav_file>\n" <<
			"wav -e <input_wav_file> <input_binary_file> <output_wav_file(optional)> -k <key>\n" <<
			"wav -d <input_wav_file> -k <key>\n" <<
	std::endl;
}

//function definition
int wavInfoHandler(char* wavFilePath)
{
	WavFile wavFile(wavFilePath, 1);

	wavFile.printFileInfo();

	return 0;
};

int wavEncoderHandler(char *wavFilePath, char *wavBinaryPath, uint16_t key)
{
	return wavEncoderHandler(wavFilePath, wavBinaryPath, (char *)"output_file.wav", key);
}

int wavEncoderHandler(char* wavFilePath, char* wavBinaryPath, char* outFilePath, uint16_t key)
{
	WavFile wavFile(wavFilePath, key);
	if (wavFile.encryptFile(wavFilePath, wavBinaryPath, outFilePath) != WAV_SUCCESS)
	{
		LOG(LOG_WARN, "Encoding failed");
		return -1;
	}

	LOG(LOG_INF, "Encoding done");
	return 0;
};

int wavDecoderHandler(char* wavFilePath, uint16_t key)
{
	WavFile wavFile(wavFilePath, key);
	if (wavFile.decryptFile(wavFilePath) != WAV_SUCCESS)
	{
		LOG(LOG_WARN, "Decoding failed");
		return -1;
	}

	LOG(LOG_INF, "Decoding done");
	return 0;
};
