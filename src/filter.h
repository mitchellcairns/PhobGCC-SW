#ifndef FILTER_H
#define FILTER_H

#include "phob_includes.h"

//The median filter can be either length 3, 4, or 5.
#define MEDIANLEN 5
//Edit MEDIANARRAY to be MEDIANLEN long
#define MEDIANARRAY {0,0,0,0,0}
//Comment this define to disable it entirely.
//#define USEMEDIAN

void runMedian(float *val, float valArray[MEDIANLEN], unsigned int *medianIndex);
float velDampFromSnapback(const int snapback);
void recomputeGains(const ControlConfig_s *controls, FilterGains_s *gains, FilterGains_s *normGains);
void runKalman(float *xPosFilt, float *yPosFilt, const float xZ,const float yZ, const ControlConfig_s *controls, const FilterGains_s *normGains);
float calcWaveshapeMult(const int setting);
void aRunWaveShaping(const float xPos, const float yPos, float *xOut, float *yOut, const ControlConfig_s *controls, const FilterGains_s *normGains);
void cRunWaveShaping(const float xPos, const float yPos, float *xOut, float *yOut, const ControlConfig_s *controls, const FilterGains_s *normGains);

#endif
