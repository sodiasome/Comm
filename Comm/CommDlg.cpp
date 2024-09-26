#include "pch.h"
#include "framework.h"
#include "Comm.h"
#include "CommDlg.h"
#include "afxdialogex.h"
#include "CDlgDatabaseCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CCommDlg::CCommDlg(CWnd* pParent /*=nullptr*/)
	: CDialogExt(pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogExt::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SETTING, m_treeSetting);
}

BEGIN_MESSAGE_MAP(CCommDlg, CDialogExt)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCommDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SETTING, &CCommDlg::OnNMClickTreeSetting)
	ON_COMMAND(ID_DATASET, &CCommDlg::OnDataset)
END_MESSAGE_MAP()

BOOL CCommDlg::OnInitDialog()
{
	CDialogExt::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_menu.LoadMenuW(IDR_MENU1);
	SetMenu(&m_menu);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	InitUi();

	return TRUE;
}

void CCommDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialogExt::OnSysCommand(nID, lParam);
	}
}

void CCommDlg::OnPaint()
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
		CDialogExt::OnPaint();
	}
}

HCURSOR CCommDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommDlg::OnBnClickedOk()
{
	CDialogExt::OnOK();
}

void CCommDlg::InitUi()
{
	m_treeSetting.InitTree();
	HTREEITEM hRoot = m_treeSetting.InsertItem(_T("通讯设置"), 0, 0);
	HTREEITEM hRootSocket = m_treeSetting.InsertItem(_T("网口"), 0, 0);
	hSocketTcp = m_treeSetting.InsertItem(_T("TCP"), 1, 1, hRootSocket, TVI_LAST);
	m_treeSetting.InsertItem(_T("UDP"), 2, 2, hRootSocket, TVI_LAST);
	HTREEITEM hRootSerial = m_treeSetting.InsertItem(_T("串口"), 0, 0);
	m_treeSetting.InsertItem(_T("RS232"), 3, 3, hRootSerial, TVI_LAST);
	m_treeSetting.Expand(hRootSocket, TVE_EXPAND);
	m_treeSetting.Expand(hRootSerial, TVE_EXPAND);

	//默认页面
	CRect rectParent;
	CString strLog;
	GetDlgItem(IDC_PIC_DISPLAY)->GetWindowRect(&rectParent);
	//strLog.Format(_T("\nIDC_PIC_DISPLAY大小:%ld,%ld,%ld,%ld"), rectParent.left, rectParent.top, rectParent.right, rectParent.bottom);
	//::OutputDebugString(strLog);
	ScreenToClient(rectParent);
	m_DlgWinCtrl.CreateWin(DlgWinCtrl::SOCKET_TCP_WIN, IDD_DLG_SOCKETWIN, GetDlgItem(IDC_PIC_DISPLAY), &rectParent);
}


void CCommDlg::OnNMClickTreeSetting(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	//获取树形列表内容
	CTreeCtrl* ptreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREE_SETTING);
	CPoint ptTree;
	::GetCursorPos(&ptTree);
	ptreeCtrl->ScreenToClient(&ptTree);
	UINT uFlags;
	HTREEITEM hItem = ptreeCtrl->HitTest(ptTree,&uFlags);
	CString strItemText = ptreeCtrl->GetItemText(hItem);

	if (0 == strItemText.CompareNoCase(_T("TCP")))
	{
		//位置
		CRect rectParent;
		GetDlgItem(IDC_PIC_DISPLAY)->GetWindowRect(&rectParent);
		ScreenToClient(rectParent);
		//窗口
		m_DlgWinCtrl.CreateWin(DlgWinCtrl::SOCKET_TCP_WIN,IDD_DLG_SOCKETWIN, GetDlgItem(IDC_PIC_DISPLAY),&rectParent);
	}
	else if (0 == strItemText.CompareNoCase(_T("UDP")))
	{
		
	}
	else if(0 == strItemText.CompareNoCase(_T("RS232")))
	{
		//位置
		CRect rectParent;
		GetDlgItem(IDC_PIC_DISPLAY)->GetWindowRect(&rectParent);
		ScreenToClient(rectParent);
		//窗口
		m_DlgWinCtrl.CreateWin(DlgWinCtrl::SERIAL_WIN, IDD_DLG_SERIAL, GetDlgItem(IDC_PIC_DISPLAY), &rectParent);
	}

}

void CCommDlg::OnDataset()
{
	CDlgDatabaseCtrl dlg;
	dlg.DoModal();
}
