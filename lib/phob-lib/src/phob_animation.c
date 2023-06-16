#include "phob_animation.h"

void animate_blockingdelay(int time)
{
  int startTime = millis();
	int delta = 0;
	while(delta < time){
		delta = millis() - startTime;
	}
}

// Displays the rumble value using the right stick Y axis
//Old name: showRumble
void animate_showrumble(int time, int val)
{
	btn.Cx = (uint8_t)INT_ORIGIN;
	btn.Cy = (uint8_t)(val + FLOAT_ORIGIN);

	animate_clearbuttons(time);
}

// Old name: freezeSticks
void animate_freezesticks(int time)
{
  btn.Cx = (uint8_t) (255);
	btn.Cy = (uint8_t) (255);
	btn.Ax = (uint8_t) (255);
	btn.Ay = (uint8_t) (255);
	btn.La = (uint8_t) (255 + 60.0);
	btn.Ra = (uint8_t) (255 + 60.0);

	_phob_input_outgoing = 0;

	hardware.A = (uint8_t) 0;
	hardware.B = (uint8_t) 0;
	hardware.X = (uint8_t) 0;
	hardware.Y = (uint8_t) 0;
	hardware.L = (uint8_t) 0;
	hardware.R = (uint8_t) 0;
	hardware.Z = (uint8_t) 0;
	hardware.S = (uint8_t) 0;


}

// Old name: freezeSticksToggleIndicator
void animate_freezesticks_toggle(int time, bool toggle)
{
  uint8_t mod = (FLOAT_ORIGIN + (toggle ? 50 : -50));
  btn.Cx = mod;
	btn.Cy = mod;
	btn.Ax = mod;
	btn.Ay = mod;
	btn.La = (uint8_t) (255 + 60.0);
	btn.Ra = (uint8_t) (255 + 60.0);

	btn.A = (uint8_t) 0;
	btn.B = (uint8_t) 0;
	btn.X = (uint8_t) 0;
	btn.Y = (uint8_t) 0;
	btn.L = (uint8_t) 0;
	btn.R = (uint8_t) 0;
	btn.Z = (uint8_t) 0;
	btn.S = (uint8_t) 0;

	hardware.L = (uint8_t) 0;
	hardware.R = (uint8_t) 0;
	hardware.X = (uint8_t) 0;
	hardware.Y = (uint8_t) 0;
	hardware.Z = (uint8_t) 0;

	int startTime = millis();
	int delta = 0;
	while(delta < time){
		delta = millis() - startTime;
	}
}

// Old name: clearButtons
// This clears all the buttons but doesn't overwrite the sticks or shoulder buttons.
void animate_clearbuttons(int time)
{
  _phob_input_outgoing.buttons_all = 0;
	_phob_input_internal.buttons_all = 0;
	animate_blockingdelay(time);
}
