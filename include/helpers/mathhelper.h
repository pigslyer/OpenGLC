#ifndef MATHHELPER_H_SEEN
#define MATHHELPER_H_SEEN

#include <global.h>

#define PI 3.1415926535
#define TAU PI * 2

#define PIf 3.1415926535f
#define TAUf PIf * 2

#define EPSILONF 1.401298E-45f

#define DEG2RADF(deg) deg * (PIf / 180)
#define RAD2DEGF(rad) rad * (180 / PIf)

#define MAX(val1, val2) (val1) > (val2) ? (val1) : (val2)
#define MIN(val1, val2) (val1) < (val2) ? (val1) : (val2)
#define ABS(value) (value) > 0 ? (value) : (-value)

struct vec2i
{
	int x,y;
} typedef vec2i;

struct vec2f
{
	float x, y;
} typedef vec2f;

float wrapAnglef(float angle);

bool isEqualApproxf(float what, float toWhat);
bool isZeroApproxf(float what);

float distV2f(vec2f a, vec2f b);
float distf(float x1, float y1, float x2, float y2);

#endif