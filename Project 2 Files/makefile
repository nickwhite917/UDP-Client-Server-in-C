CC = gcc
CFLAGS = -Wall -g -I.
DEPS = commands.h Fat12Data.h Fat12Files.h
OBJ = main.c commands.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

RunProgram: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ RunProgram
