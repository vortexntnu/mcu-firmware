# Base EFM32 CMake file
#
## Copyright (c) 2016 Ryan Kurte
# This file is covered under the MIT license available at: https://opensource.org/licenses/MIT

###### Project Environment #####

# Set minimum CMake version
cmake_minimum_required(VERSION 2.8.4)

# Set efm32-base project location
set(BASE_LOCATION efm32-base)

# Set the compiler (must be prior to project setup)
include(${BASE_LOCATION}/toolchain/arm-gcc.cmake)

##### Project Setup #####

# Set our output target
set(TARGET mcu-firmware)

# Configure project and languages
project(${TARGET} C CXX ASM)

# Set device
if(NOT DEVICE)
set(DEVICE EFM32GG942F1024)
endif()

# Set build
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE DEBUG)
endif()

##### Modules #####

# Libraries can be added to the LIBS variable
# or manually included here.

# Add base libs (emlib, CMSIS, device files)
include(${BASE_LOCATION}/toolchain/efm32-base.cmake)

##### Files #####

# Add project headers
include_directories(${BASE_LOCATION}/include)
include_directories(efm32gg/include)
# Add project sources
set(SOURCES
  efm32gg/src/main.c
  efm32gg/src/crc.c
  efm32gg/src/pwm.c
  efm32gg/src/uart.c
  efm32gg/src/watchdog.c
  efm32gg/src/rov_utilities.c
)

##### Outputs #####

# Generate executable and link
add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${LIBS})

##### Post build #####

# Add post build commands
include(${BASE_LOCATION}/toolchain/post-build.cmake)

# Add JLink commands
include(${BASE_LOCATION}/toolchain/jlink.cmake)
