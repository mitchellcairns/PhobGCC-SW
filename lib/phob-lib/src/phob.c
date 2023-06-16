#include "phob.h"

// Internal mode
PhobAPIMode_t _phob_api_mode = PHOB_API_MODE_UNSET;

const GCJoybusInput_s defaultGcReport = {
    .a=0, .b=0, .x=0, .y=0, .start=0, .pad0=0,
    .dLeft=0, .dRight=0, .dDown=0, .dUp=0, .z=0, .r=0, .l=0, .pad1=1,
    .xStick=127,
    .yStick=127,
    .cxStick=127,
    .cyStick=127,
    .analogL=0,
    .analogR=0
};

float _ADCScale = 1;
float _ADCScaleFactor = 1;
int _rumblePower = 0;
float _dT;

void phob_set_api_mode(PhobAPIMode_t mode)
{
  // Check if mode is OK to set on current hardware
  if (_hw_phob_set_api_mode(mode))
  {
    // Mode is OK to set
    _phob_api_mode = mode;
  }
  else
  {
    // Mode is not OK to set
  }
}
