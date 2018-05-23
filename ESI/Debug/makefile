################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: ESI

dependents:
	-cd /home/utnso/workspace/commons/Debug && $(MAKE) all
	-cd /home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug && $(MAKE) all
	-cd /home/utnso/workspace/parsi/Debug && $(MAKE) all

# Tool invocations
ESI: $(OBJS) $(USER_OBJS) /home/utnso/workspace/commons/Debug/libcommons.so /home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug/libsocket.so /home/utnso/workspace/parsi/Debug/libparsi.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/parsi/Debug" -L"/home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug" -L"/home/utnso/workspace/commons/Debug" -o "ESI" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) ESI
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/commons/Debug/libcommons.so:
/home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug/libsocket.so:
/home/utnso/workspace/parsi/Debug/libparsi.so:

-include ../makefile.targets
