# enough to check syntax on my cygwin box
check:  fakebuild fakeclean

fakebuild:
	cp chutil.ino chutil.cpp
	'/cygdrive/c/Program Files/Arduino/hardware/tools/avr/bin/avr-g++.exe' \
	  -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections \
	  -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	  -I'C:\Program Files\Arduino\hardware\arduino\cores\arduino' \
	  -I'C:\Program Files\Arduino\hardware\arduino\variants\standard' \
	  -I'C:\Documents and Settings\zeroth\My Documents\Arduino\libraries\Adafruit_NeoPixel' \
	  chutil.cpp -o chutil.o

fakeclean:
	rm chutil.cpp chutil.o chutil.d

.PHONY:  check fakebuild fakeclean
