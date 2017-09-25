################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/weather/weather.cpp 

OBJS += \
./src/weather/weather.o 

CPP_DEPS += \
./src/weather/weather.d 


# Each subdirectory must supply rules for building sources it contributes
src/weather/%.o: ../src/weather/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/jlrisco/TrabajoExtra/GIT/jlrisco@github.com/devs-slide/lib/xdevs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


