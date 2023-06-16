#include "tusb_config.h"
#include "hardware/spi.h"
#include "main.h"
#include "phob.h"

// Callback the API does to read the buttons
// and analog sticks
void cb_phob_read_buttons(Buttons_u *btns)
{
  // Set up buffers for each axis
  uint8_t buffer_lx[3] = {0};
  uint8_t buffer_ly[3] = {0};
  uint8_t buffer_rx[3] = {0};
  uint8_t buffer_ry[3] = {0};

  // CS left stick ADC
  gpio_put(PGPIO_LS_CS, false);
  // Read first axis for left stick
  spi_read_blocking(spi0, X_AXIS_CONFIG, buffer_lx, 3);

  // CS left stick ADC reset
  gpio_put(PGPIO_LS_CS, true);
  gpio_put(PGPIO_LS_CS, false);

  // Set up and read axis for left stick Y  axis
  spi_read_blocking(spi0, Y_AXIS_CONFIG, buffer_ly, 3);

  // CS right stick ADC
  gpio_put(PGPIO_LS_CS, true);
  gpio_put(PGPIO_RS_CS, false);

  spi_read_blocking(spi0, Y_AXIS_CONFIG, buffer_ry, 3);

  // CS right stick ADC reset
  gpio_put(PGPIO_RS_CS, true);
  gpio_put(PGPIO_RS_CS, false);

  spi_read_blocking(spi0, X_AXIS_CONFIG, buffer_rx, 3);

  // Release right stick CS ADC
  gpio_put(PGPIO_RS_CS, true);

  // Convert data
  data->lx = BUFFER_TO_UINT16(buffer_lx);
  data->ly = BUFFER_TO_UINT16(buffer_ly);
  data->rx = BUFFER_TO_UINT16(buffer_rx);
  data->ry = BUFFER_TO_UINT16(buffer_ry);
}

// Callback the API does to set rumble
void cb_phob_rumble(bool enable, uint16_t intensity)
{
  if(enable)
  {
    pwm_set_gpio_level(PGPIO_RUMBLE_BRAKE, 0);
    pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, 150);
  }
  else
  {
    pwm_set_gpio_level(PGPIO_RUMBLE_MAIN, 0);
    pwm_set_gpio_level(PGPIO_RUMBLE_BRAKE, 255);
  }
}

// Callback the API does when it is ready to set up hardware (GPIO, SPI, etc. Do that here)
void cb_phob_hardware_setup()
{

}

void main()
{

}
