CC = gcc
CFLAGS = -g3 -Wall -Wextra
LDFLAGS = 

BDIR = bin
ODIR = build
IDIR = include
SDIR = source
PDIR = pipes
LDIR = log_files

EXECUTABLE = master

_DEPS = pipes.h diseaseAggregator.h workers.h list.h patient.h hashTable.h avlTree.h queriesAnswering.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o pipes.o workers.o diseaseAggregator.o list.o patient.o hashTable.o avlTree.o queriesAnswering.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean run valgrind

run:
	./$(EXECUTABLE) -w 4 -b 20 -i ./bashScript/input_dir/ -s 127.0.0.1 -p 3001

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE) -w 4 -b 20 -i ./bashScript/input_dir/ -s 127.0.0.1 -p 3001

clean:
	rm -f $(ODIR)/*.o
	rm -f $(LDIR)/*
	rm -f $(PDIR)/*
	rm master
	rm statistics.txt
	rm newStatistics.txt