# motor_control_interface
[![Build Status](https://travis-ci.org/vortexntnu/motor-control-interface.svg?branch=master)](https://travis-ci.org/vortexntnu/motor-control-interface)

MCU interface with electronic speed controller (ESC). Prototyping is done on an
EFM32 Giant Gecko 3700 Starter Kit.

## Building
There are two ways to build this firmware, with Simplicity Studio or using a Docker image.

### Docker
To build with Docker you of course need to have Docker installed, get that
[here](https://docs.docker.com/install/linux/docker-ce/ubuntu/#install-using-the-repository).
Then you simply run the included `build.sh` script, which will generate binaries into the build
folder.

### Simplicity Studio
First you must install these dependencies:

- [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio)

- [Toolchain](https://launchpad.net/gcc-arm-embedded/4.8/4.8-2013-q4-major)

- [Gecko SDK](https://github.com/SiliconLabs/Gecko_SDK/releases)

Then you create a workspace in Simplicity Studio and symlink the source and include folder into
that workspace.
