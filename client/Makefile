CC = gcc
CFLAGS = -g3 -Wall -Wextra -pthread
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source

EXECUTABLE = client

_DEPS = client.h queue.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o client.o queue.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -q queryFile -w 5 -sp 3002 -sip 127.0.0.1

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --fair-sched=yes ./$(EXECUTABLE) -q queryFile -w 5 -sp 3002 -sip 127.0.0.1

clean:
	rm -f $(ODIR)/*.o
	rm -f $(LDIR)/*
	rm diseaseAggregator
	rm statistics.txt
	rm newStatistics.txt