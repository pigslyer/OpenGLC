#ifndef FILEHELPER_H_SEEN
#define FILEHELPER_H_SEEN

#include <global.h>


char* readFile(const char* path, size_t* length);

unsigned char* readBmp(const char* path, int* width, int* height, int* numChannels);

#endif