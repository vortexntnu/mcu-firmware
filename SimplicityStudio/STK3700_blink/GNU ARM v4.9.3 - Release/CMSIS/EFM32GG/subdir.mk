################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/Device/SiliconLabs/EFM32GG/Source/system_efm32gg.c 

S_UPPER_SRCS += \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/Device/SiliconLabs/EFM32GG/Source/GCC/startup_efm32gg.S 

OBJS += \
./CMSIS/EFM32GG/startup_efm32gg.o \
./CMSIS/EFM32GG/system_efm32gg.o 

C_DEPS += \
./CMSIS/EFM32GG/system_efm32gg.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/EFM32GG/startup_efm32gg.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/Device/SiliconLabs/EFM32GG/Source/GCC/startup_efm32gg.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Assembler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" '-DEFM32GG990F1024=1' -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/EFM32GG/system_efm32gg.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/platform/Device/SiliconLabs/EFM32GG/Source/system_efm32gg.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"CMSIS/EFM32GG/system_efm32gg.d" -MT"CMSIS/EFM32GG/system_efm32gg.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


