#include <helpers/filehelper.h>
#include <rendering/shaders.h>
#include <rendering/drawing.h>

#include <global.h>
#include <player.h>
#include <level.h>


void runInits(void);
void runUpdates(void);
void runDraws(void);

void onError(int error, const char* desc);
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
void onWindowResized(GLFWwindow* window, int width, int height);

void onExit(void);

GLFWwindow* mainWindow = NULL;


int main(void)
{
    if (!glfwInit())
	{
		return -1;
	}

    atexit(onExit);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    GLFWwindow* window;
        
    mainWindow = window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "What a weird way to print \"Hello world\"...", NULL, NULL);
	
	if (!window)
    {
        return -1;
    }

	glfwMakeContextCurrent(window);

	glfwSetErrorCallback(onError);
	glfwSetKeyCallback(window, onKey);
    glfwSetFramebufferSizeCallback(window, onWindowResized);


//	glEnable(GL_TEXTURE_2D);
	//glad_glOrtho(0, VIEWPORT_WIDTH, 0, VIEWPORT_HEIGHT, -1, 1);
	gladLoadGL();

    drawInit();

	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

//	glOrtho(0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 0, -1, 1);

	glfwSwapInterval(1);

	//runInits();
	
    
    float rainbowTriangle[] = 
    {
        -0.5f, -0.5f, +0.0f, +1.0f, +0.0f, +0.0f,
        +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f,
        +0.0f, +0.5f, +0.0f, +0.0f, +0.0f, +1.0f,
    };
    
    unsigned int VAOs[3];
    unsigned int VBOs[3];

    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);

    // bounds 0s
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(rainbowTriangle), rainbowTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)) );
    glEnableVertexAttribArray(1);

    // bounds 1s
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(rainbowTriangle), rainbowTriangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    

    // texture VAO, VBOs, EBOs
/*
    float texturedRectangle[] =
    {
        // positions // colors // texture coords
        +0.5f, +0.5f, +0.0f, +1.0f, +0.0f, +0.0f, +1.0f, +1.0f, // top right
        +0.5f, -0.5f, +0.0f, +0.0f, +1.0f, +0.0f, +1.0f, +0.0f, // bottom right
        -0.5f, -0.5f, +0.0f, +0.0f, +0.0f, +1.0f, +0.0f, +0.0f, // bottom left
        -0.5f, +0.5f, +0.0f, +1.0f, +1.0f, +0.0f, +0.0f, +1.0f // top left
    };

    unsigned int texturedRectangleIndicies[] =
    {
        0, 1, 3,
        1, 2, 3,
    };

    unsigned int EBO, ProgramTextured;

    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedRectangle), texturedRectangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedRectangleIndicies), texturedRectangleIndicies, GL_STATIC_DRAW);

    glBindVertexArray(VAOs[2]);

    unsigned int texture;
    glGenTextures(1, &texture);
    
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
*/
    
    /*uint32_t width, height;
    unsigned char* image = readBmp("assets/fontExtended.bmp", &width, &height);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)width, (int)height, 0, GL_RED, GL_UNSIGNED_BYTE, image);

    free(image);*/
/*
    ProgramTextured = loadShader("shaders/basicTexturedModulate.vert","shaders/basicTexturedModulate.frag");

    if (ProgramTextured == 0)
    {
        return -1;
    }
*/
/*
    float rectangleVertices[] =
    {
        +0.5f, +0.5f, +0.0f, // top left
        +0.5f, -0.5f, +0.0f, // bottom left
        -0.5f, -0.5f, +0.0f, // bottom right
        -0.5f, +0.5f, +0.0f, // top right
    };

    unsigned int rectangleIndicies[] = 
    {
        0, 1, 3,
        1, 2, 3,
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    
    glBindVertexArray(VAO);

    // vertex buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // send our triangle to vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndicies), rectangleIndicies, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
*/

    // get the shader and select it
    /*
    unsigned int shaderProgram = loadShader("shaders/basic.vert", "shaders/basic.frag");

    if (shaderProgram == 0)
    {
        return -1;
    }
    */

  //  glUseProgram(shaderProgram);

// wireframe
 //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {

        runDraws();

        
        glfwPollEvents();
/*        float time = (float) glfwGetTime();
        float gren = sinf(time) * 0.5f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "modulation");
        glUniform4f(vertexColorLocation, 0.0f, gren, 0.0f, 1.0f);*/

    //    glBindVertexArray(VAO);

        /*

        glUseProgram(ProgramTextured);
        int sizeLocation = glGetUniformLocation(ProgramTextured, "atlasSize");
        glUniform2f(sizeLocation, 1.0f / 5.0f, 1.0f / 6.0f);

        int positionLocation = glGetUniformLocation(ProgramTextured, "atlasPos");
        glUniform2f(positionLocation, 3.0f, 3.0f);

        glBindVertexArray(VAOs[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
*/
       // glUseProgram(shaderProgram);

      // glBindVertexArray(VAOs[0]);
       //glDrawArrays(GL_TRIANGLES, 0, 3);

//        glBindVertexArray(VAOs[1]);
//        glDrawArrays(GL_TRIANGLES, 3, 3);

//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//runUpdates();

     
		//runDraws();

    }

    drawTerminate();

    glfwTerminate();
    return 0;
}

void runInits(void)
{
	levelInit();
	playerInit();
}

void runUpdates(void)
{
	levelUpdate();
	playerUpdate();
}


void runDraws(void)
{
    glClearColor(0.4f, 0.4f, 0.4f, 1);
    glClear(GL_COLOR_BUFFER_BIT);


	//levelDraw();
	//playerDraw();

    
    drawText("Hello world!", 12, (vec2f){-0.6f, 0.8f}, (vec2f){+1.2f, -0.7f});
    
    drawText("Hi there!", 9, (vec2f){-0.6f, -0.4f}, (vec2f){+1.2f, -0.2f});
    
    drawText("YOOOOO, WE DONE IT???", 21, (vec2f){-0.5f, -0.7f}, (vec2f){+1.2f, -0.2f});
    

    glfwSwapBuffers(mainWindow);

    drawClear();
}

void onError(int error, const char* desc)
{
	printf("error: %s\n", desc);
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && (mods & GLFW_MOD_CONTROL) != 0)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void onWindowResized(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    runDraws();
}

void onExit(void)
{
    glfwTerminate();
}