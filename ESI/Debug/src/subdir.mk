################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ESI.c \
../src/funcionesESI.c 

OBJS += \
./src/ESI.o \
./src/funcionesESI.o 

C_DEPS += \
./src/ESI.d \
./src/funcionesESI.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/parsi" -I"/home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket" -I"/home/utnso/workspace/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


