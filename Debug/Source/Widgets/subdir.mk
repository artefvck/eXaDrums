################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Source/Widgets/Fader.cpp \
../Source/Widgets/InstrumentSelector.cpp \
../Source/Widgets/SoundTypeAndPath.cpp \
../Source/Widgets/TriggerIdAndLocation.cpp \
../Source/Widgets/TriggerSelector.cpp 

OBJS += \
./Source/Widgets/Fader.o \
./Source/Widgets/InstrumentSelector.o \
./Source/Widgets/SoundTypeAndPath.o \
./Source/Widgets/TriggerIdAndLocation.o \
./Source/Widgets/TriggerSelector.o 

CPP_DEPS += \
./Source/Widgets/Fader.d \
./Source/Widgets/InstrumentSelector.d \
./Source/Widgets/SoundTypeAndPath.d \
./Source/Widgets/TriggerIdAndLocation.d \
./Source/Widgets/TriggerSelector.d 


# Each subdirectory must supply rules for building sources it contributes
Source/Widgets/%.o: ../Source/Widgets/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++17 -I"/home/jeremy/Desktop/Prog/eXaDrums/libeXaDrums" -I../../libeXaDrums -I/usr/include/gtkmm-3.0 -I/usr/lib/x86_64-linux-gnu/gtkmm-3.0/include -I/usr/include/atkmm-1.6 -I/usr/include/gtk-3.0/unix-print -I/usr/include/gdkmm-3.0 -I/usr/lib/x86_64-linux-gnu/gdkmm-3.0/include -I/usr/include/giomm-2.4 -I/usr/lib/x86_64-linux-gnu/giomm-2.4/include -I/usr/include/pangomm-1.4 -I/usr/lib/x86_64-linux-gnu/pangomm-1.4/include -I/usr/include/glibmm-2.4 -I/usr/lib/x86_64-linux-gnu/glibmm-2.4/include -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/atk-1.0 -I/usr/include/cairomm-1.0 -I/usr/lib/x86_64-linux-gnu/cairomm-1.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


