################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_bcc.c \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk.c \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk_leds.c \
/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_trace.c 

OBJS += \
./BSP/bsp_bcc.o \
./BSP/bsp_stk.o \
./BSP/bsp_stk_leds.o \
./BSP/bsp_trace.o 

C_DEPS += \
./BSP/bsp_bcc.d \
./BSP/bsp_stk.d \
./BSP/bsp_stk_leds.d \
./BSP/bsp_trace.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_bcc.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_bcc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_bcc.d" -MT"BSP/bsp_bcc.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_stk.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_stk.d" -MT"BSP/bsp_stk.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_stk_leds.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_stk_leds.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_stk_leds.d" -MT"BSP/bsp_stk_leds.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_trace.o: /usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1/hardware/kit/common/bsp/bsp_trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32GG990F1024=1' '-DNDEBUG=1' -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//app/mcu_example/EFM32GG_STK3700/blink" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/EFM32GG_STK3700/config" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"/usr/local/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32GG/Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -MMD -MP -MF"BSP/bsp_trace.d" -MT"BSP/bsp_trace.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


