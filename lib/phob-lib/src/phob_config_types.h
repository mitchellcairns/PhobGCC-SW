#ifndef PHOB_CONFIG_TYPES_H
#define PHOB_CONFIG_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

// Board types
#define PCFG_BOARD_TYPE_RP2040 0
#define PCFG_BOARD_TYPE_TEENSY 1

// Data GPIO types
// Determines if one or two GPIO are used for
// data (some boards may use two data pins and a diode for quicker switching)
#define PCFG_JOYBUS_DATA_SINGLE 0
#define PCFG_JOYBUS_DATA_DUAL   1



#ifdef __cplusplus
}
#endif

#endif
