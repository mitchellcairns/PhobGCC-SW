add_library(phob_lib INTERFACE)

target_sources(phob_lib INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/phob_rp2040.c
  ${CMAKE_CURRENT_LIST_DIR}/joybus.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../src/curve_fitting.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../src/filter.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../src/phob.c
  ${CMAKE_CURRENT_LIST_DIR}/../../../src/stick_cal.c
)

target_include_directories(phob_lib INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../../../src
)

pico_generate_pio_header(phob_lib ${CMAKE_CURRENT_LIST_DIR}/../pio_phob/joybus.pio)
#pico_generate_pio_header(phob_lib ${CMAKE_CURRENT_LIST_DIR}/../pio/cvideo.pio)

add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/../pio_generated/joybus.pio.h
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/../pio_phob/joybus.pio
	COMMAND Pioasm ${CMAKE_CURRENT_LIST_DIR}/../pio_phob/joybus.pio ${CMAKE_CURRENT_LIST_DIR}/../pio_generated/joybus.pio.h
)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/cvideo.pio.h
#	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/cvideo.pio
#	COMMAND Pioasm ${CMAKE_CURRENT_LIST_DIR}/cvideo.pio ${CMAKE_CURRENT_LIST_DIR}/generated/cvideo.pio.h
#	)

# Pull in pico libraries that we need
target_link_libraries(phob_lib INTERFACE
  pico_stdlib
  pico_stdlib
	pico_bootrom
	pico_multicore
	pico_platform
	hardware_dma
	hardware_flash
	hardware_pio
	hardware_sync
	hardware_timer

  tinyusb_device
  tinyusb_board
)
