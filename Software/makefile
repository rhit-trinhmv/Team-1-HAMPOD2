# Compiler flags
CC = gcc
CFLAGS = -rdynamic  -Wall -g -lm
LDFLAGS = -lpthread -lhamlib -lwiringPi 
# Files
HEADERS := $(wildcard Modes/*.h) $(wildcard HamlibWrappedFunctions/*.h) $(wildcard APIObjects/*.h) $(wildcard ConfigSettings/*.h) 
LIB_SRCS := $(wildcard Modes/*.c) $(wildcard HamlibWrappedFunctions/*.c) $(wildcard APIObjects/*.c) $(wildcard ConfigSettings/*.c)
EXEC_SRC := Startup.c

# Shared library name
LIB_NAME := libModes.so

# Executable name
EXEC_NAME := Software.elf

ifeq ($(LEVEL),1)
    LDFLAGS += -DOUTPUTLEVEL1
endif

ifeq ($(LEVEL),2)
    LDFLAGS += -DOUTPUTLEVEL1
	LDFLAGS += -DOUTPUTLEVEL2
endif

ifeq ($(TESTING),2)
    LDFLAGS += -DTESTING
	EXEC_SRC = UnitTesting/UnitTesting.c
endif

# Targets
all: $(EXEC_NAME)

# Build shared library
$(LIB_NAME): $(LIB_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -rdynamic -DSHAREDLIB -fPIC -shared -o $(LIB_NAME) $(LIB_SRCS) $(LDFLAGS)

# Build executable
$(EXEC_NAME): $(EXEC_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) -rdynamic -o $(EXEC_NAME) $(EXEC_SRC) -I. -L. -l:$(LIB_NAME) -ldl $(LDFLAGS) -Wl,-rpath,'$$ORIGIN'

# Clean
clean:
	rm -f $(LIB_NAME) $(EXEC_NAME)
	@rm -f /s *.o   # Remove object files
	@rm -f *.exe  # Remove the executable (replace 'my_program' with the actual name)
	@rm -f *.elf
	@rm -f *.wav
	@rm -f *.so

# Phony targets
.PHONY: all clean
