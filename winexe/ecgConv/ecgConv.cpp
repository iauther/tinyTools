// ecgConv.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>

/*
#define INT8	char
#define UINT8   unsigned char
#define INT32	signed int
#define UINT32	unsigned int
#define UINT16	unsigned short
*/

#define  MAXLEAD   12
#define MAXCHANNEL  8

enum {
	LEAD_I = 0,
	LEAD_II,
	LEAD_III,
	LEAD_V1,
	LEAD_V2,
	LEAD_V3,
	LEAD_V4,
	LEAD_RLD,

	LEAD_MAX
};


typedef struct _LEADPARAM
{
	TCHAR cLeadName[4];
	INT32 iDataIndex;
	INT32 iGain;		//
	INT32 bFilter;		//
	INT32 bDisplay;		//
	INT32 iLeadColor;
}LEADPARAM;

typedef struct _PARAMSET
{
	UINT32 iHead;		//头
	INT32 iVer;			//版本
	INT32 iSpeed;		//走速	
	INT32 iGain;		//增益
	INT32 bFilter;		//是否滤波
	INT32 bAmp;			//测量幅度/时间
	INT32 bNet;			//网格
	INT32 bBlank;		//黑色/白色背景
	UINT8 iIP[4];		//IP
	INT32 iPort;		//端口
	LEADPARAM sLead[MAXLEAD];	//导联
	INT32 iOneDataLen;	//每次传输的长度
	INT32 iSampling;	//采样率
	INT32 iOneVData;	//1mv电压
	INT32 iOneDisplayTime;//每次显示波形的时间（ms）
	INT32 iWindowWidth;
	INT32 iWindowHi;
	INT32 iUseDevice;//0=网络，1= 文件，2= 串口
	INT32 iComIndex;
	INT32 iBaudRate;
	INT32 bSigned;
	INT32 bWaveTurn;

	_PARAMSET()
	{
		Reset();
	}
	void Reset()
	{
		iHead = 0x1bdf9bdf;
		iVer = 1;
		iSpeed = 1;
		iGain = 3;
		bFilter = 1;
		bAmp = 0;
		bNet = 0;
		bBlank = 0;
		ZeroMemory(iIP, sizeof(iIP));
		iPort = 0;
		iOneDataLen = 1024;
		iSampling = 256;
		iOneVData = 100;
		sLead[3].iDataIndex = -1;
		sLead[3].iDataIndex = -2;
		sLead[3].iDataIndex = -3;
		iOneDisplayTime = 40;
		iWindowWidth = 800;
		iWindowHi = 600;
		iUseDevice = 0;
		iComIndex = 8;
		iBaudRate = 115200;
		bSigned = 0;
		bWaveTurn = 0;
	}
}PARAMSET;

#pragma pack (1)
typedef struct _LEADOFF
{
	UINT8 iOff_P;
	UINT8 iOff_N;
	_LEADOFF()
	{
		iOff_P = 0;
		iOff_N = 0;
	}
}LEADOFF;
typedef struct _PACKETHEAD
{
	UINT16 nHead;
	UINT8 byVer;
	UINT16 nDataNo;
	UINT8 byChannels;
	UINT8 byChannelFormat; //数据排列方式 1= 112233,2= 123123
	UINT8 byBit;
	UINT8 byBigEnd;//0=高位在前，1=低位在前
	INT8 iCurrent;
	INT8 iACLead;
	LEADOFF sOff[8];
	UINT16 nECGDataLen;

	_PACKETHEAD()
	{
		Reset();
	}
	void Reset()
	{
		nHead = 0x1BDF;
		byVer = 1;
		nDataNo = 0;
		byChannels = 8;
		byChannelFormat = 2;
		byBit = 16;
		byBigEnd = 1;
		iCurrent = 0;
		iACLead = 0;
		ZeroMemory(sOff, sizeof(sOff));
		nECGDataLen = 0;
	}
}PACKETHEAD;

typedef struct {
	PARAMSET	para;
	PACKETHEAD	head;
}file_hdr_t;

typedef struct {
	UINT16       adc[LEAD_MAX];
}ecg_8233_t;

typedef struct {
	INT32        adc[LEAD_MAX];
}ecg_1298_t;
#pragma pack ()


static long get_flen(FILE* fp)
{
	long len;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return len;
}

static void ad8233_print(char* data, FILE* fp)
{
	ecg_8233_t* ecg = (ecg_8233_t*)data;

	for (int i = 0; i < LEAD_MAX; i++) {
		fprintf(fp, "%d,", ecg->adc[i]);
	}
	fprintf(fp, "\r");
}

static void ads1298_print(char* data, FILE* fp)
{
	ecg_1298_t* ecg = (ecg_1298_t*)data;

	for (int i = 0; i < LEAD_MAX; i++) {
		fprintf(fp, "%d,", ecg->adc[i]);
	}
	fprintf(fp, "\r");
}

typedef struct {
	char path[1000];
}fpath_t;

static int getFiles(char* path, fpath_t* plist, int max)
{
	int nfile = 0;
	long   hFile = 0;
	char p[700] = { 0 };
	struct _finddata_t fileinfo;

	strcpy(p, path);
	strcat(p, "\\*");
	if ((hFile = _findfirst(p, &fileinfo)) != -1) {
		do {

			if (nfile>=max) {
				printf("_____ files are too much!!!\n");
				break;
			}

			if ((fileinfo.attrib & _A_SUBDIR)) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					continue;
			}
			else {
				strcpy(plist[nfile].path, path);
				strcat(plist[nfile].path, "\\");
				strcat(plist[nfile++].path, fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return nfile;
}
static int is_dir(char *path)
{
	int r;
	struct _stat buf;
	int rt = _stat(path, &buf);
	if (rt) {
		return 0;
	}

	if (_S_IFDIR & buf.st_mode) {
		r = 1;
	}
	else if (_S_IFREG & buf.st_mode) {
		r = 0;
	}

	return r;
}

static int file_conv(char *path)
{
	FILE* fp;
	long  flen,rlen,hlen= sizeof(file_hdr_t);
	file_hdr_t* hdr;
	char newPath[1000];
	char* fhead,*pdata,*ftail;

	fp = fopen(path, "rb");
	if (!fp) {
		printf("%s open failed\r\n", path);
		return -1;
	}
	flen = get_flen(fp);

	fhead = (char*)malloc(flen);
	if (!fhead) {
		fclose(fp);
		return -1;
	}
	ftail = fhead + flen;

	rlen = fread(fhead, 1, flen, fp);
	fclose(fp);

	strcpy(newPath, path);
	char *dot = strrchr(newPath, '.');
	if (dot) *(dot+1) = 0;
	strcat(newPath, "csv");

	fp = fopen(newPath, "wt");
	if (!fp) {
		printf("%s create failed\r\n", newPath);
		return -1;
	}

	hdr = (file_hdr_t*)fhead;
	pdata = fhead + hlen;
	if (hdr->head.byBit == 16) {//8233
		while (pdata + sizeof(ecg_8233_t) <= ftail) {
			ad8233_print(pdata, fp);
			pdata += sizeof(ecg_8233_t);
		}
	}
	else {		//1298
		while (pdata + sizeof(ecg_1298_t) <= ftail) {
			ads1298_print(pdata, fp);
			pdata += sizeof(ecg_1298_t);
		}
	}
	free(fhead);
	fclose(fp);

	return 0;
}

#define FILEMAX	 1000
int main(int argc, char* argv[])
{
	int i,n;
	fpath_t* pList;
	
	char* path;

	if (argc==1) {
		path = (char*)".";
	}
	else {
		path = argv[1];
	}
	
	if (is_dir(path)) {
		pList = (fpath_t*)malloc(sizeof(fpath_t)* FILEMAX);
		if (!pList) {
			printf("_____ pList malloc failed\r\n");
			return -1;
		}
		n = getFiles(path, pList, FILEMAX);
		for (i = 0; i < n; i++) {
			file_conv(pList[i].path);
		}
		free(pList);
	}
	else {
		file_conv(path);
	}

	return 0;
}


