################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/xdevs/DevsAllocator.cpp \
../src/xdevs/DevsChiller.cpp \
../src/xdevs/DevsCooling.cpp \
../src/xdevs/DevsEnergyCalculator.cpp \
../src/xdevs/DevsIrc.cpp \
../src/xdevs/DevsJobsGenerator.cpp \
../src/xdevs/DevsPump.cpp \
../src/xdevs/DevsRack.cpp \
../src/xdevs/DevsRoom.cpp \
../src/xdevs/DevsServer.cpp \
../src/xdevs/DevsSlide.cpp \
../src/xdevs/DevsWeather.cpp \
../src/xdevs/Job.cpp \
../src/xdevs/main_xdevs.cpp 

OBJS += \
./src/xdevs/DevsAllocator.o \
./src/xdevs/DevsChiller.o \
./src/xdevs/DevsCooling.o \
./src/xdevs/DevsEnergyCalculator.o \
./src/xdevs/DevsIrc.o \
./src/xdevs/DevsJobsGenerator.o \
./src/xdevs/DevsPump.o \
./src/xdevs/DevsRack.o \
./src/xdevs/DevsRoom.o \
./src/xdevs/DevsServer.o \
./src/xdevs/DevsSlide.o \
./src/xdevs/DevsWeather.o \
./src/xdevs/Job.o \
./src/xdevs/main_xdevs.o 

CPP_DEPS += \
./src/xdevs/DevsAllocator.d \
./src/xdevs/DevsChiller.d \
./src/xdevs/DevsCooling.d \
./src/xdevs/DevsEnergyCalculator.d \
./src/xdevs/DevsIrc.d \
./src/xdevs/DevsJobsGenerator.d \
./src/xdevs/DevsPump.d \
./src/xdevs/DevsRack.d \
./src/xdevs/DevsRoom.d \
./src/xdevs/DevsServer.d \
./src/xdevs/DevsSlide.d \
./src/xdevs/DevsWeather.d \
./src/xdevs/Job.d \
./src/xdevs/main_xdevs.d 


# Each subdirectory must supply rules for building sources it contributes
src/xdevs/%.o: ../src/xdevs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/jlrisco/TrabajoExtra/GIT/greendisc@github.com/devs-slide/lib/xdevs" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


