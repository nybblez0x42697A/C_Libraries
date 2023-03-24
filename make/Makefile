.PHONY: all clean check debug profile break valgrind design writeup testplan
# ----- Includes ----- #
includes := $(shell find ./lib -type f -name "Makefile")
$(info includes = $(includes))
include $(includes)
# ----- End Includes ----- #

#-------- GCC Flags ---------#
CFLAGS := -std=c18
CFLAGS += -Wall -Werror -Wextra -Wpedantic -Winline
CFLAGS += -Wwrite-strings -Wvla -Wfloat-equal -Waggregate-return
CFLAGS += -D_DEFAULT_SOURCE
# LIB_FLAGS := -D_MAIN_EXCLUDED
#-------- End GCC Flags ---------#

############ Directories ############
SRC_DIR := src
BUILTINS := /builtins
OBJ_DIR := obj
BIN_DIR := bin
TST_DIR := test
TST_OBJ_DIR := test/obj
DOC_DIR := doc
COV_DIR := coverage
LIB_DIR := lib
LIB_OBJ_DIR := lib/obj
############ End Directories ############

#----------- Sources and Objects -----------#
STATIC_LIBS := $(shell find $(LIB_DIR) -type f -name "*.a")
SHARED_LIBS += $(shell find $(LIB_DIR) -type f -name "*.so")

SRCS := $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
SRCS += $(wildcard $(SRC_DIR)/%.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TSTS := $(shell find $(TST_DIR) -type f -name "*.c")
TSTS_SRCS := $(notdir $(TSTS))
TST_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TST_OBJS := $(filter-out $(OBJ_DIR)/$(EXE_NAME).o $(OBJ_DIR)/main.o, $(TST_OBJS))
TST_OBJS := $(patsubst $(TST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TSTS))

$(info Primary: SHARED_LIBS is $(LIBS))
$(info $(basename $(notdir $(LIBS)).c))

$(info SRCS is $(SRCS))
$(info OBJS is $(OBJS))
$(info TSTS is $(TSTS))
$(info TSTS_SRCS is $(TSTS_SRCS))
$(info TST_OBJS is $(TST_OBJS))
#----------- End Sources and Objects -----------#


############ Executable ############
EXE_NAME := minora
EXE := $(BIN_DIR)/$(EXE_NAME)
EXE_ARGS :=
DEBUG_EXE := $(BIN_DIR)/$(EXE_NAME)_debug
############ End Executable ############

CHECK := $(EXE_NAME)_check
DRIVER := $(EXE_NAME)_driver

ifeq ($(strip $(TSTS)),)
# No Tests specified
    TST_FLAGS :=
else
    TST_FLAGS := -lcheck -lm 
	TST_FLAGS += -pthread -lrt -lsubunit -DTESTING
endif



DESIGN_DOC := $(DOC_DIR)/design.tex
WRITEUP_DOC := $(DOC_DIR)/writeup.tex
TESTPLAN_DOC := $(DOC_DIR)/testplan.tex

LATEX_FLAGS := -halt-on-error -interaction=nonstopmode

EXE_STATS := $(EXE_NAME)_stats

CC := gcc
#-------- Targets ---------#
all: export LD_LIBRARY_PATH=lib/Signals/src
all: libraries $(OBJS) $(EXE)
	echo $(LD_LIBRARY_PATH)

check: $(CHECK)

driver: CFLAGS += -g3 -pg
driver: $(DRIVER)

clean:
	@rm -rf $(COV_DIR) $(OBJ_DIR) $(BIN_DIR) $(CHECK) *.exe $(DRIVER) gmon.out error.log > /dev/null
	clear

clean_doc:
	@rm -f doc/*.aux doc/*.log doc/*.out doc/*.toc > /dev/null
	clear

reset_doc:
	@rm -f doc/*.pdf > /dev/null
	clear

profile: CFLAGS += -g3 -pg
profile: all $(EXE)
	@./$(EXE) $(EXE_ARGS)
	@gprof -b $(EXE) gmon.out

debug: CFLAGS += -g3 -D__DEBUG__

debug: all $(EXE)
	@gdb --args $(EXE) $(EXE_ARGS)
#	@lldb $(DEBUG_EXE) $(EXE_ARGS)

valgrind: CFLAGS += -g3 -D__DEBUG__
valgrind: all $(EXE)
	@valgrind --leak-check=full --tool=memcheck --show-leak-kinds=all --show-reachable=yes --trace-children=yes --error-exitcode=1 -s ./$(EXE) $(EXE_ARGS)

break_lib: $(EXE)
	@for element in {1..20} ; do \
	$(EXE) /dev/urandom; \
	$(EXE) /dev/null; \
	$(EXE) /dev/zero; \
	done

gcov: CFLAGS += -g3 -fprofile-arcs -ftest-coverage --coverage
gcov: all $(EXE) | $(COV_DIR)
	@./$(EXE) $(EXE_ARGS) > /dev/null
	@gcov $(SRCS)
	@lcov -d $(OBJ_DIR) -o $(COV_DIR)/main_coverage.info -i -c



############ Latex Target Configuration ############
design: LATEX_FLAGS += -output-directory=$(DOC_DIR)
design: $(DESIGN_DOC)

writeup: LATEX_FLAGS += -output-directory=$(DOC_DIR)
writeup: $(WRITEUP_DOC)

testplan: LATEX_FLAGS += -output-directory=$(DOC_DIR)
testplan: $(TESTPLAN_DOC)

docs: LATEX_FLAGS += -output-directory=$(DOC_DIR)
docs: $(DESIGN_DOC) $(WRITEUP_DOC) $(TESTPLAN_DOC)

$(DESIGN_DOC): $(DOC_DIR)
	@pdflatex $(LATEX_FLAGS) $(DESIGN_DOC) > /dev/null
	@make clean_doc > /dev/null

$(WRITEUP_DOC):	$(DOC_DIR)
	@pdflatex $(LATEX_FLAGS) $(WRITEUP_DOC) > /dev/null
	@make clean_doc > /dev/null

$(TESTPLAN_DOC): $(DOC_DIR)
	@pdflatex $(LATEX_FLAGS) $(TESTPLAN_DOC) > /dev/null
	@make clean_doc > /dev/null
############# End Latex Target Configuration ############

libraries:
	@for lib_makefile in $(includes); do \
		echo "Building library: $$lib_makefile"; \
		make -C $$(dirname $$lib_makefile); \
	done

$(OBJ_DIR) $(BIN_DIR) $(SRC_DIR) $(TST_DIR) $(DOC_DIR) $(COV_DIR):
	@mkdir -p $@

$(OBJS) $(CHECK): | $(OBJ_DIR) $(SRC_DIR)$(BUILTINS)


$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(TST_OBJS): $(OBJ_DIR)/%.o: $(TST_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(TST_FLAGS) -c $< -o $@



$(EXE): $(SHARED_LIBS) | $(BIN_DIR)
	$(info made these objects: $(OBJS))
	$(info making $(EXE) with these flags: $(CFLAGS))
	$(CC) $(CFLAGS) $(OBJS) $(SHARED_LIBS) -o $@

$(DEBUG_EXE): $(OBJS) $(LIB_OBJS) | $(BIN_DIR)
	$(info making $(DEBUG_EXE) with these flags: $(CFLAGS))
	@$(CC) $(CFLAGS) $(OBJS) $(SHARED_LIBS) $^ -lm -o $@

$(CHECK): $(TST_OBJS) $(SHARED_LIBS)
	$(info making $(CHECK) with these flags: $(CFLAGS))
	@$(CC) $(CFLAGS) $(TST_FLAGS) $^ -lm -o $@ $(TST_LIBS)
	@./$(CHECK)

$(DRIVER): $(SRC_DIR)/$(DRIVER).c
	@$(CC) $(CFLAGS) $^ -o $@
	@./$(DRIVER)


#-------- End Targets ---------#
