#include <rendering/shaders.h>
#include <helpers/filehelper.h>

#define ON_FAILED exit(1)

bool checkShaderStatus(unsigned int shader, GLenum status);
bool checkProgramInfoLog(unsigned int program, GLenum status);

unsigned int loadShader(const char* const vertexPath, const char* const fragmentPath)
{
	size_t length;	
	char* shaderCode;

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);

	shaderCode = readFile(vertexPath, &length);

	if (shaderCode == NULL)
	{
		printf("%s doesn't exist.", vertexPath);
		glDeleteShader(vertex);
		
		ON_FAILED;
	}

	glShaderSource(vertex, 1, (const char* const*) &shaderCode, (const int*) &length);
	free((void*)shaderCode);

	glCompileShader(vertex);
	
	if (!checkShaderStatus(vertex, GL_COMPILE_STATUS))
	{
		glDeleteShader(vertex);
		ON_FAILED;
	}
	
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	
	shaderCode = readFile(fragmentPath, &length);
	
	if (shaderCode == NULL)
	{
		printf("%s doesn't exist.", vertexPath);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		ON_FAILED;
	}

	glShaderSource(fragment, 1, (const char* const*)&shaderCode, (const int*) &length);
	free((void*)shaderCode);

	glCompileShader(fragment);

	if (!checkShaderStatus(fragment, GL_COMPILE_STATUS))
	{
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		ON_FAILED;
	}

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	if (!checkProgramInfoLog(program, GL_LINK_STATUS))
	{
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteProgram(program);

		ON_FAILED;
	}

	glDetachShader(program, vertex);
	glDetachShader(program, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}

bool checkShaderStatus(unsigned int shader, GLenum status)
{
	int succ;
	glGetShaderiv(shader, status, &succ);
	
	if (!succ)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);

		printf(log);

		return false;
	}

	return true;
}

bool checkProgramInfoLog(unsigned int program, GLenum status)
{
	int succ;
	glGetProgramiv(program, status, &succ);
	
	if (!succ)
	{
		char log[512];
		glGetProgramInfoLog(program, 512, NULL, log);

		printf(log);

		return false;
	}

	return true;
}
