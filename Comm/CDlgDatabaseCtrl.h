#pragma once


// CDlgDatabaseCtrl 对话框

class CDlgDatabaseCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDatabaseCtrl)

public:
	CDlgDatabaseCtrl(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDatabaseCtrl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MYSQLDB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConn();
	virtual BOOL OnInitDialog();
};
