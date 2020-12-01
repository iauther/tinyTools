#ifndef __BUFFER_Hx__
#define __BUFFER_Hx__


enum BType
{
    RING,       //circus continus: such as audio data
    NODE,       //block node: pointer and data length
    PPONG,      //pingpong:  read/write  producer/consumer
};



typedef struct {
	int     pr;
	int     pw;
	int     dlen;
	int     size;
    char    *buf;
}buffer_t;


class RBuffer
{
public:
	RBuffer();
	~RBuffer();
	int init(int size);
	int read(void *buf, int len, int update);
	int write(void *buf, int len, int update);
	int update(int rw, int len);                //0: read   1: write
	int get_size(void);
	int get_datalen(void);

private:
	buffer_t    rb;

	void        *mutex;
	void lock(void);
	void unlock(void);
	void reset(buffer_t *b);
};

#endif

