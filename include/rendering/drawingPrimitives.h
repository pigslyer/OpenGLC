#ifndef DRAWING_PRIMITIVES_H_SEEN
#define DRAWING_PRIMITIVES_H_SEEN

#include <global.h>
#include <rendering/drawing.h>

void drawPrimitivesInit(void);
void drawPrimitivesTerminate(void);

void drawPrimitivesClear(void);

void drawLineColoredv(vec2f from, vec2f to, vec4f color);

#define drawLineColored(fromx, fromy, tox, toy, color) drawLineColoredv((vec2f){fromx, fromy}, (vec2f){tox, toy}, color)
#define drawLine(fromx, fromy, tox, toy) drawLineColored(fromx, fromy, tox, toy, COLOR4_1(1.0f, 1.0f, 1.0f, 1.0f))

#endif