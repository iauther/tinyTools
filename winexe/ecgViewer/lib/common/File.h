#ifndef __FILE_Hx__
#define __FILE_Hx__

typedef struct _fnode {
	char* path;
	struct _fnode* next;
}flist_t;


class XFILE
{
public:
	static int size(char* path);
	static int save(char* path, void* data, int len);
	static int read(char* path, void* data, int len);
	static int get_list(char* path, flist_t** flist);
	static int free_list(flist_t** flist);
	static int copy(char* dst, char* src);
	static int move(char* dst, char* src);
	static int remove(char* path);

	static int open(char* path, void** handle);
	static int read(void* handle, void* buffer, int len);
	static int write(void* handle, void* buffer, int len);
	static int close(void* handle);

private:
	int fool;
};


#endif



