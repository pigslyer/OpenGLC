#ifndef VBO_MANAGER_H_SEEN
#define VBO_MANAGER_H_SEEN

#include "../global.h"

struct vboManager
{
	GLuint* vbos;
	GLuint* vaos;
	int unusedIndex;
	int capacity;	
} typedef vboManager;


vboManager* initVboMan(int capacity, void (*vboInit)(void));
void freeVboMan(vboManager* man);


void bindUnusedVbo(vboManager* man);
void clearVbos(vboManager* man);

#endif