################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/emdrv/sleep/src/sleep.c 

OBJS += \
./Drivers/sleep.o 

C_DEPS += \
./Drivers/sleep.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/sleep.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/emdrv/sleep/src/sleep.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DGCC_ARMCM3=1' '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/freertos_blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/common/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/dmadrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/ezradiodrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/nvm/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/rtcdrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/spidrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/tempdrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/uartdrv/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/ustimer/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/dmadrv/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/gpiointerrupt/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/nvm/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/rtcdrv/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/sleep/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/spidrv/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/uartdrv/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/ustimer/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emdrv/tempdrv/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//util/third_party/freertos/Source/include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//util/third_party/freertos/Demo/Common/include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//util/third_party/freertos/Source/portable/GCC/ARM_CM3" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"Drivers/sleep.d" -MT"Drivers/sleep.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


