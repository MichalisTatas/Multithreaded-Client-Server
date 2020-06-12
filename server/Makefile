CC = gcc
CFLAGS = -g3 -Wall -Wextra -pthread
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source

EXECUTABLE = server

_DEPS = server.h list.h message.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o server.o list.o message.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -q 3002 -s 3001 -w 5 -b 10

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE) -q 3002 -s 3001 -w 5 -b 20

clean:
	rm -f $(ODIR)/*.o
	rm server