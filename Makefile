# enough to check syntax on my cygwin box
check:  fakebuild fakeclean

fakebuild:
	( echo '#line 1 "dlg4.ino"' ; cat dlg4.ino ) > dlg4.cpp
	'/cygdrive/c/Program Files/Arduino/hardware/tools/avr/bin/avr-g++.exe' \
	  -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections \
	  -mmcu=atmega328p -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
	  -I'C:\Program Files\Arduino\hardware\arduino\cores\arduino' \
	  -I'C:\Program Files\Arduino\hardware\arduino\variants\standard' \
	  -I'C:\Documents and Settings\zeroth\My Documents\Arduino\libraries\Adafruit_NeoPixel' \
	  dlg4.cpp -o dlg4.o

fakeclean:
	rm dlg4.cpp dlg4.o dlg4.d

.PHONY:  check fakebuild fakeclean
