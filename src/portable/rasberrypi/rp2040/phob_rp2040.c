#include "phob_includes.h"

#define FLASH_TARGET_OFFSET (1536 * 1024)

bool _hw_phob_set_api_mode(PhobAPIMode_t mode)
{
  switch(mode)
  {
    default:
      return false;
      break;

    case PHOB_API_MODE_GCC:
      return true;
      break;

    case PHOB_API_MODE_XINPUT:
      return true;
      break;
  }
}

void _hw_phob_init_api()
{



}

// LL Load settings
void _hw_phob_load_settings(Settings_s *settings_out, uint8_t index)
{
  // Avoid zero IDX.
  index += 1;

  static_assert(sizeof(Settings_s) <= FLASH_SECTOR_SIZE)
      const uint8_t *target_read = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET + (index * FLASH_SECTOR_SIZE));
  memcpy(settings_out, target_read, sizeof(Settings_s));
}

void _hw_phob_save_settings(Settings_s *settings_in, uint8_t index, const bool noLock = false)
{

  // Lock core
  if (!noLock)
  {
    multicore_lockout_start_blocking();
  }

  // Avoid zero IDX.
  index += 1;

  // Check that we are less than our flash sector size
  static_assert(sizeof(Settings_s) <= FLASH_SECTOR_SIZE);

  // Store interrupts status and disable
  uint32_t ints = save_and_disable_interrupts();

  // Calculate storage bank address via index
  uint32_t memoryAddress = FLASH_TARGET_OFFSET + (index * FLASH_SECTOR_SIZE);

  // Create blank page data
  uint8_t page[FLASH_SECTOR_SIZE] = {0x00};
  // Copy settings into our page buffer
  memcpy(page, settings_in, sizeof(Settings_s));

  // Erase the settings flash sector
  flash_range_erase(memoryAddress, FLASH_SECTOR_SIZE);

  // Program the flash sector with our page
  flash_range_program(memoryAddress, page, FLASH_SECTOR_SIZE);

  // Restore interrups
  restore_interrupts(ints);

  // Unlock core.
  if (!noLock)
  {
    multicore_lockout_end_blocking();
  }
}

void _hw_phob_rumble(bool rumble, uint8_t intensity)
{

}
