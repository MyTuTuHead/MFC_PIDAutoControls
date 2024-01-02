#include "pch.h"
#include "SerialPort.h"

CSerialPort::CSerialPort()
{
    m_hComm = NULL;
}

CSerialPort::~CSerialPort()
{
	CloseComm();
}

void CSerialPort::StartComm()
{
	m_hThreadComm.SetThreadData((DWORD64)this);
	//m_hThreadComm.SetThreadData();
	m_hThreadComm.start();
}

BOOL CSerialPort::OpenComm(CString strComm)
{
	if (NULL == m_hComm)
	{
		m_hComm = CreateFile((TCHAR*)(LPCTSTR)strComm,//COM口  //tchar  lpctstr
			GENERIC_READ | GENERIC_WRITE, //允许读和写  //generic_read
			0, //独占方式  
			NULL,
			OPEN_EXISTING, //打开而不是创建  //open_existing
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //同步方式  //file_attribute_normal | file_flag_overlapped
			NULL);
		if (INVALID_HANDLE_VALUE == m_hComm)//invalid_handle_value
		{
			int nError = GetLastError();
			m_hComm = NULL;
			AfxMessageBox(_T("串口打开失败"));
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSerialPort::SetCommState(DWORD dwBaudrate, BYTE byParity, BYTE byByteSize, BYTE byStopBits)
{
	if (NULL == m_hComm) return FALSE;
	DCB dcb;
	BOOL bRet = ::GetCommState(m_hComm, &dcb);
	if (!bRet)
	{
		if (m_hComm)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}
		return FALSE;
	}
	dcb.BaudRate = dwBaudrate;
	dcb.Parity = byParity;
	dcb.ByteSize = byByteSize;
	dcb.StopBits = byStopBits;

	bRet = ::SetCommState(m_hComm, &dcb);
	if (!bRet)
	{
		if (m_hComm)
		{
			CloseHandle(m_hComm);
			m_hComm = NULL;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CSerialPort::SetupComm(DWORD dwInQueue, DWORD dwOutQueue)
{
	if (NULL == m_hComm) return FALSE;
	return ::SetupComm(m_hComm, dwInQueue, dwOutQueue);
}

BOOL CSerialPort::PurgeComm(DWORD dwFlags)
{
	if (NULL == m_hComm) return FALSE;
	return ::PurgeComm(m_hComm, dwFlags);
}

BOOL CSerialPort::SetCommMask(DWORD dwEvtNask)
{
	if (NULL == m_hComm) return FALSE;
	return ::SetCommMask(m_hComm, dwEvtNask);
}

BOOL CSerialPort::WriteFile(
	IN LPCVOID lpBuffer, // 写入的数据存储的地址：即以该指针的值为首地址的nNumberOfBytesToWrite个字节的数据将要写入
	//串口的发送数据缓冲区。 
	IN DWORD nNumberOfBytesToWrite, //要写入的数据的字节数  
	OUT LPDWORD lpNumberOfBytesWritten,// 指向指向一个DWORD数值，该数值返回实际写入的字节数 
	IN LPOVERLAPPED lpOverlapped// 重叠操作时，该参数指向一个OVERLAPPED结构；同步操作时，该参数为NULL。 
)
{
	if (NULL == m_hComm) return FALSE;
	return ::WriteFile(m_hComm, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

}

BOOL CSerialPort::ReadFile(
	OUT LPVOID lpBuffer,// 读入的数据存储的地址：即读入的数据将存储在以该指针的值为首地址的一片内存区  
	IN DWORD nNumberOfBytesToRead, // 要读入的数据的字节数  
	OUT LPDWORD lpNumberOfBytesRead,// 指向一个DWORD数值，该数值返回读操作实际读入的字节数  
	IN LPOVERLAPPED lpOverlapped// 重叠操作时，该参数指向一个OVERLAPPED结构，同步操作时，该参数为NULL 
)
{
	if (NULL == m_hComm) return FALSE;
	return ::ReadFile(m_hComm, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL CSerialPort::ClearCommError(OUT LPDWORD lpError, OUT LPCOMSTAT lpStat)
{
	if (NULL == m_hComm) return FALSE;
	return ::ClearCommError(m_hComm, lpError, lpStat);
}

BOOL CSerialPort::GetOverlappedResult(
	IN LPOVERLAPPED lpOverlapped,//指向重叠操作开始时指定的OVERLAPPED结构  
	OUT LPDWORD lpNumberOfBytesTransferred,//指向一个32位变量，该变量的值返回实际读写操作传输的字节数。  
	IN BOOL bWait// 该参数用于指定函数是否一直等到重叠操作结束：如果该参数为TRUE，函数直到操作结束才返回；
	//如果该参数为FALSE，函数直接返回，这时如果操作没有完成，通过调用GetLastError()函数会
	//返回ERROR_IO_INCOMPLETE。 
)
{
	if (NULL == m_hComm) return FALSE;
	return ::GetOverlappedResult(m_hComm, lpOverlapped, lpNumberOfBytesTransferred, bWait);
}

void CSerialPort::CloseComm()
{
	m_hThreadComm.stop();
	if (m_hComm)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
}


