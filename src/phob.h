#ifndef PHOB_H
#define PHOB_H

#include "phob_includes.h"

// Public variables

extern Buttons_u _btn;
extern Buttons_u _hardware;
extern RawStick_s _raw;
extern StickParams_s _aStickParams;
extern StickParams_s _cStickParams;

extern float _ADCScale = 1;
extern float _ADCScaleFactor = 1;
extern int _rumblePower = 0;
extern float _dT;

// Functions that are expected to be defined by
// each 'project' for each supported board
void phob_read_buttons();

void phob_read_analog();

void phob_hardware_setup();

// General API functions
void phob_init_api();

void phob_set_api_mode();

void phob_stop_api();

void phob_start_calibration();

void phob_stop_calibration();

void phob_save_settings();

void phob_load_settings();

#endif
