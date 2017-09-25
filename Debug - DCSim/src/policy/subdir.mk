################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/policy/budget_heat.cpp \
../src/policy/cooling_policy.cpp \
../src/policy/fixed_inlet.cpp 

OBJS += \
./src/policy/budget_heat.o \
./src/policy/cooling_policy.o \
./src/policy/fixed_inlet.o 

CPP_DEPS += \
./src/policy/budget_heat.d \
./src/policy/cooling_policy.d \
./src/policy/fixed_inlet.d 


# Each subdirectory must supply rules for building sources it contributes
src/policy/%.o: ../src/policy/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


