#ifndef PHOB_SETTINGS_H
#define PHOB_SETTINGS_H

#include "phob_includes.h"

#define TRIGGER_CONFIG_MIN  0
#define TRIGGER_CONFIG_MAX  6

#define TRIGGER_DEFAULT 0
#define TRIGGER_MIN     49
#define TRIGGER_MAX     227

#define CMAX  127
#define CMIN  -127

#define RUMBLE_MIN  0
#define RUMBLE_MAX  11
#define RUMBLE_DEFAULT  9 // 5 is max for 3v cell rumble

#define SNAPBACK_MIN  -10
#define SNAPBACK_MAX  10
#define SNAPBACK_DEFAULT  4

#define SMOOTHING_MIN     0
#define SMOOTHING_MAX     9
#define SMOOTHING_DEFAULT 0

#define WAVESHAPING_MIN       0
#define WAVESHAPING_MAX       15
#define WAVESHAPING_DEFAULT   0

#define CARDINALSNAPPING_MIN      -1
#define CARDINALSNAPPING_MAX      6
#define CARDINALSNAPPING_DEFAULT  6

#define ANALOG_SCALER_MIN         82
#define ANALOG_SCALER_MAX         125
#define ANALOG_SCALER_DEFAULT     100

#define TOURNAMENT_TOGGLE_MIN       0
#define TOURNAMENT_TOGGLE_MAX       5
#define TOURNAMENT_TOGGLE_DEFAULT   0

#define INTERLACE_OFFSET_MIN      -150
#define INTERLACE_OFFSET_MAX      150
#define INTERLACE_OFFSET_DEFAULT  0

#endif
