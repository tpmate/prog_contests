CC=g++
CFLAGS=-Wall -Wpedantic -ansi -O0 -g3

DEPS = matrix.h solver.h generator.h multiPassSolver.h

OBJ = lamps.o matrix.o solver.o generator.o multiPassSolver.o

%.o: %.cpp $(DEPS)
	@echo "(CC) $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

lamps: $(OBJ)
	@echo "Linking..."
	@g++ -o $@ $^ $(CFLAGS)
	@echo "Done"

.PHONY: clean

clean:
	rm -f *.o lamps