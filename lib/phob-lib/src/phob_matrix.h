#ifndef PHOB_MATRIX_H
#define PHOB_MATRIX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

void matrix_inverse(const float in[3][3], float out[3][3]);
void matrix_multiply(const float left[3][3], const float right[3][3], float output[3][3]);
void matrix_print(const float matrix[3][3]);

#ifdef __cplusplus
}
#endif

#endif
