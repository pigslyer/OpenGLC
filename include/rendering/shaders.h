#ifndef SHADERS_H_SEEN
#define SHADERS_H_SEEN

#include <global.h>

#define NO_PROGRAM 0
#define NO_LOCATION -1
typedef GLint ShdLoc;

unsigned int loadShader(const char* const vertex, const char* const fragment);


#endif