.PHONY: all clean check debug profile break valgrind design writeup
.PHONY: testplan

# Define a list of original recipe names that you want to support for silent execution
ORIGINAL_RECIPES := clean output break
#-------- GCC Flags ---------#
CFLAGS := -std=c18
CFLAGS += -Wall -Werror -Wextra -Wpedantic -Winline
CFLAGS += -Wwrite-strings -Wvla -Wfloat-equal -Waggregate-return -Wunreachable-code
CFLAGS += -D_DEFAULT_SOURCE
# CFLAGS += -D_POSIX_C_SOURCE
CFLAGS += -g3
#-------- End GCC Flags ---------#


#-------- Targets ---------#

#---------- Directories ----------#
SRC_DIR := src
BUILTINS := /builtins
OBJ_DIR := obj
BIN_DIR := bin
TST_DIR := test
TST_OBJ_DIR := test/obj
DOC_DIR := doc
COV_DIR := coverage
#---------- End Directories ----------#

#----------- Sources and Objects -----------#
SRCS := $(wildcard $(addsuffix /*.c, $(SRC_DIR)))

OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

BIN := $(patsubst %.c,$(BIN_DIR)/lib%.so, $(notdir $(SRCS)))

CHECK := $(subst lib,,$(BIN)_check)

ifeq ($(shell find . -type d -name "test"), $(TST_DIR))
	TSTS := $(shell find $(TST_DIR) -type f -name "*.c")
	TSTS_SRCS := $(notdir $(TSTS))
	TST_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
	TST_OBJS := $(filter-out $(OBJ_DIR)/$(EXE_NAME).o $(OBJ_DIR)/main.o, $(TST_OBJS))
	TST_OBJS := $(patsubst $(TST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TSTS))
	TST_FLAGS := -lcheck -lm 
	TST_FLAGS += -pthread -lrt -lsubunit -DTESTING

endif
MAIN_FILES := $(shell grep -Pzl 'int[[:space:]]main.*' $(SRCS))
ifeq ($(shell grep -Pzl '[[:space:]]pthread.h' $(SRCS)), $(wildcard $MAIN_FILES))
	CFLAGS += -pthread -D__THREADS
	# SRCS := $(filter-out $(MAIN_FILES), $(SRCS))
endif


#----------- End Sources and Objects -----------#

# --------- Compiler ----- #
CC := gcc
BIN_ARGS :=
# ----- End Compiler ----- #

#-------- Targets ---------#

help: ## print this help menu
	@printf "\nusage: make <commands> \n\nthe following commands are available : \n\n"
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_-]+:.*?## / { sub("\\\\n",sprintf("\n%*s", 31, "")); printf "\033[36m%-30s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
	@printf "\n"

all: $(OBJS) $(BIN) ## Compile all library objects and link them to produce a shared library

## Build the executable and run tests
check: $(CHECK)

clean: ## Clean up the objects and binaries to start a fresh build
	rm -rf $(OBJ_DIR) $(TST_OBJ_DIR) $(BIN_DIR) error.log > /dev/null

debug: CFLAGS += -D__DEBUG__
debug: clean all ## Run gdb against the executable
	@gdb --args $(DEBUG_LIBS) $(EXE_ARGS)
#	@lldb $(DEBUG_LIBS) $(EXE_ARGS)

break: $(BIN) ## Pass /dev/urandom, /dev/null, and /dev/zero as input to the executable. Good for testing garbage input and fixing any edge cases.
	for element in {1..20} ; do \
	$(EXE) /dev/urandom; \
	$(EXE) /dev/null; \
	$(EXE) /dev/zero; \
	done


valgrind: VALGRIND_FLAGS := --tool=memcheck --error-exitcode=1
valgrind: VALGRIND_FLAGS += --leak-check=full --show-leak-kinds=all
valgrind: VALGRIND_FLAGS += --show-reachable=yes --track-origins=yes
valgrind: CFLAGS += -D__DEBUG__
valgrind: clean $(BIN) ## Run the library against valgrind
	@valgrind $(VALGRIND_FLAGS) -s ./$(BIN) $(BIN_ARGS)

helgrind: VALGRIND_FLAGS += --tool=helgrind
helgrind: CFLAGS += -D__DEBUG__
helgrind: clean $(BIN) ## Run the library against helgrind
	@valgrind $(VALGRIND_FLAGS) -s ./$(BIN) $(EXE_ARGS)

symbols: $(BIN) ## List the symbol table for this library
	nm -D $^

depend: $(BIN) ## List all dependencies for this library
	ldd $^

# Static pattern rule to run any original recipe silently
$(ORIGINAL_RECIPES:%=%.s): %.s:
	@$(MAKE) -s $*

# Static pattern rule to run any original recipe and ignore errors
$(ORIGINAL_RECIPES:%=%.i): %.i: ## Run the original recipe with --ignore-errors to ignore errors. If this option is used, any errors will be ignored and make will continue to process all instructions, despite the outcome. Any command listed in this help menu can be run with '.i' suffixed. Example: make all.i
	@$(MAKE) $* --ignore-errors

# Static pattern rule to run any original recipe silently, and ignore errors
IGNORE_S := is si
$(1).is: %.is: ## Run the original recipe with --ignore-errors and -s to silence output. Any command listed in this help menu can be run with '.is' suffixed. Example: make all.is
	@$(MAKE) $* --ignore-errors -s

# Directory creation rule
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Add dependencies for object files
$(OBJS): $(OBJ_DIR)
# General pattern rule for building object files
$(OBJS): obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -fPIC -o $@
	 

# Rule for building shared library
$(BIN): LIB_FLAGS += -D_MAIN_EXCLUDED
$(BIN): %.so: $(OBJS) $(BIN_DIR)
	echo "Building library: $$lib_makefile"
	$(CC) $(CFLAGS) -shared $< -o $@ $(LIB_FLAGS)

$(CHECK): $(TST_OBJS) $(LIB_OBJS)
	$(info making $(CHECK) with these flags: $(CFLAGS) $(LIB_FLAGS))
	@$(CC) $(CFLAGS) $(TST_FLAGS) $^ -o $@ $(TST_LIBS) $(TST_FLAGS) $(LIB_FLAGS) $(LIBS)
	@./$(CHECK)


#-------- End Targets ---------#
output: ## show all variable values
	$(info SRCS = $(SRCS))
	$(info OBJS = $(OBJS))
	$(info BIN = $(BIN))
	$(info CHECK = $(CHECK))

