#include "hexto.h"


static buffer_t fileBuf;
static buffer_t dataBuf;
static init_t initPara;
static int get_size(FILE *fp)
{
    int fsize;
    unsigned int pos=ftell(fp);

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    
    return fsize;
}

static U8 hex2int(U8 hex)
{
    if(hex>='0' && hex<='9') {
        return (hex-48);
    }
    else if(hex>='A'&& hex<='F') {
        return (hex-55);
    }
    
    return 0xff;
}


static void free_buf(buffer_t *b)
{
	if (b->buf) free(b->buf);
	b->buf = NULL;
	b->blen = 0;
	b->dlen = 0;
}


int hexto_init(init_t *init)
{
	if (!init) {
		return -1;
	}

	initPara = *init;
	free_buf(&fileBuf);
	free_buf(&fileBuf);

	return 0;
}




int hexto_read(char *path)
{
	int flen;
	FILE *fp = fopen(path, "rb");
	if (!fp) {
		printf("%s open failed, check the file path please\n", path);
		return -1;
	}
        
	flen = get_size(fp);
	fileBuf.buf = malloc(flen);
	if (fileBuf.buf) {
		fread(fileBuf.buf, 1, flen, fp);
		fileBuf.blen = fileBuf.dlen = flen;
    }

	dataBuf.buf = malloc(flen);
	if (dataBuf.buf) {
		dataBuf.blen = flen;
		dataBuf.dlen = 0;
	}
    fclose(fp);

	return 0;
}



/*
*   return -1:  reach the end of data
            0:  ok
*/
static int parse_line(U8 *line, line_hdr_t *hdr)
{
	int i;

	hdr->mark = line[0];
	hdr->len = hex2int(line[1]) << 4 | hex2int(line[2]);
	hdr->addr = hex2int(line[3]) << 12 | hex2int(line[4]) << 8 | hex2int(line[5]) << 4 | hex2int(line[6]);
	hdr->type = hex2int(line[7]) << 4 | hex2int(line[8]);

	for (i = 0; i<hdr->len; i++) {
		hdr->data[i] = hex2int(line[i * 2 + 9]) << 4 | hex2int(line[i * 2 + 9 + 1]);
	}

	return 0;
}

//解析每一行原始数据到l链表
line_hdr_t hdr;
int hexto_parse(void)
{
    int r;
    U8  *ptr, *end;
    
	if (!fileBuf.buf || !dataBuf.buf) {
		printf("fileBuf or dataBuf Null!\n");
        return -1;
    }
    
	ptr = fileBuf.buf;
	end = fileBuf.buf + fileBuf.dlen;
    
    while(1) {

		while (*ptr != ':') {
			if (ptr >= end) return -1;
			ptr++;
		}

        r = parse_line(ptr, &hdr);
        if(r<0) {
            break;
        }
        
        if(hdr.type==REC_DATA) {
			memcpy(dataBuf.buf+dataBuf.dlen, hdr.data, hdr.len);
			dataBuf.dlen += hdr.len;
        }
		else if (hdr.type == REC_END) {
			break;
		}

		ptr += hdr.len * 2 + 11;
    }
	free_buf(&fileBuf);
    
    return 0;
}


//读解析后的链表l中的数据存入name文件
int hexto_write(char *path)
{
    int i,j;
	int lc;
    F32 *f32;
	FILE *fp;
	
	if (!dataBuf.buf) {
		printf("dataBuf Null!\n");
		return -1;
	}

	fp = fopen(path, "wt");
	if (!fp) {
		printf("%s open failed, check the path please!", path);
		return -1;
	}
	   
    for(i=1; i<=initPara.chs; i++) {
        fprintf(fp, "CH%1d,", i);
    }
	fprintf(fp, "\n");
    
	lc = dataBuf.dlen / initPara.chs;
	for (i = 0; i<(lc / 4); i++) {
		
		for (j = 0; j < initPara.chs; j++) {
			f32 = (F32*)(dataBuf.buf + lc*j);
			fprintf(fp, "%.6e,    ", f32[i]);
		}
		fprintf(fp, "\n");
    }

	free_buf(&dataBuf);
    fclose(fp);

	return 0;
}
