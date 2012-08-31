################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: BolsaBass.axf

# Tool invocations
BolsaBass.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/owen/workspace-redsuite/StdPeriphLib_v100_STM32F4xx/Debug" -L"/home/owen/workspace-redsuite/CMSISv2p10_STM32F4xx/Debug" -L"/home/owen/workspace-redsuite/CMSISv2p10_DSPLIB_CM4/lib" -Xlinker -Map="BolsaBass.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mthumb -T "STM32F4_test_Debug.ld" -o"BolsaBass.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) BolsaBass.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "BolsaBass.axf";  arm-none-eabi-objcopy -O binary "BolsaBass.axf" "BolsaBass.bin" ; checksum -p STM32F407VE -d "BolsaBass.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
