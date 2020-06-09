CC = gcc
CFLAGS = -g3 -Wall -Wextra
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source

EXECUTABLE = server

_DEPS = server.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o server.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -q 3002 -s 3001 -w 5 -b 10

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE) -q 3002 -s 3001 -w 5 -b 10

clean:
	rm -f $(ODIR)/*.o
	rm -f $(LDIR)/*
	rm diseaseAggregator
	rm statistics.txt
	rm newStatistics.txt