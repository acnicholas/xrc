xrc:
	gcc xrc.c -lpng -o xrc

all: xrc

test_run:
	./xrc -4 -b -i bg.png -o xlint_bg_x16.bin

test_map:
	./xrc -d -t -i tilemap.tmx -o map.bin

install:
	cp ./xrc /usr/local/bin/

clean:
	rm -rf xrc
	rm -rf *bin

