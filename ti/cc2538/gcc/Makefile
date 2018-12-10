###############################################################################

# Project name and files to compile
PROJECT_NAME = libdriverlib
PROJECT_HOME = .

###############################################################################

# Toolchain executables
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
OBJSIZE = arm-none-eabi-size

###############################################################################

# C compiler flags
CFLAGS += -mthumb -mcpu=cortex-m3 -mlittle-endian
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fstrict-aliasing
CFLAGS += -fshort-enums
CFLAGS += -fomit-frame-pointer
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=c99
CFLAGS += -Wall -pedantic
CFLAGS += -O0
CFLAGS += -g -ggdb

# C archiver flags
ARFLAGS = r

# Makefile flags
MAKEFLAGS = -j1

###############################################################################

# Define the Platform path
PLATFORM_SRC = $(PROJECT_HOME)/../src
PLATFORM_INC = $(PROJECT_HOME)/../inc

# Append to the include path
INC_PATH += -I $(PLATFORM_SRC)
INC_PATH += -I $(PLATFORM_INC)

# Extend the virtual path
VPATH += $(PLATFORM_SRC) $(PLATFORM_INC)

###############################################################################

# Include the Makefiles in the various subdirectories in the current folder
# include $(PLATFORM_SRC)/Makefile.include
SRC_FILES := $(shell find $(PLATFORM_SRC) -name '*.c')

###############################################################################

# Define the name and path where the temporary object files are stored
BIN_PATH = bin

TMP_FILES = $(notdir ${SRC_FILES})

# Coverts the source files (c and cpp) to object files (o) to be used as targets
TMP_FILES2 = $(patsubst %.c, %.o, ${TMP_FILES})

# Filters out the source files (c and cpp) from the object files (o) to be compiled
BIN_FILES = $(filter-out %.c %.h, $(TMP_FILES2))

# Adds the path to where the object files need to be stored
BIN_TARGET = $(addprefix $(BIN_PATH)/, $(BIN_FILES))

###############################################################################

.DEFAULT_GOAL = library

.PHONY: library
library: pre build post

pre:
	@echo "Building '$(PROJECT_NAME)' library..."
	@mkdir -p $(BIN_PATH)

build: $(BIN_TARGET)

post: $(PROJECT_NAME).a
	@echo "Archiving '$(PROJECT_NAME)' done."

.PHONY: clean
clean:
	@echo "Cleaning '$(PROJECT_NAME)' done."
	@rm -rf $(BIN_PATH)
	@rm -f $(PROJECT_NAME).a

###############################################################################

# Target to compile C files into object files
$(BIN_PATH)/%.o: %.c
	@echo "Compiling $<..."
	@-$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

# Target to build the library from the object files
$(PROJECT_NAME).a: $(BIN_TARGET)
	@-$(AR) $(ARFLAGS) $@ $^
