################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../socket/generarConfigs.c \
../socket/socket.c 

OBJS += \
./socket/generarConfigs.o \
./socket/socket.o 

C_DEPS += \
./socket/generarConfigs.d \
./socket/socket.d 


# Each subdirectory must supply rules for building sources it contributes
socket/%.o: ../socket/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


