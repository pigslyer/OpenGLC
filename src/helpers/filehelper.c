#include <helpers/filehelper.h>


uint32_t intFrom4Char(char* start);

char* readFile(const char* path, size_t* length)
{
	const size_t CHUNK_SIZE = 1024;

	FILE* file = fopen(path, "r");

	if (file == NULL)
	{
		*length = 0;
		return NULL;
	}

	char* ret = (char*) malloc(0);
	char chunk[CHUNK_SIZE];
	size_t bufferPos = 0;
	size_t bufferSize = 0;
	size_t readChars;

	while ((readChars = fread(chunk, sizeof(char), CHUNK_SIZE, file)) > 0)
	{
		bufferSize += readChars;
		ret = (char*) realloc(ret, bufferSize);
		
		for (size_t i = 0; i < readChars; i++)
		{
			ret[bufferPos + i] = chunk[i];
		}

		bufferPos = bufferSize;
	}

	ret = realloc(ret, bufferSize + 1);
	ret[bufferSize] = '\0';

	*length = bufferSize + 1;

	fclose(file);

	return ret;
}

unsigned char* readBmp(const char* path, int* width, int* height, int* numChannels)
{
	const int TOTAL_SIZE = 0x2;
	const int PIXEL_ARRAY_START = 0xA;
	const int WIDTH = 0x12;
	const int HEIGHT = 0x16;
	const int COLOR_WIDTH = 0x1C;

	FILE* file = fopen(path, "rb");

	if (file == NULL)
	{
		*width = *height = 0;
		return NULL;
	}

	fseek(file, TOTAL_SIZE, SEEK_SET);

	char fileSizeChars[4];
	fgets(fileSizeChars, 4, file);
	
	size_t totalSize = (size_t) intFrom4Char(fileSizeChars);
	
	fseek(file, 0, SEEK_SET);

	// i technically don't need to copy all of rawData twice, if loading because an issue seeking to positions and reading from there
	// would be faster
	char* rawData = (char*) malloc(totalSize * sizeof(char));
	
	for (size_t i = 0; i < totalSize; i++)
	{
		rawData[i] = (char) fgetc(file);
	}

	fclose(file);

	uint32_t rawSize = intFrom4Char(rawData + TOTAL_SIZE);
	uint32_t pixelStart = intFrom4Char(rawData + PIXEL_ARRAY_START);

	// in bytes
	uint32_t colorWidth = intFrom4Char(rawData + COLOR_WIDTH) / 8;
 
	*width = (int)intFrom4Char(rawData + WIDTH);
	*height = (int)intFrom4Char(rawData + HEIGHT);
	*numChannels = (int) colorWidth;

	// with padding
	size_t imageSize = (size_t) (rawSize - pixelStart);

	unsigned char* imageData = (unsigned char*) malloc(imageSize * sizeof(unsigned char));

	for (uint32_t i = 0; i < imageSize; i += colorWidth)
	{
		for (uint32_t j = 0; j < colorWidth; j++)
		{
			imageData[i + j] = (unsigned char) rawData[pixelStart + i + colorWidth - 1 - j];
		}
	}

	free(rawData);
	return imageData;
}

inline uint32_t intFrom4Char(char* start)
{
	return (((uint32_t) start[0]) & 0xFF) | ((((uint32_t) start[1]) << 8) & 0xFF00) | ((((uint32_t) start[2]) << 16) & 0xFF0000) | ((((uint32_t) start[3]) << 24) & 0xFF000000); 
}