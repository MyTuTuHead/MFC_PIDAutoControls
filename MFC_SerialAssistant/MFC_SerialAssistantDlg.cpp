
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

	DDX_Control(pDX, IDC_EDIT_RECV, m_JZEditRx);
	DDX_Control(pDX, IDC_EDIT_SEND, m_JZEditTx);

	DDX_Control(pDX, IDC_PROGRESS_SHOW_ONOFF, m_Progress_Show_ONOFF);
	DDX_Control(pDX, IDC_PROGRESS_COM_ONOFF, m_Progress_Com_ONOFF);
	DDX_Control(pDX, IDC_PROGRESS_SAVE_ONOFF, m_Progress_Save_ONOFF);
}

int CMFCSerialAssistantDlg::GetSerialPortIds(std::vector<int>& comIds)
{
	comIds.clear();

	HKEY hComKey;

	TCHAR keyName[MAX_PATH + 1];
	BYTE keyVal[1024];


	DWORD keySize = sizeof(keyName) / sizeof(TCHAR);
	DWORD type = REG_SZ;
	DWORD valLen = sizeof(keyVal);

	memset(keyName, 0, sizeof(keyName));
	memset(keyVal, 0, sizeof(keyVal));

	::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hComKey);

	DWORD index = 0;

	while (::RegEnumValue(hComKey, index, keyName, &keySize, 0, &type, keyVal, &valLen) != ERROR_NO_MORE_ITEMS)
	{
		int comId = 0;
		sscanf_s((const char*)&keyVal[3], "%d", &comId);
		comIds.push_back(comId);

		keySize = sizeof(keyName);
		type = REG_SZ;
		valLen = sizeof(keyVal);
		memset(keyName, 0, sizeof(keyName));
		memset(keyVal, 0, sizeof(keyVal));

		index++;
		if (index >= 256)
		{
			break;
		}
	}
	::RegCloseKey(hComKey);

	std::sort(comIds.begin(), comIds.end(), std::greater<int>());

	return (int)comIds.size();
}

void CMFCSerialAssistantDlg::InitSerialPortBaud(int baudControlId)
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(baudControlId);
	CString cs;

	int bauds[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800,614400, 921600 };
	for (int i = 0; i < sizeof(bauds) / sizeof(int); ++i)
	{
		cs.Format(_T("%d"), bauds[i]);
		pComboBox->InsertString(i, cs);
	}
	pComboBox->SetCurSel(7);//初始化波特率
}

void CMFCSerialAssistantDlg::InitSerialPortControl(int comControlId, int baudControlId, CComm* pSerialPort)
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(comControlId);
	static std::vector<int> comIds;
	if (comIds.size() == 0)
	{
		GetSerialPortIds(comIds);
	}
	CString cs;
	for (size_t i = 0; i < comIds.size(); i++)
	{
		cs.Format(_T("COM%d"), comIds[i]);
		pComboBox->InsertString(0, cs);//初始化串口号			
	}
	pComboBox->SetCurSel(0);

	pComboBox = (CComboBox*)GetDlgItem(baudControlId);
	int bauds[] = { 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800,614400, 921600 };
	for (int i = 0; i < sizeof(bauds) / sizeof(int); ++i)
	{
		cs.Format(_T("%d"), bauds[i]);
		pComboBox->InsertString(i, cs);
	}
	pComboBox->SetCurSel(9);//初始化波特率

	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CHECKBIT);
	pComboBox->SetCurSel(0);
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DATABIT);
	pComboBox->SetCurSel(2);
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_STOPBIT);
	pComboBox->SetCurSel(0);

	//pSerialPort->SetWnd(this->m_hWnd);//设置串口消息处理窗口
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
	ON_BN_CLICKED(IDC_BT_SHOW_ONOFF, &CMFCSerialAssistantDlg::OnBnClickedBtShowOnoff)
	ON_BN_CLICKED(IDC_BT_SAVE, &CMFCSerialAssistantDlg::OnBnClickedBtSave)
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
	//InitSerialPortBaud(IDC_COMBO_BAUDRATE);
	InitSerialPortControl(IDC_COMBO_COMM,IDC_COMBO_BAUDRATE,&m_SerialPort);


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

	Tim_val.SetWindowTextA("10");

	CString str;

	kp = 70;
	ki = 0.35;
	kd = 23;
	error_sum = 0.0;
	Pid_Step = 0.001;
	str.Format(_T("%.4f"), kp);
	GetDlgItem(IDC_EDIT_KP)->SetWindowTextA(str);
	str.Format(_T("%.4f"), ki);
	GetDlgItem(IDC_EDIT_KI)->SetWindowTextA(str);
	str.Format(_T("%.4f"), kd);
	GetDlgItem(IDC_EDIT_Kd)->SetWindowTextA(str);
	str.Format(_T("%.4f"), Pid_Step);
	GetDlgItem(IDC_EDIT_STEP)->SetWindowTextA(str);

	RecvEditShowOn = 1;//1 开启显示

	m_Progress_Show_ONOFF.SetRange(0,1);
	m_Progress_Show_ONOFF.SetStep(1);
	m_Progress_Show_ONOFF.SetBarColor((COLORREF)0x0000FF00);
	m_Progress_Show_ONOFF.SetPos(RecvEditShowOn);

	m_Progress_Com_ONOFF.SetRange(0, 1);
	m_Progress_Com_ONOFF.SetStep(1);
	m_Progress_Com_ONOFF.SetBarColor((COLORREF)0x000000FF);
	m_Progress_Com_ONOFF.SetPos(2);

	m_Progress_Save_ONOFF.SetRange(0, 1);
	m_Progress_Save_ONOFF.SetStep(1);
	m_Progress_Save_ONOFF.SetBarColor((COLORREF)0x000000FF);
	m_Progress_Save_ONOFF.SetPos(1);

	GetDlgItem(IDC_BTN_SEND)->EnableWindow(false);
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
	int commId = 1;
	int baud = 115200;
	int databit = 8;
	int parity = 0;
	int stopbits = 0;
	CString str;

	GetDlgItem(IDC_COMBO_COMM)->GetWindowText(str);
	sscanf_s(str,"COM%d", &commId);

	GetDlgItem(IDC_COMBO_BAUDRATE)->GetWindowText(str);
	sscanf_s(str, "%d", &baud);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_CHECKBIT);
	parity = pComboBox->GetCurSel();

	GetDlgItem(IDC_COMBO_DATABIT)->GetWindowText(str);
	sscanf_s(str, "%d Bit", &databit);

	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_STOPBIT);
	stopbits = pComboBox->GetCurSel();

	if (m_SerialPort.IsOpened())
	{
		m_SerialPort.Destroy();
		GetDlgItem(IDC_BTN_SEND)->EnableWindow(false);
		GetDlgItem(IDC_BTN_COMMCONTROL)->SetWindowText(_T("打开串口"));
		m_Progress_Com_ONOFF.SetBarColor((COLORREF)0x000000FF);
	}
	else
	{
		if (m_SerialPort.Create(commId, baud, databit, parity, stopbits))
		{
			GetDlgItem(IDC_BTN_SEND)->EnableWindow(true);
			GetDlgItem(IDC_BTN_COMMCONTROL)->SetWindowText(_T("关闭串口"));
			m_Progress_Com_ONOFF.SetBarColor((COLORREF)0x0000FF00);
			::Sleep(200);
		}
		else
		{
			MessageBox(_T("串口打开错误！"));
			m_Progress_Com_ONOFF.SetBarColor((COLORREF)0x000000FF);
		}
	}
}


void CMFCSerialAssistantDlg::OnBnClickedBtnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_SerialPort.IsOpened()) {
		return;
	}

	char* pch = NULL;
	int len = m_JZEditTx.GetData(&pch);

	if (pch)
	{
		for (int i = 0; i < len; i += 100)
			m_SerialPort.SendData(pch + i, (i + 100 <= len) ? 100 : (len - i));
		delete[]pch;
	}
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

	int len = 0;
	char data[1024];
	memset(data, 0, sizeof(1024));

	if (nIDEvent == 1) {
		if (m_SerialPort.IsOpened())
		{
			len = m_SerialPort.m_dataBuff.GetQueLen();
			if (len > 1024)
			{
				len = 1024;
			}
			m_SerialPort.m_dataBuff.OutQueue(data, len);

			if (len && RecvEditShowOn) {
				m_JZEditRx.AppendData(data, len);
			}

			if (len) {
	/*			if (.m_hFile != INVALID_HANDLE_VALUE)
					.Write(data, len);*/
			}
		}
	/*	pLineSeries->AddPoint(clc_cnt, Pid_Buffer[clc_cnt]);

		error = Pid_Buffer[clc_cnt];
		error_sum += error;
		error_sum = error_sum > 6000 ? 6000 : error_sum;

		out = kp * error + ki * error_sum + kd * ((error-last_error) - (before_last - last_error));

		before_last = last_error;
		last_error = error;

		pLineSeries_clc->AddPoint(clc_cnt, out);*/
			
	}
}


void CMFCSerialAssistantDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;

	m_Edit_Kp.GetWindowText(temp);
	kp = _ttof(temp);

	m_Edit_Ki.GetWindowText(temp);
	ki = _ttof(temp);

	m_Edit_Kp.GetWindowText(temp);
	kd = _ttof(temp);

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


void CMFCSerialAssistantDlg::OnBnClickedBtShowOnoff()
{
	// TODO: 在此添加控件通知处理程序代码

	RecvEditShowOn = 1 - RecvEditShowOn;

	if (RecvEditShowOn == 1)
	{
		GetDlgItem(IDC_BT_SHOW_ONOFF)->SetWindowText(_T("停止显示"));
		m_Progress_Show_ONOFF.SetBarColor((COLORREF)0x0000FF00);
		//m_Progress_Show_ONOFF.SetPos(RecvEditShowOn);
	}
	else 
	{
		GetDlgItem(IDC_BT_SHOW_ONOFF)->SetWindowText(_T("继续显示"));
		m_Progress_Show_ONOFF.SetBarColor((COLORREF)0x000000FF);
		//m_Progress_Show_ONOFF.SetPos(RecvEditShowOn);
	}
}


void CMFCSerialAssistantDlg::OnBnClickedBtSave()
{
	// TODO: 在此添加控件通知处理程序代码

	m_Progress_Save_ONOFF.SetBarColor((COLORREF)0x000000FF);
}
