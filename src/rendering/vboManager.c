#include "vboManager.h"

vboManager* initVboMan(int capacity, void (*initVAO)(void))
{
	vboManager* vbo = (vboManager*) malloc(sizeof(vboManager));
	
	vbo->vbos = (GLuint*) malloc((size_t)capacity * sizeof(GLuint));
	vbo->vaos = (GLuint*) malloc((size_t)capacity * sizeof(GLuint));
	vbo->unusedIndex = 0;
	vbo->capacity = capacity;

	glGenVertexArrays(capacity, vbo->vaos);
	glGenBuffers(capacity, vbo->vbos);


	for (int i = 0; i < capacity; i++)
	{
		glBindVertexArray(vbo->vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo->vbos[i]);
		initVAO();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vbo;
}

void freeVboMan(vboManager* vbo)
{
	glDeleteVertexArrays(vbo->capacity, vbo->vaos);
	glDeleteBuffers(vbo->capacity, vbo->vbos);

	free(vbo->vbos);
	free(vbo);
}

void bindUnusedVbo(vboManager* vbo)
{
	if (!(vbo->unusedIndex < vbo->capacity))
	{
		printf("ran out of VBOs, current capacity: %d\n", vbo->capacity);
		exit(1);
	}

	glBindVertexArray(vbo->vaos[vbo->unusedIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo->vbos[vbo->unusedIndex++]);
}

void clearVbos(vboManager* vbo)
{
	for (int i = 0; i < vbo->unusedIndex; i++)
	{
		glBindVertexArray(vbo->vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo->vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vbo->unusedIndex = 0;
}