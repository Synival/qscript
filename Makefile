OBJ_LIB    = $(shell ls src/*.c    | sed "s/\.c/.o/")
OBJ_PARSER = $(shell ls parser/*.c | sed "s/\.c/\.o/")
CC=gcc
LIB    = libqscript.so
PARSER = qscript
CFLAGS_LIB    = -g -Iinclude -Wall -fPIC
CFLAGS_PARSER = -g -Wall
LFLAGS_LIB    = -lm
LFLAGS_PARSER = -L. -lqscript
LIBMAJOR = 0
LIBFULL  = 0.0.1

default: all

all: $(LIB) $(PARSER) example

$(PARSER): $(OBJ_PARSER)
	$(CC) -Wl,-R,'$$ORIGIN' $(OBJ_PARSER) -o $(PARSER) $(LFLAGS_PARSER)

$(LIB): $(OBJ_LIB)
	$(CC) -fPIC -shared -Wl,-soname,$(LIB).$(LIBMAJOR) $(OBJ_LIB) -o $(LIB).$(LIBFULL) $(LFLAGS_LIB)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB).$(LIBMAJOR)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB)

src/%.o: src/%.c
	$(CC) $(CFLAGS_LIB) -o $@ -c $<
parser/%.o: parser/%.c
	$(CC) $(CFLAGS_PARSER) -o $@ -c $<

.FORCE:
example: .FORCE
	cd example && make

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f $(LIB) $(LIB).$(LIBMAJOR) $(LIB).$(LIBFULL) $(PARSER)
	cd example && make clean
