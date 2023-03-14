.PHONY: all clean check debug profile break valgrind design writeup testplan
#-------- GCC Flags ---------#
CFLAGS := -std=c18
CFLAGS += -Wall -Werror -Wextra -Wpedantic -Winline
CFLAGS += -Wwrite-strings -Wvla -Wfloat-equal -Waggregate-return
CFLAGS += -D_DEFAULT_SOURCE
#-------- End GCC Flags ---------#



############ Directories ############
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .
TST_DIR = test
DOC_DIR:= doc
COV_DIR:= coverage
############ End Directories ############



#----------- Sources and Objects -----------#
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
#----------- End Sources and Objects -----------#



############ Executable ############
EXE_NAME := scheduler
EXE := $(BIN_DIR)/$(EXE_NAME)
EXE_ARGS :=
DEBUG_EXE := $(BIN_DIR)/$(EXE_NAME)_debug
############ End Executable ############

CHECK := $(EXE_NAME)_check
DRIVER := $(EXE_NAME)_driver

TSTS := $(wildcard $(TST_DIR)/*.c)
TST_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TST_OBJS := $(filter-out $(OBJ_DIR)/$(EXE_NAME).o, $(TST_OBJS))
TST_OBJS := $(filter-out $(OBJ_DIR)/$(EXE_NAME)_driver.o, $(OBJS))
TST_OBJS += $(patsubst $(TST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TSTS))
TST_LIBS := -lcheck -lm -pthread -lrt -lsubunit -D TESTING

DESIGN_DOC := $(DOC_DIR)/design.tex
WRITEUP_DOC := $(DOC_DIR)/writeup.tex
TESTPLAN_DOC := $(DOC_DIR)/testplan.tex

LATEX_FLAGS := -halt-on-error -interaction=nonstopmode

EXE_STATS := $(EXE_NAME)_stats

CC := gcc

#-------- Targets ---------#
all: $(EXE)

check: $(CHECK)

driver: CFLAGS += -g3 -pg
driver: $(DRIVER)

clean:
	@rm -rf $(OBJ_DIR) $(COV_DIR) $(EXE) $(EXE)_debug $(CHECK) *.exe $(DRIVER) gmon.out error.log > /dev/null
	clear

clean_doc:
	@rm -f doc/*.aux doc/*.log doc/*.out doc/*.toc > /dev/null
	clear

reset_doc:
	@rm -f doc/*.pdf > /dev/null
	clear

profile: CFLAGS += -g3 -pg
profile: $(DEBUG_EXE)
	@./$(DEBUG_EXE) $(EXE_ARGS)
	@gprof -b $(DEBUG_EXE) gmon.out

debug: CFLAGS += -g3 -D__DEBUG__
debug: $(DEBUG_EXE)
	@gdb $(DEBUG_EXE)

valgrind: CFLAGS += -g3 -D__DEBUG__
valgrind: $(DEBUG_EXE)
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -s ./$< $(EXE_ARGS) > /dev/null

break: $(EXE)
	@for element in {1..20} ; do \
	$(EXE) /dev/urandom; \
	$(EXE) /dev/null; \
	$(EXE) /dev/zero; \
	done

gcov: CFLAGS += -g3 -fprofile-arcs -ftest-coverage --coverage
gcov: $(DEBUG_EXE) | $(COV_DIR)
	@./$(DEBUG_EXE) $(EXE_ARGS) > /dev/null
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

$(OBJ_DIR) $(BIN_DIR) $(SRC_DIR) $(TST_DIR) $(DOC_DIR) $(COV_DIR):
	@mkdir -p $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TST_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJS) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -lm -o $@

$(DEBUG_EXE): $(OBJS) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -lm -o $@

$(CHECK): $(TST_OBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(TST_LIBS)
	@./$(CHECK)

$(DRIVER): $(SRC_DIR)/$(DRIVER).c
	@$(CC) $(CFLAGS) $^ -o $@
	@./$(DRIVER)

#-------- End Targets ---------#
