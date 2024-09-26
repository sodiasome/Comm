#pragma once
#include <afxdialogex.h>
#include "ConfigEx.h"


class CSocketWinCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CSocketWinCtrl)

public:
	CSocketWinCtrl(CWnd* pParent = nullptr);
	virtual ~CSocketWinCtrl();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SOCKETWIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	//控件
	CButton m_rdTcpType;
	CButton m_rdClient;
	CIPAddressCtrl m_ipctrl;
	CComboBox m_cbDataType;
	CComboBox m_cbAlgorithm;
	CComboBox m_cbHosts;	//接入主机队列
	//变量
	SocketCtrl m_socketCtrl;	//socket
	CString m_strRecvBuff;		//接收临时显示
	CString m_strSendBuff;		//临时发送显示

	//函数
	void InitUi();
	//消息
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdServer();
	afx_msg void OnBnClickedRdClient();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg LRESULT OnSocketRecv(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnBnClickedBtnSend();
};
