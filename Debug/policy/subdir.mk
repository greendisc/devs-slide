################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../policy/budget_heat.cpp \
../policy/cooling_policy.cpp \
../policy/fixed_inlet.cpp 

OBJS += \
./policy/budget_heat.o \
./policy/cooling_policy.o \
./policy/fixed_inlet.o 

CPP_DEPS += \
./policy/budget_heat.d \
./policy/cooling_policy.d \
./policy/fixed_inlet.d 


# Each subdirectory must supply rules for building sources it contributes
policy/%.o: ../policy/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


