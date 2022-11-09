.PHONY: all clean check debug profile

CFLAGS := -std=c18
CFLAGS += -Wall -Werror -Wextra -Wpedantic -Winline
CFLAGS += -Wwrite-strings -Wvla -Wfloat-equal -Waggregate-return
CFLAGS += -D_DEFAULT_SOURCE

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TST_DIR = test

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

EXE_NAME := C_Libraries
EXE := $(BIN_DIR)/$(EXE_NAME)
CHECK := $(EXE_NAME)_check

TSTS := $(wildcard $(TST_DIR)/*.c)
TST_OBJS := $(filter-out $(OBJ_DIR)/$(BIN).o, $(OBJS))
TST_OBJS += $(patsubst $(TST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TSTS))
TST_LIBS := -lcheck -lm -pthread -lrt -lsubunit

EXE_STATS := $(EXE_NAME)_stats

CC := gcc

all: $(EXE)

check: $(CHECK)

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(CHECK)

profile: CFLAGS += -g3 -pg
profile: $(EXE)
	gprof -b $(EXE) gmon.out

debug: CFLAGS += -g3
debug: $(EXE)

valgrind: $(EXE)
	valgrind --leak-check=full --error-exitcode=1 -s $(EXE)

$(OBJ_DIR) $(BIN_DIR) $(SRC_DIR) $(TST_DIR):
	@mkdir -p $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CHECK): $(TST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(TST_LIBS)
	./$(CHECK)
