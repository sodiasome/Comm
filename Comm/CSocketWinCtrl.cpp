#include "pch.h"
#include "Comm.h"
#include "afxdialogex.h"
#include "CSocketWinCtrl.h"
#include "ConfigEx.h"

IMPLEMENT_DYNAMIC(CSocketWinCtrl, CDialogEx)

CSocketWinCtrl::CSocketWinCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SOCKETWIN, pParent)
{

}

CSocketWinCtrl::~CSocketWinCtrl()
{
}

void CSocketWinCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RD_SERVER, m_rdTcpType);
	DDX_Control(pDX, IDC_RD_CLIENT, m_rdClient);
	DDX_Control(pDX, IDC_IP_TCP, m_ipctrl);
	DDX_Control(pDX, IDC_CB_DATATYPE, m_cbDataType);
	DDX_Control(pDX, IDC_CB_ALGORITHM, m_cbAlgorithm);
	DDX_Control(pDX, IDC_CB_HOSTS, m_cbHosts);
}


BEGIN_MESSAGE_MAP(CSocketWinCtrl, CDialogEx)
	ON_BN_CLICKED(IDC_RD_SERVER, &CSocketWinCtrl::OnBnClickedRdServer)
	ON_BN_CLICKED(IDC_RD_CLIENT, &CSocketWinCtrl::OnBnClickedRdClient)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CSocketWinCtrl::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CSocketWinCtrl::OnBnClickedBtnConnect)
	ON_MESSAGE(WM_RECVSOCKDATA, OnSocketRecv)
	ON_BN_CLICKED(IDC_BTN_SEND, &CSocketWinCtrl::OnBnClickedBtnSend)
END_MESSAGE_MAP()

void CSocketWinCtrl::OnBnClickedRdServer()
{
	UpdateData(FALSE);
	SetDlgItemText(IDC_BTN_CONNECT, _T("启动"));
	g_config.WriteString(_T("SocketSet"), _T("TcpType"), _T("Server"));

	//关闭控件
	GetDlgItem(IDC_CB_HOSTS)->EnableWindow(TRUE);
}
void CSocketWinCtrl::OnBnClickedRdClient()
{
	UpdateData(FALSE);
	SetDlgItemText(IDC_BTN_CONNECT, _T("连接"));
	g_config.WriteString(_T("SocketSet"), _T("TcpType"), _T("Client"));

	//关闭控件
	GetDlgItem(IDC_CB_HOSTS)->EnableWindow(FALSE);
}
void CSocketWinCtrl::OnBnClickedBtnApply()
{
	//ip
	BYTE byteA, byteB, byteC, byteD;
	m_ipctrl.GetAddress(byteA, byteB, byteC, byteD);
	CString strIpAdress;
	strIpAdress.Format(_T("%d.%d.%d.%d"), byteA, byteB, byteC, byteD);
	g_config.WriteString(_T("SocketSet"), _T("TcpAdress"), strIpAdress);

	//port
	CString strPort;
	GetDlgItemText(IDC_ED_PORT, strPort);
	g_config.WriteString(_T("SocketSet"), _T("TcpPort"), strPort);

	//传输数据类型
	int nDataType = m_cbDataType.GetCurSel();
	g_config.WriteInt(_T("SocketSet"), _T("DataType"), nDataType);

	//加密算法
	int nAlgorithm = m_cbAlgorithm.GetCurSel();
	g_config.WriteInt(_T("SocketSet"), _T("Algorithm"), nAlgorithm);
	//启用加密算法
	if (((CButton*)GetDlgItem(IDC_CK_ENCRYPT))->GetCheck())
		g_config.WriteBool(_T("SocketSet"), _T("EnableAlgorithm"), TRUE);
	else
		g_config.WriteBool(_T("SocketSet"), _T("EnableAlgorithm"), FALSE);
}


BOOL CSocketWinCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitUi();
	return TRUE;
}

void CSocketWinCtrl::InitUi()
{
	m_cbAlgorithm.AddString(_T("MD5"));
	m_cbAlgorithm.SetCurSel(0);

	//tcp类型
	CString strTcpType = g_config.ReadString(_T("SocketSet"), _T("TcpType"), _T("Server"));
	if (strTcpType.CompareNoCase(_T("Server")) == 0)
	{
		SetDlgItemText(IDC_BTN_CONNECT, _T("启动"));
		m_rdTcpType.SetCheck(TRUE);
		//关闭控件
		GetDlgItem(IDC_CB_HOSTS)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_BTN_CONNECT, _T("连接"));
		m_rdClient.SetCheck(TRUE);
		//关闭控件
		GetDlgItem(IDC_CB_HOSTS)->EnableWindow(FALSE);
	}	

	//ip
	CString strTcpAdress = g_config.ReadString(_T("SocketSet"), _T("TcpAdress"), _T("127.0.0.1"));
	m_ipctrl.SetWindowTextW(strTcpAdress);

	//port
	CString strPort = g_config.ReadString(_T("SocketSet"), _T("TcpPort"), _T("8888"));
	SetDlgItemText(IDC_ED_PORT, strPort);

	//datatype
	m_cbDataType.AddString(_T("HEX"));
	m_cbDataType.AddString(_T("文本"));
	m_cbDataType.AddString(_T("文件"));
	m_cbDataType.AddString(_T("视频"));
	m_cbDataType.SetCurSel(0);

	//传输数据类型
	 int nDataType = g_config.ReadInt(_T("SocketSet"), _T("DataType"), 0);
	 m_cbDataType.SetCurSel(nDataType);

	//加密算法
	int nAlgorithm = g_config.ReadInt(_T("SocketSet"), _T("Algorithm"), 0);
	m_cbAlgorithm.SetCurSel(nAlgorithm);
	//启用
	BOOL bAlrithm = g_config.ReadBool(_T("SocketSet"), _T("EnableAlgorithm"), TRUE);
	((CButton*)GetDlgItem(IDC_CK_ENCRYPT))->SetCheck(bAlrithm);
	
}


void CSocketWinCtrl::OnBnClickedBtnConnect()
{
	CString strTcpAdress = g_config.ReadString(_T("SocketSet"), _T("TcpAdress"), _T("127.0.0.1"));
	CString strPort = g_config.ReadString(_T("SocketSet"), _T("TcpPort"), _T("8888"));
	if (m_rdTcpType.GetCheck())
	{
		m_socketCtrl.InitServer(this->GetSafeHwnd(), strTcpAdress, _ttoi(strPort));
	}
	else if (m_rdClient.GetCheck())
	{
		m_socketCtrl.InitClient(this->GetSafeHwnd(), strTcpAdress, _ttoi(strPort));
		if (m_socketCtrl.m_bConnect)
		{
			SetDlgItemText(IDC_ST_STATUS, _T("连接服务器成功..."));
		}
		else
		{
			SetDlgItemText(IDC_ST_STATUS, _T("连接服务器失败！"));
		}
	}
}
LRESULT CSocketWinCtrl::OnSocketRecv(WPARAM wParam, LPARAM lParam)
{
	if (m_rdClient.GetCheck())
	{
		int nSignal = (int)wParam;
		if (nSignal == WM_SOCKETRECV)
		{
			CString strRecv((char*)lParam);
			SetDlgItemText(IDC_ED_RECV, strRecv);
			m_socketCtrl.ClearRecvBuff();
		}
		else if (nSignal == WM_SOCKETCLOSE)
		{
			SetDlgItemText(IDC_ED_RECV, _T("服务器端断开！"));
			m_socketCtrl.ClearRecvBuff();
		}
	}
	else if (m_rdTcpType.GetCheck())
	{
		int nSignal = (int)wParam;
		if (nSignal == WM_SOCKETACCEPT)
		{
			CString strRecv((char*)lParam);
			//m_log.OutPutLog(_T("客户端接入:") + strRecv);
			m_cbHosts.AddString(strRecv);
			m_cbHosts.SetCurSel(0);
			m_socketCtrl.SendMsg(_T("你已经接入服务器，你的id编码：")+strRecv);
		}
		else if (nSignal == WM_SOCKETRECV)
		{
			CString strRecv((char*)lParam);
			CString strMsg;
			strMsg.Format(_T("%d发来消息："), m_socketCtrl.m_nSendFlag);

			//m_log.OutPutLog(_T("客户端发来数据:") + strRecv);
			m_strRecvBuff += strMsg + strRecv;
			m_strRecvBuff += _T("\r\n");
			
			SetDlgItemText(IDC_ED_RECV, m_strRecvBuff);
			m_socketCtrl.ClearRecvBuff();

			m_socketCtrl.SendMsg(_T("你好，这是我的邮箱：sodiisome@163.com") );
		}
		else if (nSignal == WM_SOCKETCLOSE)
		{
			CString strRecv((char*)lParam);
			//m_log.OutPutLog(_T("客户端断开:") + strRecv);

			CString strSelText;
			for (int i = 0; i < m_cbHosts.GetCount(); ++i)
			{
				m_cbHosts.GetLBText(i, strSelText);
				if (strSelText == strRecv)
				{
					m_cbHosts.DeleteString(i);
				}
			}
			m_cbHosts.SetCurSel(0);
		}
		else if (nSignal == WM_SOCKETLISTEN)
		{
			SetDlgItemText(IDC_ST_STATUS, _T("服务器正在监听..."));
		}
	}
	return 1;
}


void CSocketWinCtrl::OnBnClickedBtnSend()
{
	CString strSendMsg;
	GetDlgItemText(IDC_ED_SEND, strSendMsg);
	CString strClientFlag;


	if (m_socketCtrl.m_nType == SocketCtrl::TCP_SERVER)
	{
		int nCurrSel = m_cbHosts.GetCurSel();
		m_cbHosts.GetLBText(nCurrSel, strClientFlag);
		m_socketCtrl.SendMsg(strSendMsg, _ttoi(strClientFlag));
	}	
	else if (m_socketCtrl.m_nType == SocketCtrl::TCP_CLIENT)
		m_socketCtrl.SendMsg(strSendMsg);

	//发送记录
	m_strSendBuff += strClientFlag + _T("发送：") + strSendMsg;
	m_strSendBuff += _T("\r\n");

	SetDlgItemText(IDC_ED_SENDLOG, m_strSendBuff);
}
