# Project name and files to compile
PROJECT_NAME  = freertos-tickless
PROJECT_FILES = freertos-tickless.c stm32l1_low_power.c stm32l1xx_it.c
PROJECT_DIR   = .

# Location of the root directory
PROJECT_HOME = ../..

# Include the current path
INC_PATH += -I $(PROJECT_DIR)

# Define options passed to the C compiler
DOPTIONS += -DUSE_STDPERIPH_DRIVER
DOPTIONS += -DSTM32L1XX_MD

# Configure compiling options
USE_KERNEL = TRUE
USE_LIBRARY = FALSE

# Include the Makefile in the root directory
include $(PROJECT_HOME)/Makefile.include
