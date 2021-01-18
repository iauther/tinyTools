//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <stdafx.h>
#include <windows.h>
#include "Buffer.h"

void RBuffer::reset(buffer_t *b)
{
    b->pr   = 0;
	b->pw   = 0;
	b->dlen = 0;
	b->size = 0;
}


RBuffer::RBuffer()
{
    reset(&rb);
    rb.buf = NULL;
	mutex = CreateMutex(NULL,false,NULL);
}


RBuffer::~RBuffer()
{
    CloseHandle(mutex);
	delete rb.buf;
    reset(&rb);
    mutex = NULL;
}


int RBuffer::init(int size)
{
	lock();

	rb.size = size;
    delete rb.buf;
	rb.buf = (char*)new BYTE[size];;

	unlock();
	
	return 0;
}


int RBuffer::read(void *buf, int len, int update)
{
    int rlen;
    
	if(!buf || !len) {
        return -1;
    }
    
    //buffer is null
    if(rb.dlen==0) {
        return 0;
    }
    
    lock();
    
    if(len<=rb.dlen) {     //�������࣬��ȡ���ȵ���Ҫ��ĳ���
        
        rlen = len;
        if(rb.pr <= rb.pw) {
			memcpy(buf, rb.buf + rb.pr, rlen);
        }
        else {
            int l = rb.size-rb.pr;
            if(rlen<=l) {
				memcpy(buf, (char*)rb.buf + rb.pr, rlen);
            }
            else {
				memcpy(buf, rb.buf + rb.pr, l);
                memcpy((char*)buf+l, rb.buf, rlen-l);
            }
        } 
    }
    else {      //���ݲ���, ��ȫ������������������С��Ҫ�󳤶�
        rlen = rb.dlen;
        if(rb.pr < rb.pw) {
			memcpy(buf, rb.buf + rb.pr, rlen);
        }
        else {
            int l = rb.size-rb.pr;
            if(rlen<=l) {
				memcpy(buf, (char*)rb.buf + rb.pr, rlen);
            }
            else {
				memcpy(buf, rb.buf + rb.pr, l);
				memcpy((char*)buf + l, rb.buf, rlen - l);
            }
        }
    }
    
    if(update>0) {
        rb.pr = (rb.pr+rlen)%rb.size;        //���¶�ָ��
        rb.dlen -= rlen;
    }
    
    unlock();

	return rlen;
}



int RBuffer::write(void *buf, int len, int update)
{
    int wlen;
    
	if(!buf || !len) {
        return -1;
    }

    //buffer is full
    if(rb.dlen==rb.size) {
        return 0;
    }

    lock();

    if(len <= (rb.size-rb.dlen)) {    //��ȫ��д��
        wlen = len;
        if(rb.pw <= rb.pr) {      //дָ����ǰ��ֻ��д1��
			memcpy(rb.buf + rb.pw, buf, wlen);
        }
        else {
            int l = rb.size-rb.pw;
            if(wlen<=l) {   //дָ���ں󣬺����㹻��ֻ��д1��
                memcpy(rb.buf+rb.pw, buf, wlen);
            }
            else {  //дָ���ں󣬺���ʣ��ռ䲻�㣬��д2��
				memcpy(rb.buf + rb.pw, buf, l);
				memcpy(rb.buf, (char*)buf + l, wlen - l);
            }
        }
    }
    else {  //ֻ�ܲ���д�룬��д��buffer
        wlen = rb.size-rb.dlen;
        if(rb.pw <= rb.pr) {
            memcpy(rb.buf+rb.pw, buf, wlen);
        }
        else {
            int l = rb.size-rb.pw;
            memcpy(rb.buf+rb.pw, buf, l);
			memcpy(rb.buf, (char*)buf + l, wlen - l);
        } 
    }
    
    if(update>0) {
        rb.pw = (rb.pw+wlen)%rb.size;        //����дָ��
        rb.dlen += wlen;
    }
    
    unlock();

	return wlen;
}


char* RBuffer::find(find_callback callback, int update)
{
    int dl,flen;
    char* ptr=NULL;

    if (rb.dlen==0 || !callback) {
        return NULL;
    }

    lock();

    if (rb.pr < rb.pw) {
        ptr = callback(rb.buf + rb.pr, rb.dlen);
    }
    else {
        dl = rb.size - rb.pr;
        ptr = callback(rb.buf + rb.pr, dl);
        if (!ptr) {
            ptr = callback(rb.buf, rb.dlen - dl);
        }
        else {
            //flen = 
        }
    }

    if (ptr && update>0) {
        //
    }

    unlock();

    return ptr;
}


int RBuffer::clear(void)
{
    lock();

    reset(&rb);

    unlock();

    return 0;
}


int RBuffer::update(int rw, int len)
{
    lock();
    
    if(rw==0) {
        rb.pr = (rb.pr+len)%rb.size;        //���¶�ָ��
        rb.dlen -= len;
    }
    else {
        rb.pw = (rb.pw+len)%rb.size;        //����дָ��
        rb.dlen += len;
    }
    
    unlock();
    
    return 0;
}


int RBuffer::get_size(void)
{
	return rb.size;
}


int RBuffer::get_datalen(void)
{
	return rb.dlen;
}


void RBuffer::lock(void)
{
    WaitForSingleObject(mutex, INFINITE);
}

void RBuffer::unlock(void)
{
    ReleaseMutex(mutex);
}

