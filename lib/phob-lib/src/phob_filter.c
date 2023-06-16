#include "filter.h"

// Old name: runMedian
void filter_median(float *val, float valArray[MEDIANLEN], unsigned int *medianIndex)
{
	// takes the value, inserts it into the value array, and then
	//  writes the median back to the value
	valArray[*medianIndex] = *val;
	*medianIndex = (*medianIndex + 1) % MEDIANLEN;

	// We'll hardcode different sort versions according to how long the median is
	// These are derived from RawTherapee's median.h.
#if MEDIANLEN == 3
	*val = fmax(fmin(valArray[0], valArray[1]), fmin(valArray[2], fmax(valArray[0], valArray[1])));
#elif MEDIANLEN == 4
	float maximin = fmax(fmin(valArray[0], valArray[1]), fmin(valArray[2], valArray[3]));
	float minimax = fmin(fmax(valArray[0], valArray[1]), fmax(valArray[2], valArray[3]));
	*val = (maximin + minimax) / 2.0f;
#else // MEDIANLEN == 5
	float tmpArray[MEDIANLEN];
	float tmp;
	tmp = fmin(valArray[0], valArray[1]);
	tmpArray[1] = fmax(valArray[0], valArray[1]);
	tmpArray[0] = tmp;
	tmp = fmin(valArray[3], valArray[4]);
	tmpArray[4] = fmax(valArray[3], valArray[4]);
	tmpArray[3] = fmax(tmpArray[0], tmp);
	tmpArray[1] = fmin(tmpArray[1], tmpArray[4]);
	tmp = fmin(tmpArray[1], valArray[2]);
	tmpArray[2] = fmax(tmpArray[1], valArray[2]);
	tmpArray[1] = tmp;
	tmp = fmin(tmpArray[2], tmpArray[3]);
	*val = fmax(tmpArray[1], tmp);
#endif
}

// Old name: velDampFromSnapback
float filter_dampensnapback(int snapback)
{
	if (snapback >= 0)
	{
		return 0.125 * pow(2, (snapback - 4) / 3.0); // 4 should yield 0.125, 10 should yield 0.5, don't care about 0
	}
	else
	{
		return 1 - 0.25 * pow(2, (snapback + 4) / 3.0); //-1 should yield 0.5, -10 should yield 0.938, don't care about 0
	}
}

// Old name: runKalman
void filter_kalman(float *xPosFilt, float *yPosFilt, float xZ, float yZ, Settings_s *settings, FilterGains_s *normGains)
{
	// Serial.println("Running Kalman");

	// kalman filter state variables saved across iterations
	static float xPos = 0; // input of kalman filter
	static float yPos = 0; // input of kalman filter
	static float xVel = 0;
	static float yVel = 0;
	static float xVelFilt = 0;
	static float yVelFilt = 0;

	// save previous values of state
	float oldXPos = xPos;
	float oldYPos = yPos;
	float oldXPosFilt = *xPosFilt;
	float oldYPosFilt = *yPosFilt;
	float oldXVel = xVel;
	float oldYVel = yVel;
	float oldXVelFilt = xVelFilt;
	float oldYVelFilt = yVelFilt;

	// compute new (more trivial) state
	xPos = xZ;
	yPos = yZ;
	xVel = xPos - oldXPos;
	yVel = yPos - oldYPos;
	float xVelSmooth = 0.5 * (xVel + oldXVel);
	float yVelSmooth = 0.5 * (yVel + oldYVel);
	float xAccel = xVel - oldXVel;
	float yAccel = yVel - oldYVel;
	float oldXPosDiff = oldXPos - oldXPosFilt;
	float oldYPosDiff = oldYPos - oldYPosFilt;

	// compute stick position exponents for weights
	float stickDistance2 = fmin(normGains->maxStick, xPos * xPos + yPos * yPos) / normGains->maxStick; // 0-1
	float stickDistance6 = stickDistance2 * stickDistance2 * stickDistance2;

	// the current velocity weight for the filtered velocity is the stick r^2
	float velWeight1 = stickDistance2;
	float velWeight2 = 1 - velWeight1;

	// modified velocity to feed into our kalman filter.
	// We don't actually want an accurate model of the velocity, we want to suppress snapback without adding delay
	// term 1: weight current velocity according to r^2
	// term 2: the previous filtered velocity, weighted the opposite and also set to decay
	// term 3: a corrective factor based on the disagreement between real and filtered position

	// the current position weight used for the filtered position is whatever is larger of
	//   a) 1 minus the sum of the squares of
	//     1) the smoothed velocity divided by the velocity threshold
	//     2) the acceleration divided by the accel threshold
	//   b) stick r^6
	// When the stick is moving slowly, we want to weight it highly, in order to achieve
	//   quick control for inputs such as tilts. We lock out using both velocity and
	//   acceleration in order to rule out snapback.
	// When the stick is near the rim, we also want instant response, and we know snapback
	//   doesn't reach the rim.

	// In calculating the filtered stick position, we have the following components
	// term 1: current position, weighted according to the above weight
	// term 2: a predicted position based on the filtered velocity and previous filtered position,
	//   with the filtered velocity damped, and the overall term weighted inverse of the previous term
	// term 3: the integral error correction term

	// But if we xSnapback or ySnapback is zero, we skip the calculation
	if (settings->xSnapback > 0)
	{
		xVelFilt = velWeight1 * xVel + (1 - normGains->xVelDecay) * velWeight2 * oldXVelFilt + normGains->xVelPosFactor * oldXPosDiff;

		float xPosWeightVelAcc = 1 - fmin(1, xVelSmooth * xVelSmooth * normGains->velThresh + xAccel * xAccel * normGains->accelThresh);
		float xPosWeight1 = fmax(xPosWeightVelAcc, stickDistance6);
		float xPosWeight2 = 1 - xPosWeight1;

		*xPosFilt = xPosWeight1 * xPos +
								xPosWeight2 * (oldXPosFilt + (1 - normGains->xVelDamp) * xVelFilt);
	}
	else if (settings->xSnapback < 0)
	{
		float xLPF = oldXPosFilt * normGains->xVelDamp + xPos * (1 - normGains->xVelDamp);

		float xPosWeightVelAcc = 1 - fmin(1, xVelSmooth * xVelSmooth * normGains->velThresh + xAccel * xAccel * normGains->accelThresh);
		float xPosWeight1 = fmax(xPosWeightVelAcc, stickDistance6);
		float xPosWeight2 = 1 - xPosWeight1;

		*xPosFilt = xPosWeight1 * xPos + xPosWeight2 * xLPF;
	}
	else
	{
		*xPosFilt = xPos;
	}

	if (settings->ySnapback > 0)
	{
		yVelFilt = velWeight1 * yVel + (1 - normGains->yVelDecay) * velWeight2 * oldYVelFilt + normGains->yVelPosFactor * oldYPosDiff;

		float yPosWeightVelAcc = 1 - fmin(1, yVelSmooth * yVelSmooth * normGains->velThresh + yAccel * yAccel * normGains->accelThresh);
		float yPosWeight1 = fmax(yPosWeightVelAcc, stickDistance6);
		float yPosWeight2 = 1 - yPosWeight1;

		*yPosFilt = yPosWeight1 * yPos +
								yPosWeight2 * (oldYPosFilt + (1 - normGains->yVelDamp) * yVelFilt);
	}
	else if (settings->ySnapback < 0)
	{
		float yLPF = oldYPosFilt * normGains->yVelDamp + yPos * (1 - normGains->yVelDamp);

		float yPosWeightVelAcc = 1 - fmin(1, yVelSmooth * yVelSmooth * normGains->velThresh + yAccel * yAccel * normGains->accelThresh);
		float yPosWeight1 = fmax(yPosWeightVelAcc, stickDistance6);
		float yPosWeight2 = 1 - yPosWeight1;

		*yPosFilt = yPosWeight1 * yPos + yPosWeight2 * yLPF;
	}
	else
	{
		*yPosFilt = yPos;
	}
};

// Old name: calcWaveshapeMult
// The input setting should range from 0 to 15.
// The output should be 0 for 0.
float filter_waveshapemultiply(int setting)
{
	if (setting > 0 && setting <= 5)
	{
		return 1.0 / (440 - 40 * setting);
	}
	else if (setting > 5 && setting <= 15)
	{
		return 1.0 / (340 - 20 * setting);
	}
	else
	{
		return 0;
	}
}

// Old name: aRunWaveShaping
// This simulates an idealized sort of pode:
//  if the stick is moving fast, it responds poorly, while
//  if the stick is moving slowly, it follows closely.
// It's not suitable to be the sole filter, but when put after
//  the smart snapback filter, it should be able to hold the
//  output at the rim longer when released.
void filter_waveshaping(WhichStick_t stick, AnalogPairFloat_s *analog, Settings_s *settings, FilterGains_s *normGains)
{
	volatile static float oldCXPos = 0;
	volatile static float oldCYPos = 0;
	volatile static float oldCXVel = 0;
	volatile static float oldCYVel = 0;

	volatile static float oldCXOut = 0;
	volatile static float oldCYOut = 0;

	volatile static float oldAXPos = 0;
	volatile static float oldAYPos = 0;
	volatile static float oldAXVel = 0;
	volatile static float oldAYVel = 0;

	volatile static float oldAXOut = 0;
	volatile static float oldAYOut = 0;

	float *oldXPos = &oldAXPos;
	float *oldYPos = &oldAYPos;
	float *oldXVel = &oldAXVel;
	float *oldYVel = &oldAYVel;
	float *oldXOut = &oldAXOut;
	float *oldYOut = &oldAYOut;

	if (stick == C_STICK)
	{
		oldXPos = &oldCXPos;
		oldYPos = &oldCYPos;
		oldXVel = &oldCXVel;
		oldYVel = &oldCYVel;
		oldXOut = &oldCXOut;
		oldYOut = &oldCYOut;
	}

	float xVel = analog->x - *oldXPos;
	float yVel = analog->y - *oldYPos;
	float xVelSmooth = 0.5 * (xVel + *oldXVel);
	float yVelSmooth = 0.5 * (yVel + *oldYVel);

	// The lower this value, the stronger the effect.
	// Per Rienne's experimentation:
	// Minimum setting should be 500; this does nearly nothing
	// Sweetspot for higher stick speeds is 200-240
	// Sweetspot for lower stick speeds is 160-200
	// Frame timing goes weird around 160
	// max functional setting probably 80
	// extreme pode is like 32-80
	// 32 should be the limit

	float xFactor = calcWaveshapeMult(controls->axWaveshaping);
	float yFactor = calcWaveshapeMult(controls->ayWaveshaping);

	float oldXPosWeight = fmin(1, xVelSmooth * xVelSmooth * normGains->velThresh * xFactor);
	float newXPosWeight = 1 - oldXPosWeight;
	float oldYPosWeight = fmin(1, yVelSmooth * yVelSmooth * normGains->velThresh * yFactor);
	float newYPosWeight = 1 - oldYPosWeight;

	analog->x = *oldXOut * oldXPosWeight + analog->x * newXPosWeight;
	analog->y = *oldYOut * oldYPosWeight + analog->y * newYPosWeight;

	*oldXPos = xPos;
	*oldYPos = yPos;
	*oldXVel = xVel;
	*oldYVel = yVel;

	*oldXOut = analog->x;
	*oldYOut = analog->y;
}
