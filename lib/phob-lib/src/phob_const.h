#ifndef PHOB_CONST_H
#define PHOB_CONST_H

#define NOTCH_COUNT 16
#define INT_ORIGIN 127
#define FLOAT_ORIGIN (float) 127.5f
#define NOTCH_CALIBRATION_POINTS (NOTCH_COUNT*2)
#define NOTCH_ADJUSTMENT_NUMBER 12
#define FIT_ORDER 3 // fit order used in the linearization step
#define MAX_STICK_ANGLE (float) 0.4886921906f // 28 degrees this is the max angular deflection of the stick.

#endif
