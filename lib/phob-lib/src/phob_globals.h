#ifndef PHOB_GLOBALS_H
#define PHOB_GLOBALS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

// Public variables

// State of buttons that is broadcast
extern Buttons_u _btn;

// State of buttons internal to the controller
extern Buttons_u _hardware;

extern RawStick_s _raw;
extern StickParams_s _aStickParams;
extern StickParams_s _cStickParams;

// Used to store internal button state
extern PhobInput_s _phob_input_internal;

// This is the data which is handed off
// to be converted for our output report
extern PhobInput_s _phob_input_outgoing;

extern float _ADCScale;
extern float _ADCScaleFactor;
extern int _rumblePower;
extern float _dT;

#ifdef __cplusplus
}
#endif

#endif
