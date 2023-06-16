#include "phob_settings.h"

Settings_s _phob_loaded_settings = {0};
Settings_s* _phob_loaded_settings_ptr = &_phob_loaded_settings;
uint16_t _phob_rumble_power = 0;

// Returns a pointer to our global settings
Settings_s* settings_get_ptr()
{
	return _phob_loaded_settings_ptr;
}

// Old name: calcRumblePower
uint16_t settings_calculate_rumblepower(int rumble){
	if(rumble > 0) {
		return pow(2.0, 7+((rumble-3)/8.0)); //should be 256 when rumble is 11
	} else {
		return 0;
	}
}

// Old name: changeRumble
// Adjusts rumble and returns the new value
int settings_adjust_rumble(int adjustment)
{
	debug_println("Adjusting rumble.");
	int c = _phob_loaded_settings.rumble;

	c += adjustment;
	if (c > RUMBLE_MAX) c = RUMBLE_MAX;
	if (c < 0) c = 0;

	_phob_loaded_settings.rumble = c;

	// Convert setting into proper rumble power
	_phob_rumble_power = settings_calculaterumblepower(c);
	return c;
	// animate_showrumble(750);
}

// Returns the rumble power for PWM purposes
uint16_t settings_get_rumblepower()
{
	return _phob_rumble_power;
}

// <0 toggles. 0 disables. 1> enables.
// Returns the new value
bool settings_adjust_autoinit(int val)
{
	if (val < 0)
	{
		// Inverts setting, only one or the other.
		_phob_loaded_settings.autoInit = !_phob_loaded_settings.autoInit;
	}
	else
	{
		_phob_loaded_settings.autoInit = (val > 0);
	}

	return _phob_loaded_settings.autoInit;
}


// Original workflow
// Load settings -> recomputeGains -> cleanCalPoints -> linearizeCal -> notchCalibrate

/*******************
  Previously known as recomputeGains
  Responsible for generating the working FilterGains_s
*******************/
void settings_process_filtergains(Settings_s *settings, FilterGains_s *gains, FilterGains_s *normGains) {

	//Adjust the snapback and smoothing gains->according to the controls config
	gains->xVelDamp = velDampFromSnapback(settings->xSnapback);
	gains->yVelDamp = velDampFromSnapback(settings->ySnapback);

	gains->xSmoothing = settings->axSmoothing/10.0f;
	gains->ySmoothing = settings->aySmoothing/10.0f;

	gains->cXSmoothing = settings->cxSmoothing/10.0f;
	gains->cYSmoothing = settings->cySmoothing/10.0f;

	//Recompute the intermediate gains->used directly by the kalman filter
	//This happens according to the time between loop iterations.
	//Before, this happened every iteration of runKalman, but now
	//the event loop runs at a fixed 1000 Hz
	//Even if it's not *exactly* 1000 Hz, it should be constant enough.
	//Hopefully.
	//So now, this should be called any time gains->gets changed.
	const float _timeFactor = 1.0 / 1.2;
	const float _timeDivisor = 1.2 / 1.0;
	normGains->maxStick      = gains->maxStick*gains->maxStick;//we actually use the square
	normGains->xVelDecay     = gains->xVelDecay      * _timeFactor;
	normGains->yVelDecay     = gains->yVelDecay      * _timeFactor;
	normGains->xVelPosFactor = gains->xVelPosFactor  * _timeFactor;
	normGains->yVelPosFactor = gains->yVelPosFactor  * _timeFactor;
	normGains->xVelDamp      = gains->xVelDamp;

	if(controls->xSnapback >= 0) {
		normGains->xVelDamp *= _timeDivisor;
	}

	normGains->yVelDamp      = gains->yVelDamp;

	if(controls->ySnapback >= 0) {
		normGains->yVelDamp *= _timeDivisor;
	}

	normGains->velThresh     = 1/(gains->velThresh   * _timeFactor);  //slight optimization by using the inverse
	normGains->accelThresh   = 1/(gains->accelThresh * _timeFactor);
	normGains->velThresh     = normGains->velThresh*normGains->velThresh; //square it because it's used squared
	normGains->accelThresh   = normGains->accelThresh*normGains->accelThresh;
	normGains->xSmoothing    = pow(1-gains->xSmoothing, _timeDivisor);
	normGains->ySmoothing    = pow(1-gains->ySmoothing, _timeDivisor);
	normGains->cXSmoothing   = pow(1-gains->cXSmoothing, _timeDivisor);
	normGains->cYSmoothing   = pow(1-gains->cYSmoothing, _timeDivisor);
};

/*******************
	settings_process_stickparams (Previously linearizeCal )
  Responsible for generating the working StickParams_s
	Generate a fit to linearize the stick response.
	Inputs:
		cleaned points X and Y as CalPoints_s struct type, (must be 17 points for each of these, the first being the center, the others starting at 3 oclock and going around counterclockwise)
	Outputs:
		linearization fit coefficients for X and Y as StickParams_s struct
*******************/
void settings_process_stickparams(CalPoints_s *cal, StickParams_s *stickParams)
{
	debug_println("beginning linearization");

	float *calX = &cal->xPoints;
	float *calY = &cal->yPoints;

	// do the curve fit first
	// generate all the notched/not notched specific cstick values we will need

	double fitPointsX[5];
	double fitPointsY[5];

	fitPointsX[0] = calX[8 + 1];												// right
	fitPointsX[1] = (calX[6 + 1] + calX[10 + 1]) / 2.0; // right 45 deg
	fitPointsX[2] = calX[0];														// center
	fitPointsX[3] = (calX[2 + 1] + calX[14 + 1]) / 2.0; // left 45 deg
	fitPointsX[4] = calX[0 + 1];												// left

	fitPointsY[0] = calY[12 + 1];											 // down
	fitPointsY[1] = (calY[10 + 1] + calY[14 + 1]) / 2.0; // down 45 deg
	fitPointsY[2] = calY[0];														 // center
	fitPointsY[3] = (calY[6 + 1] + calY[2 + 1]) / 2.0;	 // up 45 deg
	fitPointsY[4] = calY[4 + 1];												 // up

	//////determine the coefficients needed to linearize the stick
	// create the expected output, what we want our curve to be fit too
	// this is hard coded because it doesn't depend on the notch adjustments
	//                    -100 -74.246        0     74.246         100, centered around 0-255
	// It's not sin(45 deg) because it's a spherical motion, not planar.
	double x_output[5] = {27.5, 53.2537879754, 127.5, 201.7462120246, 227.5};
	double y_output[5] = {27.5, 53.2537879754, 127.5, 201.7462120246, 227.5};

	debug_println("The fit input points are (x,y):");
	for (int i = 0; i < 5; i++)
	{
		debug_print(fitPointsX[i], 8);
		debug_print(",");
		debug_println(fitPointsY[i], 8);
	}

	debug_println("The corresponding fit output points are (x,y):");
	for (int i = 0; i < 5; i++)
	{
		debug_print(x_output[i]);
		debug_print(",");
		debug_println(y_output[i]);
	}

	// perform the curve fit, order is 3
	double tempCoeffsX[FIT_ORDER + 1];
	double tempCoeffsY[FIT_ORDER + 1];

	fitCurve(FIT_ORDER, 5, fitPointsX, x_output, FIT_ORDER + 1, tempCoeffsX);
	fitCurve(FIT_ORDER, 5, fitPointsY, y_output, FIT_ORDER + 1, tempCoeffsY);

	// write these coefficients to the array that was passed in, this is our first output
	for (int i = 0; i < (FIT_ORDER + 1); i++)
	{
		stickParams->fitCoeffsX[i] = tempCoeffsX[i];
		stickParams->fitCoeffsY[i] = tempCoeffsY[i];
	}

	// we will now take out the offset, making the range -100 to 100 instead of 28 to 228
	// calculate the offset
	float xZeroError = linearize((float)fitPointsX[2], stickParams->fitCoeffsX);
	float yZeroError = linearize((float)fitPointsY[2], stickParams->fitCoeffsY);

	// Adjust the fit's constant coefficient so that the stick zero position is 0
	stickParams->fitCoeffsX[3] = stickParams->fitCoeffsX[3] - xZeroError;
	stickParams->fitCoeffsY[3] = stickParams->fitCoeffsY[3] - yZeroError;

	debug_println("The fit coefficients are  are (x,y):");
	for (int i = 0; i < 4; i++)
	{
		debug_print(stickParams->fitCoeffsX[i]);
		debug_print(",");
		debug_println(stickParams->fitCoeffsY[i]);
	}

	debug_println("The linearized points are:");
	for (int i = 0; i <= NOTCH_COUNT; i++)
	{
		calX[i] = linearize(calX[i], stickParams->fitCoeffsX);
		calY[i] = linearize(calY[i], stickParams->fitCoeffsY);
		debug_print(calX[i], 8);
		debug_print(",");
		debug_println(calY[i], 8);
	}
};
