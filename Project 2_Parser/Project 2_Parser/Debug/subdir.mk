################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/users/guest/d/dhyer3/Old\ Labs/CS236/Lab\ 1_Scanner/main.cpp 

OBJS += \
./main.o 

CPP_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
main.o: /users/guest/d/dhyer3/Old\ Labs/CS236/Lab\ 1_Scanner/main.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


