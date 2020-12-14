
#include "hexto.h"


#define TEST_FILE	"g.hex"


static void get_new_path(char *path, char *ext, char *path2)
{
	char *p;

	strcpy(path2, path);
	p = strrchr(path2, '.');
	if (p) {
		strcpy(p + 1, ext);
	}
	else {
		strcat(path2, ".");
		strcat(path2, ext);
	}
}



int main (int argc, char *argv[])
{
	int r;
	int      chs;
	char     *path;
	char     *ext;
	char     path2[512];
	init_t   init;

	if (argc<4) {
		printf("wrong params\n, usage: hexto *(hex file) 8 txt[csv]\n");
		return -1;
	}

	path = argv[1];
	ext  = argv[3];
	get_new_path(path, ext, path2);
    
	chs = atoi(argv[2]);
	if (chs <= 0){
		printf("wrong channel number input\n");
		return -1;
	}

	init.chs = chs;
	init.ext = ext;
	hexto_init(&init);

	r = hexto_read(path);
	if (r < 0) return r;

	r = hexto_parse();
	if (r < 0) return r;

	hexto_write(path2);
	if (r < 0) return r;
    
    return 0;
}