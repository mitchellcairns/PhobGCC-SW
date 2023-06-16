#ifndef PHOB_INCLUDES_H
#define PHOB_INCLUDES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"

#include "hardware/flash.h"
#include "hardware/dma.h"
#include "hardware/pio.h"

#include "joybus.pio.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "phob_const.h"
#include "phob_types.h"

#incldue "phob_globals.h"

#include "curve_fitting.h"

#include "phob_math.h"
#include "phob_matrix.h"
#include "phob_filter.h"
#include "phob_settings.h"
#include "phob_animation.h"

#include "phob_board_config.h"
#include "phob.h"

#ifdef __cplusplus
}
#endif

#endif
