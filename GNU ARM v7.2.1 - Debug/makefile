################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include drivers/subdir.mk
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
all: hazi_feladat_torpedo.axf

# Tool invocations
hazi_feladat_torpedo.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "hazi_feladat_torpedo.ld" -Xlinker --gc-sections -Xlinker -Map="hazi_feladat_torpedo.map" --specs=nano.specs -o hazi_feladat_torpedo.axf "./CMSIS/EFM32GG/startup_efm32gg.o" "./CMSIS/EFM32GG/system_efm32gg.o" "./drivers/segmentlcd.o" "./emlib/em_cmu.o" "./emlib/em_lcd.o" "./emlib/em_system.o" "./src/game_logic.o" "./src/hardware_control.o" "./src/main.o" "./src/segmentlcd_individual.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: hazi_feladat_torpedo.hex'
	arm-none-eabi-objcopy -O ihex "hazi_feladat_torpedo.axf" "hazi_feladat_torpedo.hex"
	@echo ' '

	@echo 'Building bin file: hazi_feladat_torpedo.bin'
	arm-none-eabi-objcopy -O binary "hazi_feladat_torpedo.axf" "hazi_feladat_torpedo.bin"
	@echo ' '

	@echo 'Building s37 file: hazi_feladat_torpedo.s37'
	arm-none-eabi-objcopy -O srec "hazi_feladat_torpedo.axf" "hazi_feladat_torpedo.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "hazi_feladat_torpedo.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) hazi_feladat_torpedo.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
