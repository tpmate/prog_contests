CC=g++
CFLAGS=-Wall -Wpedantic -ansi -O0 -g3

DEPS = matrix.h

OBJ = lamps.o matrix.o

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