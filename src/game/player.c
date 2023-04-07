#include <game/player.h>
#include <game/level.h>

#define PLAYER_MOVEMENT_SPEED 5
#define PLAYER_TURN_SPEED 0.08f
#define PLAYER_COLLISION_LOOKAHEAD 5

#define PLAYER_BEAK_LENGTH 40

vec2f playerPosition;
float playerRotation;

void playerInit(void)
{
	playerPosition.x = playerPosition.y = 300;
	playerRotation = PIf;
}

#define MAX_SLIDES 8

void playerUpdate(void)
{
	float turning = (float)(GET_KEY(GLFW_KEY_A) - GET_KEY(GLFW_KEY_D));
	float forward = (float)(GET_KEY(GLFW_KEY_W) - GET_KEY(GLFW_KEY_S));

	playerRotation = wrapAnglef(playerRotation + turning * PLAYER_TURN_SPEED);

	float sin, cos;
	sincosf(playerRotation, &sin, &cos);

	rayResults collisionResult;
	rayData* finalResult;
	vec2f speed = VEC2F2_1(forward * cos * PLAYER_MOVEMENT_SPEED, forward * sin * PLAYER_MOVEMENT_SPEED);

	// move and slide implementation
	for (int slideCount = 0; slideCount < MAX_SLIDES && (speed.x != 0 || speed.y != 0); slideCount++)
	{
		castRayV(playerPosition, speed, &collisionResult);

		// there's absolutely nothing in our way
		if (collisionResult.resultCount == 0)
		{
			playerPosition.x += speed.x;
			playerPosition.y -= speed.y;
			break;
		}

		finalResult = &(collisionResult.results[collisionResult.resultCount - 1]);

		if (SQR(finalResult->rayLength) > lenV2fSqr(speed) || true)
		{
			playerPosition.x += speed.x;
			playerPosition.y -= speed.y;
			break;
		}

		printf("detected a collision\n");
		// first configure new speed since we need the previous player position to figure out "actual" speed delta
		speed.x = (collisionResult.normal.x == 0) * (speed.x - (finalResult->globalPosHit.x - playerPosition.x));
		speed.y = (collisionResult.normal.y == 0) * (speed.y - (finalResult->globalPosHit.y - playerPosition.y));

		// move to wall collision point, pushed away from the wall by collision lookahead (so we can't rub out face on the wall)
		playerPosition.x = finalResult->globalPosHit.x + collisionResult.normal.x * PLAYER_COLLISION_LOOKAHEAD;
		playerPosition.y = finalResult->globalPosHit.y + collisionResult.normal.y * PLAYER_COLLISION_LOOKAHEAD;
	}
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

