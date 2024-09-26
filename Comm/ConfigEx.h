#pragma once
/*===============================================================
*	��    �ƣ�Config.h
*	��    �ߣ�huangdatian
*	��    �䣺thinkinheart@163.com
*	��    �������õ����ò����͵��ԣ�MFC�ؼ��̳кͳ��õ���ӿڷ�װ
*	ʱ    �䣺2023��1��3�� 09:27:00  �� 2023��2��12�� 09:27:00
*================================================================*/

#include <afxwin.h>
#include <afxext.h>
#include "Resource.h"
#include <winbase.h>//CRITICAL_SECTION
#include <vector>
#include <afxcmn.h>
#include "ConfigInc/include_mysql/mysql.h"
#include <string>


/*===============================================================
*	ģ������
*	��  ����
*			������ɫ
*================================================================*/
#define GREEN	RGB(0,255,0)		//��ɫ
#define RED		RGB(255,0,0)		//��ɫ
#define BLUE	RGB(0,0,255)		//��ɫ
#define GRAY	RGB(192,192,192)	//��ɫ
#define BLACK	RGB(0,0,0)			//��ɫ
#define WHITE	RGB(255,255,255)	//��ɫ

/*===============================================================
*	ģ������CLogPrint
*	��  ����
*			��־������ļ���cmd����(��־�ļ�ֻ����1��������)
*================================================================*/
class CLogPrint
{
private:
	CString m_strFoder;//�����־�ļ���

	TCHAR *m_pBuff;
	int m_nCapacity;//��ǰʹ����
	int m_nBuffSize;//�ڴ�����
public:
	CRITICAL_SECTION m_cs;
	CString m_strPath;
public:
	CLogPrint();
	~CLogPrint();
	void GetExePath();//��ȡ��ǰ����Ŀ¼
	void SetLogFoder(CString strLogFoder);
	void OutPutLog(CString strLog);	//�����debug����
	void WriteLog(CString strLog);	//������ļ�
	void Split(char splitChar,CString strText,CStringArray &csa);//�ָ����ָ�����
};
extern CLogPrint g_logPrint;
/*===============================================================
*	ģ������CIniFile
*	��  ����
*			��־������ļ���cmd����(��־�ļ�ֻ����1��������)
*================================================================*/
class CIniFile
{
private:
	CString m_strPath;
public:
	CIniFile()
	{
		g_logPrint.GetExePath();
		m_strPath = g_logPrint.m_strPath + _T("LogTest.ini");
	}
	~CIniFile(){}
	void SetPath(CString strPath) { m_strPath = strPath; }
	void WriteString(CString strSection,CString strItem,CString strValue);
	CString ReadString(CString strSection,CString strItem,CString strDefault);
	void WriteBool(CString strSection, CString strItem, BOOL bValue);
	BOOL ReadBool(CString strSection, CString strItem, BOOL bDefault);
	int ReadInt(CString strSection, CString strItem, int nDefault);
	void WriteInt(CString strSection, CString strItem, int nValue);
};
extern CIniFile g_config;
/*===============================================================
*	ģ������CDialogExt
*	��  ����
*			�Զ���Ի�����,�贴����Դid IDD_DLG_WND
*================================================================*/
class CDialogExt : public CDialog
{
public:
	CDialogExt(CWnd* pParent = NULL);
	~CDialogExt();

	enum {
		IDD = IDD_COMM_DIALOG
	};

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	POINT m_ptClient;					//�ͻ��˵Ŀ��						
	CFont* m_pFont;						//�ؼ����������
	void ResizeMember(CWnd *pWnd=NULL);	//�ؼ���Ի����С�仯
	void SetFontEx(CWnd *pWnd,int nWidth,int nHeight,CString strFontType);
	void SetBkPicture(CString strPicPath);				//���ñ���ͼƬ

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};

/*===============================================================
*	ģ������CStaticEx
*	��  ����
*			��չ��ɫ������
*================================================================*/
class CStaticEx : public CStatic
{
	DECLARE_DYNAMIC(CStaticEx)
public:
	CStaticEx();
	virtual ~CStaticEx();

private:
	COLORREF    m_ForeColor;        //�ı���ɫ
	COLORREF    m_BackColor;        //����ɫ
	CBrush      m_BkBrush;          //����ˢ
	CFont*      p_Font;             //����

public:
	void SetForeColor(COLORREF color);					//�����ı���ɫ
	void SetBkColor(COLORREF color);					//���ñ�����ɫ
	void SetTextFont(int FontHight,LPCTSTR FontName);   //��������

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

/*===============================================================
*	ģ������CButtonEx
*	��  ����
*			��ť���¡�̧�����λͼͼƬ
*================================================================*/
class CButtonEx :public CButton
{
private:
	HBITMAP m_bitmapDown;		//����֮����ص�ͼƬ
	HBITMAP m_bitmapUp;			//̧��֮����ص�ͼƬ
public:
	CButtonEx();
	~CButtonEx();
	void SetBitmapEx(CString strPicDown,CString strPicUp);			//����λͼ
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
/*===============================================================
*	ģ������CListBoxEx
*	��  ����
*			��չ�϶�����
*================================================================*/
class CListBoxEx : public CListBox
{
	DECLARE_DYNAMIC(CListBoxEx)

public:
	CListBoxEx();
	virtual ~CListBoxEx();
public:
	int AddString( LPCTSTR lpszItem );
	int InsertString( int nIndex, LPCTSTR lpszItem );
	void RefushHorizontalScrollBar( void );// ����ˮƽ���������
protected:
	DECLARE_MESSAGE_MAP()
};

/*===============================================================
*	ģ������CTreeCtrlEx
*	��  ����
*			���οؼ�
*================================================================*/

class CTreeCtrlEx : public CTreeCtrl
{
public:
	CImageList m_imageList;

	//HTREEITEM
	HICON m_icon[4];
	void InitTree();

	CTreeCtrlEx();
	~CTreeCtrlEx();
};



/*===============================================================
*	ģ������BasicExcelEx
*	��  ����
*			Excel��������ȡ��д��
*			ֻ֧��.xls��ʽ�ı�������.xlsx��ʽ�ı����Ҫ���Ϊ.xls
*================================================================*/
class BasicExcelEx
{
public:
	CString m_strExcelPath;		//Excel�ļ�·��
public:
	BasicExcelEx(CString strExcelPath);
	~BasicExcelEx();
	BOOL ReanExcelData();		//��ȡExcel�������
};
/*===============================================================
*	ģ������HttpCtrlEx
*	��  ����
*			http�ͻ���Get��Post����
*================================================================*/
class HttpCtrlEx  
{  
public:  
	HttpCtrlEx();
	~HttpCtrlEx();
	 CString GetData(CString strUrl);
};
/*===============================================================
*	ģ������DatabaseCtrlEx
*	��  ����
*			���ݿ���ɾ���
*================================================================*/
struct DbPara
{
	CString m_strAdress;
	int		m_nPort;
	CString m_strUser;
	CString m_strPasswd;
	CString m_strDbName;
	int		m_nDbType;
};
class DatabaseCtrlEx
{
public:
	enum{
		driverODBC = 0,
		driverSQLServer, 
		driverACCESS97,
		driverACCESS2000,
		driverACCESSXP,
		driverEND,
		Mysql,
		MogoDB,
		Redis
	};
	MYSQL m_mysql;
	DbPara m_dbPara;

public:
	void InitDatabase();
	BOOL ConnectDB();//�������ݿ�
	void DisConnDb();
	BOOL QueryMysql(CString strSql, std::vector<std::vector<std::string>>& qryResult);
	BOOL ExcuteMysql(CString strSql);
	DatabaseCtrlEx(DbPara dbPara);
	~DatabaseCtrlEx();

protected:
	BOOL ConnMysql();
	BOOL ConnSQL();
};
/*===============================================================
*	ģ������ComCtrlEx
*	��  ����
*			�����շ�����
*================================================================*/
class ComCtrlEx
{
private:
	HANDLE m_hComm;										//���ھ��
public:
	CString m_strComPort;		//com��
	CString m_strBaudRate;		//������
	CString m_strByteSize;		//����λ
	CString m_strStopBits;		//ֹͣλ
	CString m_strParity;		//У��λ
public:
	ComCtrlEx();
	~ComCtrlEx();
	BOOL SetTimeOut(COMMTIMEOUTS timeOut);				//���ô��ڳ�ʱ
	BOOL SetCommDcb(DCB dcb);							//���ô��ڲ���
	void ReadComm(CString &strReadData,DWORD dwLen );
	void WriteComm(CString &strWriteData,DWORD dwLen);
	void CloseComm();
	BOOL OpenComm();
};
/*===============================================================
*	ģ������SocketCtrlEx
*	��  ����
*			�����շ����ݣ�֧�ֲ�����
*================================================================*/
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <functional>
#include <list>
#pragma comment(lib, "Ws2_32.lib")

#define WM_RECVSOCKDATA (WM_USER+100)	//socket��Ϣ

#define WM_SOCKETACCEPT (WM_USER+200)	//����
#define WM_SOCKETRECV	(WM_USER+201)	//����
#define WM_SOCKETCLOSE	(WM_USER+202)	//�Ͽ�
#define WM_SOCKETLISTEN (WM_USER+203)	//����

//typedef std::tr1::function<void()> EventCallback;

extern CLogPrint g_log;

class SocketCtrl
{
public:
	enum {
		TCP_CLIENT=0, 
		TCP_SERVER
	};
	SocketCtrl();
	~SocketCtrl();
	//Server
	SOCKET	m_sockServer;				//accept ����
	CString m_strServerIp;				//server IP
	int		m_nServerPort;				//server port
	//std::vector<int> m_vClientFlag;	//��ǰ���ӵ�client 
	int		m_nSendFlag;				//���·���Ϣ��client
	std::list<SOCKET> m_sockList;		//�����client socket
	void InitServer(HWND hParentWnd, CString strIp, int nPort);
	static UINT Accept(LPVOID pParam);
	void toIp(char* buf, size_t size, const struct sockaddr_in& addr);

	//Client
	SOCKET	m_sockClient;				//�ͻ���
	void InitClient(HWND hParentWnd,CString strServerIp, int nPort);

	//����
	BOOL	m_bConnect;				//�Ƿ�ɹ�����
	int		m_nType;					//TCP����
	char	m_szRecvBuff[256];			//���ջ���
	void SendMsg(CString strMsg,size_t nSendFlag=0);
	void ClearRecvBuff();
	static UINT Loop(LPVOID pParam);
};
/*===============================================================
*	ģ������SocketCtrlUdpEx
*	��  ����
*			�����շ����ݣ�֧�ֲ�����
*================================================================*/
class SocketCtrlUdpEx
{

};