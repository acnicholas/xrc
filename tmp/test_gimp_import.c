#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
        FILE* file = fopen("palette8.txt", "r"); /* should check the result */
        char line[256];
		while (fgets(line, sizeof(line), file))
        {
			long test = strtol(line+1, line+7 , 16);
			int red = ((int)test >> 16) & 0x000000ff;
			int green = ((int)test >> 8) & 0x000000ff;
			int blue = (int)test & 0x000000ff;
			printf(" %s, %d, %d, %d, %d\n", line,(int)test, red, green, blue);
        };

        fclose(file);
		return 0;
}
