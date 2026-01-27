# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ps1controlleremu C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ps1controlleremu src/main.c )

pico_generate_pio_header(ps1controlleremu ${CMAKE_SOURCE_DIR}/psxSPI.pio)

pico_set_program_name(ps1controlleremu "ps1controlleremu")
pico_set_program_version(ps1controlleremu "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_usb(ps1controlleremu 1)

# Uncomment the following line to use RP2040-Zero pinout 
#target_compile_definitions(ps1controlleremu PRIVATE RP2040ZERO)

# Add the standard library to the build
target_link_libraries(ps1controlleremu
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(ps1controlleremu PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        PRIVATE include
)

# Add any user requested libraries
target_link_libraries(ps1controlleremu 
        hardware_pio
        )

pico_add_extra_outputs(ps1controlleremu)

