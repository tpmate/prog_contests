CC=gcc
CFLAGS=-Wall -Wpedantic -ansi -O0 -g3

DEPS = matrix.h

OBJ = lamps.o matrix.o

%.o: %.c $(DEPS)
	@echo "(CC) $<"
	@$(CC) -c -o $@ $< $(CFLAGS)

lamps: $(OBJ)
	@echo "Linking..."
	@gcc -o $@ $^ $(CFLAGS)
	@echo "Done"

.PHONY: clean

clean:
	rm -f *.o lamps