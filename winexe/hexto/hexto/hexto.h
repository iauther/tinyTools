#ifndef __HEXTO_H__
#define __HEXTO_H__

#include "list.h"


typedef enum{
    REC_DATA = 0,
    REC_END,
    REC_SADDR,
    REC_LADDR,
    REC_LADDR_START,
}REC_TYPE;


#pragma pack (1)

typedef struct {
	U8      chs;	//channel number
	char    *ext;
}init_t;


typedef struct {
    U8      mark;
    U8      len;
    U16     addr;
    U8      type;
    U8      data[0xff];
}line_hdr_t;
#pragma pack ()


int hexto_init(init_t *init);

int hexto_read(char *path);

int hexto_parse(void);

int hexto_write(char *path);

#endif
