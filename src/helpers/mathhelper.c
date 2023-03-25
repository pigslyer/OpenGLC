#include "mathhelper.h"

float wrapAnglef(float angle)
{
	if (angle > TAUf)
	{
		return angle - TAUf;
	}
	if (angle < 0)
	{
		return angle + TAUf;
	}

	return angle;
}

bool isEqualApproxf(float what, float toWhat)
{
	return ABS(what - toWhat) < EPSILONF;
}

bool isZeroApproxf(float what)
{
	return ABS(what) < EPSILONF;
}

inline float distV2f(vec2f a, vec2f b)
{
	return distf(a.x, a.y, b.x, b.y);
}

float distf(float x1, float y1, float x2, float y2)
{
	float diffx = x1 - x2, diffy = y1 - y2;
	return sqrtf(diffx * diffx + diffy * diffy);
}