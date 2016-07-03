OBJ_LIB    = $(shell ls src/*.c    | sed "s/\.c/.o/")
OBJ_PARSER = $(shell ls parser/*.c | sed "s/\.c/\.o/")
OBJ_MPC    = $(shell ls mpc/*.c    | sed "s/\.c/\.o/")
CC=gcc
LIB    = libqscript.so
PARSER = qscript
CFLAGS_LIB    = -O3 -Iinclude -Impc -Wall -fPIC
CFLAGS_PARSER = -O3 -Wall
CFLAGS_MPC    = -O3 -Wall -Impc -Wall -fPIC
LFLAGS_LIB    = -lm -Wl,--version-script=exportmap
LFLAGS_PARSER = -L. -lqscript
LFLAGS_MPC    =
LIBMAJOR = 0
LIBFULL  = 0.0.1

CFLAGS = -O0 -g
LFLAGS =

default: all

all: $(LIB) $(PARSER) example

$(PARSER): $(OBJ_PARSER)
	$(CC) -Wl,-R,'$$ORIGIN' $(OBJ_PARSER) -o $(PARSER) $(LFLAGS_PARSER) $(LFLAGS)

$(LIB): $(OBJ_LIB) $(OBJ_MPC)
	$(CC) -fPIC -shared -Wl,-soname,$(LIB).$(LIBMAJOR) $(OBJ_LIB) $(OBJ_MPC) -o $(LIB).$(LIBFULL) $(LFLAGS_LIB) $(LFLAGS)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB).$(LIBMAJOR)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB)

src/%.o: src/%.c
	$(CC) $(CFLAGS_LIB) $(CFLAGS) -o $@ -c $<
parser/%.o: parser/%.c
	$(CC) $(CFLAGS_PARSER) $(CFLAGS) -o $@ -c $<
mpc/%.o: mpc/%.c
	$(CC) $(CFLAGS_MPC) $(CFLAGS) -o $@ -c $<

.FORCE:
example: .FORCE
	cd example && make

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f $(LIB) $(LIB).$(LIBMAJOR) $(LIB).$(LIBFULL) $(PARSER)
	cd example && make clean
