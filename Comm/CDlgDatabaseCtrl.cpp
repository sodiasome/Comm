// CDlgDatabaseCtrl.cpp: 实现文件
//

#include "pch.h"
#include "Comm.h"
#include "CDlgDatabaseCtrl.h"
#include "afxdialogex.h"
#include "ConfigEx.h"
#include <string>


// CDlgDatabaseCtrl 对话框

IMPLEMENT_DYNAMIC(CDlgDatabaseCtrl, CDialogEx)

CDlgDatabaseCtrl::CDlgDatabaseCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MYSQLDB, pParent)
{

}

CDlgDatabaseCtrl::~CDlgDatabaseCtrl()
{
}

void CDlgDatabaseCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDatabaseCtrl, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONN, &CDlgDatabaseCtrl::OnBnClickedBtnConn)
END_MESSAGE_MAP()


// CDlgDatabaseCtrl 消息处理程序


void CDlgDatabaseCtrl::OnBnClickedBtnConn()
{
	DbPara dbPara;
	CString strPort;
	GetDlgItemText(IDC_ED_ADRESS, dbPara.m_strAdress);
	GetDlgItemText(IDC_ED_PORT, strPort);
	GetDlgItemText(IDC_ED_USER, dbPara.m_strUser);
	GetDlgItemText(IDC_ED_PASSWD, dbPara.m_strPasswd);
	GetDlgItemText(IDC_ED_DBNAME, dbPara.m_strDbName);

	dbPara.m_nPort = _ttoi(strPort);
	dbPara.m_nDbType = DatabaseCtrlEx::Mysql;

	//保存
	g_config.WriteString(_T("Mysql"),_T("IpAdress"), dbPara.m_strAdress);
	g_config.WriteString(_T("Mysql"), _T("Port"), strPort);
	g_config.WriteString(_T("Mysql"), _T("User"), dbPara.m_strUser);
	g_config.WriteString(_T("Mysql"), _T("Passwd"), dbPara.m_strPasswd);
	g_config.WriteString(_T("Mysql"), _T("DbName"), dbPara.m_strDbName);

	DatabaseCtrlEx db(dbPara);
	db.ConnectDB();

	std::vector<std::vector<std::string>> qryResult;
	db.QueryMysql(_T("select * from db;"), qryResult);

	db.ExcuteMysql(_T("CREATE TABLE IF NOT EXISTS tb_ChatLog(\
		Message VARCHAR(128) NOT NULL,\
		Time VARCHAR(32) NOT NULL\
	)ENGINE = InnoDB DEFAULT CHARSET = utf8; "));
}


BOOL CDlgDatabaseCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strAdress;
	CString strPort;
	CString strUser;
	CString strPasswd;
	CString strDbName;

	strAdress = g_config.ReadString(_T("Mysql"), _T("IpAdress"), _T(""));
	strPort = g_config.ReadString(_T("Mysql"), _T("Port"), _T(""));
	strUser = g_config.ReadString(_T("Mysql"), _T("User"), _T(""));
	strPasswd = g_config.ReadString(_T("Mysql"), _T("Passwd"), _T(""));
	strDbName = g_config.ReadString(_T("Mysql"), _T("DbName"), _T(""));

	SetDlgItemText(IDC_ED_ADRESS, strAdress);
	SetDlgItemText(IDC_ED_PORT, strPort);
	SetDlgItemText(IDC_ED_USER, strUser);
	SetDlgItemText(IDC_ED_PASSWD, strPasswd);
	SetDlgItemText(IDC_ED_DBNAME, strDbName);

	return TRUE; 
}
