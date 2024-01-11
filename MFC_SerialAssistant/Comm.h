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



//����buffer��
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



//Ϊ���ڷ����������
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


//������
class CComm
{
public:
	CComm();
	virtual ~CComm();

	// ����
	BOOL Create(int Port,	 // Com Port
		DWORD dwBaudRate = CBR_115200, // 115200
		BYTE bByteSize = 8,          // 8λ
		BYTE bParity = NOPARITY,	 // ��У��
		//		BYTE bParity = EVENPARITY,	 // żУ��
		BYTE bStopBits = ONESTOPBIT	 // 1
	);

	// ��������
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

	CJRing m_ringBuf;	//����buffer�����

private:
	// ����

	BOOL CreateComPort(int Port, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits); // ����ָ���Ĵ���
	// ��ȡ����
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
	HANDLE m_hCom;	// ͨѶ�豸���
	OVERLAPPED m_oRead; // �첽I/O��
	OVERLAPPED m_oWrite; // �첽I/Oд

	// ���ڶ��߳�
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
