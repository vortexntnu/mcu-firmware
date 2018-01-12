################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/blink.c 

OBJS += \
./src/blink.o 

C_DEPS += \
./src/blink.d 


# Each subdirectory must supply rules for building sources it contributes
src/blink.o: ../src/blink.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"src/blink.d" -MT"src/blink.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


