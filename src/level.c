#include <level.h>
#include <player.h>

#include <rendering/drawing.h>

#define POWER_OF_SIZE 6
#define CELL_SIZE (1 << POWER_OF_SIZE)
#define CELL_SIZEF ((float)(CELL_SIZE))

int mapWidth, mapHeight;
int mapArea;

int map[] = 
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,1,
	1,1,1,1,1,1,1,1,
};

rayData castRay(vec2f from, float angle)
{
	int dof = 0;

	float hDist = INFINITY, vDist = INFINITY;

	float sin, cos, tan, cotan;
	sincosf(angle, &sin, &cos);
	tan = sin / cos;
	cotan = 1 / tan;

	vec2f off;
	int mult;

	// vertical bit
	vec2f vertRes; vec2i vertMap;

	// looking right
	if (cos > 0.001f)
	{
		vertRes.x = (float)((((int)from.x) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
		vertRes.y = (from.x - vertRes.x) * tan + from.y;
		off.x = CELL_SIZEF;
		off.y = -off.x * tan;
	}
	// looking left
	else if (cos < -0.001f)
	{
		vertRes.x = (float)((((int)from.x) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
		vertRes.y = (from.x - vertRes.x) * tan + from.y;
		off.x = -CELL_SIZEF;
		off.y = -off.x * tan;
	}
	// god knows where we're looking
	else
	{
		vertRes = from;
		dof = RENDER_RAYCAST_DOF_MAX;
	}


	while (dof < RENDER_RAYCAST_DOF_MAX)
	{
		vertMap.x = ((int)vertRes.x) >> POWER_OF_SIZE;
		vertMap.y = ((int)vertRes.y) >> POWER_OF_SIZE;


		mult = vertMap.x * vertMap.y;
		if (mult >= 0 && mult < mapArea && map[vertMap.y * mapWidth + vertMap.x] != 0)
		{
			vDist = cos * (vertRes.x - from.x) - sin * (vertRes.y - from.y);
			break;
		}

		vertRes.x += off.x;
		vertRes.y += off.y;
		dof += 1;
	}

	dof = 0;

	vec2f horzRes; vec2i horzMap;

	// looking up
	if (sin > 0.001f)
	{
		horzRes.y = (float)(((int)(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
		horzRes.x = (from.y - horzRes.y) * cotan + from.x;
		off.y = -CELL_SIZEF;
		off.x = -off.y * cotan;
	}
	else if (sin < -0.001f)
	{
		horzRes.y = (float)(((int)(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
		horzRes.x = (from.y - horzRes.y) * cotan + from.x;
		off.y = CELL_SIZEF;
		off.x = -off.y * cotan;
	}
	else
	{
		horzRes.x = from.x; horzRes.y = from.y;
		dof = RENDER_RAYCAST_DOF_MAX;
	}


	while (dof < RENDER_RAYCAST_DOF_MAX)
	{
		horzMap.x = ((int)horzRes.x) >> POWER_OF_SIZE;
		horzMap.y = ((int)horzRes.y) >> POWER_OF_SIZE;

		mult = horzMap.x * horzMap.y;
		if (mult >= 0 && mult < mapArea && map[horzMap.y * mapWidth + horzMap.x] != 0)
		{
			hDist = cos * (horzRes.x - from.x) - sin * (horzRes.y - from.y);
			break;
		}

		horzRes.x += off.x;
		horzRes.y += off.y;
		dof += 1;
	}

	if (hDist == INFINITY && vDist == INFINITY)
	{
		return (rayData){false, 0, 0, 0, 0, -1};
	}

	if (hDist < vDist)
	{
		return (rayData){true, horzRes, horzMap, hDist};
	}

	return (rayData){true, vertRes, vertMap, vDist};

}

void levelInit(void)
{
	mapWidth = mapHeight = 8;
	mapArea = 64;
}

float magic = 0.0f;

void levelUpdate(void)
{
	float diff = F(GET_KEY(GLFW_KEY_T) - GET_KEY(GLFW_KEY_G));
	magic += diff;

	if (GET_KEY(GLFW_KEY_SPACE))
	{
		printf("magic number: %f\n", magic);
	}
}

void levelDraw()
{
	
//	// overhead view
//	glLineWidth(8);
//	glBegin(GL_QUADS);
//
//	vec2i tl;
//
//	for (int x = 0; x < mapWidth; x++)
//	{
//		tl.x = x * CELL_SIZE;
//
//		for (int y = 0; y < mapHeight; y++)
//		{
//			tl.y = y * CELL_SIZE;
//
//			if (map[y * mapWidth + x])
//			{
//				glColor3f(0, 0, 0);
//			}
//			else
//			{
//				glColor3f(1, 1, 1);
//			}
//
//			glVertex2i(tl.x + 1, tl.y + 1);
//			glVertex2i(tl.x + CELL_SIZE - 1, tl.y + 1);
//			glVertex2i(tl.x + CELL_SIZE - 1, tl.y + CELL_SIZE - 1);
//			glVertex2i(tl.x + 1, tl.y + CELL_SIZE - 1);
//		}
//	}
//
//	glEnd();

	// "3d" view

	const float ANGLE_STEP = PLAYER_FOV / F(RENDER_RAYCAST_COUNT);
	const float DRAW_STEP = F(VIEWPORT_WIDTH) / F(RENDER_RAYCAST_COUNT); //F(VIEWPORT_WIDTH / 2) / F(RENDER_RAYCAST_COUNT);

	float curAngle = playerRotation + PLAYER_FOV * 0.5f;
	float curDraw = DRAW_STEP * 0.5f;//F(VIEWPORT_WIDTH / 2) + DRAW_STEP * 0.5f;
	
	float lineHeight, unmaxLineHeight, lineOff;
	rayData collisionData;

	float breathingAnim = sinf(F(glfwGetTime()) * 1.2f) * 10.0f + 10.0f;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		collisionData = castRay(playerPosition, curAngle);
	
		if (collisionData.hasHit)
		{
			unmaxLineHeight = F(50 * VIEWPORT_HEIGHT) / (collisionData.rayLength * cosf(playerRotation - curAngle));
			lineHeight = MIN(unmaxLineHeight, F(VIEWPORT_HEIGHT));
			lineOff = F(VIEWPORT_HEIGHT / 2) - lineHeight * 0.5f;


			drawLineColored(curDraw, 0, curDraw, lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.4f, 0.4f, 0.4f));
			drawLineColored(curDraw, lineOff + breathingAnim, curDraw, lineHeight + lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.0f, 0.0f, 0.7f));
			drawLineColored(curDraw, lineHeight + lineOff + breathingAnim, curDraw, F(VIEWPORT_HEIGHT), DRAW_STEP, COLOR3_1(1.0f, 1.0f, 1.0f));
		}

		curDraw += DRAW_STEP;
		curAngle -= ANGLE_STEP;
	}
	
}