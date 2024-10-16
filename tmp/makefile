SRC = test_gimp_import.c
ifeq ($(OS),Windows_NT)
BIN = test_gimp_import.exe
RMCMD = wsl rm -f
else
BIN = xrc
RMCMD = rm -f
endif

test_gimp_import:
	gcc $(SRC) -lpng -o $(BIN)

all: test_gimp_import

clean:
	$(RMCMD) $(BIN)
	

