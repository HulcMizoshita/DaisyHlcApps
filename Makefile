# Project Name
TARGET = DaisyHlcApps
# APP_TYPE = BOOT_SRAM

OPT = -Os -Wno-unknown-pragmas

# Sources
CPP_SOURCES += \
DaisyHlcApps.cpp \
Utils/SampleHolder.cpp \
Utils/DelayLine.cpp \
Apps/AppUnit.cpp \
Apps/BiquadFilter/BiquadFilter.cpp \
Apps/DualVca/DualVca.cpp

# Library Locations
LIBDAISY_DIR = ./OutSourced/libDaisy/
DAISYSP_DIR = ./OutSourced/DaisySP/

# Includes FatFS source files within project.
USE_FATFS = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile