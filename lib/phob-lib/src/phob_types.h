#ifndef PHOB_TYPES_H
#define PHOB_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

typedef enum
{
	PHOB_API_MODE_UNSET,
	PHOB_API_MODE_GCC,
	PHOB_API_MODE_VISION,
	PHOB_API_MODE_XINPUT,
	PHOB_API_MODE_MAX,
} PhobAPIMode_t;

typedef enum
{
  DEFAULTJUMP,
  SWAP_XZ,
  SWAP_YZ,
  SWAP_XL,
  SWAP_YL,
  SWAP_XR,
  SWAP_YR,
	JUMP_CONFIG_MAX,
} JumpConfig_t;

typedef enum
{
  LTRIGGER,
  RTRIGGER
} WhichTrigger_t;

typedef enum
{
  MASTER = 0,
  STORAGE = 1,
} PageIndexes_t;

typedef enum
{
  INCREASE,
  DECREASE
} Increase_t;

typedef enum
{
  AX_AXIS,
  AY_AXIS,
	CX_AXIS,
	CY_AXIS
} WhichAxis_t;

typedef enum
{
  A_STICK,
  C_STICK
} WhichStick_t;

typedef enum
{
  HARD_RESET,
  SOFT_RESET,
  FACTORY_RESET
} HardReset_t;

typedef enum
{
  TERT_INACTIVE,
  TERT_ACTIVE,
  SECONDARY,
  CARDINAL
} NotchStatus_t;

typedef enum
{
  EXTRAS_UP,
  EXTRAS_DOWN,
  EXTRAS_LEFT,
  EXTRAS_RIGHT,
  EXTRAS_SIZE,
  EXTRAS_UNSET
} ExtrasSlot_t;

typedef union
{
  int intValue;
  float floatValue;
} IntOrFloat_u;

typedef struct
{
  IntOrFloat_u config[4];
} ExtrasConfig_s;

// Configuration Struct
typedef struct
{
  JumpConfig_t 	jumpConfig;
  int 	lConfig;
  int 	rConfig;
  int 	lTriggerOffset;
  int 	rTriggerOffset;

  int 	cxOffset;
  int 	cyOffset;

  int 	xSnapback; 	// 0 disables the filter entirely, 4 is default
  int 	ySnapback;
  int 	axSmoothing;
  int 	aySmoothing;

  int 	cxSmoothing;
  int 	cySmoothing;

  int 	rumble; 		// 0 is off, nonzero is on, higher is stronger

  int 	autoInit;

  int 	axWaveshaping;
  int 	ayWaveshaping;

  int 	cxWaveshaping;
  int 	cyWaveshaping;

  float axPoints[32];
  float ayPoints[32];

  float cxPoints[32];
  float cyPoints[32];
  float aAngles[16];
  float cAngles[16];

  IntOrFloat_u uExtras[4];
  IntOrFloat_u dExtras[4];
  IntOrFloat_u lExtras[4];
  IntOrFloat_u rExtras[4];

  int schema;

  int AstickCardinalSnapping;
  int CstickCardinalSnapping;

  int 	AstickAnalogScaler;
  int 	CstickAnalogScaler;

  int interlaceOffset;

  int tournamentToggle;
} __attribute__((packed)) Settings_s;

typedef struct
{
	float xPoints[32];
	float yPoints[32];
	float nxPoints[16];
	float nyPoints[16];
} CalPoints_s;

typedef struct
{
	float x;
	float y;
} AnalogPairFloat_s;

typedef struct{
	int pinLa;
	int pinRa;
	int pinL;
	int pinR;
	int pinAx;
	int pinAy;
	int pinCx;
	int pinCy;
	int pinRX;
	int pinTX;
	int pinDr;
	int pinDu;
	int pinDl;
	int pinDd;
	int pinX;
	int pinY;
	int pinA;
	int pinB;
	int pinZ;
	int pinS;
} Pins_s;

typedef union{
	uint8_t arr[10];
	struct {

		// byte 0
		uint8_t A : 1;
		uint8_t B : 1;
		uint8_t X : 1;
		uint8_t Y : 1;
		uint8_t S : 1;
		uint8_t orig : 1;
		uint8_t errL : 1;
		uint8_t errS : 1;

		// byte 1
		uint8_t Dl : 1;
		uint8_t Dr : 1;
		uint8_t Dd : 1;
		uint8_t Du : 1;
		uint8_t Z : 1;
		uint8_t R : 1;
		uint8_t L : 1;
		uint8_t high : 1;

		//byte 2-7
		uint8_t Ax : 8;
		uint8_t Ay : 8;
		uint8_t Cx : 8;
		uint8_t Cy : 8;
		uint8_t La : 8;
		uint8_t Ra : 8;

		// magic byte 8 & 9 (only used in origin cmd)
		// have something to do with rumble motor status???
		// ignore these, they are magic numbers needed
		// to make a cmd response work
		uint8_t magic1 : 8;
		uint8_t magic2 : 8;
	};
} Buttons_u;

typedef struct {
	uint8_t L;
	uint8_t R;
	uint8_t Z;
	uint8_t X;
	uint8_t Y;
} HardwareButtons_s;

typedef struct{
	float axRaw;
	float ayRaw;
	float cxRaw;
	float cyRaw;
	float axLinearized;
	float ayLinearized;
	float cxLinearized;
	float cyLinearized;
	float axUnfiltered;
	float ayUnfiltered;
	float cxUnfiltered;
	float cyUnfiltered;
} RawStick_s;

typedef struct {
	uint8_t l : 1;
	uint8_t r : 1;
	uint8_t u : 1;
	uint8_t d : 1;
} Cardinals_s;

typedef struct {
	//What's the max stick distance from the center
	float maxStick;
	//filtered velocity terms
	//how fast the filtered velocity falls off in the absence of stick movement.
	//Probably don't touch this.
	float xVelDecay;//0.1 default for 1.2ms timesteps, larger for bigger timesteps
	float yVelDecay;
	//how much the current position disagreement impacts the filtered velocity.
	//Probably don't touch this.
	float xVelPosFactor;//0.01 default for 1.2ms timesteps, larger for bigger timesteps
	float yVelPosFactor;
	//how much to ignore filtered velocity when computing the new stick position.
	//DO CHANGE THIS
	//Higher gives shorter rise times and slower fall times (more pode, less snapback)
	float xVelDamp;//0.125 default for 1.2ms timesteps, smaller for bigger timesteps
	float yVelDamp;
	//speed and accel thresholds below which we try to follow the stick better
	//These may need tweaking according to how noisy the signal is
	//If it's noisier, we may need to add additional filtering
	//If the timesteps are *really small* then it may need to be increased to get
	//  above the noise floor. Or some combination of filtering and playing with
	//  the thresholds.
	float velThresh;//1 default for 1.2ms timesteps, larger for bigger timesteps
	float accelThresh;//5 default for 1.2ms timesteps, larger for bigger timesteps
	//This just applies a low-pass filter.
	//The purpose is to provide delay for single-axis ledgedashes.
	//Must be between 0 and 1. Larger = more smoothing and delay.
	float xSmoothing;
	float ySmoothing;
	//Same thing but for C-stick
	float cXSmoothing;
	float cYSmoothing;
} FilterGains_s;

typedef struct {
	//these are the linearization coefficients
	float fitCoeffsX[4];
	float fitCoeffsY[4];

	//these are the notch remap parameters
	float affineCoeffs[16][4]; //affine transformation coefficients for all regions of the stick
	float boundaryAngles[16]; //angles at the boundaries between regions of the stick (in the plane)
} StickParams_s;

typedef enum {
	CM_NULL,//do nothing
	CM_REACTION,//record immediately
	CM_STICK_RISE2,//starting when a stick coord exceeds a threshold distance from center; 100 pt 2-axis
	CM_STICK_RISE,//starting when a stick coord exceeds a threshold distance from center
	CM_STICK_FALL,//starting when a stick coord falls below a threshold
	CM_STICK_PIVOT,//starting when a stick coord falls below a threshold
	CM_TRIG,//increasing threshold on triggers
	CM_JUMP,//x, y, or melee tap jump threshold
	CM_PRESS,//any button press
} CaptureMode_t;

typedef enum {
	GV_AX,//a-stick x-axis filtered & unfiltered
	GV_AY,
	GV_AXY,//a-stick x and y, only filtered
	GV_CX,
	GV_CY,
	GV_CXY,
	GV_L,
	GV_R,
} GraphVar_t;

typedef struct {
	//these are the params
	CaptureMode_t mode;
	WhichStick_t triggerStick;
	WhichStick_t captureStick;
	WhichAxis_t whichAxis;
	uint8_t stickmap;//which stickmap to display
	GraphVar_t graphVar;//which variable to graph
	bool begin;
	bool triggered;
	bool done;
	bool autoRepeat;
	uint32_t delay;//for recording reaction time
	uint8_t stickThresh;
	uint8_t triggerThresh;
	uint8_t startIndex;
	uint8_t endIndex;
	uint8_t viewIndex;//for stepping through manually and looking at coordinates and button presses
	uint8_t a1[100];//6 frames for analog
	uint8_t a2[100];
	uint8_t a1Unfilt[100];
	uint8_t a2Unfilt[100];
	uint8_t abxyszrl[200];//12 frames
	uint8_t axaycxcyrl[200];//12 frames
	float percents[3];
} DataCapture_s;

typedef struct {
	union
	{
		struct
		{
			uint8_t a 		: 1;
			uint8_t b 		: 1;
			uint8_t x			: 1;
			uint8_t y 		: 1;
			uint8_t start : 1;
			uint8_t pad0 		: 3;
			uint8_t dLeft 	: 1;
			uint8_t dRight 	: 1;
			uint8_t dDown 	: 1;
			uint8_t dUp 		: 1;
			uint8_t z 			: 1;
			uint8_t r 			: 1;
			uint8_t l 			: 1;
			uint8_t pad1 		: 1;
		};
		uint16_t buttons_all
	};
    uint8_t xStick;
    uint8_t yStick;
    uint8_t cxStick;
    uint8_t cyStick;
    uint8_t analogL;
    uint8_t analogR;
} __attribute__((packed)) GCJoybusInput_s;

/** @brief This is a struct for containing all of the
 * button input data as bits. This saves space
 * and allows for easier handoff to the various
 * controller cores in the future.
**/
typedef struct
{
    union
    {
        struct
        {
            // D-Pad
            uint8_t dup         : 1;
            uint8_t ddown       : 1;
            uint8_t dleft       : 1;
            uint8_t dright      : 1;
            // Buttons
            uint8_t a       : 1;
            uint8_t b     : 1;
            uint8_t x     : 1;
            uint8_t y    : 1;
            // Triggers
            uint8_t l       : 1;
            uint8_t zl      : 1;
            uint8_t r       : 1;
            uint8_t zr      : 1;

            // Special Functions
            uint8_t start    : 1;
            uint8_t minus    : 1;

            // Stick clicks
            uint8_t stick_left   : 1;
            uint8_t stick_right  : 1;
        };
        uint16_t buttons_all;
    };

    union
    {
        struct
        {
            // Menu buttons (Not remappable by API)
            uint8_t capture  : 1;
            uint8_t home     : 1;
            uint8_t padding         : 6;
        };
        uint8_t buttons_system;
    };

		uint8_t x_axis;
		uint8_t y_axis;
		uint8_t cx_axis;
		uint8_t cy_axis;
		uint8_t la_axis;
		uint8_t ra_axis;
} __attribute__ ((packed)) PhobInput_s;

#ifdef __cplusplus
}
#endif

#endif
