# xrc ([commander] x16 raster convertor) #

Read an indexed png file and convert it to commander x16 format (VERA).
Can also create commander x16 tilemap from *basic* filed *.tmx files


## Usage: ##

xrc [options] -i inputfile -o outputfile

options:

- -a create an acme source file
- -b create an binary file
- -c create a ca65 source file
- -d add heaDer bytes to start of export file
- -h show help
- -i input file (png)
- -o output file name
- -p create a palette 
- -P create a palette using GIMP pallete export (txt format) 
- -q quiet mode (no messages)
- -s create a sprite (i.e. input image is tiled)
- -t create a tilemap from tiled .tmx file
- -1 1bpp
- -2 2bpp
- -4 4bpp (default)
- -8 8bpp
- -sw image width (only required for sprite sheets)
- -sh image height (only required for sprite sheets)
- -v print version


## Examples: ##

Create a map from a tiled *tmx file:

	xrc -q -d -t -i tilemap.tmx -o level01.bin

Create a color pallete:

	xrc -q -p -i x16-palette_00.png -o palette.bin

Create a tileset:

	xrc -q -s -i tileset.png -sw 16 -sh 16 -o tileset.bin

Create a sprite:

	xrc -q -s -i sprite.png -sw 16 -sh 16 -o sprite.bin

