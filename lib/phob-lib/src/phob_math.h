#ifndef PHOB_MATH_H
#define PHOB_MATH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

void math_angleonsphere(const float x, const float y, float *angle);
void math_angletocoords(float angle, float *x, float *y);
void math_coordstoangles(CalPoints_s *calPoints, float stickAngles[]);

#ifdef __cplusplus
}
#endif

#endif
