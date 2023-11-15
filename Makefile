# Project Name
TARGET = DaisyHlcApps
# APP_TYPE = BOOT_SRAM

OPT = -Os

# Sources
CPP_SOURCES += \
DaisyHlcApps.cpp \
Utils/SampleHolder.cpp \
Utils/MainDelayLine.cpp \
Apps/AppUnit.cpp

# Library Locations
LIBDAISY_DIR = ./OutSourced/libDaisy/
DAISYSP_DIR = ./OutSourced/DaisySP/

# Includes FatFS source files within project.
USE_FATFS = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile