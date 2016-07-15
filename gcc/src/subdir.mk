# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/socket.cpp \
../src/webServer.cpp 

OBJS += \
./src/socket.o \
./src/webServer.o 

CPP_DEPS += \
./src/socket.d \
./src/webServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"../../cjCore/src" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


