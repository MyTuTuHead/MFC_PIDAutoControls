
// MFC_SerialAssistantDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_SerialAssistant.h"
#include "MFC_SerialAssistantDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int Tim_Cnt = 0;

extern int Pid_cnt;
int Pid_Buffer[8000];
int clc_cnt = 0;
int stop_flag = 0;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCSerialAssistantDlg 对话框



CMFCSerialAssistantDlg::CMFCSerialAssistantDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_SERIALASSISTANT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSerialAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CUSTOM2, m_Pic);
	DDX_Control(pDX, IDC_EDIT_TIM, Tim_val); 
	DDX_Control(pDX, IDC_EDIT_KP, m_Edit_Kp);
	DDX_Control(pDX, IDC_EDIT_KI, m_Edit_Ki);
	DDX_Control(pDX, IDC_EDIT_Kd, m_Edit_Kd);
	DDX_Control(pDX, IDC_EDIT_STEP, m_Edit_Step);
}

void CMFCSerialAssistantDlg::InitComboBox()
{
	//串口设置
	CComboBox* pComboComm = (CComboBox*)GetDlgItem(IDC_COMBO_COMM);
	ASSERT(pComboComm);
	for (int i = 1; i <= 8; i++)
	{
		CString strComm;
		strComm.Format(_T("COM%d"), i);
		pComboComm->AddString(strComm);
	}
	pComboComm->SetCurSel(0);//选择默认值

	//波特率设置
	CComboBox* pComboBaudRate = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	ASSERT(pComboBaudRate);
	//pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("300")), 300);
	//pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("600")), 600);
	//pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("1200")), 1200);
	//pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("2400")), 2400);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("4800")), 4800);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("9600")), 9600);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("19200")), 19200);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("38400")), 38400);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("43000")), 43000);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("56000")), 56000);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("57600")), 57600);
	pComboBaudRate->SetItemData(pComboBaudRate->AddString(_T("115200")), 115200);
	pComboBaudRate->SetCurSel(1);//默认选择9600

	//奇偶校验位设置
	CComboBox* pComboCheckBit = (CComboBox*)GetDlgItem(IDC_COMBO_CHECKBIT);
	ASSERT(pComboCheckBit);
	pComboCheckBit->SetItemData(pComboCheckBit->AddString(_T("无NULL")), NOPARITY);//no parity
	pComboCheckBit->SetItemData(pComboCheckBit->AddString(_T("奇ODD")), ODDPARITY);//odd parity
	pComboCheckBit->SetItemData(pComboCheckBit->AddString(_T("偶EVEN")), EVENPARITY);//even parity
	pComboCheckBit->SetCurSel(0);//选择默认值

	//数据位设置
	CComboBox* pComboDataBit = (CComboBox*)GetDlgItem(IDC_COMBO_DATABIT);
	ASSERT(pComboDataBit);
	pComboDataBit->SetItemData(pComboDataBit->AddString(_T("6")), 6);
	pComboDataBit->SetItemData(pComboDataBit->AddString(_T("7")), 7);
	pComboDataBit->SetItemData(pComboDataBit->AddString(_T("8")), 8);
	pComboDataBit->SetCurSel(2);//选择默认值

	//停止位设置
	CComboBox* pComboStopBit = (CComboBox*)GetDlgItem(IDC_COMBO_STOPBIT);
	ASSERT(pComboStopBit);
	pComboStopBit->SetItemData(pComboStopBit->AddString(_T("1")), ONESTOPBIT);
	pComboStopBit->SetItemData(pComboStopBit->AddString(_T("2")), TWOSTOPBITS);
	pComboStopBit->SetCurSel(0);//选择默认值
}


BEGIN_MESSAGE_MAP(CMFCSerialAssistantDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_COMMCONTROL, &CMFCSerialAssistantDlg::OnBnClickedBtnCommcontrol)
	ON_BN_CLICKED(IDC_BTN_SEND, &CMFCSerialAssistantDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCSerialAssistantDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, &CMFCSerialAssistantDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMFCSerialAssistantDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_KPADD, &CMFCSerialAssistantDlg::OnBnClickedButtonKpadd)
	ON_BN_CLICKED(IDC_BUTTON_KPSUB, &CMFCSerialAssistantDlg::OnBnClickedButtonKpsub)
	ON_BN_CLICKED(IDC_BUTTON_KIADD, &CMFCSerialAssistantDlg::OnBnClickedButtonKiadd)
	ON_BN_CLICKED(IDC_BUTTON_KISUB, &CMFCSerialAssistantDlg::OnBnClickedButtonKisub)
	ON_BN_CLICKED(IDC_BUTTON_KDADD, &CMFCSerialAssistantDlg::OnBnClickedButtonKdadd)
	ON_BN_CLICKED(IDC_BUTTON_KDSUB, &CMFCSerialAssistantDlg::OnBnClickedButtonKdsub)
END_MESSAGE_MAP()


// CMFCSerialAssistantDlg 消息处理程序

BOOL CMFCSerialAssistantDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitComboBox();
	
	memset(Pid_Buffer, 0, sizeof(Pid_Buffer));

	m_Pic.SetBackColor(RGB(0, 0, 0));
	//m_Pic.ShowMouseCursor(1);
	m_Pic.SetBorderColor(RGB(240, 240, 240));
	m_Pic.GetLegend()->SetVisible(true);//设置图例显示
	//m_Pic.SetEdgeType(0);//去除边框
	m_Pic.SetPanEnabled(true);//启用平滑

	pAxis = NULL;
	pAxis = m_Pic.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis->SetMinMax(0, 500);
	pAxis->SetTextColor(RGB(200, 200, 200));

	pAxis = m_Pic.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(true);
	pAxis->SetTextColor(RGB(200, 200, 200));
	pAxis->SetMinMax(-6000, 6000);

	pLineSeries = m_Pic.CreateLineSerie();
	pLineSeries->SetColor(RGB(0,240,0));
	pLineSeries->SetSeriesOrdering(poXOrdering);//设置无序

	pLineSeries_clc = m_Pic.CreateLineSerie();
	pLineSeries_clc->SetColor(RGB(255, 128, 0));
	pLineSeries_clc->SetSeriesOrdering(poXOrdering);//设置无序

	Tim_value = 1;
	Tim_val.SetWindowTextW(_T("1"));

	for (int i = 7000; i > 0; i--) {
		Pid_Buffer[7000-i] = i;
	}

	CString str;

	start_flag = 0;

	kp = 70;
	ki = 0.35;
	kd = 23;
	error_sum = 0.0;
	Pid_Step = 0.001;
	str.Format(_T("%.4f"), kp);
	GetDlgItem(IDC_EDIT_KP)->SetWindowTextW(str);
	str.Format(_T("%.4f"), ki);
	GetDlgItem(IDC_EDIT_KI)->SetWindowTextW(str);
	str.Format(_T("%.4f"), kd);
	GetDlgItem(IDC_EDIT_Kd)->SetWindowTextW(str);
	str.Format(_T("%.4f"), Pid_Step);
	GetDlgItem(IDC_EDIT_STEP)->SetWindowTextW(str);

	max_cnt = 7990;
	Pid_cnt = 0;//调试用,发布注释

	str.Format(_T("%d"), max_cnt);
	GetDlgItem(IDC_EDIT_DATACNT)->SetWindowTextW(str);

	SetTimer(1, 500, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCSerialAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCSerialAssistantDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCSerialAssistantDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCSerialAssistantDlg::OnBnClickedBtnCommcontrol()
{
	// TODO: 在此添加控件通知处理程序代码
	static BOOL bIsOpen = FALSE;
	CButton* pBtnCommControl = (CButton*)GetDlgItem(IDC_BTN_COMMCONTROL); 
	ASSERT(pBtnCommControl);

	CComboBox* pComboBoxComm = (CComboBox*)GetDlgItem(IDC_COMBO_COMM);
	ASSERT(pComboBoxComm);
	int nSel = pComboBoxComm->GetCurSel();
	CString strComm;
	pComboBoxComm->GetLBText(nSel, strComm);

	CComboBox* pComboBoxBaudrate = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	ASSERT(pComboBoxBaudrate);
	nSel = pComboBoxBaudrate->GetCurSel();
	DWORD dwBaudrate = (DWORD)pComboBoxBaudrate->GetItemData(nSel);

	CComboBox* pComboBoxCheckbit = (CComboBox*)GetDlgItem(IDC_COMBO_CHECKBIT);
	ASSERT(pComboBoxCheckbit);
	nSel = pComboBoxCheckbit->GetCurSel();
	BYTE byParity = (BYTE)pComboBoxCheckbit->GetItemData(nSel);

	CComboBox* pComboBoxDatabit = (CComboBox*)GetDlgItem(IDC_COMBO_DATABIT);
	ASSERT(pComboBoxDatabit);
	nSel = pComboBoxDatabit->GetCurSel();
	BYTE byDataSize = (BYTE)pComboBoxDatabit->GetItemData(nSel);

	CComboBox* pComboBoxStopbit = (CComboBox*)GetDlgItem(IDC_COMBO_STOPBIT);
	ASSERT(pComboBoxStopbit);
	nSel = pComboBoxStopbit->GetCurSel();
	BYTE byStopBits = (BYTE)pComboBoxStopbit->GetItemData(nSel);

	if (!bIsOpen)
	{
		bIsOpen = gSerialPort.OpenComm(strComm);
		if (bIsOpen)
		{
			BOOL bRet = gSerialPort.SetCommState(dwBaudrate, byParity, byDataSize, byStopBits);
			if (!bRet)
			{
				gSerialPort.CloseComm();
				AfxMessageBox(_T("设置串口属性失败！"));
				return;
			}

			bRet = gSerialPort.SetupComm(1024, 1024);
			if (!bRet)
			{
				gSerialPort.CloseComm();
				AfxMessageBox(_T("设置串口输入输出缓冲区失败！"));
				return;
			}

			bRet = gSerialPort.PurgeComm(PURGE_RXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			//purge_rxabort | purge_rxabort | purge_txclear | purge_rxclear
			if (!bRet)
			{
				gSerialPort.CloseComm();
				AfxMessageBox(_T("无法清除串口的错误状态！"));
				return;
			}

			bRet = gSerialPort.SetCommMask(EV_RXCHAR);//ev_rxchar
			if (!bRet)
			{
				gSerialPort.CloseComm();
				AfxMessageBox(_T("设置串口时间出错！"));
				return;
			}

			gSerialPort.StartComm();

			bIsOpen = TRUE;
			pBtnCommControl->SetWindowText(_T("关闭串口"));
		}
		else
		{
			pBtnCommControl->SetWindowText(_T("打开串口"));
		}
	}
	else
	{
		gSerialPort.CloseComm();
		bIsOpen = FALSE;
		pBtnCommControl->SetWindowText(_T("打开串口"));
	}
}


void CMFCSerialAssistantDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL == gSerialPort.m_hComm)
	{
		AfxMessageBox(_T("请打开串口后发送数据！"));
		return;
	}
	CEdit* pEditSend = (CEdit*)GetDlgItem(IDC_EDIT_SEND);
	ASSERT(pEditSend);
	CEdit* pEditRecv = (CEdit*)GetDlgItem(IDC_EDIT_RECV);
	ASSERT(pEditRecv);

	CString strSend;
	CString strRecv;
	pEditSend->GetWindowText(strSend);
	strSend = strSend.Trim();//去掉空格等修饰
	if (strSend.IsEmpty())
	{
		return;
	}

	OVERLAPPED overlappedWrite;
	ZeroMemory(&overlappedWrite, sizeof(OVERLAPPED));
	overlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	int nLen = (strSend.GetLength() + 1) * sizeof(TCHAR);
	DWORD dwWrite = 0;
	gSerialPort.WriteFile((TCHAR*)strSend.GetBuffer(), nLen, &dwWrite, &overlappedWrite);

	//pEditSend->SetWindowText(_T(""));

	pEditRecv->GetWindowText(strRecv);
	strRecv = strRecv+ _T("发：")+strSend + _T("\r\n");
	pEditRecv->SetWindowText(strRecv);


	CloseHandle(overlappedWrite.hEvent);
	Sleep(100);
}


void CMFCSerialAssistantDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit* pEditRecv = (CEdit*)GetDlgItem(IDC_EDIT_RECV);
	ASSERT(pEditRecv);
	CString strRecv;
	strRecv = "";
	pEditRecv->SetWindowText(strRecv);
}



void CMFCSerialAssistantDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	CDialogEx::OnTimer(nIDEvent);
	int avg = 0;
	long sum = 0;

	if (nIDEvent == 1) {
		Tim_Cnt++;
		if (stop_flag == 0) {
			str.Format(_T("%d"), Pid_cnt);
			if (stop_flag == 2) {
				str.Format(_T("Max->%d"),Pid_cnt);
			}
			GetDlgItem(IDC_EDIT_RECV_CNT)->SetWindowTextW(str);
		}
		if (clc_cnt >= max_cnt || clc_cnt >= Pid_cnt) {
			clc_cnt = 0;
			start_flag = 0;
			Pid_cnt = 0;
			stop_flag = 0;
			SetTimer(1, 500, NULL);
			GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(_T("开始分析"));
		}
			if (start_flag == 1) {
				//Tim_Cnt = 0;
				clc_cnt++;
				//for (int i = clc_cnt; i < clc_cnt + 50; i++) {
				//	//sum += Pid_Buffer[i];
				//	sum += out;
				//}
				//avg = sum / 50;

				////m_Pic.GetLeftAxis()->SetMinMax(avg-300, avg + 300);

				//if (clc_cnt < 300) {
				//	m_Pic.GetBottomAxis()->SetMinMax(0, 300);
				//}
				//else {
				//	m_Pic.GetBottomAxis()->SetMinMax(clc_cnt - 150, clc_cnt+150);
				//}
				pLineSeries->AddPoint(clc_cnt, Pid_Buffer[clc_cnt]);

				error = Pid_Buffer[clc_cnt];
				error_sum += error;
				error_sum = error_sum > 6000 ? 6000 : error_sum;

				out = kp * error + ki * error_sum + kd * ((error-last_error) - (before_last - last_error));

				before_last = last_error;
				last_error = error;

				pLineSeries_clc->AddPoint(clc_cnt, out);
			}
			
	}
}


void CMFCSerialAssistantDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	Tim_val.GetWindowText(temp);
	Tim_value = _ttoi(temp);
	GetDlgItem(IDC_EDIT_DATACNT)->GetWindowText(temp);
	max_cnt = _ttoi(temp);

	m_Edit_Kp.GetWindowText(temp);
	kp = _ttof(temp);

	m_Edit_Ki.GetWindowText(temp);
	ki = _ttof(temp);

	m_Edit_Kp.GetWindowText(temp);
	kd = _ttof(temp);

	//sscanf_s(temp, "%d", &Tim_value);

	
	Tim_Cnt = 0;
	clc_cnt = 0;

	start_flag = 1 - start_flag;

	//int avg = 0;
	//long sum = 0;
	//int p = max_cnt > Pid_cnt ? Pid_cnt : max_cnt;
	//for (int i = 0; i < p; i++) {
	//	sum += Pid_Buffer[i];
	//	//sum += out;
	//}
	//avg = sum / 50;

	//m_Pic.GetLeftAxis()->SetMinMax(avg - 6000,avg + 6000);

	if (start_flag == 1) {
		if (Pid_cnt == 0) {
			MessageBox(_T("数据量为0！无法分析！"));
			return;
		}
		pLineSeries->ClearSerie();
		pLineSeries_clc->ClearSerie();
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(_T("结束分析"));
		SetTimer(1, Tim_value, NULL);
	}
	else {
		Pid_cnt = 0;
		stop_flag = 0;
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(_T("开始分析"));
		SetTimer(1, 500, NULL);
	}
}


void CMFCSerialAssistantDlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	pLineSeries->ClearSerie();
	pLineSeries_clc->ClearSerie();
}



void CMFCSerialAssistantDlg::OnBnClickedButtonKpadd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Kp.GetWindowText(temp);
	kp = _ttof(temp);
	
	kp += Pid_Step;
	temp.Format(_T("%.4f"), kp);
	GetDlgItem(IDC_EDIT_KP)->SetWindowText(temp);

}


void CMFCSerialAssistantDlg::OnBnClickedButtonKpsub()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Kp.GetWindowText(temp);
	kp = _ttof(temp);

	kp -= Pid_Step;
	temp.Format(_T("%.4f"), kp);
	GetDlgItem(IDC_EDIT_KP)->SetWindowText(temp);
}


void CMFCSerialAssistantDlg::OnBnClickedButtonKiadd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Ki.GetWindowText(temp);
	ki = _ttof(temp);

	ki += Pid_Step;
	temp.Format(_T("%.4f"), ki);
	GetDlgItem(IDC_EDIT_KI)->SetWindowText(temp);
}


void CMFCSerialAssistantDlg::OnBnClickedButtonKisub()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Ki.GetWindowText(temp);
	ki = _ttof(temp);

	ki -= Pid_Step;
	temp.Format(_T("%.4f"), ki);
	GetDlgItem(IDC_EDIT_KI)->SetWindowText(temp);
}


void CMFCSerialAssistantDlg::OnBnClickedButtonKdadd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Kd.GetWindowText(temp);
	kd = _ttof(temp);

	kd += Pid_Step;
	temp.Format(_T("%.4f"), kd);
	GetDlgItem(IDC_EDIT_Kd)->SetWindowText(temp);
}


void CMFCSerialAssistantDlg::OnBnClickedButtonKdsub()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	m_Edit_Step.GetWindowText(temp);
	Pid_Step = _ttof(temp);
	m_Edit_Kd.GetWindowText(temp);
	kd = _ttof(temp);

	kd -= Pid_Step;
	temp.Format(_T("%.4f"), kd);
	GetDlgItem(IDC_EDIT_Kd)->SetWindowText(temp);
}
