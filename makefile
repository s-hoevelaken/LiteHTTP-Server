CC = gcc
CFLAGS = -Wall -Iinclude
OBJDIR = obj
BINDIR = bin
SRC = server.c lib/logger.c lib/router.c
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/server

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJ) -o $(TARGET)
	@echo "Server compiled successfully."

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)/lib
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Cleaned up generated files."

run: $(TARGET)
	$(TARGET)

.PHONY: all clean run
