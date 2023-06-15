#ifndef STICKCAL_H
#define STICKCAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

float linearize(const float point, const float coefficients[]);
void calcStickValues(float angle, float* x, float* y);
void angleOnSphere(const float x, const float y, float *angle);
void stripCalPoints(const float calPointsX[], const float calPointsY[], float strippedPointsX[], float strippedPointsY[]);
void computeStickAngles(float xInput[], float yInput[], float stickAngles[]);
void cleanNotches(float notchAngles[], float measuredNotchAngles[], NotchStatus_t notchStatus_t[]);
void notchRemap(const float xIn, const float yIn, float* xOut, float* yOut, const int regions, const StickParams_s *stickParams, int currentCalStep, const ControlConfig_s *controls, const WhichStick_t whichStick);
void transformCalPoints(const float xInput[], const float yInput[], float xOutput[], float yOutput[], const StickParams_s *stickParams, const ControlConfig_s *controls, const WhichStick_t whichStick);
void cleanCalPoints(const float calPointsX[], const float calPointsY[], const float notchAngles[], float cleanedPointsX[], float cleanedPointsY[], float notchPointsX[], float notchPointsY[], NotchStatus_t notchStatus_t[]);
void legalizeNotch(const int notchIndex, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus_t[]);
void legalizeNotches(const int currentStepIn, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus_t[]);
void adjustNotch(const int currentStepIn, const float loopDelta, const WhichStick_t whichStick, float measuredNotchAngles[], float notchAngles[], NotchStatus_t notchStatus_t[], Buttons_u *btn, Buttons_u *hardware);
void displayNotch(const int currentStepIn, const bool calibratingAStick, const float notchAngles[], Buttons_u *btn);
void insertCalPoints(const WhichStick_t whichStick, const int currentStepIn, float calPointsX[], float calPointsY[], Pins_s *pin, float X, float Y);
void linearizeCal(const float inX[], const float inY[], float outX[], float outY[], StickParams_s *stickParams);
void inverse(const float in[3][3], float out[3][3]);
void matrixMatrixMult(const float left[3][3], const float right[3][3], float output[3][3]);
void print_mtx(const float matrix[3][3]);
void notchCalibrate(const float xIn[], const float yIn[], const float xOut[], const float yOut[], const int regions, StickParams_s *stickParams);

#ifdef __cplusplus
}
#endif

#endif
