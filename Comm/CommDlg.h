
// CommDlg.h: 头文件
//

#pragma once
#include "ConfigEx.h"
#include "DlgWinCtrl.h"


// CCommDlg 对话框
class CCommDlg : public CDialogExt//CDialogEx
{
// 构造
public:
	CCommDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMM_DIALOG };
#endif

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
	afx_msg void OnBnClickedOk();
	void InitUi();
	CTreeCtrlEx m_treeSetting;			//左边树形目录
	HTREEITEM hSocketTcp;
	DlgWinCtrl m_DlgWinCtrl;			//右边客户区
	CMenu m_menu;						//菜单
	afx_msg void OnNMClickTreeSetting(NMHDR* pNMHDR, LRESULT* pResult);
	
	afx_msg void OnDataset();
};
