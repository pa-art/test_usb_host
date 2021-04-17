cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project)

pico_sdk_init()

add_executable(dropping_monsters_usb
        dropping_monsters_usb.c pico_ntsc_grph.c mini_gamepad.c
        )

target_include_directories(dropping_monsters_usb PRIVATE ${CMAKE_CURRENT_LIST_DIR})
# Pull in our (to be renamed) simple get you started dependencies
target_link_libraries(dropping_monsters_usb PRIVATE pico_stdlib hardware_adc hardware_pwm tinyusb_host tinyusb_board)

# create map/bin/hex file etc.
pico_add_extra_outputs(dropping_monsters_usb)

pico_enable_stdio_usb(dropping_monsters_usb 0)

pico_enable_stdio_uart(dropping_monsters_usb 0)
