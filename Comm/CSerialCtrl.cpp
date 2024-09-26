// CSerialCtrl.cpp: 实现文件
//

#include "pch.h"
#include "Comm.h"
#include "CSerialCtrl.h"
#include "afxdialogex.h"


// CSerialCtrl 对话框

IMPLEMENT_DYNAMIC(CSerialCtrl, CDialogEx)

CSerialCtrl::CSerialCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SERIAL, pParent)
{

}

CSerialCtrl::~CSerialCtrl()
{
}

void CSerialCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSerialCtrl, CDialogEx)
END_MESSAGE_MAP()


// CSerialCtrl 消息处理程序
