#ifndef PHOB_H
#define PHOB_H

#include "phob_includes.h"

// Public variables

extern Buttons_u _btn;
extern Buttons_u _hardware;
extern RawStick_s _raw;
extern StickParams_s _aStickParams;
extern StickParams_s _cStickParams;

extern const GCReport_s defaultGcReport = {
    .a=0, .b=0, .x=0, .y=0, .start=0, .pad0=0,
    .dLeft=0, .dRight=0, .dDown=0, .dUp=0, .z=0, .r=0, .l=0, .pad1=1,
    .xStick=127,
    .yStick=127,
    .cxStick=127,
    .cyStick=127,
    .analogL=0,
    .analogR=0
};

extern float _ADCScale = 1;
extern float _ADCScaleFactor = 1;
extern int _rumblePower = 0;
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

#endif
