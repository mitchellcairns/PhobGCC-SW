#ifndef PHOB_FILTER_H
#define PHOB_FILTER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

//The median filter can be either length 3, 4, or 5.
#define MEDIANLEN 5
//Edit MEDIANARRAY to be MEDIANLEN long
#define MEDIANARRAY {0,0,0,0,0}
//Comment this define to disable it entirely.
//#define USEMEDIAN

void  filter_median(float *val, float valArray[MEDIANLEN], unsigned int *medianIndex);
float filter_dampensnapback(int snapback);
void  filter_kalman(float *xPosFilt, float *yPosFilt, float xZ, float yZ,
                    Settings_s *settings, FilterGains_s *normGains);
float filter_waveshapemultiply(int setting);
void  filter_waveshaping(WhichStick_t stick, AnalogPairFloat_s *analog,
                         Settings_s *settings, FilterGains_s *normGains);

#ifdef __cplusplus
}
#endif

#endif
