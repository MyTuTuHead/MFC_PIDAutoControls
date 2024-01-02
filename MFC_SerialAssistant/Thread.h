#pragma once
#include "Thread.h"

class CThread
{
public:
	CThread();
	~CThread();

public:
	HANDLE m_hThread;//线程句柄
	bool m_bExit;//线程退出标志位
	DWORD64 m_dwParam;//通知线程什么参数可以传到线程当中

public:
	void start();//启动线程
	void stop();//停止线程

public:
	virtual void SetThreadData(DWORD64 dwParam);//额外参数
	virtual DWORD64 GetThreadData();//额外参数

public:
	virtual void run();

public:
	static DWORD ThreadProc(LPVOID pParam);
};

