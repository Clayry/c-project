# ─────────────────────────────────────────────────
#  Terminal Tetris — Makefile
# ─────────────────────────────────────────────────

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2 \
           -Iinclude
LDFLAGS := -lncurses

TARGET  := tetris
SRCDIR  := src
OBJDIR  := build

SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete → ./$(TARGET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all run clean
