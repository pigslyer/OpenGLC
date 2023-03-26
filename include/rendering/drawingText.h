#ifndef DRAWING_TEXT_H_SEEN
#define DRAWING_TEXT_H_SEEN

#include <global.h>

void drawTextInit(void);
void drawTextTerminate(void);

void drawTextClear(void);

void drawTextColoredv(char* text, int length, vec2f pos, vec2f size, vec4f color);

float _heightToWidth(float height);

#define drawTextColored(text, length, posx, posy, sizex, sizey, color) drawTextColoredv(text, length, (vec2f){posx, posy}, (vec2f){sizex, sizey}, color)
#define drawText(text, length, posx, posy, sizex, sizey) drawTextColored(text, length, posx, posy, sizex, sizey, COLOR4_1(1.0f, 1.0f, 1.0f, 1.0f))
#define drawTexth(text, length, posx, posy, height) drawText(text, length, posx, posy, _heightToWidth(height), height)
#define drawTexthColored(text, length, posx, posy, height, color) drawTextColored(text, length, posx, posy, _heightToWidth(height), height, color)

#endif