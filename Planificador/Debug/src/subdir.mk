################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Planificador.c \
../src/funcionesESI.c \
../src/funcionesPlanificador.c 

OBJS += \
./src/Planificador.o \
./src/funcionesESI.o \
./src/funcionesPlanificador.o 

C_DEPS += \
./src/Planificador.d \
./src/funcionesESI.d \
./src/funcionesPlanificador.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


