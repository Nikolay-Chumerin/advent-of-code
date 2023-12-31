# ifndef VERBOSE
# .SILENT:
# endif

# Compiler flags
CC     = gcc
CFLAGS = -Wall -Werror -Wextra
LDFLAGS = -lm

# Project files
SRCS = solution.c
EXE  = solution
TEST_FILES := test*.txt
INPUT_FILE := input.txt


# Debug build settings
DBGCFLAGS = -g -O0 -DDEBUG

# Release build settings
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release run remake all_tests test% input part%

# Default build
all: release

# Remake rule
remake: clean all

# Release rules
release: $(SRCS)
	gcc $(SRCS) $(RELCFLAGS) -o $(EXE) $(CFLAGS) $(LDFLAGS)

# Debug rules
debug: $(SRCS)
	gcc $(SRCS) $(DBGCFLAGS) -o $(EXE) $(CFLAGS) $(LDFLAGS)

# Test rules
all_tests: debug
	for i in $(TEST_FILES); do \
		echo "Running solution on the $$i file as input"; \
		./solution $$i; \
	done

test%: debug
	echo "Running solution on the $@.txt file as input"; \
	./solution $@.txt

part%: solution
	echo "Running solution on the $@.txt file as input"; \
	./solution $@.txt


# Run on input.txt rule
input: solution input.txt
	echo "Running soluiton on the input.txt file as input"; \
	./solution input.txt


clean:
	rm -rf *.o solution