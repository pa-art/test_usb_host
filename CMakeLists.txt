cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project)

pico_sdk_init()

add_executable(host_hid
        host_hid.c pico_ntsc_grph.c
        )

target_include_directories(host_hid PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(host_hid PRIVATE pico_stdlib hardware_pwm tinyusb_host tinyusb_board)
pico_add_extra_outputs(host_hid)

# add url via pico_set_program_url
#example_auto_set_url(host_hid)

pico_enable_stdio_usb(host_hid 0)
pico_enable_stdio_uart(host_hid 0)