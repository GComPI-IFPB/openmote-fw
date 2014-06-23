# Project name and files to compile
PROJECT_NAME  = test-stm32l1
PROJECT_FILES = test-stm32l1.cpp stm32l1xx_it.cpp
PROJECT_DIR   = .

# Location of the root directory
PROJECT_HOME = ../..

# Include the current path
INC_PATH += -I $(PROJECT_DIR)

# Define options passed to the C compiler
DOPTIONS += -DUSE_STDPERIPH_DRIVER
DOPTIONS += -DSTM32L1XX_MD

# Configure compiling
USE_KERNEL = TRUE
USE_LIBRARY = FALSE

# Include the Makefile in the root directory
include $(PROJECT_HOME)/Makefile.include
