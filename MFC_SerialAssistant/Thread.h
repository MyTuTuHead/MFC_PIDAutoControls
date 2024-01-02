#pragma once
#include "Thread.h"

class CThread
{
public:
	CThread();
	~CThread();

public:
	HANDLE m_hThread;//�߳̾��
	bool m_bExit;//�߳��˳���־λ
	DWORD64 m_dwParam;//֪ͨ�߳�ʲô�������Դ����̵߳���

public:
	void start();//�����߳�
	void stop();//ֹͣ�߳�

public:
	virtual void SetThreadData(DWORD64 dwParam);//�������
	virtual DWORD64 GetThreadData();//�������

public:
	virtual void run();

public:
	static DWORD ThreadProc(LPVOID pParam);
};

