#include <helpers/filehelper.h>
#include <rendering/shaders.h>
#include <rendering/drawing.h>
#include <rendering/sandbox.h>

#include <global.h>
#include <game/player.h>
#include <game/level.h>


void runInits(void);
void runUpdates(void);
void runDraws(void);

void onError(int error, const char* desc);
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
void onWindowResized(GLFWwindow* window, int width, int height);

void onExit(void);

GLFWwindow* mainWindow = NULL;

// what's run if we're not in sandbox mode
int mainActual(void);

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
	
        
    mainWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "What a weird way to print \"Hello world\"...", NULL, NULL);
	
	if (!mainWindow)
    {
        return -1;
    }

	glfwMakeContextCurrent(mainWindow);

	glfwSetErrorCallback(onError);
    glfwSetKeyCallback(mainWindow, onKey);
    
	gladLoadGL();
    
	glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // run actual main
#ifndef SANDBOX

    mainActual();
#else

    sandboxLoop();

#endif

    glfwTerminate();
}

int mainActual(void)
{
    glfwSetFramebufferSizeCallback(mainWindow, onWindowResized);

    drawInit();

	glfwSwapInterval(1);

	runInits();

    levelDraw();
	

// wireframe
 //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(mainWindow))
    {
        runUpdates();
        runDraws();

        glfwPollEvents();
    }

    drawTerminate();
    return 0;
}

void runInits(void)
{
	levelLogicInit();
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

	levelDraw();
	//playerDraw();

    drawText("Hello world!", 12, 0, 0, 400, 100);

    drawText("Rats?", 5, 0, 200, 400, 100);

    //drawText("Hello world!", 12, 200, 110, 400, 100);

    //drawTexthColored("Hello world!", 12, 400, 320, 100, COLOR4_1(0.0f, 0.0f, 1.0f, 0.6f));

    //drawTextColored("Hello world!", 12, 0, 0, F(VIEWPORT_WIDTH), F(VIEWPORT_HEIGHT), COLOR4_1(1.0f, 0.6f, 0.6f, 0.8f));

    

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