
#pragma warning(disable: 4800)    //disable waring of TRUE => true
#pragma warning(disable: 4267)    //disable waring of size_t => int
#include "stdafx.h"
#include "Serial.h"
#include <tchar.h>
#include <windows.h>


std::wstring stringToWString(const std::string &str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	int size = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring ret = std::wstring(size, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &ret[0], size);

	return ret;
}



CSerial::CSerial(void) : m_hComm(NULL), m_hThread(NULL)
{
    memset(&m_WaitOverlapped,  0, sizeof(OVERLAPPED));
    memset(&m_ReadOverlapped,  0, sizeof(OVERLAPPED));
    memset(&m_WriteOverlapped, 0, sizeof(OVERLAPPED));

    m_WaitOverlapped.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_ReadOverlapped.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_WriteOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hCommEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hComm = NULL;
	m_hCommEvent = NULL;
	m_hThread = NULL;
	
	m_DCB.BaudRate = BaudRate115200;
	m_DCB.ByteSize = DataBits8;
	m_DCB.Parity = ParityNone;
	m_DCB.StopBits = StopOne;
	m_DCB.fBinary = true;
	m_DCB.fInX = false;
	m_DCB.fOutX = false;
	m_DCB.fAbortOnError = false;
	m_DCB.fNull = false;
}


bool CSerial::Open(string port)
{
	DWORD err;
	TCHAR *tcPortName = NULL;

    if (m_hComm)
        return false;    //串口已打开
    
	m_portName = "\\\\.\\" + port;
    
#ifdef UNICODE
	std::wstring wstr = stringToWString(m_portName);
	tcPortName = const_cast<TCHAR *>(wstr.c_str());
#else
	tcPortName = const_cast<TCHAR *>(m_portName.c_str());
#endif

	m_hComm = CreateFile(tcPortName,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         FILE_FLAG_OVERLAPPED,
                         0
                         );

    bool bOpened = (m_hComm != INVALID_HANDLE_VALUE);
    
	if (!bOpened) {
		m_hComm = NULL;
		err = GetLastError();
	}
	else
    {
        bool bState = Setup() && SetBuffer() && SetMask() && SetTimeouts();    
        bOpened = bState;

        if (!bState) {
            Close();
        }
        else {
            PurgeComm(m_hComm, PURGE_RXABORT | PURGE_RXCLEAR);
        }
    }

    return bOpened;
}


bool CSerial::Setup(DWORD dwBaudRate, DWORD dwByteSize, DWORD dwParity, DWORD dwStopBits)
{
    if(!m_hComm)
        return false;

	m_DCB.BaudRate = dwBaudRate;
	m_DCB.ByteSize = dwByteSize;
	m_DCB.Parity = dwParity;
	m_DCB.StopBits = dwStopBits;
    
	return SetCommState(m_hComm, &m_DCB);
}
    
    
bool CSerial::SetBuffer(DWORD dwInQueue, DWORD dwOutQueue)    
{
    if(!m_hComm)
        return false;

    return SetupComm(m_hComm, dwInQueue, dwOutQueue);
}

   
bool CSerial::Close()
{
    if(!m_hComm)
        return true;

    CloseHandle(m_hComm);
    m_hComm = NULL;

    return true;
}


bool CSerial::Clear()
{
    if(!m_hComm)
        return true;

	return PurgeComm(m_hComm, PURGE_RXCLEAR);
}


int CSerial::GetLength(void)
{
    BOOL    ret;
    DWORD   dwError;
    COMSTAT state;

    ret = ClearCommError(m_hComm, &dwError, &state);

    return (ret == TRUE) ? state.cbInQue : -1;
}

    


int g_ipos = 0;

int CSerial::Read(void* lpBuffer, DWORD len, DWORD dwWait)
{
    COMSTAT state;
    DWORD   dwMask, dwRead, dwError;

    if (!m_hComm)
        return 0;

    if (!ReadFile(m_hComm, lpBuffer, len, &dwRead, &m_ReadOverlapped)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            WaitForSingleObject(m_ReadOverlapped.hEvent, dwWait);
        }
        return 0;
    }

   

    return dwRead;
}



int CSerial::Write(const void* lpBuffer, DWORD len)
{
    if(!m_hComm)
        return 0;

    DWORD dwWritten;
    DWORD dwError;

    if (!ClearCommError(m_hComm, &dwError, NULL)) {
        //清除输出缓冲
        PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR);
        return 0;
    }

    if (!WriteFile(m_hComm, lpBuffer, len, &dwWritten, &m_WriteOverlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            //等待写完成
            if (!GetOverlappedResult(m_hComm, &m_WriteOverlapped, &dwWritten, TRUE))
                return 0;                
        }
    }

    return dwWritten;
}


CSerial::~CSerial(void)
{
    if (m_hComm)
        SetCommMask(m_hComm, 0);

    Close();

    CloseHandle(m_ReadOverlapped.hEvent);
    CloseHandle(m_WriteOverlapped.hEvent);
    CloseHandle(m_WaitOverlapped.hEvent);
    CloseHandle(m_hCommEvent);
}
    


inline bool CSerial::SetTimeouts(DWORD r_ms, DWORD w_ms)
{
    if (!m_hComm)
        return false;

    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout = r_ms;  //ReadFile马上返回
    timeouts.WriteTotalTimeoutConstant = w_ms; //单次写超时时间

    return SetCommTimeouts(m_hComm, &timeouts);
}

//设置事件掩码
inline bool CSerial::SetMask()
{    
    if (!m_hComm)
        return false;

    return SetCommMask(m_hComm, EV_RXCHAR | EV_ERR);
}
    

