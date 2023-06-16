#ifndef PHOB_H
#define PHOB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

// Functions that are expected to be defined by
// each 'project' for each supported board
void cb_phob_read_buttons(Buttons_u *btns);

void cb_phob_read_sticks();

void cb_phob_read_triggers();

void cb_phob_rumble(bool enable, uint16_t intensity);

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
