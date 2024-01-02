#include "pch.h"
#include "Thread.h"

CThread::CThread()
{
	m_bExit = false;
	m_dwParam = 0;
	m_hThread = NULL;
}

CThread::~CThread()
{
	if (!m_bExit)
	{
		stop();
	}
}

void CThread::start()
{
	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, &dwThreadID);//lpthread_start_routine
	ASSERT(hThread);
	m_hThread = hThread;
}

void CThread::stop()
{
	if (m_hThread)
	{
		m_bExit = true;
		::WaitForSingleObject(m_hThread, INFINITE);//�ȴ��߳���ȫ�˳�
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

void CThread::SetThreadData(DWORD64 dwParam)
{
	if (m_dwParam != dwParam)
	{
		m_dwParam = dwParam;
	}
}

DWORD64 CThread::GetThreadData()
{
	return m_dwParam;
}

void CThread::run()
{
	Sleep(100);
}

DWORD CThread::ThreadProc(LPVOID pParam)
{
	CThread* pThis = (CThread*)pParam;//ǿ��ת��
	ASSERT(pThis);

	while (!pThis->m_bExit)
	{
		pThis->run();
	}
	return TRUE;
}

