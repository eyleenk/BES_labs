CC := arm-none-eabi-gcc
NAME := lab

vpath %.c ../common ../ti

OBJECTS := main.o led_interface.o startup.o uartstdio.o syscalls.o
DEPS := $(OBJECTS:.o=.d)

CFLAGS := -I../ti
CFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -std=c99 -Wall -Wextra -Wshadow -pedantic -g3 -gdwarf-4 -O0
CFLAGS += -Wstack-usage=128 -fanalyzer -Wno-analyzer-infinite-loop
CFLAGS += -MMD -MP
CFLAGS += -DTARGET_IS_TM4C123_RB1 -DPART_TM4C123GH6PM

LDFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
LDFLAGS += -Wl,--entry=ResetISR
LDFLAGS += -Wl,-T,../common/config.ld -Wl,-Map,$(NAME).map
LDFLAGS += -Wl,--gc-sections -static
LDFLAGS += --specs=nano.specs --specs=nosys.specs

LDLIBS := -L../ti/ -ldriver -lgcc -lc -lm

ifeq ($(OS),Windows_NT)
	HOST_OS := win
	RM := del /f /q
	EXE := .exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		HOST_OS := linux
	else ifeq ($(UNAME_S),Darwin)
		HOST_OS := mac
	endif
	EXE := 
endif

HOST_CC := gcc
UNITY_DIR := ../unity

TESTABLE_SRC := led_interface.c
TEST_OBJ := test/test_led_interface_$(HOST_OS).o
TEST_TARGET := test_runner

TEST_INC := -I. -I$(UNITY_DIR)
TEST_CFLAGS := -std=c99 -Wall -pedantic -g -O0 -ffunction-sections -fdata-sections

GRADER_SRC := verify.c
GRADER_OBJ := $(GRADER_SRC:.c=.o)

.PHONY: all clean test grader
.SILENT: $(OBJECTS) $(GRADER_OBJ)

all: $(NAME).axf
	@echo "SUCCESS: Compilation completed successfully."

$(NAME).axf: $(OBJECTS) $(GRADER_OBJ)
	@$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(TESTABLE_SRC) $(UNITY_DIR)/unity.c $(TEST_OBJ)
	@$(HOST_CC) $(TEST_CFLAGS) $(TEST_INC) -o $(TEST_TARGET) $^
	@echo "--------------------------------------------------"
	@echo "Running Unit Tests on Host PC:"
	@echo "--------------------------------------------------"
	@./$(TEST_TARGET)

grader:
	@$(CC) $(CFLAGS) -Os -g0 -c $(GRADER_SRC) -o $(GRADER_OBJ)

-include $(DEPS)

clean:
	@$(RM) $(OBJECTS) $(DEPS) $(NAME).axf $(NAME).map $(TEST_TARGET)$(EXE)
