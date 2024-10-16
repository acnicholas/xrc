SRC = xrc.c
ifeq ($(OS),Windows_NT)
BIN = xrc.exe
RMCMD = wsl rm -f
else
BIN = xrc
RMCMD = rm -f
endif

xrc:
	gcc $(SRC) -lpng -o $(BIN)

all: xrc

test:
	./$(BIN) -4 -b -i tests/bg.png -o tests/bg.bin
	./$(BIN) -d -t -i tests/tilemap.tmx -o tests/tilemap.bin
	./$(BIN) -p -i tests/palette.png -o tests/palette.bin
	./$(BIN) -s -i tests/sprite.png -sw 16 -sh 16 -o tests/sprite.bin
	./$(BIN) -s -i tests/tileset.png -sw 16 -sh 16 -o tests/tileset.bin

install:
	cp ./$(BIN) /usr/local/bin/

clean:
	$(RMCMD) $(BIN)
	$(RMCMD) tests/*bin

