#ifndef PHOB_HW_H
#define PHOB_HW_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "phob_includes.h"

// Need to be defined by each portable lib
bool _hw_phob_set_api_mode(PhobAPIMode_t mode);

#ifdef __cplusplus
}
#endif

#endif
