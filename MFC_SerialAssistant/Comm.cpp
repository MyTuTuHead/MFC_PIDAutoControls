// Comm.cpp: implementation of the CComm class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Comm.h"
#include <stdio.h> 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CComm::CComm()
{
	m_dataBuff = 2048;
	m_hCom = NULL;
	m_pReadCommThread = NULL;

	memset(&m_oRead, 0, sizeof(OVERLAPPED));
	memset(&m_oWrite, 0, sizeof(OVERLAPPED));

	m_bRun = FALSE;
	m_bThreadStop = TRUE;

	m_bLock = FALSE;

	m_nDZBuffLen = 0;
	m_nYCBuffLen = 0;
	DataProc = NULL;

	m_ringBuf.Clear();

	m_bConnect = false;
}


void CComm::SetDataProcess(CommDataProcess  pProc)
{
	DataProc = pProc;
}

CComm::~CComm()
{
	Destroy();

	POSITION pos = m_DataList.GetHeadPosition();
	while (pos)
		delete m_DataList.GetNext(pos);
}

/**********************************************************************
函数名称: Create

功能：创建

参数：（略）

返回值： 成功返回TRUE,否则返回FALSE
/**********************************************************************/
BOOL CComm::Create(int Port, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
	if (!CreateComPort(Port, dwBaudRate, bByteSize, bParity, bStopBits))
	{
		Destroy();
		return FALSE;
	}

	m_pReadCommThread = AfxBeginThread(ReadCommThread, this);	//创建一个串口读工作线程
	if (!m_pReadCommThread)
	{
		Destroy();
		return FALSE;
	}

	m_bConnect = true;
	return TRUE;
}


/**********************************************************************
函数名称: Destroy

功能：销毁

参数：无

返回值： 无
/**********************************************************************/
void CComm::Destroy()
{
	m_bConnect = false;
	if (m_pReadCommThread)		//串口读线程的指针是否为空
	{
		m_bRun = FALSE;
		while (m_bThreadStop == FALSE)	//如果串口读线程还存在，则主线程休眠1ms，便于
		{								//串口读线程结束
			Sleep(1);
		}
		m_pReadCommThread = NULL;
	}

	if (m_hCom)
	{
		CloseHandle(m_hCom);
		m_hCom = NULL;
	}

	if (m_oRead.hEvent)
	{
		CloseHandle(m_oRead.hEvent);
		m_oRead.hEvent = NULL;
	}
	if (m_oWrite.hEvent)
	{
		CloseHandle(m_oWrite.hEvent);
		m_oWrite.hEvent = NULL;
	}
}


/**********************************************************************
函数名称: CreateComPort

功能：创建指定的串口

参数：
	Port(in)：串口名称
	BaudRate(in): 波特率
	ByteSize：数据位
	Parity：奇偶校验
	StopBits：停止位

返回值：
	创建成功返回TRUE, 否则返回FALSE
/**********************************************************************/
BOOL CComm::CreateComPort(int Port, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
	ASSERT(m_hCom == NULL);

	COMMTIMEOUTS  CommTimeOuts;

	CHAR chCom[100] = "\0";
	sprintf_s(chCom, 100, "\\\\.\\COM%d", Port);

	m_hCom = CreateFile(chCom,
		GENERIC_READ | GENERIC_WRITE,
		0,    // exclusive access 
		NULL, // no security attributes 
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (m_hCom == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置缓冲区长度
	SetupComm(m_hCom, 4096, 4096);
	// 清除I/O缓冲区
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT |
		PURGE_TXCLEAR | PURGE_RXCLEAR);
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 500;
	SetCommTimeouts(m_hCom, &CommTimeOuts);

	//	
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hCom, &dcb);

	dcb.BaudRate = dwBaudRate; // 指定当前的波特率 
	dcb.ByteSize = bByteSize;  // 指定端口当前使用的数据位 
	dcb.Parity = bParity;	    // 指定奇偶校验是否允许
	dcb.StopBits = bStopBits;  // 指定端口当前使用的停止位数

	dcb.fOutxDsrFlow = FALSE;	// 指定DSR用于检测发送控制
	dcb.fDtrControl = DTR_CONTROL_DISABLE;

	dcb.fOutxCtsFlow = FALSE;	// 指定CTS用于检测发送控制
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

	dcb.fTXContinueOnXoff = 1;
	// setup software flow control
	dcb.fInX = dcb.fOutX = 0;
	dcb.XonChar = '\021';
	dcb.XoffChar = '\023';
	dcb.XonLim = 2048;
	dcb.XoffLim = 512;

	dcb.fBinary = TRUE;		// 指定是否允许二进制模式
	dcb.fParity = 0;			// 指定奇偶校验是否允许

	// 改变串口设置
	if (!SetCommState(m_hCom, &dcb))
		return FALSE;

	SetCommMask(m_hCom, EV_CTS | EV_DSR);
	m_oWrite.Offset = 0;
	m_oWrite.OffsetHigh = 0;
	m_oRead.Offset = 0;
	m_oRead.OffsetHigh = 0;

	m_oRead.hEvent = CreateEvent(NULL,    // no security
		TRUE,		// explicit reset req
		FALSE,		// initial event reset
		NULL);		// no name
	if (m_oRead.hEvent == NULL)
		return FALSE;

	m_oWrite.hEvent = CreateEvent(NULL,    // no security
		TRUE,    // explicit reset req
		FALSE,   // initial event reset
		NULL); // no name
	if (m_oWrite.hEvent == NULL)
		return FALSE;

	return TRUE;
}


/**********************************************************************
函数名称: SendData

功能：发送数据

参数：
	pBuff(in):	  要发送的字符串
	nBuffLen(in): 要发送的字符串的长度

返回值：
	  发送成功返回TRUE, 否则返回FALSE
/**********************************************************************/
BOOL CComm::SendData(const char* pBuff, int nBuffLen)
{
	BOOL fWriteStat;
	DWORD dwBytesWritten;
	DWORD dwErrorFlags;
	DWORD dwError;
	COMSTAT ComStat;

	if (m_hCom == NULL)
		return FALSE;

	fWriteStat = WriteFile(m_hCom, pBuff, nBuffLen,
		&dwBytesWritten, &m_oWrite);

	if (!fWriteStat)
	{
		dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult(m_hCom,
				&m_oWrite, &dwBytesWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError == ERROR_IO_INCOMPLETE)
					continue;
				else
				{
					ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
					break;
				}
			}
		}
		else if (dwError == ERROR_INVALID_HANDLE)
		{
			return (FALSE);
		}
		else
		{
			ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
			return (TRUE);
		}
	}
	return (TRUE);
}


/**********************************************************************
函数名称: ReadBuffer

功能：读取数据

参数：
	pBuff(out): 接收串口数据的字符串指针
	nMaxBuffLen(in): pBuff字符串的最大长度

返回值：
	接收串口数据的大小
/**********************************************************************/
int CComm::ReadBuffer(char* pBuff, int nMaxBuffLen)
{
	BOOL fReadStat;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	DWORD dwLength;
	DWORD dwError;

	// only try to read number of bytes in queue
	ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
	dwLength = min((DWORD)nMaxBuffLen, ComStat.cbInQue);

	if (dwLength > 0)
	{
		fReadStat = ReadFile(m_hCom, pBuff,
			dwLength, &dwLength, &m_oRead);
		if (!fReadStat)
		{
			dwError = GetLastError();
			if (dwError == ERROR_IO_PENDING)
			{
				while (!GetOverlappedResult(m_hCom,
					&m_oRead, &dwLength, TRUE))
				{
					dwError = GetLastError();
					if (dwError == ERROR_IO_INCOMPLETE)
						// normal result if not finished
						continue;
					else
					{
						// an error occurred, try to recover
						ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
						break;
					}
				}
			}
			else if (dwError == ERROR_INVALID_HANDLE)
			{
				dwLength = -1;
			}
			else
			{
				// some other error occurred
				dwLength = 0;
				ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
			}
		}
	}
	return (dwLength);
}

/**********************************************************************
函数名称: ReadCommThread

功能：串口读线程

参数：LPVOID

返回值： UINT
/**********************************************************************/
UINT CComm::ReadCommThread(LPVOID lpVoid)		//串口读由独立的读线程自动完成，不需要人为干预
{
	CComm* pCComm = (CComm*)lpVoid;

	char chBuffer[100];
	int nLen;

	pCComm->m_bRun = TRUE;			//m_bRun与m_bThreadStop被用于主辅线程间的交互
	pCComm->m_bThreadStop = FALSE;	//该线程把m_bRun为0作为自身退出的标志，主线程把m_bThreadStop为1作为串口读线程将结束时给自己的回应

	while (pCComm->m_bRun)			//该读线程在主线程执行Destory()时，退出
	{
		nLen = pCComm->ReadBuffer(chBuffer, 100);
		if (nLen > 0)
		{
			for (int i = 0; i < nLen; i++)
			{
				//                pCComm->m_ringBuf.Push_back(chBuffer[i]);	//放入环形缓冲区
				pCComm->m_dataBuff.InQueue(chBuffer[i]);
			}

			/*
			if(pCComm->DataProc != NULL)
			{
				pCComm->DataProc(chBuffer, nLen);
			}//*/
		}
		else
		{
			Sleep(100);		//睡眠0.1s，基本上是每0.1s就读一次串口缓冲区
		}
	}

	pCComm->m_bThreadStop = TRUE;
	return 0;
}

void CComm::Stop()
{
	this->DataProc = NULL;
}

CCommData* CComm::GetData()
{
	CCommData* pData = NULL;

	while (m_bLock);		//m_bLock相当于临界区的一把锁
	m_bLock = TRUE;

	if (m_DataList.GetCount() > 0)
	{
		pData = m_DataList.RemoveHead();
	}
	m_bLock = FALSE;

	return pData;
}



void CComm::ProcessBuff(char* pBuff, int len)
{
	for (int i = 0; i < len; i++)
	{

	}
}





bool CComm::IsOpened(void)
{
	if (m_hCom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BOOL CComm::SetDCB(DWORD dwBaudRate, BYTE bByteSize, BYTE bStopBits, BYTE bParity)
{
	DCB dcb;

	if (!m_hCom)
		return FALSE;

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hCom, &dcb);

	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = bByteSize;
	dcb.Parity = bParity;
	dcb.StopBits = bStopBits;

	return SetCommState(m_hCom, &dcb);
}
