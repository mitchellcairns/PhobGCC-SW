#ifdef __cplusplus
extern "C"
{
#endif

// Button GPIO Defines
#define PGPIO_BUTTON_A       17
#define PGPIO_BUTTON_B       16
#define PGPIO_BUTTON_X       18
#define PGPIO_BUTTON_Y       19

#define PGPIO_BUTTON_DUP     9
#define PGPIO_BUTTON_DDOWN   10
#define PGPIO_BUTTON_DLEFT   8
#define PGPIO_BUTTON_DRIGHT  11

#define PGPIO_BUTTON_L       22
#define PGPIO_BUTTON_R       21
#define PGPIO_BUTTON_Z       20
#define PGPIO_BUTTON_START   5

#define PGPIO_INPUT_ARRAY ((uint8_t[]) {PGPIO_BUTTON_A, PGPIO_BUTTON_B, PGPIO_BUTTON_X, PGPIO_BUTTON_Y, PGPIO_BUTTON_DUP, PGPIO_BUTTON_DDOWN, PGPIO_BUTTON_DLEFT, PGPIO_BUTTON_DRIGHT, PGPIO_BUTTON_L, PGPIO_BUTTON_R, PGPIO_BUTTON_Z, PGPIO_BUTTON_START})

// Analog L Trigger ADC
#define PADC_LT 1
// Analog R Trigger ADC
#define PADC_RT 0

// Analog L Trigger GPIO
#define PGPIO_LT 27
// Analog R Trigger GPIO
#define PGPIO_RT 26

// Rumble GPIOs
#define PGPIO_RUMBLE_MAIN   25
#define PGPIO_RUMBLE_BRAKE  29

// SPI ADC CLK pin
#define PGPIO_SPI_CLK 6
// SPI ADC TX pin
#define PGPIO_SPI_TX  7
// SPI ADC RX pin
#define PGPIO_SPI_RX  4

// Left stick ADC Chip Select
#define PGPIO_LS_CS   1
// Right stick ADC Chip Select
#define PGPIO_RS_CS   4

#ifdef __cplusplus
}
#endif
