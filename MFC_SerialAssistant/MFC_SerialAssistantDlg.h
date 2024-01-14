
// MFC_SerialAssistantDlg.h: 头文件
//

#pragma once
#include "resource.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartBarSerie.h"
#include "Comm.h"
#include "MyEdit.h"

// CMFCSerialAssistantDlg 对话框
class CMFCSerialAssistantDlg : public CDialogEx
{
// 构造
public:
	CMFCSerialAssistantDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_SERIALASSISTANT_DIALOG };
#endif
	/*enum { IDD = IDD_MFC_SERIALASSISTANT_DIALOG };*/

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void CMFCSerialAssistantDlg::InitSerialPortBaud(int baudControlId);
	int CMFCSerialAssistantDlg::GetSerialPortIds(std::vector<int>& comIds);
	void CMFCSerialAssistantDlg::InitSerialPortControl(int comControlId, int baudControlId, CComm* pSerialPort);
	afx_msg void OnBnClickedBtnCommcontrol();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedButton1();

public:
	CComm m_SerialPort;

	CChartCtrl m_Pic;
	CChartAxis* pAxis;
	CChartLineSerie* pLineSeries;
	CChartLineSerie*  pLineSeries_clc;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonClear();

	CEdit Tim_val;
	CEdit m_Edit_Kp;
	CEdit m_Edit_Ki;
	CEdit m_Edit_Kd;
	CEdit m_Edit_Step;

	float Pid_Step;
	float kp;
	float ki;
	float kd;
	float error;
	float last_error;
	float before_last;
	float error_sum;
	float out;

	afx_msg void OnBnClickedButtonKpadd();
	afx_msg void OnBnClickedButtonKpsub();
	afx_msg void OnBnClickedButtonKiadd();
	afx_msg void OnBnClickedButtonKisub();
	afx_msg void OnBnClickedButtonKdadd();
	afx_msg void OnBnClickedButtonKdsub();

	bool RecvEditShowOn;

	CMyEdit m_JZEditRx;
	CMyEdit m_JZEditTx;
	afx_msg void OnBnClickedBtShowOnoff();

	CProgressCtrl m_Progress_Show_ONOFF;
	CProgressCtrl m_Progress_Com_ONOFF;
	CProgressCtrl m_Progress_Save_ONOFF;
	afx_msg void OnBnClickedBtSave();
	afx_msg void OnBnClickedBtSelectSavepath();

	CFile m_cJzFile;
	CString m_JzFilePath;
	bool m_JzFileSaveBT_Flag;
};
