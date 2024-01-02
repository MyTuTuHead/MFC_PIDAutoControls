#include "pch.h"
#include "ThreadComm.h"
#include "MFC_SerialAssistantDlg.h"
#include "SerialPort.h"
#include "resource.h"
#include "MFC_SerialAssistant.h"



extern int Pid_Buffer[500];
int Pid_cnt = 0;
int Multiple = 1;
extern int stop_flag;

CThreadComm::CThreadComm()
{
}

CThreadComm::~CThreadComm()
{
}

void CThreadComm::run()
{
	Sleep(100);
	CWnd* pWinApp = (CWnd*)AfxGetApp()->m_pMainWnd;
	if (NULL == pWinApp)
	{
		return;
	}

	CSerialPort* pSerialPort = (CSerialPort*)GetThreadData();
	if (NULL == pSerialPort) return;

	DWORD dwError = 0;
	COMSTAT comStat;
	BOOL bRet = TRUE;
	TCHAR recvTemp[512];//临时缓冲区
	ZeroMemory(recvTemp, sizeof(recvTemp));

	TCHAR recvBuf[4096];//真正的缓冲区
	ZeroMemory(recvBuf, sizeof(recvBuf));


	DWORD dwRead = 0;
	int nLength = 0;

	pSerialPort->ClearCommError(&dwError, &comStat);
	if (comStat.cbInQue > 0)//判断输入缓存区是否有数据
	{
		OVERLAPPED overlappedRead;
		ZeroMemory(&overlappedRead, sizeof(OVERLAPPED));
		overlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (comStat.cbInQue < 512)
		{
			bRet = pSerialPort->ReadFile(recvTemp, comStat.cbInQue, &dwRead, &overlappedRead);
		}
		else
		{
			bRet = pSerialPort->ReadFile(recvTemp, 500, &dwRead, &overlappedRead);
		}

		if (comStat.cbInQue >= dwRead)
		{
			nLength = 0;
			memcpy(recvBuf + nLength, recvTemp, dwRead);
			if (stop_flag == 0) {
				for (int i = 0; i < comStat.cbInQue; i++) {
					if (recvBuf[i] == 'e' && i >= 6) {
						Pid_cnt++;
						if (Pid_cnt >= 7990 && stop_flag == 0) {
							//Pid_cnt = 0;
							stop_flag = 2;
						}
						Multiple = 1;
						for (int j = i - 1; j > i - 6; j--) {
							if (recvBuf[j] == 'e') {
								Multiple = 1;
								break;
							}
							else if (recvBuf[j] >= '0' && recvBuf[j] <= '9') {
								Pid_Buffer[Pid_cnt] += (recvBuf[j] - 0x30) * Multiple;
								Multiple *= 10;
							}
							else {
								break;
							}
						}
					}
				}
			}
			nLength += dwRead;
		}

		CloseHandle(overlappedRead.hEvent);


		if (comStat.cbInQue == dwRead)
		{
			nLength = 0;
			CEdit* pEditRecv = (CEdit*)pWinApp->GetDlgItem(IDC_EDIT_RECV);//IDC_EDIT_RECV包含在：#include "resource.h"
			if (pEditRecv)
			{
				CString strRecv;
				pEditRecv->GetWindowText(strRecv);
				strRecv = strRecv + _T("收：") + recvBuf + _T("\r\n");
				pEditRecv->SetWindowText(strRecv);
			}
		}

		if (!bRet)//判断是否重叠
		{
			if (ERROR_IO_PENDING == GetLastError())
			{
				while (!bRet)
				{
					bRet = pSerialPort->GetOverlappedResult(NULL, &dwRead, TRUE);
					if (GetLastError() != ERROR_IO_INCOMPLETE)
					{
						pSerialPort->ClearCommError(&dwError, &comStat);
						break;
					}
				}
			}
		}
	}
}
