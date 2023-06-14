#include "phob.h"

// Internal mode
PhobAPIMode_t _phob_api_mode = PHOB_API_MODE_UNSET;

void phob_set_api_mode(PhobAPIMode_t mode)
{
  // Check if mode is OK to set on current hardware
  if (_hw_phob_set_api_mode(PhobAPIMode_t mode))
  {
    // Mode is OK to set
    _phob_api_mode = mode;
  }
  else
  {
    // Mode is not OK to set
  }
}
