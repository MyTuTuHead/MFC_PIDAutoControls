#pragma once
#include "ThreadComm.h"

class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();
public:
	BOOL OpenComm(CString strComm);
	BOOL SetCommState(DWORD dwBaudrate, BYTE byParity, BYTE byByteSize, BYTE byStopBits);
	BOOL SetupComm(DWORD dwInQueue, DWORD dwOutQueue);
	BOOL PurgeComm(DWORD dwFlags);
	BOOL SetCommMask(DWORD dwEvtNask);
	BOOL WriteFile(
		IN LPCVOID lpBuffer, // 写入的数据存储的地址：即以该指针的值为首地址的nNumberOfBytesToWrite个字节的数据将要写入
		//串口的发送数据缓冲区。 
		IN DWORD nNumberOfBytesToWrite, //要写入的数据的字节数  
		OUT LPDWORD lpNumberOfBytesWritten,// 指向指向一个DWORD数值，该数值返回实际写入的字节数 
		IN LPOVERLAPPED lpOverlapped// 重叠操作时，该参数指向一个OVERLAPPED结构；同步操作时，该参数为NULL。 
	);
	BOOL ReadFile(
		OUT LPVOID lpBuffer,// 读入的数据存储的地址：即读入的数据将存储在以该指针的值为首地址的一片内存区  
		IN DWORD nNumberOfBytesToRead, // 要读入的数据的字节数  
		OUT LPDWORD lpNumberOfBytesRead,// 指向一个DWORD数值，该数值返回读操作实际读入的字节数  
		IN LPOVERLAPPED lpOverlapped// 重叠操作时，该参数指向一个OVERLAPPED结构，同步操作时，该参数为NULL 
	);
	BOOL ClearCommError(OUT LPDWORD lpError, OUT LPCOMSTAT lpStat);

	BOOL GetOverlappedResult(
		IN LPOVERLAPPED lpOverlapped,//指向重叠操作开始时指定的OVERLAPPED结构  
		OUT LPDWORD lpNumberOfBytesTransferred,//指向一个32位变量，该变量的值返回实际读写操作传输的字节数。  
		IN BOOL bWait// 该参数用于指定函数是否一直等到重叠操作结束：如果该参数为TRUE，函数直到操作结束才返回；
		//如果该参数为FALSE，函数直接返回，这时如果操作没有完成，通过调用GetLastError()函数会
		//返回ERROR_IO_INCOMPLETE。 
	    );
	void CloseComm();
public:
	void StartComm();

public:
	CThreadComm m_hThreadComm;
public:
	HANDLE m_hComm;

};

