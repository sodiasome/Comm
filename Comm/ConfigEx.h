#pragma once
/*===============================================================
*	名    称：Config.h
*	作    者：huangdatian
*	邮    箱：thinkinheart@163.com
*	描    述：常用的配置参数和调试，MFC控件继承和常用的类接口封装
*	时    间：2023年1月3日 09:27:00  至 2023年2月12日 09:27:00
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
*	模块名：
*	描  述：
*			常用颜色
*================================================================*/
#define GREEN	RGB(0,255,0)		//绿色
#define RED		RGB(255,0,0)		//红色
#define BLUE	RGB(0,0,255)		//蓝色
#define GRAY	RGB(192,192,192)	//灰色
#define BLACK	RGB(0,0,0)			//黑色
#define WHITE	RGB(255,255,255)	//白色

/*===============================================================
*	模块名：CLogPrint
*	描  述：
*			日志输出到文件、cmd窗口(日志文件只保留1个月以内)
*================================================================*/
class CLogPrint
{
private:
	CString m_strFoder;//输出日志文件夹

	TCHAR *m_pBuff;
	int m_nCapacity;//当前使用量
	int m_nBuffSize;//内存容量
public:
	CRITICAL_SECTION m_cs;
	CString m_strPath;
public:
	CLogPrint();
	~CLogPrint();
	void GetExePath();//获取当前进程目录
	void SetLogFoder(CString strLogFoder);
	void OutPutLog(CString strLog);	//输出到debug窗口
	void WriteLog(CString strLog);	//输出到文件
	void Split(char splitChar,CString strText,CStringArray &csa);//分隔符分割数据
};
extern CLogPrint g_logPrint;
/*===============================================================
*	模块名：CIniFile
*	描  述：
*			日志输出到文件、cmd窗口(日志文件只保留1个月以内)
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
*	模块名：CDialogExt
*	描  述：
*			自定义对话框风格,需创建资源id IDD_DLG_WND
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
	POINT m_ptClient;					//客户端的宽高						
	CFont* m_pFont;						//控件变大后的字体
	void ResizeMember(CWnd *pWnd=NULL);	//控件随对话框大小变化
	void SetFontEx(CWnd *pWnd,int nWidth,int nHeight,CString strFontType);
	void SetBkPicture(CString strPicPath);				//设置背景图片

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
};

/*===============================================================
*	模块名：CStaticEx
*	描  述：
*			扩展颜色、字体
*================================================================*/
class CStaticEx : public CStatic
{
	DECLARE_DYNAMIC(CStaticEx)
public:
	CStaticEx();
	virtual ~CStaticEx();

private:
	COLORREF    m_ForeColor;        //文本颜色
	COLORREF    m_BackColor;        //背景色
	CBrush      m_BkBrush;          //背景刷
	CFont*      p_Font;             //字体

public:
	void SetForeColor(COLORREF color);					//设置文本颜色
	void SetBkColor(COLORREF color);					//设置背景颜色
	void SetTextFont(int FontHight,LPCTSTR FontName);   //设置字体

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

/*===============================================================
*	模块名：CButtonEx
*	描  述：
*			按钮按下、抬起添加位图图片
*================================================================*/
class CButtonEx :public CButton
{
private:
	HBITMAP m_bitmapDown;		//按下之后加载的图片
	HBITMAP m_bitmapUp;			//抬起之后加载的图片
public:
	CButtonEx();
	~CButtonEx();
	void SetBitmapEx(CString strPicDown,CString strPicUp);			//设置位图
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
/*===============================================================
*	模块名：CListBoxEx
*	描  述：
*			扩展拖动滑条
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
	void RefushHorizontalScrollBar( void );// 计算水平滚动条宽度
protected:
	DECLARE_MESSAGE_MAP()
};

/*===============================================================
*	模块名：CTreeCtrlEx
*	描  述：
*			树形控件
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
*	模块名：BasicExcelEx
*	描  述：
*			Excel表格基本读取、写入
*			只支持.xls格式的表格，如果是.xlsx格式的表格需要另存为.xls
*================================================================*/
class BasicExcelEx
{
public:
	CString m_strExcelPath;		//Excel文件路径
public:
	BasicExcelEx(CString strExcelPath);
	~BasicExcelEx();
	BOOL ReanExcelData();		//读取Excel表格数据
};
/*===============================================================
*	模块名：HttpCtrlEx
*	描  述：
*			http客户端Get、Post请求
*================================================================*/
class HttpCtrlEx  
{  
public:  
	HttpCtrlEx();
	~HttpCtrlEx();
	 CString GetData(CString strUrl);
};
/*===============================================================
*	模块名：DatabaseCtrlEx
*	描  述：
*			数据库增删查改
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
	BOOL ConnectDB();//连接数据库
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
*	模块名：ComCtrlEx
*	描  述：
*			串口收发数据
*================================================================*/
class ComCtrlEx
{
private:
	HANDLE m_hComm;										//串口句柄
public:
	CString m_strComPort;		//com口
	CString m_strBaudRate;		//波特率
	CString m_strByteSize;		//数据位
	CString m_strStopBits;		//停止位
	CString m_strParity;		//校验位
public:
	ComCtrlEx();
	~ComCtrlEx();
	BOOL SetTimeOut(COMMTIMEOUTS timeOut);				//设置串口超时
	BOOL SetCommDcb(DCB dcb);							//设置串口参数
	void ReadComm(CString &strReadData,DWORD dwLen );
	void WriteComm(CString &strWriteData,DWORD dwLen);
	void CloseComm();
	BOOL OpenComm();
};
/*===============================================================
*	模块名：SocketCtrlEx
*	描  述：
*			网口收发数据（支持并发）
*================================================================*/
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <functional>
#include <list>
#pragma comment(lib, "Ws2_32.lib")

#define WM_RECVSOCKDATA (WM_USER+100)	//socket消息

#define WM_SOCKETACCEPT (WM_USER+200)	//连接
#define WM_SOCKETRECV	(WM_USER+201)	//接收
#define WM_SOCKETCLOSE	(WM_USER+202)	//断开
#define WM_SOCKETLISTEN (WM_USER+203)	//监听

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
	SOCKET	m_sockServer;				//accept 调用
	CString m_strServerIp;				//server IP
	int		m_nServerPort;				//server port
	//std::vector<int> m_vClientFlag;	//当前连接的client 
	int		m_nSendFlag;				//最新发消息的client
	std::list<SOCKET> m_sockList;		//接入的client socket
	void InitServer(HWND hParentWnd, CString strIp, int nPort);
	static UINT Accept(LPVOID pParam);
	void toIp(char* buf, size_t size, const struct sockaddr_in& addr);

	//Client
	SOCKET	m_sockClient;				//客户端
	void InitClient(HWND hParentWnd,CString strServerIp, int nPort);

	//共用
	BOOL	m_bConnect;				//是否成功连接
	int		m_nType;					//TCP类型
	char	m_szRecvBuff[256];			//接收缓存
	void SendMsg(CString strMsg,size_t nSendFlag=0);
	void ClearRecvBuff();
	static UINT Loop(LPVOID pParam);
};
/*===============================================================
*	模块名：SocketCtrlUdpEx
*	描  述：
*			网口收发数据（支持并发）
*================================================================*/
class SocketCtrlUdpEx
{

};