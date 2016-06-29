OBJ_LIB  = $(shell ls src/*.c  | sed "s/\.c/.o/")
OBJ_TEST = $(shell ls test/*.c | sed "s/\.c/\.o/")
CC=gcc
TEST = qs_test
LIB  = libqscript.so
# CFLAGS_LIB  = -O3 -Iinclude -Wall -fPIC
CFLAGS_LIB  = -g -Iinclude -Wall -fPIC
CFLAGS_TEST = -g -Wall
LFLAGS_LIB  = -lm
LFLAGS_TEST = -L. -lqscript
LIBMAJOR = 0
LIBFULL  = 0.0.1

default: all

all: $(LIB) $(TEST)

$(TEST): $(OBJ_TEST)
	$(CC) -Wl,-R,'$$ORIGIN' $(OBJ_TEST) -o $(TEST) $(LFLAGS_TEST)

$(LIB): $(OBJ_LIB)
	$(CC) -fPIC -shared -Wl,-soname,$(LIB).$(LIBMAJOR) $(OBJ_LIB) -o $(LIB).$(LIBFULL) $(LFLAGS_LIB)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB).$(LIBMAJOR)
	ln -fs ./$(LIB).$(LIBFULL) ./$(LIB)

src/%.o: src/%.c
	$(CC) $(CFLAGS_LIB) -o $@ -c $<
test/%.o: test/%.c
	$(CC) $(CFLAGS_TEST) -o $@ -c $<

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f $(LIB) $(LIB).$(LIBMAJOR) $(LIB).$(LIBFULL) $(TEST)
