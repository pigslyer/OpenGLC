#ifndef PLAYER_H_SEEN
#define PLAYER_H_SEEN

#include <global.h>

extern vec2f playerPosition;
extern float playerRotation;

void playerInit(void);
void playerUpdate(void);
void playerDraw(void);



#endif
