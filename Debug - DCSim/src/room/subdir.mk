################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/room/apc-irc.cpp \
../src/room/decathlete-server.cpp \
../src/room/dell-server.cpp \
../src/room/factory.cpp \
../src/room/irc.cpp \
../src/room/mghpcc-pod.cpp \
../src/room/rack-irc.cpp \
../src/room/rack.cpp \
../src/room/room.cpp \
../src/room/server.cpp \
../src/room/solana-server.cpp 

OBJS += \
./src/room/apc-irc.o \
./src/room/decathlete-server.o \
./src/room/dell-server.o \
./src/room/factory.o \
./src/room/irc.o \
./src/room/mghpcc-pod.o \
./src/room/rack-irc.o \
./src/room/rack.o \
./src/room/room.o \
./src/room/server.o \
./src/room/solana-server.o 

CPP_DEPS += \
./src/room/apc-irc.d \
./src/room/decathlete-server.d \
./src/room/dell-server.d \
./src/room/factory.d \
./src/room/irc.d \
./src/room/mghpcc-pod.d \
./src/room/rack-irc.d \
./src/room/rack.d \
./src/room/room.d \
./src/room/server.d \
./src/room/solana-server.d 


# Each subdirectory must supply rules for building sources it contributes
src/room/%.o: ../src/room/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


