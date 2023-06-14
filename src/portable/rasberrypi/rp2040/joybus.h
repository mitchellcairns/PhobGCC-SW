#ifndef __JOYBUS_H
#define __JOYBUS_H

#include "phob_includes.h"

/**
 * @short Enters the Joybus communication mode
 *
 * @param dataPin GPIO number of the console data line pin
 * @param func Function to be called to obtain the GCReport to be sent to the console
 */
void enterMode(const int dataPin, const int rumblePin, const int brakePin, int rumblePower);

#endif
