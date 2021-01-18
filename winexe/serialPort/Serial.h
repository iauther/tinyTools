#ifndef __SERIAL_Hx__
#define __SERIAL_Hx__

//#pragma once

#include <windows.h>
#include <iostream>
using namespace std;

typedef void(*rxCallback)(int len);


enum BaudRate
{
	BaudRate110 = 110,       ///< 110
	BaudRate300 = 300,       ///< 300
	BaudRate600 = 600,       ///< 600
	BaudRate1200 = 1200,     ///< 1200
	BaudRate2400 = 2400,     ///< 2400
	BaudRate4800 = 4800,     ///< 4800
	BaudRate9600 = 9600,     ///< 9600 recommend 推荐
	BaudRate14400 = 14400,   ///< 14400
	BaudRate19200 = 19200,   ///< 19200
	BaudRate38400 = 38400,   ///< 38400
	BaudRate56000 = 56000,   ///< 56000
	BaudRate57600 = 57600,   ///< 57600
	BaudRate115200 = 115200, ///< 115200
	BaudRate921600 = 921600  ///< 921600
};


enum DataBits
{
	DataBits5 = 5, ///< 5 data bits 5位数据位
	DataBits6 = 6, ///< 6 data bits 6位数据位
	DataBits7 = 7, ///< 7 data bits 7位数据位
	DataBits8 = 8  ///< 8 data bits 8位数据位
};

enum Parity
{
	ParityNone = 0,  ///< No Parity 无校验
	ParityOdd = 1,   ///< Odd Parity 奇校验
	ParityEven = 2,  ///< Even Parity 偶校验
	ParityMark = 3,  ///< Mark Parity 1校验
	ParitySpace = 4, ///< Space Parity 0校验
};

enum StopBits
{
	StopOne = 0,        ///< 1 stop bit 1位停止位
	StopOneAndHalf = 1, ///< 1.5 stop bit 1.5位停止位 - This is only for the Windows platform
	StopTwo = 2         ///< 2 stop bit 2位停止位
};




class CSerial
{
public:
    // 用于窗体模式的数据到达消息
    enum {WM_ONCOMM = WM_USER + 2005};

	CSerial();
    ~CSerial(void);

	bool Open(string port);
	bool Close();
	bool Clear();

	bool Setup(DWORD dwBaudRate = BaudRate115200, DWORD dwByteSize = DataBits8, DWORD dwParity = ParityNone, DWORD dwStopBits = StopOne);
	bool SetBuffer(DWORD dwInQueue = 4096, DWORD dwOutQueue = 4096);

	int  GetLength(void);
	int  Read(void* lpBuffer, DWORD len, DWORD dwWait = 0);
    int  Write(const void* lpBuffer, DWORD len);
    DCB           m_DCB;

protected:
    inline bool SetTimeouts(DWORD r_ms = MAXDWORD, DWORD w_ms = 100);		//MAXWORD
    inline bool SetMask();

protected:
    HANDLE        m_hComm;    
    HANDLE        m_hCommEvent;
    HANDLE        m_hThread;
    OVERLAPPED    m_ReadOverlapped;
    OVERLAPPED    m_WriteOverlapped;
    OVERLAPPED    m_WaitOverlapped;

	string        m_portName;

	
};

#endif
