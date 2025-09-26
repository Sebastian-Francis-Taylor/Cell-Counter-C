CC = gcc
CFLAGS = -Wall -O3 -I./src
DEBUG_CFLAGS = -Wall -g -O0 -DDEBUG -I./src
TIMING_CFLAGS = -Wall -O3 -DTIMING -I./src
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/cbmp.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEBUG_OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_debug.o)
TIMING_OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_timing.o)
TARGET = $(BIN_DIR)/cell-counter
TARGET_EXE = $(BIN_DIR)/cell-counter.exe
DEBUG_TARGET = $(BIN_DIR)/cell-counter-debug
TIMING_TARGET = $(BIN_DIR)/cell-counter-timing

.PHONY: all debug timing clean valgrind

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_EXE): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(DEBUG_CFLAGS) -o $@ $^

timing: $(TIMING_TARGET)

$(TIMING_TARGET): $(TIMING_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(TIMING_CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_debug.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_timing.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(TIMING_CFLAGS) -c $< -o $@

valgrind: debug
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all $(DEBUG_TARGET)

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*_debug.o $(BUILD_DIR)/*_timing.o $(TARGET) $(TARGET_EXE) $(DEBUG_TARGET) $(TIMING_TARGET)
