#pragma once
// Comm.h: interface for the CComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMM_H__2B8778C6_A1A8_48C6_87FA_A6EF17BF8E95__INCLUDED_)
#define AFX_COMM_H__2B8778C6_A1A8_48C6_87FA_A6EF17BF8E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "cycqueue.h"
#define MAX_BUFFER 1024

#define SAFE_DELETE(p) {if(p) { delete p; p=NULL; }}
#define SAFE_DELETE_ARRAY(p) {if(p) { delete []p; p=NULL; }}


typedef   void (*CommDataProcess)(char* pBuf, int len);



//环形buffer类
#define RING_LENGTH 102400

class CJRing
{
public:
	CJRing(void) { Clear(); }

public:
	~CJRing(void) {}

public:
	int   head;
	int   tail;
	BYTE  buf[RING_LENGTH];

public:
	void  Clear()
	{
		head = 0;
		tail = 0;
	}
	void  Push_back(BYTE cb)
	{
		buf[tail++] = cb;

		if (tail >= RING_LENGTH)
		{
			tail = 0;
		}

	}

	bool  Get(BYTE& cb)
	{
		if (tail == head)
		{
			return false;
		}
		cb = buf[head++];
		if (head >= RING_LENGTH)
		{
			head = 0;
		}
		return true;
	}

};



//为串口服务的数据类
class CCommData
{
public:
	CCommData()
	{
		m_pData = NULL;
		int m_nLen = 0;
	}

	~CCommData()
	{
		SAFE_DELETE(m_pData);
	}

	unsigned char* m_pData;
	int m_nLen;
};


//串口类
class CComm
{
public:
	CComm();
	virtual ~CComm();

	// 创建
	BOOL Create(int Port,	 // Com Port
		DWORD dwBaudRate = CBR_115200, // 115200
		BYTE bByteSize = 8,          // 8位
		BYTE bParity = NOPARITY,	 // 无校验
		//		BYTE bParity = EVENPARITY,	 // 偶校验
		BYTE bStopBits = ONESTOPBIT	 // 1
	);

	// 发送数据
	bool m_bConnect;
	BOOL SendData(const char* pBuff, int nBuffLen);

	BOOL GetData(char cb);

	CCommData* GetData();
	void Destroy();
	void Stop();
	void SetDataProcess(CommDataProcess  pProc);

	bool IsRuning()
	{
		return m_bRun;
	}

	CJRing m_ringBuf;	//环形buffer类对象

private:
	// 销毁

	BOOL CreateComPort(int Port, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits); // 创建指定的串口
	// 读取数据
	int ReadBuffer(char* pBuff, int nMaxBuffLen = 100);

	void SendDZData(char* pch, int len);
	void GetYCBuff(unsigned char c);
	void GetDZBuff(char c);

	char m_chDZBuff[MAX_BUFFER];
	int m_nDZBuffLen;

	char m_chYCBuff[MAX_BUFFER];
	int m_nYCBuffLen;


	CommDataProcess DataProc;

	BOOL m_b10;

public:
	CCycQueue<char> m_dataBuff;

private:
	HANDLE m_hCom;	// 通讯设备句柄
	OVERLAPPED m_oRead; // 异步I/O读
	OVERLAPPED m_oWrite; // 异步I/O写

	// 串口读线程
	CWinThread* m_pReadCommThread;
	volatile bool m_bRun;
	volatile bool m_bThreadStop;
	static UINT ReadCommThread(LPVOID lpVoid);

protected:
	virtual void ProcessBuff(char* pBuff, int len);

	volatile bool m_bLock;

	CList<CCommData*, CCommData*> m_DataList;
public:
	bool IsOpened(void);
public:
	BOOL SetDCB(DWORD dwBaudRate, BYTE bByteSize, BYTE bStopBits, BYTE bParity);
};

#endif // !defined(AFX_COMM_H__2B8778C6_A1A8_48C6_87FA_A6EF17BF8E95__INCLUDED_)
