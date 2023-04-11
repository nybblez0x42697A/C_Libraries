.PHONY: all clean check debug profile break valgrind design writeup testplan
#-------- GCC Flags ---------#
CFLAGS := -std=c18
CFLAGS += -Wall -Werror -Wextra -Wpedantic -Winline
CFLAGS += -Wwrite-strings -Wvla -Wfloat-equal -Waggregate-return
CFLAGS += -D_DEFAULT_SOURCE
LIB_FLAGS := -D_MAIN_EXCLUDED
LIB_FLAGS := -lcheck -lm 
LIB_FLAGS += -pthread -lrt -lsubunit
#-------- End GCC Flags ---------#

############ Directories ############
DIR := */src
LIB_SRC_DIRS := $(dir $(wildcard $(DIR)/**/*.c $(DIR)/*.c))
$(info LIB_SRC_DIRS = $(LIB_SRC_DIRS))

LIB_OBJ_DIR := $(word 1, $(subst /, ,$(dir $(LIB_SRC_DIRS))))/obj
$(info LIB_OBJ_DIR = $(LIB_OBJ_DIR))
############ End Directories ############

#----------- Sources and Objects -----------#
LIB_SRCS := $(wildcard $(DIR)/**/*.c $(DIR)/*.c)
$(info LIB_SRCS = $(LIB_SRCS))
LIB_OBJS := $(patsubst $(LIB_SRC_DIRS)%.c,$(LIB_OBJ_DIR)/%.o,$(LIB_SRCS))
$(info LIB_OBJS = $(LIB_OBJS))
LIBS := $(patsubst %.c, %.so, $(dir $(LIB_SRC_DIRS))lib_$(notdir $(LIB_SRCS)))
$(info LIBS = $(LIBS))
DEBUG_LIBS := $(patsubst %.c, %.so, $(dir $(LIB_SRCS_DIRS))lib_$(notdir $(LIB_SRCS)))
$(info DEBUG_LIBS = $(DEBUG_LIBS))
#----------- End Sources and Objects -----------#

# --------- Compiler ----- #
CC := gcc
# ----- End Compiler ----- #

#-------- Targets ---------#
# imported:
# 	echo "$(LIB_SRCS) was imported."
all: $(LIB_OBJS) $(LIBS)

check: $(CHECK)

clean_objs:
	@rm -rf $(LIB_OBJ_DIR) $(CHECK) error.log > /dev/null
	clear

debug_lib: CFLAGS += -g3 -D__DEBUG__
debug_lib: $(DEBUG_LIBS)

debug_lib: $(DEBUG_LIBS)
	@gdb --args $(DEBUG_LIBS) $(EXE_ARGS)
#	@lldb $(DEBUG_LIBS) $(EXE_ARGS)

break: $(EXE)
	@for element in {1..20} ; do \
	$(EXE) /dev/urandom; \
	$(EXE) /dev/null; \
	$(EXE) /dev/zero; \
	done

$(LIB_OBJ_DIR) $(LIB_SRC_DIRS):
	@mkdir -p $@

$(LIB_OBJS) $(CHECK): | $(LIB_OBJ_DIR)

$(LIB_OBJS): $(LIB_SRCS)
	$(info making $(LIB_OBJS))
	@$(CC) $(CFLAGS) -D_MAIN_EXCLUDED -c -fPIC $<  -o $@

$(LIBS): $(LIB_OBJS)
	# $(info making $(LIBS))
	@$(CC) $(CFLAGS) -D_MAIN_EXCLUDED -shared $< -o $@ $(LIB_FLAGS)

$(CHECK): $(TST_OBJS) $(LIB_OBJS)
	$(info making $(CHECK) with these flags: $(CFLAGS) $(LIB_FLAGS))
	@$(CC) $(CFLAGS) $(TST_FLAGS) $^ -o $@ $(TST_LIBS) $(TST_FLAGS) $(LIB_FLAGS) $(LIBS)
	@./$(CHECK)


#-------- End Targets ---------#
