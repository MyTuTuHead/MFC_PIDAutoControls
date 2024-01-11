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
��������: Create

���ܣ�����

���������ԣ�

����ֵ�� �ɹ�����TRUE,���򷵻�FALSE
/**********************************************************************/
BOOL CComm::Create(int Port, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
	if (!CreateComPort(Port, dwBaudRate, bByteSize, bParity, bStopBits))
	{
		Destroy();
		return FALSE;
	}

	m_pReadCommThread = AfxBeginThread(ReadCommThread, this);	//����һ�����ڶ������߳�
	if (!m_pReadCommThread)
	{
		Destroy();
		return FALSE;
	}

	m_bConnect = true;
	return TRUE;
}


/**********************************************************************
��������: Destroy

���ܣ�����

��������

����ֵ�� ��
/**********************************************************************/
void CComm::Destroy()
{
	m_bConnect = false;
	if (m_pReadCommThread)		//���ڶ��̵߳�ָ���Ƿ�Ϊ��
	{
		m_bRun = FALSE;
		while (m_bThreadStop == FALSE)	//������ڶ��̻߳����ڣ������߳�����1ms������
		{								//���ڶ��߳̽���
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
��������: CreateComPort

���ܣ�����ָ���Ĵ���

������
	Port(in)����������
	BaudRate(in): ������
	ByteSize������λ
	Parity����żУ��
	StopBits��ֹͣλ

����ֵ��
	�����ɹ�����TRUE, ���򷵻�FALSE
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

	// ���û���������
	SetupComm(m_hCom, 4096, 4096);
	// ���I/O������
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

	dcb.BaudRate = dwBaudRate; // ָ����ǰ�Ĳ����� 
	dcb.ByteSize = bByteSize;  // ָ���˿ڵ�ǰʹ�õ�����λ 
	dcb.Parity = bParity;	    // ָ����żУ���Ƿ�����
	dcb.StopBits = bStopBits;  // ָ���˿ڵ�ǰʹ�õ�ֹͣλ��

	dcb.fOutxDsrFlow = FALSE;	// ָ��DSR���ڼ�ⷢ�Ϳ���
	dcb.fDtrControl = DTR_CONTROL_DISABLE;

	dcb.fOutxCtsFlow = FALSE;	// ָ��CTS���ڼ�ⷢ�Ϳ���
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

	dcb.fTXContinueOnXoff = 1;
	// setup software flow control
	dcb.fInX = dcb.fOutX = 0;
	dcb.XonChar = '\021';
	dcb.XoffChar = '\023';
	dcb.XonLim = 2048;
	dcb.XoffLim = 512;

	dcb.fBinary = TRUE;		// ָ���Ƿ����������ģʽ
	dcb.fParity = 0;			// ָ����żУ���Ƿ�����

	// �ı䴮������
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
��������: SendData

���ܣ���������

������
	pBuff(in):	  Ҫ���͵��ַ���
	nBuffLen(in): Ҫ���͵��ַ����ĳ���

����ֵ��
	  ���ͳɹ�����TRUE, ���򷵻�FALSE
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
��������: ReadBuffer

���ܣ���ȡ����

������
	pBuff(out): ���մ������ݵ��ַ���ָ��
	nMaxBuffLen(in): pBuff�ַ�������󳤶�

����ֵ��
	���մ������ݵĴ�С
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
��������: ReadCommThread

���ܣ����ڶ��߳�

������LPVOID

����ֵ�� UINT
/**********************************************************************/
UINT CComm::ReadCommThread(LPVOID lpVoid)		//���ڶ��ɶ����Ķ��߳��Զ���ɣ�����Ҫ��Ϊ��Ԥ
{
	CComm* pCComm = (CComm*)lpVoid;

	char chBuffer[100];
	int nLen;

	pCComm->m_bRun = TRUE;			//m_bRun��m_bThreadStop�����������̼߳�Ľ���
	pCComm->m_bThreadStop = FALSE;	//���̰߳�m_bRunΪ0��Ϊ�����˳��ı�־�����̰߳�m_bThreadStopΪ1��Ϊ���ڶ��߳̽�����ʱ���Լ��Ļ�Ӧ

	while (pCComm->m_bRun)			//�ö��߳������߳�ִ��Destory()ʱ���˳�
	{
		nLen = pCComm->ReadBuffer(chBuffer, 100);
		if (nLen > 0)
		{
			for (int i = 0; i < nLen; i++)
			{
				//                pCComm->m_ringBuf.Push_back(chBuffer[i]);	//���뻷�λ�����
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
			Sleep(100);		//˯��0.1s����������ÿ0.1s�Ͷ�һ�δ��ڻ�����
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

	while (m_bLock);		//m_bLock�൱���ٽ�����һ����
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
