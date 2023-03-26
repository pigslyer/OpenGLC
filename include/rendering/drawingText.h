#ifndef DRAWING_TEXT_H_SEEN
#define DRAWING_TEXT_H_SEEN

#include <global.h>

void drawTextInit(void);
void drawTextTerminate(void);

void drawTextClear(void);

void drawTextColored(char* text, int length, vec2f pos, vec2f size, vec4f color);

vec2f _heightToSize(float height);

#define drawText(text, length, posx, posy, sizex, sizey) drawTextColored(text, length, (vec2f){posx, posy}, (vec2f){sizex, sizey}, (vec4f){1.0f, 1.0f, 1.0f, 1.0f})
#define drawTexth(text, length, posx, posy, height) drawText(text, length, pos, _heightToSize(height))
#define drawTexthColored(text, length, posx, posy, height, color) drawTextColored(text, length, (vec2f){posx, posy}, _heightToSize(height), color)

#endif