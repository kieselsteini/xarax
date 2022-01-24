CC=cc -O2 -Wall -Wextra `sdl2-config --cflags`
LIB=`sdl2-config --libs`
OBJ=./src/xarax.o
BIN=xarax

default: $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LIB)

clean:
	rm -f $(BIN) $(OBJ)
