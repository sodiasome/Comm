#pragma once


// CSerialCtrl 对话框

class CSerialCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(CSerialCtrl)

public:
	CSerialCtrl(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSerialCtrl();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SERIAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
