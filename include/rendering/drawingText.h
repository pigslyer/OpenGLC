#ifndef DRAWING_TEXT_H_SEEN
#define DRAWING_TEXT_H_SEEN

#include <global.h>

void drawTextInit(void);
void drawTextTerminate(void);

void drawTextClear(void);

void drawText(char* text, int length, vec2f pos, vec2f size);

void drawTexth(char* text, int length, vec2f pos, float height);

#endif