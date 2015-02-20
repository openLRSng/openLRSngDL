##################################################################
#
# Makefile for OpenLRSng
#

#
# If your Arduino is in a weird place, you'll need to change this.
#
ARDUINO_PATH=/usr/share/arduino

#
# Board type can be one of 6 values:
# 0 - Flytron M1 TX
# 1 - Flytron M1 RX
# 2 - Flytron M2, M3 TX, Orange-TX
# 3 - Flytron V2 RX, Hawkeye RX, HK Orange-RX
# 4 - Hawkeye TX, OpenLRSng TX
# 5 - DTF 4ch RX
# 6 - Deluxe TX
# 7 - PowerTowerRX
#
BOARD_TYPE=3
#BOARD_TYPES=0 2 3 4 5 6 7 8 9
BOARD_TYPES=3

#
# No real user options below here.
##################################################################

#
# You don't want to change this unless you really know that you
# need to.  CPU clock.
#
CLOCK=16000000L

#
# Board type 6 requires a different Arduino target
#
ifeq ($(BOARD_TYPE),6)
CPU=atmega32u4
USB_VID=0x2341
USB_PID=0x8036
VARIANT=leonardo
BOOTLOADER=Caterina-Leonardo.hex
else
CPU=atmega328p
USB_VID=null
USB_PID=null
VARIANT=standard
BOOTLOADER=optiboot_atmega328.hex
endif

#
# C preprocessor defines
#
DEFINES=-DBOARD_TYPE=$(BOARD_TYPE) -DCOMPILE_TX=$(COMPILE_TX) -DRFMTYPE=$(RFMTYPE)

#
# AVR GCC info
#
EXEPREFIX=avr-
ifneq (,$(wildcard $(ARDUINO_PATH)/hardware/tools/avr/bin/avr-gcc))
	EXEPATH=$(ARDUINO_PATH)/hardware/tools/avr/bin
else ifneq (,$(wildcard /usr/bin/avr-gcc))
	EXEPATH=/usr/bin
endif

#
# AVR gcc and binutils
#
CC=$(EXEPATH)/$(EXEPREFIX)gcc
CXX=$(EXEPATH)/$(EXEPREFIX)g++
AR=$(EXEPATH)/$(EXEPREFIX)ar
SIZE=$(EXEPATH)/$(EXEPREFIX)size
OBJCOPY=$(EXEPATH)/$(EXEPREFIX)objcopy

#
# Shell commands
#
RM=rm
MKDIR=mkdir
LS=ls
SED=sed
CAT=cat

#
# Styling
#
ASTYLE=astyle
ASTYLEOPTIONS=--options=none --style=1tbs --indent=spaces=2 --suffix=none --lineend=linux

#
# Compile flags
#
COPTFLAGS= -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
	   -fno-inline-small-functions -Wl,--relax -mcall-prologues

CFLAGS=-Wall -ffunction-sections -fdata-sections -mmcu=$(CPU) -DF_CPU=$(CLOCK) -MMD \
	-DUSB_VID=$(USB_VID) -DUSB_PID=$(USB_PID) -DARDUINO=105 -D__PROG_TYPES_COMPAT__ $(DEFINES)
CXXFLAGS=-fno-exceptions

#
# Arduino libraries used, compilation settings.
#
ARDUINO_LIBS=
ARDUINO_LIB_PATH=$(ARDUINO_PATH)/libraries/
ARDUINO_LIB_DIRS=$(addprefix $(ARDUINO_LIB_PATH),$(ARDUINO_LIBS))
ARDUINO_LIB_INCL=$(addsuffix $(ARDUINO_LIBS),-I$(ARDUINO_LIB_PATH))
ARDUINO_LIB_SRCS=$(addsuffix .cpp,$(addprefix $(ARDUINO_LIB_PATH),$(ARDUINO_LIBS)/$(ARDUINO_LIBS)))
ARDUINO_LIB_OBJS=$(patsubst %.cpp, libraries/%.o, $(addsuffix .cpp,$(ARDUINO_LIBS)))

#
# Arduino variant settings
#
ARDUINO_VARIANT_PATH=$(ARDUINO_PATH)/hardware/arduino/variants/$(VARIANT)

#
# Arduino library files used, compilation settings.
#
ARDUINO_CORELIB_PATH=$(ARDUINO_PATH)/hardware/arduino/cores/arduino/
ARDUINO_CORELIB_SRCS=WInterrupts.c wiring.c wiring_digital.c main.cpp
ARDUINO_CORELIB_OBJS= $(patsubst %.c, libraries/%.o, $(patsubst %.cpp, libraries/%.o, $(ARDUINO_CORELIB_SRCS)))


#
# Arduino stdc library files used, compilation settings.
#
ARDUINO_LIBC_PATH=$(ARDUINO_PATH)/hardware/arduino/cores/arduino/avr-libc/
ARDUINO_LIBC_SRCS=malloc.c realloc.c

#
# Master include path
#
INCLUDE=-I$(ARDUINO_CORELIB_PATH) -I$(ARDUINO_VARIANT_PATH) $(ARDUINO_LIB_INCL) -I.

#
# Project folders
#
LIBRARIES_FOLDER=libraries
OUT_FOLDER=out

#
# Target object files
#
OBJS=openLRSngDL.o serial.o $(ARDUINO_LIB_OBJS) $(LIBRARIES_FOLDER)/libcore.a

#
# Master target
#
allfw: 433 868 915

all: mkdirs openLRSngDL.hex

#
# From here down are build rules
#
VPATH := $(ARDUINO_LIB_DIRS) $(ARDUINO_CORELIB_PATH) $(ARDUINO_LIBC_PATH)

define ino-command
	@$(CXX) -c $(COPTFLAGS) $(CXXFLAGS) $(CFLAGS) $(INCLUDE) -o $@ -x c++ $<
endef
define cc-command
	@$(CC) -c $(COPTFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $<
endef
define cxx-command
	@$(CXX) -c $(COPTFLAGS) $(CXXFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $<
endef

.PHONY: all clean upload astyle 433 868 915 allfw

%.o: %.ino
	$(ino-command)

%.o: %.c
	$(cc-command)

%.o: %.cpp
	$(cxx-command)

$(LIBRARIES_FOLDER)/%.o: %.c
	$(cc-command) 2>/dev/null

$(LIBRARIES_FOLDER)/%.o: %.cpp
	$(cxx-command) 2>/dev/null

#
# Other targets
#
clean: clean_compilation_products
	@$(RM) -rf $(OUT_FOLDER) *~ \#*\#

clean_compilation_products:
	@$(RM) -rf $(LIBRARIES_FOLDER)
	@$(RM) -f *.[aod] *.elf *.eep *.d *.hex

mkdirs:
	@$(MKDIR) -p $(LIBRARIES_FOLDER)

openLRSngDL.hex: $(OBJS)
	@$(CC) -Os -Wl,--gc-sections -mmcu=$(CPU) -o openLRSngDL.elf $(OBJS) -L$(LIBRARIES_FOLDER) -lm
	@$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load \
		--no-change-warnings --change-section-lma .eeprom=0 \
		openLRSngDL.elf openLRSngDL.eep
	@$(OBJCOPY) -O ihex -R .eeprom openLRSngDL.elf openLRSngDL.hex
	@echo "NOTE: Deployment size is text + data."
	@$(SIZE) openLRSngDL.elf
	@$(SED) "/:00000001FF/d" openLRSngDL.hex > openLRSngDLBL.hex
	@$(CAT) bootloaders/$(BOOTLOADER) >> openLRSngDLBL.hex

$(LIBRARIES_FOLDER)/libcore.a: $(ARDUINO_CORELIB_OBJS)
	@$(AR) rcs $(LIBRARIES_FOLDER)/libcore.a $(ARDUINO_CORELIB_OBJS)

astyle:
	$(ASTYLE) $(ASTYLEOPTIONS) openLRSngDL.ino *.h *.cpp

433 868 915:
	$(RM) -rf $(OUT_FOLDER)/$@
	$(MKDIR) -p $(OUT_FOLDER)/$@
	$(foreach type, $(BOARD_TYPES), make -s RFMTYPE=$@ BOARD_TYPE=$(type) clean_compilation_products all && cp openLRSngDL.hex $(OUT_FOLDER)/$@/DL-$(type).hex && cp openLRSngDLBL.hex $(OUT_FOLDER)/$@/DL-$(type)-bl.hex;)



