#ifndef DRAWING_PRIMITIVES_H_SEEN
#define DRAWING_PRIMITIVES_H_SEEN

#include <global.h>
#include <rendering/drawing.h>

void drawPrimitivesInit(void);
void drawPrimitivesTerminate(void);

void drawPrimitivesClear(void);

void drawLineColoredv(vec2f from, vec2f to, float width, vec4f color);

#define drawLineColored(fromx, fromy, tox, toy, width, color) drawLineColoredv((vec2f){fromx, fromy}, (vec2f){tox, toy}, width, color)
#define drawLine(fromx, fromy, tox, toy, width) drawLineColored(fromx, fromy, tox, toy, width, COLOR4_1(1.0f, 1.0f, 1.0f, 1.0f))

#endif