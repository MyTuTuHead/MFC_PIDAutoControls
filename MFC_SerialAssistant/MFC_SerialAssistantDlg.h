
// MFC_SerialAssistantDlg.h: 头文件
//

#pragma once
#include "resource.h"
#include "SerialPort.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartBarSerie.h"

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
	void InitComboBox();
	afx_msg void OnBnClickedBtnCommcontrol();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedButton1();

public:
	CChartCtrl m_Pic;
	CChartAxis* pAxis;
	CChartLineSerie* pLineSeries;
	CChartLineSerie*  pLineSeries_clc;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonClear();

	int Tim_value;
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

	int max_cnt;
	afx_msg void OnBnClickedButtonKpadd();
	afx_msg void OnBnClickedButtonKpsub();
	afx_msg void OnBnClickedButtonKiadd();
	afx_msg void OnBnClickedButtonKisub();
	afx_msg void OnBnClickedButtonKdadd();
	afx_msg void OnBnClickedButtonKdsub();

	int start_flag;
};
