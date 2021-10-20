CC = gcc
EXE = smp
LIB = -lncurses -lvlc -lpthread

SRC = src/*.c
OBJ = *.o

all:
	$(MAKE) build
	$(MAKE) clean
	$(MAKE) run

build:
	$(CC) -c $(SRC)
	$(CC) -o $(EXE) $(OBJ) $(LIB)

run:
	./$(EXE)

clean:
	rm *.o

install:
	$(MAKE) build
	$(MAKE) clean
	sudo mv $(EXE) /usr/bin
	sudo cp smpd.py /usr/bin

uninstall:
	sudo rm /usr/bin/$(EXE)
	sudo rm /usr/bin/smpd.py
	
