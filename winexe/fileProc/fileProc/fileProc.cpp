// fileProc.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

typedef struct {
	WORD nHead;
	BYTE byVer;
	WORD nDataNo;
	BYTE byChannels;
	BYTE byChannelFormat; //数据排列方式 1= 112233,2= 123123
	BYTE byBit;
	BYTE byBigEnd;//0=高位在前，1=低位在前
	WORD nState;
	WORD nECGDataLen;
	//BYTE nECGData[0];

}packet_hdr_t;

#define ECG_MAX   64
typedef struct {
	WORD adc[8];
}ecg_frame_t;
typedef struct {
	ecg_frame_t   frame[ECG_MAX];
}ecg_data_t;

#pragma pack ()		//取消1字节对齐



static char* find_header(char *data, int len, char *tail)
{
	BYTE* pdata = (BYTE*)data;
	int i, ecgLen;
	for (i = 0; i < len-sizeof(packet_hdr_t); i++) {
		if (pdata[i]==0xdf && pdata[i+1]==0x1b) {
			if(pdata[i + 2] == 1 &&		//version
			   pdata[i + 5] == 8 &&		//channels
			   pdata[i + 6] == 2 &&	    //format
			   pdata[i + 7] == 16 &&	//bits
			   pdata[i + 8] == 1) {		//lsb
				ecgLen = pdata[i + 12]<<8 | pdata[i + 11];
				if (data+i+ecgLen>tail) {
					return NULL;
				}

				return data + i;
			}
		}
	}

	return NULL;
}

static long get_flen(FILE* fp)
{
	long len;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return len;
}


static inline WORD my_swap(WORD v)
{
	BYTE* p = (BYTE*)&v;

	return (p[1] >> 8) | (p[0] << 8);
}

static void fileParse(const char *path)
{
	int i,j;
    FILE *fp,*fp1;
	char *fhead,*ftail,*fcur;
	packet_hdr_t* hdr;
	ecg_data_t* ecgDat;
	BYTE *pECG8;
	WORD* pECG16;
	long flen;

	fp = fopen(path, "rb");
	if (!fp) {
		return;
	}

	flen = get_flen(fp);
	fhead = (char*)malloc(flen);
	if (fhead) {
		ftail = fhead + flen;
		fread(fhead, 1, flen, fp);

		fp1 = fopen("../my_test.txt", "wt");

		fcur = fhead;
		while (fcur<ftail) {

			fcur = find_header(fcur, (ftail-fcur), ftail);
			if (!fcur) {
				break;
			}

			hdr = (packet_hdr_t*)fcur;
			pECG8 = (BYTE*)(fcur + sizeof(packet_hdr_t));
			pECG16 = (WORD*)(fcur + sizeof(packet_hdr_t));

			for (i = 0; i < ECG_MAX; i++) {
				for (j = 0; j < 8; j++) {
					fprintf(fp1, "CH%d: %05d    ", j+1, my_swap(pECG16[i*8+j]));
					//fprintf(fp1, "CH%d: 0x%04x    ", j + 1, my_swap(pECG16[i * 8 + j]));
				}
				fprintf(fp1, "\n");
			}

			fcur += sizeof(packet_hdr_t) + sizeof(ecg_data_t);
		}

		fclose(fp1);


		free(fhead);
	}
	fclose(fp);
	

}

int main()
{
    //std::cout << "Hello World!\n";

	fileParse("../my_test.bin");
















}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
