#include "stick_cal.h"

float _defaultCalPointsX[] = {
		0.3010610568, 0.3603937084, // right
		0.3010903951, 0.3000194135,
		0.3005567843, 0.3471911134, // up right
		0.3006904343, 0.3009976295,
		0.3000800899, 0.300985051, // up
		0.3001020858, 0.300852804,
		0.3008746305, 0.2548450139, // up left
		0.3001434092, 0.3012600593,
		0.3011594091, 0.2400535218, // left
		0.3014621077, 0.3011248469,
		0.3010860944, 0.2552106305, // down left
		0.3002197989, 0.3001679513,
		0.3004438517, 0.300486505, // down
		0.3002766984, 0.3012828579,
		0.3014959877, 0.346512936, // down right
		0.3013398149, 0.3007809916};

float _defaultCalPointsY[] = {
		0.300092277, 0.3003803475, // right
		0.3002205792, 0.301004752,
		0.3001241394, 0.3464200104, // up right
		0.3001331245, 0.3011881186,
		0.3010685972, 0.3606900641, // up
		0.3001520488, 0.3010662947,
		0.3008837105, 0.3461478452, // up left
		0.3011732026, 0.3007367683,
		0.3011345742, 0.3000566197, // left
		0.3006843288, 0.3009673425,
		0.3011228978, 0.2547579852, // down left
		0.3011177285, 0.301264851,
		0.3002376991, 0.2403885431, // down
		0.3006540818, 0.3010588401,
		0.3011093054, 0.2555000655, // down right
		0.3000802760, 0.3008482317};

// Defaults
//                                                          right        notch 1      up right     notch 2      up           notch 3      up left      notch 4      left         notch 5      down left    notch 6      down         notch 7      down right   notch 8
//                                                          0            1            2            3            4            5            6            7            8            9            10           11           12           13           14           15
int _calOrder[] = {0, 1, 8, 9, 16, 17, 24, 25, 4, 5, 12, 13, 20, 21, 28, 29, 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 26, 27, 30, 31};
float _notchAngleDefaults[] = {0, M_PI / 8.0, M_PI * 2 / 8.0, M_PI * 3 / 8.0, M_PI * 4 / 8.0, M_PI * 5 / 8.0, M_PI * 6 / 8.0, M_PI * 7 / 8.0, M_PI * 8 / 8.0, M_PI * 9 / 8.0, M_PI * 10 / 8.0, M_PI * 11 / 8.0, M_PI * 12 / 8.0, M_PI * 13 / 8.0, M_PI * 14 / 8.0, M_PI * 15 / 8.0};
NotchStatus_t _notchStatusDefaults[] = {CARDINAL, TERT_ACTIVE, SECONDARY, TERT_ACTIVE, CARDINAL, TERT_ACTIVE, SECONDARY, TERT_ACTIVE, CARDINAL, TERT_ACTIVE, SECONDARY, TERT_ACTIVE, CARDINAL, TERT_ACTIVE, SECONDARY, TERT_ACTIVE};
//                                                         up right     up left      down left    down right   notch 1      notch 2      notch 3      notch 4      notch 5      notch 6      notch 7      notch 8
int _notchAdjOrder[] = {2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};

/*
 * stripCalPoints removes the notches from un-cleaned cal points
 * this is so we can get the original values of the notches after the affine transform.
 * there need to be NOTCH_CALIBRATION_POINTS values in the inputs and outputs.
 */
void calibration_stripnotches(const float calPointsX[], const float calPointsY[], float strippedPointsX[], float strippedPointsY[])
{
	for (int i = 0; i < NOTCH_CALIBRATION_POINTS; i++)
	{
		// start off by just copying them wholesale
		strippedPointsX[i] = calPointsX[i];
		strippedPointsY[i] = calPointsY[i];
		if ((i + 1) % 4 == 0)
		{																			// non-cardinal non-diagonal notch (every fourth starting at index 3)
			strippedPointsX[i] = calPointsX[0]; // set equal to origin
			strippedPointsY[i] = calPointsY[0];
		}
	}
};


// sets notches to measured values if absent
void cleanNotches(float notchAngles[], float measuredNotchAngles[], NotchStatus_t notchStatus[])
{
	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		if (notchStatus[i] == TERT_INACTIVE)
		{
			notchAngles[i] = measuredNotchAngles[i];
		}
	}
};

/*******************
	notchRemap
	Remaps the stick position using affine transforms generated from the notch positions
*******************/
void notchRemap(const float xIn, const float yIn, float *xOut, float *yOut, const int regions, const StickParams_s *stickParams, int currentCalStep, Settings_s *controls, const WhichStick_t whichStick)
{
	// determine the angle between the x unit vector and the current position vector
	float angle = atan2f(yIn, xIn);

	// unwrap the angle based on the first region boundary
	if (angle < stickParams->boundaryAngles[0])
	{
		angle += M_PI * 2;
	}

	// go through the region boundaries from lowest angle to highest, checking if the current position vector is in that region
	// if the region is not found then it must be between the first and the last boundary, ie the last region
	// we check GATE_REGIONS*2 because each notch has its own very small region we use to make notch values more consistent
	// int region = regions*2-1;
	int region = regions - 1;
	for (int i = 1; i < regions; i++)
	{
		if (angle < stickParams->boundaryAngles[i])
		{
			region = i - 1;
			break;
		}
	}

	// Apply the affine transformation using the coefficients found during calibration
	*xOut = stickParams->affineCoeffs[region][0] * xIn + stickParams->affineCoeffs[region][1] * yIn;
	*yOut = stickParams->affineCoeffs[region][2] * xIn + stickParams->affineCoeffs[region][3] * yIn;

	float stickScale;
	if (whichStick == A_STICK)
	{
		stickScale = controls->astickAnalogScaler / 100.0f;
	}
	else
	{
		stickScale = controls->cstickAnalogScaler / 100.0f;
	}

	*xOut *= stickScale;
	*yOut *= stickScale;

	if (currentCalStep == -1)
	{

		if (whichStick == A_STICK)
		{
			if (controls->astickCardinalSnapping > 0)
			{
				if ((abs(*xOut) < controls->astickCardinalSnapping + 0.5) && (abs(*yOut) >= 79.5))
				{
					*xOut = 0;
				}
				if ((abs(*yOut) < controls->astickCardinalSnapping + 0.5) && (abs(*xOut) >= 79.5))
				{
					*yOut = 0;
				}
			}
			else if (controls->astickCardinalSnapping == -1)
			{
				if ((abs(*xOut) < 6.5) && (abs(*yOut) >= 79.5))
				{
					*xOut = 7;
				}
				if ((abs(*yOut) < 6.5) && (abs(*xOut) >= 79.5))
				{
					*yOut = 7;
				}
			}
		}
		else
		{
			if (controls->cstickCardinalSnapping > 0)
			{
				if ((abs(*xOut) < controls->cstickCardinalSnapping + 0.5) && (abs(*yOut) >= 79.5))
				{
					*xOut = 0;
				}
				if ((abs(*yOut) < controls->cstickCardinalSnapping + 0.5) && (abs(*xOut) >= 79.5))
				{
					*yOut = 0;
				}
			}
			else if (controls->cstickCardinalSnapping == -1)
			{
				if ((abs(*xOut) < 6.5) && (abs(*yOut) >= 79.5))
				{
					*xOut = 7;
				}
				if ((abs(*yOut) < 6.5) && (abs(*xOut) >= 79.5))
				{
					*yOut = 7;
				}
			}
		}

		if ((abs(*xOut) < 3) && (abs(*yOut) < 3))
		{
			*xOut = 0;
			*yOut = 0;
		}
	}
};

/*
 * transformCalPoints
 * remaps the cleaned calibration points from raw measurements to output coordinates
 * This seems redundant but we're feeding it coordinates without non-diagonal notches
 */
void transformCalPoints(const float xInput[], const float yInput[], float xOutput[], float yOutput[], const StickParams_s *stickParams, const ControlConfig_s *controls, const WhichStick_t whichStick)
{
	for (int i = 0; i < NOTCH_COUNT + 1; i++)
	{
		float xValue = math_linearize(xInput[i], stickParams->fitCoeffsX);
		float yValue = math_linearize(yInput[i], stickParams->fitCoeffsY);
		float outX;
		float outY;
		notchRemap(xValue, yValue, &outX, &outY, NOTCH_COUNT, stickParams, 0, controls, whichStick);
		xOutput[i] = outX;
		yOutput[i] = outY;
	}
};

/*******************
	cleanCalPoints
	take the x and y coordinates and notch angles collected during the calibration procedure,
	and generate the cleaned (non-redundant) x and y stick coordinates and the corresponding x and y notch coordinates
*******************/
void cleanCalPoints(float *calPointsX, float *calPointsY, float *notchAngles, CalPoints_s *out_cal, NotchStatus_t notchStatus[])
{
	// Set up pointers to make it easier to read
	float *notchPointsX = &out_cal->xPoints;
	float *notchPointsY = &out_cal->yPoints;
	float *cleanedPointsX = &out_cal->xPoints;
	float *cleanedPointsY = &out_cal->yPoints;

	debug_println("The raw calibration points (x,y) are:");
	for (int i = 0; i < NOTCH_CALIBRATION_POINTS; i++)
	{
		debug_print(calPointsX[i], 4);
		debug_print(",");
		debug_println(calPointsY[i], 4);
	}

	debug_println("The notch angles are:");
	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		debug_println(notchAngles[i], 4);
	}

	notchPointsX[0] = 0;
	notchPointsY[0] = 0;
	cleanedPointsX[0] = 0;
	cleanedPointsY[0] = 0;

	debug_println("The notch points are:");
	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		// add the origin values to the first x,y point
		cleanedPointsX[0] += calPointsX[i * 2];
		cleanedPointsY[0] += calPointsY[i * 2];

		// copy the cal point into the cleaned list
		cleanedPointsX[i + 1] = calPointsX[i * 2 + 1];
		cleanedPointsY[i + 1] = calPointsY[i * 2 + 1];

		// convert notch angles to x/y coords (weird since the stick moves spherically)
		math_angletocoords(notchAngles[i], notchPointsX + i + 1, notchPointsY + i + 1);
		notchPointsX[i + 1] = round(notchPointsX[i + 1]);
		notchPointsY[i + 1] = round(notchPointsY[i + 1]);

		debug_print(notchPointsX[i + 1]);
		debug_print(",");
		debug_println(notchPointsY[i + 1]);
	}

	// remove the largest and smallest two origin values to remove outliers
	// first, find their indices
	int smallestX = 0;
	int smallX = 0;
	int largeX = 0;
	int largestX = 0;
	int smallestY = 0;
	int smallY = 0;
	int largeY = 0;
	int largestY = 0;
	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		if (calPointsX[i * 2] < calPointsX[smallestX])
		{											// if it's the new smallest
			smallX = smallestX; // shuffle the old smallest to small
			smallestX = i * 2;	// record the new smallest index
		}
		else if (calPointsX[i * 2] < calPointsX[smallX])
		{									// if it's the new second-smallest
			smallX = i * 2; // record the new small index
		}
		if (calPointsX[i * 2] > calPointsX[largestX])
		{										 // if it's the new largest
			largeX = largestX; // shuffle the old largest to large
			largestX = i * 2;	 // record the new largest index
		}
		else if (calPointsX[i * 2] > calPointsX[largeX])
		{									// if it's the new second-largest
			largeX = i * 2; // record the new large index
		}
		if (calPointsY[i * 2] < calPointsY[smallestY])
		{
			smallY = smallestY;
			smallestY = i * 2;
		}
		else if (calPointsY[i * 2] < calPointsY[smallY])
		{
			smallY = i * 2;
		}
		if (calPointsY[i * 2] > calPointsY[largestY])
		{
			largeY = largestY;
			largestY = i * 2;
		}
		else if (calPointsY[i * 2] > calPointsY[largeY])
		{
			largeY = i * 2;
		}
	}
	// subtract the smallest and largest values
	cleanedPointsX[0] -= calPointsX[smallestX];
	cleanedPointsX[0] -= calPointsX[smallX];
	cleanedPointsX[0] -= calPointsX[largeX];
	cleanedPointsX[0] -= calPointsX[largestX];
	cleanedPointsY[0] -= calPointsY[smallestY];
	cleanedPointsY[0] -= calPointsY[smallY];
	cleanedPointsY[0] -= calPointsY[largeY];
	cleanedPointsY[0] -= calPointsY[largestY];

	// divide by the total number of calibration steps/2 to get the average origin value
	// except it's minus 4 steps since we removed outliers
	cleanedPointsX[0] = cleanedPointsX[0] / ((float)NOTCH_COUNT - 4);
	cleanedPointsY[0] = cleanedPointsY[0] / ((float)NOTCH_COUNT - 4);

	for (int i = 0; i < NOTCH_COUNT; i++)
	{
		// calculate radius of cleaned point from center
		float deltaX = cleanedPointsX[i + 1] - cleanedPointsX[0];
		float deltaY = cleanedPointsY[i + 1] - cleanedPointsY[0];
		float mag = sqrt(deltaX * deltaX + deltaY * deltaY);

		if (mag < 0.02 && (i % 2) != 0)
		{ // if the cleaned point was at the center and would be a firefox notch
			// average the previous and next points (cardinal & diagonal) for some sanity
			int prevIndex = ((i - 1 + NOTCH_COUNT) % NOTCH_COUNT) + 1;
			int nextIndex = ((i + 1) % NOTCH_COUNT) + 1;

			cleanedPointsX[i + 1] = (cleanedPointsX[prevIndex] + cleanedPointsX[nextIndex]) / 2.0;
			cleanedPointsY[i + 1] = (cleanedPointsY[prevIndex] + cleanedPointsY[nextIndex]) / 2.0;

			notchPointsX[i + 1] = (notchPointsX[prevIndex] + notchPointsX[nextIndex]) / 2.0;
			notchPointsY[i + 1] = (notchPointsY[prevIndex] + notchPointsY[nextIndex]) / 2.0;

			debug_print("no input was found for notch: ");
			debug_println(i + 1);

			// Mark that notch adjustment should be skipped for this
			notchStatus[i] = TERT_INACTIVE;
		}
		else
		{
			notchStatus[i] = _notchStatusDefaults[i];
		}
	}

	debug_println("The cleaned calibration points are:");
	for (int i = 0; i < (NOTCH_COUNT + 1); i++)
	{
		debug_print(cleanedPointsX[i], 4);
		debug_print(",");
		debug_println(cleanedPointsY[i], 4);
	}

	debug_println("The corresponding notch points are:");
	for (int i = 0; i < (NOTCH_COUNT + 1); i++)
	{
		debug_print(notchPointsX[i]);
		debug_print(",");
		debug_println(notchPointsY[i]);
	}

	debug_println("The notch statuses are:");
	for (int i = 0; i < (NOTCH_COUNT); i++)
	{
		debug_println(notchStatus[i]);
	}
};

// The notch adjustment is limited in order to control
// 1. displacement of points (max 12 units out of +/- 100, for now)
// 2. stretching of coordinates (max +/- 30%)
void legalizeNotch(const int notchIndex, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus[])
{
	// Limit the notch adjustment

	// Start out with the limits being 12 units around the circle at the gate
	/*this may be unnecessary in our case, because 12 units is also the 30% stretch limit
	float lowerPosLimit = measuredNotchAngles[notchIndex] - 12/100.f;
	float upperPosLimit = measuredNotchAngles[notchIndex] + 12/100.f;
	if(upperPosLimit < lowerPosLimit){
		upperPosLimit += 2*M_PI;
	}
	*/

	bool isDiagonal = false;

	// Now we need to determine the stretch/compression limit
	// Figure out the previous and next notch angles.
	// For most they're the adjacent notches.
	int prevIndex = (notchIndex - 1 + NOTCH_COUNT) % NOTCH_COUNT;
	int nextIndex = (notchIndex + 1) % NOTCH_COUNT;
	// For diagonals, the cardinals are the index points.
	if ((notchIndex - 2) % 4 == 0)
	{
		prevIndex = (notchIndex - 2 + NOTCH_COUNT) % NOTCH_COUNT;
		nextIndex = (notchIndex + 2) % NOTCH_COUNT;
		isDiagonal = true;
	}
	float prevAngle = notchAngles[prevIndex];
	float nextAngle = notchAngles[nextIndex];
	if (nextAngle < prevAngle)
	{
		nextAngle += 2 * M_PI;
	}
	float prevMeasAngle = measuredNotchAngles[prevIndex];
	float thisMeasAngle = measuredNotchAngles[notchIndex];
	float nextMeasAngle = measuredNotchAngles[nextIndex];
	if (nextMeasAngle < thisMeasAngle)
	{
		nextMeasAngle += 2 * M_PI;
	}

	float cmpAmt;
	float strAmt;
	if (isDiagonal)
	{ // tighter bounds for diagonals
		cmpAmt = 0.769;
		strAmt = 1.3;
	}
	else
	{ // wider bounds for modder notches
		cmpAmt = 0.666;
		strAmt = 1.5;
	}

	float lowerCompressLimit = prevAngle + cmpAmt * (thisMeasAngle - prevMeasAngle); // how far we can squish when reducing the angle
	float lowerStretchLimit = nextAngle - strAmt * (nextMeasAngle - thisMeasAngle);	 // how far we can stretch when reducing the angle
	float upperCompressLimit = nextAngle - cmpAmt * (nextMeasAngle - thisMeasAngle); // how far we can squish when increasing the angle
	float upperStretchLimit = prevAngle + strAmt * (thisMeasAngle - prevMeasAngle);	 // how far we can stretch when increasing the angle

	// Now, in order to apply stretch leniency to angles within the deadzone,
	//  we need to figure out whether the previous angle or next angle was a cardinal.
	// If the previous one is a cardinal AND the angle is in the deadzone, we make the upperstretchlimit bigger, only if it can't reach 0.3000.
	const float minThreshold = 0.1500 / 0.9750;	 // radians; we don't want to fix things smaller than this
	const float deadzoneLimit = 0.2875 / 0.9500; // radians; or things larger than this
	const float deadzonePlus = 0.3250 / 0.9375;	 // radians; we want to make sure the adjustment can make it here
	if (prevIndex % 4 == 0 && !isDiagonal && (thisMeasAngle - prevMeasAngle) > minThreshold && (thisMeasAngle - prevMeasAngle) < deadzoneLimit)
	{
		upperStretchLimit = prevAngle + fmax(strAmt * (thisMeasAngle - prevMeasAngle), deadzonePlus);
	}
	// If the next one is a cardinal AND the angle is in the deadzone, we make the lowerstretchlimit smaller.
	if (nextIndex % 4 == 0 && !isDiagonal && (nextMeasAngle - thisMeasAngle) > minThreshold && (nextMeasAngle - thisMeasAngle) < deadzoneLimit)
	{
		lowerStretchLimit = nextAngle - fmax(strAmt * (nextMeasAngle - thisMeasAngle), deadzonePlus);
	}

	float lowerDistortLimit = fmax(lowerCompressLimit, lowerStretchLimit);
	float upperDistortLimit = fmin(upperCompressLimit, upperStretchLimit);
	if (upperDistortLimit < lowerDistortLimit)
	{
		upperDistortLimit += 2 * M_PI;
	}

	// Combine the limits
	float lowerLimit = lowerDistortLimit; // fmax(lowerStretchLimit, lowerPosLimit);
	float upperLimit = upperDistortLimit; // fmin(upperStretchLimit, upperPosLimit);
	if (upperLimit < lowerLimit)
	{
		upperLimit += 2 * M_PI;
	}

	// Apply the limits
	notchAngles[notchIndex] = fmax(notchAngles[notchIndex], lowerLimit);
	notchAngles[notchIndex] = fmin(notchAngles[notchIndex], upperLimit);
}

void legalizeNotches(const int currentStepIn, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus[])
{
	for (int i = currentStepIn; i < 44; i++)
	{
		const int notchIndex = _notchAdjOrder[i - NOTCH_CALIBRATION_POINTS];
		legalizeNotch(notchIndex, measuredNotchAngles, notchAngles, notchStatus);
	}
}

// adjustNotch is used to adjust the angles of the notch.
// It is run after calibration points are collected.
// It runs a legalization routine to limit subsequent notches
void adjustNotch(const int currentStepIn, const float loopDelta, const WhichStick_t whichStick, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus[], Buttons_u *btn, Buttons_u *hardware)
{
	// set up variables based on current button state
	bool CW = hardware->X;
	bool CCW = hardware->Y;
	bool reset = btn->B;

	// This gets run after all the calibration points are collected
	// So we subtract the number of calibration points and switch over to notch adjust order
	const int notchIndex = _notchAdjOrder[currentStepIn - NOTCH_CALIBRATION_POINTS];

	// display the desired value on the other stick
	float x = 0;
	float y = 0;
	math_angletocoords(measuredNotchAngles[notchIndex], &x, &y);
	if (whichStick == A_STICK)
	{
		btn->Cx = (uint8_t)(x + FLOAT_ORIGIN);
		btn->Cy = (uint8_t)(y + FLOAT_ORIGIN);
	}
	else
	{
		btn->Ax = (uint8_t)(x + FLOAT_ORIGIN);
		btn->Ay = (uint8_t)(y + FLOAT_ORIGIN);
	}

	// do nothing if it's not a valid notch to calibrate
	// it'll skip them anyway but just in case
	if (notchStatus[notchIndex] == TERT_INACTIVE)
	{
		return;
	}

	// Adjust notch angle according to which button is pressed (do nothing for both buttons)
	if (CW && !CCW)
	{
		notchAngles[notchIndex] -= loopDelta * 0.000075;
	}
	else if (CCW && !CW)
	{
		notchAngles[notchIndex] += loopDelta * 0.000075;
	}
	else if (reset)
	{
		notchAngles[notchIndex] = measuredNotchAngles[notchIndex];
	}
	else
	{
		return;
	}

	legalizeNotches(currentStepIn, measuredNotchAngles, notchAngles, notchStatus);
	// legalizeNotch(notchIndex, measuredNotchAngles, notchAngles, notchStatus);
};

// displayNotch is used in lieu of adjustNotch when doing basic calibration
void displayNotch(const int currentStepIn, const bool calibratingAStick, const float notchAngles[], Buttons_u *btn)
{
	int currentStep = _calOrder[currentStepIn];
	// display the desired value on the other stick
	float x = 0;
	float y = 0;
	if (currentStep % 2)
	{
		const int notchIndex = currentStep / 2;
		math_angletocoords(notchAngles[notchIndex], &x, &y);
	}
	if (calibratingAStick)
	{
		btn->Cx = (uint8_t)(x + FLOAT_ORIGIN);
		btn->Cy = (uint8_t)(y + FLOAT_ORIGIN);
	}
	else
	{
		btn->Ax = (uint8_t)(x + FLOAT_ORIGIN);
		btn->Ay = (uint8_t)(y + FLOAT_ORIGIN);
	}
};

void insertCalPoints(const WhichStick_t whichStick, const int currentStepIn, float calPointsX[], float calPointsY[], Pins_s *pin, float X, float Y)
{
	debug_print("Inserting cal point for step: ");
	debug_println(currentStepIn);
	const int currentStep = _calOrder[currentStepIn];

	debug_print("Cal point number: ");
	debug_println(currentStep);

	calPointsX[currentStep] = X;
	calPointsY[currentStep] = Y;

	debug_println("The collected coordinates are: ");
	debug_println(calPointsX[currentStep], 8);
	debug_println(calPointsY[currentStep], 8);
};


void notchCalibrate(const float xIn[], const float yIn[], const float xOut[], const float yOut[], const int regions, StickParams_s *stickParams)
{
	for (int i = 1; i <= regions; i++)
	{
		debug_print("calibrating region: ");
		debug_println(i);

		float pointsIn[3][3];
		float pointsOut[3][3];

		if (i == (regions))
		{
			debug_println("final region");
			pointsIn[0][0] = xIn[0];
			pointsIn[0][1] = xIn[i];
			pointsIn[0][2] = xIn[1];
			pointsIn[1][0] = yIn[0];
			pointsIn[1][1] = yIn[i];
			pointsIn[1][2] = yIn[1];
			pointsIn[2][0] = 1;
			pointsIn[2][1] = 1;
			pointsIn[2][2] = 1;
			pointsOut[0][0] = xOut[0];
			pointsOut[0][1] = xOut[i];
			pointsOut[0][2] = xOut[1];
			pointsOut[1][0] = yOut[0];
			pointsOut[1][1] = yOut[i];
			pointsOut[1][2] = yOut[1];
			pointsOut[2][0] = 1;
			pointsOut[2][1] = 1;
			pointsOut[2][2] = 1;
		}
		else
		{
			pointsIn[0][0] = xIn[0];
			pointsIn[0][1] = xIn[i];
			pointsIn[0][2] = xIn[i + 1];
			pointsIn[1][0] = yIn[0];
			pointsIn[1][1] = yIn[i];
			pointsIn[1][2] = yIn[i + 1];
			pointsIn[2][0] = 1;
			pointsIn[2][1] = 1;
			pointsIn[2][2] = 1;
			pointsOut[0][0] = xOut[0];
			pointsOut[0][1] = xOut[i];
			pointsOut[0][2] = xOut[i + 1];
			pointsOut[1][0] = yOut[0];
			pointsOut[1][1] = yOut[i];
			pointsOut[1][2] = yOut[i + 1];
			pointsOut[2][0] = 1;
			pointsOut[2][1] = 1;
			pointsOut[2][2] = 1;
		}

		debug_println("In points:");
		print_mtx(pointsIn);
		debug_println("Out points:");
		print_mtx(pointsOut);

		float temp[3][3];
		inverse(pointsIn, temp);

		float A[3][3];
		matrixMatrixMult(pointsOut, temp, A);

		debug_println("The transform matrix is:");
		print_mtx(A);

		debug_println("The affine transform coefficients for this region are:");

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				stickParams->affineCoeffs[i - 1][j * 2 + k] = A[j][k];
				debug_print(stickParams->affineCoeffs[i - 1][j * 2 + k]);
				debug_print(",");
			}
		}

		debug_println();
		debug_println("The angle defining this  regions is:");
		stickParams->boundaryAngles[i - 1] = atan2f((yIn[i] - yIn[0]), (xIn[i] - xIn[0]));
		// unwrap the angles so that the first has the smallest value
		if (stickParams->boundaryAngles[i - 1] < stickParams->boundaryAngles[0])
		{
			stickParams->boundaryAngles[i - 1] += M_PI * 2;
		}
		debug_println(stickParams->boundaryAngles[i - 1]);
	}
};
