#ifndef PHOB_BOARD_CONFIG_H
#define PHOB_BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_config_types.h"

// Type of MCU being used
#define PCFG_BOARD_TYPE BOARD_TYPE_RP2040

// Only a single GPIO is used, dual mode disabled
#define PCFG_JOYBUS_DUAL_MODE 0

// GPIO pin used for data in/out
#define PCFG_JOYBUS_GPIO_INOUT 28

#ifdef __cplusplus
}
#endif

#endif
