################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../xdevs-temp/main_temp.cpp 

OBJS += \
./xdevs-temp/main_temp.o 

CPP_DEPS += \
./xdevs-temp/main_temp.d 


# Each subdirectory must supply rules for building sources it contributes
xdevs-temp/%.o: ../xdevs-temp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


