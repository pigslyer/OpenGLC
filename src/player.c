#include "player.h"
#include "level.h"

#define PLAYER_MOVEMENT_SPEED 5
#define PLAYER_TURN_SPEED 0.08f

#define PLAYER_BEAK_LENGTH 40

vec2f playerPosition;
float playerRotation;

void playerInit(void)
{
	playerPosition.x = playerPosition.y = 300;
	playerRotation = 0;
}

void playerUpdate(void)
{
	float turning = (float)(GET_KEY(GLFW_KEY_A) - GET_KEY(GLFW_KEY_D));
	float forward = (float)(GET_KEY(GLFW_KEY_W) - GET_KEY(GLFW_KEY_S));

	playerRotation = wrapAnglef(playerRotation + turning * PLAYER_TURN_SPEED);

	float sin, cos;
	sincosf(playerRotation, &sin, &cos);

	playerPosition.x += forward * cos * PLAYER_MOVEMENT_SPEED;
	playerPosition.y -= forward * sin * PLAYER_MOVEMENT_SPEED;
}

void playerDraw(void)
{
	/*
	// player raycasts

	const float ITER_STEP = PLAYER_FOV / (float)RENDER_RAYCAST_COUNT;

	float curAngle = playerRotation - PLAYER_FOV * 0.5f;

	glLineWidth(4);
	glColor3f(1, 0, 0);

	glBegin(GL_LINES);

	rayData collisionData;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		collisionData = castRay(playerPosition, curAngle);

		if (collisionData.hasHit)
		{
			glVertex2f(playerPosition.x, playerPosition.y);
			glVertex2f(collisionData.globalPosHit.x, collisionData.globalPosHit.y);
		}

		curAngle += ITER_STEP;
	}

	glEnd();
*/
/*
	rayData collisionData = castRay(playerPosition, playerRotation);

	if (collisionData.hasHit)
	{
		glLineWidth(4);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);

		glVertex2f(playerPosition.x, playerPosition.y);
		glVertex2f(collisionData.globalPosHit.x, collisionData.globalPosHit.y);

		glEnd();
	}
*/
/*
	// player los

	glLineWidth(8);
	glColor3f(0.8f, 0, 1);

	float sin, cos;
	sincosf(playerRotation, &sin, &cos);

	glBegin(GL_LINES);

	glVertex2f(playerPosition.x, playerPosition.y);
	glVertex2f(playerPosition.x + cos * PLAYER_BEAK_LENGTH, playerPosition.y - sin * PLAYER_BEAK_LENGTH);
	
	glEnd();

	// player body
	glPointSize(16);
	glColor3f(1, 0, 0);

	glBegin(GL_POINTS);

	glVertex2f(playerPosition.x, playerPosition.y);

	glEnd();
*/
}

