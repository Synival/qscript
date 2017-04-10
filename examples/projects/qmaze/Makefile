OBJ=$(shell ls *.c | sed "s/\.c/.o/")
CC=gcc
EXE=qmaze
CFLAGS=-Wall
LFLAGS=-Wl,-R,'$$ORIGIN/../../../lib' -lqscript

default: all

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f $(EXE)
