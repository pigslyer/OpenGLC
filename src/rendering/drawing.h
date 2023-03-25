#ifndef DRAWING_H_SEEN
#define DRAWING_H_SEEN

#include "../global.h"

void drawInit(void);
void drawTerminate(void);

void drawText(char* text, int length, vec2f pos, vec2f size);

// clears all vbos
void drawClear(void);

#endif