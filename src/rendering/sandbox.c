#include <rendering/sandbox.h>

// runs once when the sandbox starts
void sandboxInit(void);

// runs until glfwWindowShouldClose(mainWindow) returns true
void sandboxRenderLoop(void);

// technically doesn't have to exist, but still
void sandboxRenderTerminate(void);

void sandboxLoop(void)
{
	sandboxInit();

	while (!glfwWindowShouldClose(mainWindow))
	{
		glClearColor(0.4f, 0.4f, 0.4f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		sandboxRenderLoop();

		glfwPollEvents();
		glfwSwapBuffers(mainWindow);
	}

	sandboxRenderTerminate();
}

/// -------------------------------------------------------------------------------
/// everything below this line can be literally anything we're testing

#include <rendering/drawing.h>
#include <rendering/shaders.h>

ObjID vao1, vao2;
ObjID vboX, vboY1, vboY2;

ObjID shader;
ShdLoc modulate, xoffset, ybase;

void sandboxInit(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


	shader = loadShader("shaders/lineFromXYoffsets.vert","shaders/lineFromXYoffsets.frag");

	modulate = glGetUniformLocation(shader, "modulate");
	xoffset = glGetUniformLocation(shader, "xoffset");
	ybase = glGetUniformLocation(shader, "ybase");

	glGenVertexArrays(1, &vao1);
	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vboX);
	glGenBuffers(1, &vboY1);
	glGenBuffers(1, &vboY2);

	float xs[] = 
	{
		-1.0f, -0.8f, -0.6f, -0.4f, -0.2f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vboX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xs), xs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vao1);

	glBindBuffer(GL_ARRAY_BUFFER, vboX);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vboY1);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 4);

	glBindVertexArray(vao2);
	
	glBindBuffer(GL_ARRAY_BUFFER, vboX);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vboY2);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 4);

}

void sandboxRenderTerminate(void)
{
	glDeleteProgram(shader);

	glDeleteVertexArrays(1, &vao1);
	glDeleteVertexArrays(1, &vao2);
	glDeleteBuffers(1, &vboX);
	glDeleteBuffers(1, &vboY1);
	glDeleteBuffers(1, &vboY2);
}

void sandboxRenderLoop(void)
{
	float yoffUpper[] =
	{
		-0.1f, -0.5f, -0.3f, -0.4f, -0.2f,
	};

	float yoffLower[] = 
	{
		0.4f, 1.2f, 0.0f, 0.4f, 0.8f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vboY1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yoffUpper), yoffUpper, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboY2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yoffLower), yoffLower, GL_DYNAMIC_DRAW);	

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shader);
	glUniform1f(xoffset, 0.2f);

	// upper
	glBindVertexArray(vao1);
	glUniform1f(ybase, 1.0f);
	glUniform4f(modulate, 1.0f, 0.0f, 0.0f, 1.0f);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 20);

	// lower
	glBindVertexArray(vao2);
	glUniform1f(ybase, -1.0f);
	glUniform4f(modulate, 0.0f, 1.0f, 0.0f, 1.0f);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 20);
}


