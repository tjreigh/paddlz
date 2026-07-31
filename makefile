# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        = PONG
COMPRESSED  = NO
ICON        = icon.png
DESCRIPTION = "Pong game made by Trevor Reigh"

# ----------------------------

include $(shell cedev-config --makefile)

# The CEdev makefile reserves `test` for CEmu integration tests. These unit
# tests run on the host and do not require a calculator ROM.
HOST_CC ?= cc
UNIT_TEST_BIN = tests/build/test_game
UNIT_TEST_SOURCES = tests/test_game.c src/ball.c src/collision.c src/paddle.c
UNIT_TEST_HEADERS = $(wildcard src/*.h tests/include/*.h)

.PHONY: unit-test

unit-test: $(UNIT_TEST_BIN)
	@./$(UNIT_TEST_BIN)
	@echo "All unit tests passed."

$(UNIT_TEST_BIN): $(UNIT_TEST_SOURCES) $(UNIT_TEST_HEADERS)
	@mkdir -p $(@D)
	@$(HOST_CC) -std=c11 -Wall -Wextra -Werror \
		-Itests/include -Isrc $(UNIT_TEST_SOURCES) -o $(UNIT_TEST_BIN)
