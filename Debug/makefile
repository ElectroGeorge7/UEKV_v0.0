################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB_DEVICE/Target/subdir.mk
-include USB_DEVICE/App/subdir.mk
-include Terminal/subdir.mk
-include TempSensors/ts_max6675/subdir.mk
-include TempSensors/ts_hardware/subdir.mk
-include Tasks/subdir.mk
-include ResultCheck/ub_check/subdir.mk
-include ResultCheck/rch_hardware/subdir.mk
-include ResultCheck/ett_check/subdir.mk
-include ResultCheck/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/CDC/subdir.mk
-include Menu/rtc_hardware/subdir.mk
-include Menu/buttons_hardware/subdir.mk
-include Menu/subdir.mk
-include LedsMatrix/lm_hardware/subdir.mk
-include LedsMatrix/ARGB/subdir.mk
-include LedsMatrix/subdir.mk
-include LabPowerSupply/rs485_hardware/subdir.mk
-include FreeRTOS/Source/portable/MemMang/subdir.mk
-include FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include FreeRTOS/Source/subdir.mk
-include FATFS/sdmc_hardware/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/FatFs/src/option/subdir.mk
-include FATFS/FatFs/src/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Display/display_hardware/subdir.mk
-include Display/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := UEKV_v0.0
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
UEKV_v0.0.elf \

MAP_FILES += \
UEKV_v0.0.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
UEKV_v0.0.list \


# All Target
all: main-build

# Main-build Target
main-build: UEKV_v0.0.elf secondary-outputs

# Tool invocations
UEKV_v0.0.elf UEKV_v0.0.map: $(OBJS) $(USER_OBJS) C:\Users\EKF\Desktop\UEKV\UEKV_v0.0\STM32F446RCTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "UEKV_v0.0.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\EKF\Desktop\UEKV\UEKV_v0.0\STM32F446RCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="UEKV_v0.0.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

UEKV_v0.0.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "UEKV_v0.0.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) UEKV_v0.0.elf UEKV_v0.0.list UEKV_v0.0.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
