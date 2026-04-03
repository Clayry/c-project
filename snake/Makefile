# ─────────────────────────────────────────────────
#  Terminal Snake — Makefile
# ─────────────────────────────────────────────────

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2 \
           -Iinclude
LDFLAGS := -lncurses

TARGET  := snake
SRCDIR  := src
OBJDIR  := build

SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# ── Default target ────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete → ./$(TARGET)"

# ── Compile each .c → build/*.o ──────────────────
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ── Create build directory if needed ─────────────
$(OBJDIR):
	mkdir -p $(OBJDIR)

# ── Run the game ──────────────────────────────────
run: all
	./$(TARGET)

# ── Remove build artefacts ────────────────────────
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all run clean
