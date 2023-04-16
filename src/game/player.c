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
	float turning = F(GET_KEY(GLFW_KEY_A) - GET_KEY(GLFW_KEY_D));
	float forward = F(GET_KEY(GLFW_KEY_W) - GET_KEY(GLFW_KEY_S));

	playerRotation = wrapAnglef(playerRotation + turning * PLAYER_TURN_SPEED);

	float sin, cos;
	sincosf(playerRotation, &sin, &cos);

	rayResults collisionResult;
	rayData* finalResult;
	vec2f speed = VEC2F2_1(forward * cos * PLAYER_MOVEMENT_SPEED, forward * sin * PLAYER_MOVEMENT_SPEED);

	playerPosition.x += speed.x;
	playerPosition.y -= speed.y;

	return;

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

}

