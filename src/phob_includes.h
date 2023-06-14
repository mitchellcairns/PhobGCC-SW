#ifndef PHOB_INCLUDES_H
#define PHOB_INCLUDES_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "hardware/gpio.h"
#include "hardware/pwm.h"

#include "curve_fitting.h"
#include "stick_cal.h"

#include "phob_board_config.h"
#include "phob_types.h"

#include "hardware/pio.h"
#include "joybus.pio.h"

#endif
