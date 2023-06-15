#ifndef PHOB_H
#define PHOB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

// Public variables

extern Buttons_u _btn;
extern Buttons_u _hardware;
extern RawStick_s _raw;
extern StickParams_s _aStickParams;
extern StickParams_s _cStickParams;

extern const GCReport_s defaultGcReport;

extern float _ADCScale;
extern float _ADCScaleFactor;
extern int _rumblePower;
extern float _dT;

// Functions that are expected to be defined by
// each 'project' for each supported board
void cb_phob_read_buttons();

void cb_phob_read_analog();

void cb_phob_rumble(bool enable);

void cb_phob_hardware_setup();

// General API functions
void phob_init_api();

void phob_set_api_mode(PhobAPIMode_t mode);

void phob_stop_api();

void phob_start_calibration();

void phob_stop_calibration();

void phob_save_settings();

void phob_load_settings();

void phob_debug_print();

#ifdef __cplusplus
}
#endif

#endif
