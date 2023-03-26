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

	gladLoadGL();

    drawInit();

	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

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
    
// wireframe
 //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        runDraws();

        glfwPollEvents();
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

    drawText("Hello world!", 12, (vec2f){0.0f, 0.0f}, (vec2f){400.0f, 100.0f});
    
    drawText("Hello world!", 12, (vec2f){200.0f, 110.0f}, (vec2f){400.0f, 100.0f});

    drawTexth("Hello world!", 12, (vec2f){200.0f, 220.0f}, 100.0f);
    

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