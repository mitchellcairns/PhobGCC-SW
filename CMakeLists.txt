cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(phobgcc_rp2040 CXX C ASM)

pico_sdk_init()

add_subdirectory(lib/phob-lib/portable/raspberrypi/rp2040)

include_directories(
	include
	)

add_executable(${PROJECT_NAME}
	main.c
	)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
	#PICO_DEFAULT_BOOT_STAGE2=boot2_generic_03h
	#PICO_FLASH_SPI_CLKDIV=8
	)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	phob_lib
	hardware_gpio
	hardware_pwm
	hardware_gpio
	hardware_adc
	hardware_spi
	)

pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_usb(${PROJECT_NAME} 0)
