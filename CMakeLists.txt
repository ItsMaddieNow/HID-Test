# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(HID-Test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(HID-Test src/HID-Test.cpp src/usb_descriptors.c)

target_include_directories(HID-Test PRIVATE include/)

pico_set_program_name(HID-Test "HID-Test")
pico_set_program_version(HID-Test "0.1")

pico_enable_stdio_uart(HID-Test 1)
pico_enable_stdio_usb(HID-Test 0)

# Add the standard library to the build
target_link_libraries(HID-Test pico_stdlib)

target_link_libraries(HID-Test
        tinyusb_device
        tinyusb_board
        )

pico_add_extra_outputs(HID-Test)

