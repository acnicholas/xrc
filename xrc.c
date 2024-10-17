// xrc ([commander] x16 raster convetor)
// this reads an indexed png file and
// converts it to commander x16 format (VERA)
// usage:
//    xrc <options> inputfile
//    options:
//    -a create an acme source file
//    -b create an binary file
//    -c create a ca65 source file
//    -d add heaDer bytes to start of export file
//    -h show help
//    -i input file (png)
//    -o output file name
//    -p create a palette 
//    -P create a palette from a GIMP text file.
//    -q quiet mode (no messages)
//    -s create a sprite (i.e. input image is tiled)
//    -t create a tilemap from tiled .tmx file
//    -1 1bpp
//    -2 2bpp
//    -4 4bpp
//    -8 8bpp
//    -sw image width (only required for sprite sheets)
//    -sh image height (only required for sprite sheets)
//    -v print version

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

png_infop info_ptr;
png_bytepp row_pointers; 
png_structp png_ptr;
char *input_file;
char *output_file;
int add_header_bytes;
int bpp;
int create_acme;
int create_bin;
int create_ca65;
int create_palette;
int create_gimp_palette;
int create_sprite;
int create_tilemap;
int quiet_mode;
png_uint_32 input_image_width;
png_uint_32 input_image_height;
png_uint_32 sprite_height;
png_uint_32 sprite_width;

void write_bin(FILE *fp);
void write_palette(FILE *fp);
void write_gimp_palette(FILE *fp);
void write_tilemap_bin(FILE *fp);
void write_1bpp_bin(FILE *fp);
void write_2bpp_bin(FILE *fp);
void write_4bpp_bin(FILE *fp);
void write_8bpp_bin(FILE *fp);
void write_1bpp_src();
void write_2bpp_src();
void write_4bpp_src(FILE *fp);
void write_8bpp_src();
void write_src(FILE *fp);
void read_png();
void parse_args(int argc, char *argv[]);
void set_defaults();
void show_output_banner();
void show_version_info();
void show_usage();

//FIXME this only write 4bpp images at the moment
//TODO add all other bpp's
int main(int argc, char *argv[])
{
        set_defaults();
        parse_args(argc, argv);
        if (create_tilemap == 0 && create_gimp_palette == 0) {
                read_png();
                input_image_width = png_get_image_width(png_ptr, info_ptr);
                input_image_height = png_get_image_height(png_ptr, info_ptr);
        }
        show_output_banner();

        //TODO add error checking here!

        if (create_bin == 1 || create_sprite == 1 || create_tilemap == 1) {
                //printf("writing x16 binary data to file %s\n", output_file);
                FILE *fp = fopen(output_file, "wb");
                //write header bytes
                if (add_header_bytes == 1) {
                        uint16_t  z = 0;
                        fwrite(&z, 1, sizeof(uint16_t), fp);
                }
                write_bin(fp);
                fclose(fp);
        }

        if (create_acme == 1 || create_ca65 == 1) {
                //printf("writing x16 data to file %s\n", output_file);
                FILE *fp = fopen(output_file, "w");
                //fprintf(fp, ".byte $00,$00");
                write_src(fp);
                fclose(fp);
        }

        if (create_palette == 1) {
                char colour_type = png_get_color_type(png_ptr, info_ptr);
                FILE *fp = fopen(output_file, "w");
                if (add_header_bytes == 1) {
                        uint16_t  z = 0;
                        fwrite(&z, 1, sizeof(uint16_t), fp);
                }
                write_palette(fp);
                fclose(fp);
        }
		
		if (create_gimp_palette == 1) {
                FILE *fp = fopen(output_file, "w");
                if (add_header_bytes == 1) {
                        uint16_t  z = 0;
                        fwrite(&z, 1, sizeof(uint16_t), fp);
                }
                write_gimp_palette(fp);
                fclose(fp);
        }

        //FIXME check if the below is needed
        //png_destroy_info_struct(&info_ptr, NULL, NULL);
        png_destroy_read_struct(&png_ptr, NULL, NULL); 
}

void write_src(FILE *fp)
{
        switch(bpp) {
                case 1:
                        write_1bpp_src();
                        break;
                case 2:
                        write_2bpp_src();
                        break;
                case 4:
                        write_4bpp_src(fp);
                        break;
                case 8:
                        write_8bpp_src();
                        break;
        }
}


void write_bin(FILE *fp)
{
        if (create_tilemap == 1) write_tilemap_bin(fp);
        switch(bpp) {
                case 1:
                        write_1bpp_bin(fp);
                        break;
                case 2:
                        write_2bpp_bin(fp);
                        break;
                case 4:
                        write_4bpp_bin(fp);
                        break;
                case 8:
                        write_8bpp_bin(fp);
                        break;
        }
}

void write_palette(FILE *fp)
{
        png_byte *p = *row_pointers;
        for (int i = 0; i < input_image_width*input_image_height; i++)
        {
                        uint8_t r = *p >> 4;
                        p++;
                        uint8_t g = *p & 0xf0 ;
                        p++;
                        uint8_t b = *p >> 4;
                        uint8_t bg = g | b;
                        p++;
                        fwrite(&bg, 1, sizeof(uint8_t), fp);
                        fwrite(&r, 1, sizeof(uint8_t), fp);
        }
}

void write_gimp_palette(FILE *fp)
{
        FILE* file = fopen(input_file, "r"); /* should check the result */
        char line[256];
		while (fgets(line, sizeof(line), file))
        {
			long test = strtol(&line[1], &line[7] , 16);
			uint32_t red = ((uint32_t)test >> 16) & 0x000000ff;
			uint32_t green = ((uint32_t)test >> 8) & 0x000000ff;
			uint32_t blue = (uint32_t)test & 0x000000ff;
			//printf(" %s, %d, %d, %d, %d\n", line,(int)test, red, green, blue);
			uint8_t r = (uint8_t)(red/16);
			uint8_t g = ((uint8_t)(green/16)) << 4;
			uint8_t b = (uint8_t)(blue/16);
			uint8_t bg = g | b;
			fwrite(&bg, 1, sizeof(uint8_t), fp);
            fwrite(&r, 1,  sizeof(uint8_t), fp);
        };

        fclose(file);
}

void write_tilemap_bin(FILE *fp)
{
        FILE* file = fopen(input_file, "r"); /* should check the result */
        char line[256];

        while (fgets(line, sizeof(line), file))
        {
                if (strstr(line,"<") != NULL) continue;
                for(int i = 0; i < strlen(line); i++)
                {
                        uint16_t a = line[i] - '0';
                        uint16_t b = line[i+1] - '0';
                        if (a >= 0 && a < 10)
                        {
                                if (b >= 0 && b < 10)
                                {
                                        uint16_t c = (a * 10 + b) -1;
                                        fwrite(&c, 1, sizeof(uint16_t), fp);
                                        i++;
                                        continue;
                                } else {
                                        uint16_t c = a > 0 ? a - 1: 0;
                                        fwrite(&c, 1, sizeof(uint16_t), fp);
                                }
                        }
                }
        }

        fclose(file);
}

void write_1bpp_bin(FILE *fp)
{
        for (int y = 0; y < input_image_height; y++)
        {
                for (int x = 0; x < input_image_width; x++)
                {
                        uint8_t b;
                        b = row_pointers[y][x] << 7;
                        b = b | (row_pointers[y][x] << 6);
                        b = b | (row_pointers[y][x] << 5);
                        b = b | (row_pointers[y][x] << 4);
                        b = b | (row_pointers[y][x] << 3);
                        b = b | (row_pointers[y][x] << 2);
                        b = b | (row_pointers[y][x] << 1);
                        b = b | row_pointers[y][x];
                        fwrite(&b, 1, sizeof(uint8_t), fp);
                }
        }
}

void write_2bpp_bin(FILE *fp)
{
        for (int y = 0; y < input_image_height; y++)
        {
                for (int x = 0; x < input_image_width; x++)
                {
                        uint8_t b;
                        b = row_pointers[y][x] << 6;
                        b = b | (row_pointers[y][x] << 4);
                        b = b | (row_pointers[y][x] << 2);
                        b = b | row_pointers[y][x];
                        fwrite(&b, 1, sizeof(uint8_t), fp);
                }
        }
}

void write_4bpp_bin(FILE *fp)
{
        png_uint_32 sw = 1;
        png_uint_32 sh = 1;
        png_uint_32 h = input_image_height;
        png_uint_32 w = input_image_width;
        if (create_sprite == 1) {
                sw = input_image_width / sprite_width;
                sh = input_image_height / sprite_height;
                h = sprite_height;
                w = sprite_width;
        }

        for (int yi = 0; yi < sh; yi++)
        {
                for (int xi = 0; xi < sw; xi++)
                {
                        for (int y = yi * h; y < yi * h + h; y++)
                        {
                                for (int x = xi * w; x < xi * w + w; x++)
                                {
                                        uint8_t b;
                                        b = row_pointers[y][x] << 4;
                                        x++;
                                        b = b | row_pointers[y][x];
                                        fwrite(&b, 1, sizeof(uint8_t), fp);
                                }
                        }
                }
        }
}

void write_8bpp_bin(FILE *fp)
{
        for (int y = 0; y < input_image_height; y++)
        {
                for (int x = 0; x < input_image_width; x++)
                {
                        uint8_t b;
                        b = row_pointers[y][x];
                        fwrite(&b, 1, sizeof(uint8_t), fp);
                }
        }
}

void write_1bpp_src()
{
        //TODO
}

void write_2bpp_src()
{
        //TODO
}

void write_4bpp_src(FILE *fp)
{
         for (int y = 0; y < input_image_height; y++)
        {
                if(create_ca65 == 1)fprintf(fp, ".byte ");
                if(create_acme == 1)fprintf(fp, "!byte ");
                for (int x = 0; x < input_image_width; x++)
                {
                        int a = row_pointers[y][x];
                        x++;
                        int b = row_pointers[y][x];
                        fprintf(fp, "$%01x%01x", a, b);
                        if (x < input_image_width-1) fprintf(fp, ",");
                }
                fprintf(fp, "\n");
        }
}

void write_8bpp_src()
{
        //TODO
}

void set_defaults()
{
        create_acme = 0;
        create_ca65 = 0;
        create_bin =  0;
        create_palette =  0;
		create_gimp_palette =  0;
        create_tilemap =  0;
        add_header_bytes = 0;
        bpp = 4;
}

void show_output_banner()
{
        if (quiet_mode == 1) return;
        puts("******************************************");
        if(create_acme == 1) printf("creating acme source file\n");
        if(create_ca65 == 1) printf("creating ca65 source file\n");
        if(create_bin  == 1) printf("creating binary file\n");
        if(create_palette  == 1) printf("creating palette\n");
        if(create_sprite  == 1)  printf("creating sprite\n");
        printf("with the following settings:\n");
        printf("\tinput file = %s\n", input_file);
        printf("\timage width - %d\n", input_image_width);
        printf("\timage height - %d\n", input_image_height);
        printf("\touput file = %s\n", output_file);
        printf("\tbits per pixel = %d\n", bpp);
        if(create_sprite  == 1)  printf("\tsprite width = %d\n", sprite_width);
        if(create_sprite  == 1)  printf("\tsprite height = %d\n", sprite_height);
        puts("******************************************");
}

void show_usage()
{
        puts("xrc ([commander] x16 raster convetor)");
        puts("");
        puts("this reads an indexed png file and");
        puts("converts it to commander x16 format (VERA)");
        puts("");
        puts("usage:");
        puts("xrc <options> inputfile");
        puts("    options:");
        puts("    -a create an acme source file");
        puts("    -b create a binary file");
        puts("    -c create a ca65 source file");
        puts("    -d add heaDer bytes to export file");
        puts("    -h show help");
        puts("    -i input file (png)");
        puts("    -o output file name");
        puts("    -p create a palette");
		puts("    -P create a palette using a GIMP palette(txt)");
        puts("    -q quiet mode");
        puts("    -t create a tilemap from tiled *.tmx file");
        puts("    -1 1bpp");
        puts("    -2 2bpp");
        puts("    -4 4bpp(default)");
        puts("    -8 8bpp");
        puts("    -sw image width (only required for sprite sheets)");
        puts("    -sh image height (only required for sprite sheets)");
        puts("    -v print version");
        exit(0);
}

void show_version_info()
{
        printf("xrc (Commander X16 Raster Convertor)\nVersion 0.0.0\nA. Nicholas 2024\n");
        exit(0);
}

void parse_args(int argc, char *argv[])
{
        if (argc < 2) show_usage();
        for (int i = 0; i < argc; i++)
        {
                if (strcmp(argv[i],"-a") == 0) create_acme = 1;
                if (strcmp(argv[i],"-b") == 0) {
                        create_bin = 1;
                }
                if (strcmp(argv[i],"-c") == 0) create_ca65 = 1;
                if (strcmp(argv[i],"-d") == 0) add_header_bytes = 1;
                if (strcmp(argv[i],"-h") == 0) show_usage();
                if (strcmp(argv[i],"-i") == 0) {
                        input_file = argv[i+1];
                        i++;
                }
                if (strcmp(argv[i],"-o") == 0) {
                        output_file = argv[i+1];
                        i++;
                }
                if (strcmp(argv[i],"-p") == 0) create_palette = 1;
				if (strcmp(argv[i],"-P") == 0) create_gimp_palette = 1;
                if (strcmp(argv[i],"-q") == 0) quiet_mode = 1;
                if (strcmp(argv[i],"-s") == 0) create_sprite = 1;
                if (strcmp(argv[i],"-t") == 0) create_tilemap = 1;
                if (strcmp(argv[i],"-sh") == 0) {
                        if (i == argc-1) show_usage();
                        sprite_height = atoi(argv[i+1]);
                }
                if (strcmp(argv[i],"-sw") == 0) {
                        if (i == argc-1) show_usage();
                        sprite_width = atoi(argv[i+1]);
                }
                if (strcmp(argv[i],"-1") == 0) bpp = 1;
                if (strcmp(argv[i],"-2") == 0) bpp = 2;
                if (strcmp(argv[i],"-4") == 0) {
                        output_file = argv[i+1];
                        bpp = 4;
                }
                if (strcmp(argv[i],"-8") == 0) bpp = 8;
                if (strcmp(argv[i],"-v") == 0) show_version_info(); continue;
        }
}

void read_png()
{
        FILE *fp = fopen(input_file, "rb");
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fp);
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
        row_pointers = png_get_rows(png_ptr, info_ptr);
        fclose(fp);
}

/* vim: set sw=8 ts=8: */

