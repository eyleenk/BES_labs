CC = gcc
SRC_FILE = check_linearity.c
TARGET = test_runner
CFLAGS = -std=c99 -Wall -pedantic -g -O0 -ffunction-sections -fdata-sections

# Host OS detection
ifeq ($(OS),Windows_NT)
	HOST_OS = win
	RM = del /f /q
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		HOST_OS = linux
	endif
	ifeq ($(UNAME_S),Darwin)
		HOST_OS = mac
		endif
	RM = rm -f
endif

.PHONY: clean

# Rule to build the test and execute it immediately
$(TARGET): $(SRC_FILE)
	@$(CC) $(CFLAGS) $(SRC_FILE) -o $@
	@./$@

clean:
	@$(RM) $(TARGET)
