#include <stdio.h>
#include "File.h"

static int get_fsize(FILE* fp)
{
	int fsize;
	unsigned int pos = ftell(fp);

	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return fsize;
}

int XFILE::size(char* path)
{
	FILE* fp;
	
	fp = fopen(const path, "rwb");
	if () {

	}

	return 0;
}


int XFILE::save(char* path, void* data, int len)
{
	FILE* fp;

	fp = fopen(const path, "wb");
	if (!fp) {
		return -1;
	}
	fwrite(data, 1, len, fp);
	fclose(fp);

	return 0;
}


int XFILE::read(char* path, void* data, int len)
{
	return 0;
}


int XFILE::get_list(char* path, flist_t** flist)
{
	return 0;
}


int XFILE::free_list(flist_t** flist)
{
	return 0;
}


int XFILE::copy(char* dst, char* src)
{
	return 0;
}


int XFILE::move(char* dst, char* src)
{
	return 0;
}


int XFILE::remove(char* path)
{
	return 0;
}


int XFILE::open(char* path, void** handle)
{
	FILE* fp;

	fp = fopen(const path, "rwb");
	if (!fp) {
		return -1;
	}

	if(handle) *handle = fp;

	return 0;
}


int XFILE::read(void* handle, void* buffer, int len)
{
	FILE* fp=handle;

	if (!fp) {
		return -1;
	}
	return fread(buffer, 1, len, fp);
}


int XFILE::write(void* handle, void* buffer, int len)
{
	FILE* fp = handle;

	if (!fp) {
		return -1;
	}
	return fwrite(buffer, 1, len, fp);
}


int XFILE::close(void* handle)
{
	FILE* fp = handle;

	if (!fp) {
		return -1;
	}
	return fclose(fp);
}


