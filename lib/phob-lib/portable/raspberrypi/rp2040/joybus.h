#ifndef JOYBUS_H
#define JOYBUS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

/**
 * @short Enters the Joybus communication mode
 *
 * @param dataPin GPIO number of the console data line pin
 * @param func Function to be called to obtain the GCReport to be sent to the console
 */
void enterMode(const int dataPin);

#ifdef __cplusplus
}
#endif

#endif
