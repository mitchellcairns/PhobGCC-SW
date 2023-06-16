#include "phob_math.h"

/*
 * Convert the x/y coordinates (actually angles on a sphere) to an azimuth
 * We first convert to a 3D coordinate and then drop to 2D, then arctan it
 * This does the opposite of calcStickValues, ideally.
 */
void math_angleonsphere(const float x, const float y, float *angle)
{
	float xx = sinf(x * MAX_STICK_ANGLE / 100) * cosf(y * MAX_STICK_ANGLE / 100);
	float yy = cosf(x * MAX_STICK_ANGLE / 100) * sinf(y * MAX_STICK_ANGLE / 100);
	*angle = atan2f(yy, xx); // WHY IS THIS BACKWARDS
	if (*angle < 0)
	{
		*angle += 2 * M_PI;
	}
};

/*
 * calcStickValues computes the stick x/y coordinates from angle.
 * This requires weird trig because the stick moves spherically.
 */
void math_angletocoords(float angle, float *x, float *y)
{
	*x = 100 * atan2f((sinf(MAX_STICK_ANGLE) * cosf(angle)), cosf(MAX_STICK_ANGLE)) / MAX_STICK_ANGLE;
	*y = 100 * atan2f((sinf(MAX_STICK_ANGLE) * sinf(angle)), cosf(MAX_STICK_ANGLE)) / MAX_STICK_ANGLE;
};

float math_linearize(float point, float *coefficients)
{
	return (coefficients[0] * (point * point * point) + coefficients[1] * (point * point) + coefficients[2] * point + coefficients[3]);
};

/*
 * computeStickAngles
 * write all the stick angles into the notch angles array array
 * inputs need to be length NOTCH_COUNT+1
 * outputs need to be length NOTCH_COUNT
 */
void math_coordstoangles(CalPoints_s *calPoints, float stickAngles[])
{
  float *xInput = &calPoints->nxPoints;
  float *yInput = &calPoints->nyPoints;

	debug_println("Computed stick angles:");
	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		if (i % 2 == 0)
		{ // cardinal or diagonal
			stickAngles[i] = _notchAngleDefaults[i];
		}
		else
		{
			math_angleonsphere(xInput[i + 1], yInput[i + 1], &stickAngles[i]);
		}
	}
};
