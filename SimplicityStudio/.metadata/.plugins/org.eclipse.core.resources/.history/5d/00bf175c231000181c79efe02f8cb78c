################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFM32GG/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: motor-controller.axf

# Tool invocations
motor-controller.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "motor-controller.ld" -Xlinker --gc-sections -Xlinker -Map="motor-controller.map" --specs=nano.specs -o motor-controller.axf "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./emlib/em_cmu.o" "./emlib/em_gpio.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./src/main.o" "./src/pwm.o" "./src/uart.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: motor-controller.hex'
	arm-none-eabi-objcopy -O ihex "motor-controller.axf" "motor-controller.hex"
	@echo ' '

	@echo 'Building bin file: motor-controller.bin'
	arm-none-eabi-objcopy -O binary "motor-controller.axf" "motor-controller.bin"
	@echo ' '

	@echo 'Building s37 file: motor-controller.s37'
	arm-none-eabi-objcopy -O srec "motor-controller.axf" "motor-controller.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "motor-controller.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) motor-controller.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
