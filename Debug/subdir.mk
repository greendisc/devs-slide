################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../dc-factory.cpp \
../dc-simulator.cpp \
../sim-launcher.cpp 

OBJS += \
./dc-factory.o \
./dc-simulator.o \
./sim-launcher.o 

CPP_DEPS += \
./dc-factory.d \
./dc-simulator.d \
./sim-launcher.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


