#ifndef GLOBAL_H_SEEN
#define GLOBAL_H_SEEN

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "helpers/mathhelper.h"

#define F(of) ((float) (of))
#define I(of) ((int) (of))

#define DO_ERRORS

#ifdef DO_ERRORS
#define ERROR(message) {GLenum err; if ((err = glGetError()) != GL_NO_ERROR) {printf(message); printf(" Error: %d\n", err); }}
#else
#define ERROR(MESSAGE)
#endif

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const int VIEWPORT_WIDTH;
extern const int VIEWPORT_HEIGHT;

extern const int RENDER_RAYCAST_COUNT;
extern const int RENDER_RAYCAST_DOF_MAX;

extern const float PLAYER_FOV;

/// @brief The main (and probably only?) window we'll ever use.
extern GLFWwindow* mainWindow;

#define GET_KEY(key) glfwGetKey(mainWindow, key)

#endif