#include "ConfigEx.h"
#include <afxtempl.h> //CMap

#include "ConfigInc/BasicExcel.hpp"
#include "ConfigInc/HttpClient.h"

using namespace YExcel;

/*===============================================================
*	模块名：CLogPrint
*	描  述：
*			日志输出到文件、cmd窗口(日志文件只保留1个月以内)
*================================================================*/
CLogPrint::CLogPrint()
{
	::InitializeCriticalSection(&m_cs);

	//获取当前exe目录
	m_nCapacity = 0;
	m_nBuffSize = 256;
	m_pBuff = new TCHAR[m_nBuffSize];
	::memset(m_pBuff, 0, sizeof(TCHAR)*m_nBuffSize);
	GetExePath();

	//
	m_strFoder = _T("RunLog");
	
}

CLogPrint::~CLogPrint()
{
	if (m_pBuff != NULL)
	{
		delete[] m_pBuff;
		m_pBuff = NULL;
	}
}

void CLogPrint::OutPutLog( CString strLog )
{
	SYSTEMTIME time;
	::GetLocalTime(&time);

	CString strTimeLog;
	strTimeLog.Format(_T("%4d-%02d-%02d_%02d:%02d:%02d.%03d "), 
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strTimeLog += strLog;
	strTimeLog += _T("\n");

	::OutputDebugStringW(strTimeLog);
}

void CLogPrint::WriteLog( CString strLog )
{
	SYSTEMTIME time;
	::GetLocalTime(&time);

	//拼接文件路径
	m_strPath += m_strFoder;

	//文件名
	CString strLogFile;
	strLogFile.Format(_T("%4d-%02d-%02d.txt"), time.wYear, time.wMonth, time.wDay);

	//日志写入到文件
	CString strTimeLog;
	strTimeLog.Format(_T("%4d-%02d-%02d_%02d:%02d:%02d.%03d "), 
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strTimeLog += strLog;
	strTimeLog += _T("\n");

	::EnterCriticalSection(&m_cs);
	//创建目录
	::CreateDirectory(m_strPath, NULL);
	//写入
	CStdioFile file;
	char *oldLocale = NULL;
	if (file.Open(m_strPath+_T("//")+strLogFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
	{
		oldLocale = _strdup(setlocale(LC_CTYPE,NULL));
		setlocale(LC_CTYPE,"chs");

		file.SeekToEnd();		
		file.WriteString(strTimeLog);

		setlocale(LC_CTYPE,oldLocale);
		free(oldLocale);

		file.Close();
	}
	::LeaveCriticalSection(&m_cs);

	//写入前定时删除日志文件
	CFileFind finder;
	CString strFileName;
	BOOL bWorking = finder.FindFile(m_strPath+_T("\\*"));
	while (bWorking) 
	{ 
		bWorking = finder.FindNextFile();
		if(finder.IsDots())
			continue;
		if (finder.IsDirectory())
			continue;

		strFileName = finder.GetFileName();
		if (strLogFile.CompareNoCase(strFileName) == 0)//相同文件无需判断
			continue;
		//获取文件类型
		int nIndex = strFileName.ReverseFind('.');
		CString strFileExt = strFileName.Right(strFileName.GetLength() - nIndex);
		if(strFileExt.CompareNoCase(_T(".txt")) == 0)
		{
			//分割月份出来判断是否是同一个月份
			CStringArray csaPre;
			this->Split(_T('-'),strFileName,csaPre);
			if (csaPre.GetCount() > 0)
			{
				CStringArray csaCurr;
				this->Split(_T('-'),strLogFile,csaCurr);
				if (csaCurr.GetCount() > 0)
				{
					if (csaCurr[1].CompareNoCase(csaPre[1]) != 0)
					{
						//不同月份删除日志文件
						::DeleteFile(m_strPath+_T("\\")+strFileName);
					}
				}
			}
		}
	}
}

void CLogPrint::SetLogFoder( CString strLogFoder )
{
	m_strFoder = strLogFoder;
}

void CLogPrint::GetExePath()
{
	//内存扩容
	if (m_nCapacity >= m_nBuffSize)
	{
		m_nBuffSize +=  m_nBuffSize << 1;
		TCHAR *pNewBuff = new TCHAR[m_nBuffSize];
		::memset(pNewBuff, 0, sizeof(TCHAR)*m_nBuffSize);

		memcpy(pNewBuff,m_pBuff,m_nBuffSize);
		delete[] m_pBuff;
		m_pBuff = NULL;
		m_pBuff = pNewBuff;
	}
	
	::GetModuleFileName(NULL,m_pBuff,m_nBuffSize);
	m_strPath.Format(_T("%s"),m_pBuff);
	int nIndex = m_strPath.ReverseFind(_T('\\'));
	m_strPath = m_strPath.Left(nIndex);
	m_strPath += _T("\\");
}

void CLogPrint::Split(char splitChar,CString strText,CStringArray &csa)
{
	int iIndex = strText.Find(splitChar);
	if (iIndex == -1)
	{
		//不存在该分割符号
		csa.RemoveAll();
		return;
	}
	strText = strText + splitChar;
	CString strTmp="";	
	for(int i=0;i<strText.GetLength(); ++i)
	{		
		if (strText.GetAt(i) == splitChar)
		{
			csa.Add(strTmp);
			strTmp = "";			
		}
		else
		{
			strTmp += strText.GetAt(i);			
		}
	}
}
CLogPrint g_logPrint;
/*===============================================================
*	模块名：CIniFile
*	描  述：
*			保存配置属性
*================================================================*/
void CIniFile::WriteString( CString strSection,CString strItem,CString strValue )
{
	::WritePrivateProfileString(strSection,strItem,strValue, m_strPath);
}

CString CIniFile::ReadString( CString strSection,CString strItem,CString strDefault )
{
	TCHAR szBuff[256] = {0};
	::GetPrivateProfileString(strSection,strItem,strDefault,szBuff,256,m_strPath);
	CString strResult(szBuff);
	return strResult;
}
void CIniFile::WriteBool(CString strSection, CString strItem, BOOL bValue)
{
	if (bValue)
		::WritePrivateProfileString(strSection, strItem, _T("true"), m_strPath);
	else
		::WritePrivateProfileString(strSection, strItem, _T("false"), m_strPath);
}

BOOL CIniFile::ReadBool(CString strSection, CString strItem, BOOL bDefault)
{
	CString strDefault;
	if (bDefault)
		strDefault = _T("true");
	else
		strDefault = _T("false");

	TCHAR szBuff[256] = { 0 };
	::GetPrivateProfileString(strSection, strItem, strDefault, szBuff, 256, m_strPath);
	CString strResult(szBuff);
	if (strResult.CompareNoCase(_T("true")) == 0)
		return TRUE;
	else
		return FALSE;
		
}

int CIniFile::ReadInt(CString strSection, CString strItem, int nDefault)
{
	CString strDefault;
	strDefault.Format(_T("%d"), nDefault);

	TCHAR szBuff[256] = { 0 };
	::GetPrivateProfileString(strSection, strItem, strDefault, szBuff, 256, m_strPath);
	CString strResult(szBuff);
	return _ttoi(strResult);
}

void CIniFile::WriteInt(CString strSection, CString strItem, int nValue)
{
	CString strDefault;
	strDefault.Format(_T("%d"), nValue);
	::WritePrivateProfileString(strSection, strItem, strDefault, m_strPath);
}

CIniFile g_config;
/*===============================================================
*	模块名：CDialogExt
*	描  述：
*			自定义对话框风格
*================================================================*/
CDialogExt::CDialogExt(CWnd* pParent  /*= NULL*/)
: CDialog(CDialogExt::IDD, pParent)
{

}
CDialogExt::~CDialogExt()
{
}
BEGIN_MESSAGE_MAP(CDialogExt, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDialogExt::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogExt::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_COPYDATA()
END_MESSAGE_MAP()

void CDialogExt::OnPaint()
{
	SetBkPicture(_T(".\\ConfigSrc\\background.bmp"));

	CDialog::OnPaint();
}


void CDialogExt::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pWnd;
	pWnd = GetDlgItem(IDCANCEL);
	if (pWnd && nType!=1)
	{
		ResizeMember(pWnd);
	}
}

void CDialogExt::ResizeMember( CWnd *pWnd/*=NULL*/ )
{
	//记录上一个客户端的大小
	CString strTmp;
	strTmp.Format(_T("宽:%.1d, 高:%.1d"),m_ptClient.x,m_ptClient.y);
	//SetDlgItemText(IDC_ED_CURR,strTmp);

	//获取当前变化后的客户端大小
	CRect rtTmp;
	GetClientRect(rtTmp);

	//计算当前客户端大小
	POINT ptNew;
	ptNew.x = rtTmp.right - rtTmp.left;
	ptNew.y = rtTmp.bottom - rtTmp.top;
	strTmp.Format(_T("宽:%.1d, 高:%.1d"),ptNew.x,ptNew.y);
	//SetDlgItemText(IDC_ED_CHANGE,strTmp);

	//判断窗口是变大还是变小
	static BOOL bToMax = TRUE;	//窗口从小到大变化
	if (ptNew.x > m_ptClient.x || ptNew.y > m_ptClient.y)
		bToMax = TRUE;
	else
		bToMax = FALSE;

	//计算缩放比例
	double dZoomX,dZoomY;	//缩放比例
	//最大化最小化缩放比例
	if (bToMax)
	{
		dZoomX = (double)ptNew.x / m_ptClient.x;
		dZoomY = (double)ptNew.y / m_ptClient.y;
	}
	else
	{
		dZoomX = (double)m_ptClient.x / ptNew.x;
		dZoomY = (double)m_ptClient.y / ptNew.y;
	}


	//子控件按这个比例计算
	strTmp.Format(_T("X=%.1f, Y=%.1f"),dZoomX,dZoomY);
	//SetDlgItemText(IDC_ED_ZOOM,strTmp);

	//缩放子控件
	CRect rectChild;
	int	nChildId;
	POINT ptTLPoint;//控件左上角
	POINT ptBRPoint;//控件右下角
	HWND hWndChild = ::GetWindow(m_hWnd,GW_CHILD);
	while(hWndChild)
	{
		nChildId=::GetDlgCtrlID(hWndChild);//取得ID
		CWnd* pCtrlWnd = GetDlgItem(nChildId);
		pCtrlWnd->GetWindowRect(rectChild); 

		ScreenToClient(rectChild); 
		ptTLPoint = rectChild.TopLeft();
		ptBRPoint = rectChild.BottomRight();

		//最大化最小化缩放
		if (bToMax)
		{
			ptTLPoint.x = long(ptTLPoint.x * dZoomX);   
			ptTLPoint.y = long(ptTLPoint.y * dZoomY);  
			ptBRPoint.x = long(ptBRPoint.x * dZoomX);   
			ptBRPoint.y = long(ptBRPoint.y * dZoomY);  
		}
		else
		{
			ptTLPoint.x = long(ptTLPoint.x / dZoomX);   
			ptTLPoint.y = long(ptTLPoint.y / dZoomY);  
			ptBRPoint.x = long(ptBRPoint.x / dZoomX);   
			ptBRPoint.y = long(ptBRPoint.y / dZoomY);  
		}
		rectChild.SetRect(ptTLPoint,ptBRPoint);
		pCtrlWnd->MoveWindow(rectChild,TRUE);

		//字体缩放
		CString strNameBuff;
		::GetClassName(GetDlgItem(nChildId)->m_hWnd,strNameBuff.GetBufferSetLength(255),255);
		if (strNameBuff.CompareNoCase(_T("Button")) == 0 ||
			strNameBuff.CompareNoCase(_T("Static")) == 0 ||
			strNameBuff.CompareNoCase(_T("Edit")) == 0
			)
		{
			//CWnd* pChildCtr = (CWnd*)GetDlgItem(nChildId);

			////获取控件字体
			//CFont  *pfont = pChildCtr->GetFont();  
			//LOGFONT logfont;  
			//pfont->GetLogFont(&logfont);  

			//CString strTmpFont;
			//strTmpFont.Format(_T("变化前 宽 %d, 高 %d"),logfont.lfWidth,logfont.lfHeight);
			////SetDlgItemText(IDC_ED_FONT,strTmpFont);

			//if (bToMax)
			//{
			//	logfont.lfWidth = logfont.lfWidth * dZoomX;
			//	logfont.lfHeight = logfont.lfHeight * dZoomY; 
			//}
			//else
			//{
			//	logfont.lfWidth = logfont.lfWidth / dZoomX;
			//	logfont.lfHeight = logfont.lfHeight / dZoomY;
			//}  
			//m_pFont = new CFont;
			//m_pFont->CreateFontIndirect(&logfont);  
			//pChildCtr->SetFont(m_pFont); 

		}
		strNameBuff.ReleaseBuffer();

		hWndChild=::GetWindow(hWndChild, GW_HWNDNEXT);  
	}
	//最大化最小化状态变更
	bToMax = !bToMax;

	m_ptClient = ptNew;
}

BOOL CDialogExt::OnInitDialog()
{
	CDialog::OnInitDialog();

	//窗口最大化初始化处理
	SetClassLong(m_hWnd,GCL_STYLE,CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS); //去掉最大化后残影
	CRect rect;   
	GetClientRect(&rect);  
	m_ptClient.x = rect.right-rect.left;
	m_ptClient.y = rect.bottom-rect.top;
	CString strTmp;
	strTmp.Format(_T("宽:%.1d, 高:%.1d"),m_ptClient.x,m_ptClient.y);
	//SetDlgItemText(IDC_ED_CURR,strTmp);

	return TRUE;
}

void CDialogExt::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
void CDialogExt::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CDialogExt::SetFontEx( CWnd *pWnd, int nWidth,int nHeight,CString strFontType )
{
	m_pFont = new CFont;
	m_pFont->CreateFont(nWidth,nHeight,	//字体宽高
		0,								//字体显示的角度
		0,								//字体的角度
		FW_NORMAL,						//字体的磅数
		FALSE,							//倾斜字体
		FALSE,							//带下划线的字体
		0,								//带删除线的字体
		DEFAULT_CHARSET,				//字符集
		OUT_DEFAULT_PRECIS,				//输出的精度
		CLIP_DEFAULT_PRECIS,			//裁剪的精度
		DEFAULT_QUALITY,				//逻辑字体于输出设备的实际字体之间的精度
		DEFAULT_PITCH | FF_SWISS,		//字体间距和字体集
		strFontType						//字体名称
	);
	pWnd->SetFont(m_pFont);
}

void CDialogExt::SetBkPicture( CString strPicPath )
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	//dc.FillSolidRect(rect,WHITE);

	CBitmap bmpBackground;
	HBITMAP hBitMap = (HBITMAP)LoadImage(NULL,strPicPath,IMAGE_BITMAP,rect.Width(),rect.Height(),LR_LOADFROMFILE);

	BITMAP bitmap;
	bmpBackground.Attach(hBitMap);
	bmpBackground.GetObject(sizeof(bitmap), &bitmap);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap *pbmpOld=dcMem.SelectObject(&bmpBackground); //选择位图，将其装入内存设备上下文;
	//使用BitBlt()函数从内存设备上下文中把位图拷贝到输出设备上下文中显示,
	//下边两个实现结果一样，所以本人觉得第3和4参数设为位图宽和高也行；
	//dc.BitBlt(0,0, bitmap.bmWidth,bitmap.bmHeight,&dcMem,0,0,SRCCOPY);
	dc.BitBlt(0,0, rect.Width(),rect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject( pbmpOld );//将原来的位图重新载入
	dcMem.DeleteDC();//删除内存DC
}
BOOL CDialogExt::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	LPCTSTR lpMsg = (LPCTSTR)(pCopyDataStruct->lpData);
	CString strRecv(lpMsg);
	//SetDlgItemText(IDC_ST_FONT,strRecv);
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}


/*===============================================================
*	模块名：CStaticEx
*	描  述：
*			扩展颜色、字体
*================================================================*/
IMPLEMENT_DYNAMIC(CStaticEx, CStatic)

BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

CStaticEx::CStaticEx()
{
	m_ForeColor = BLACK;
	m_BackColor = WHITE;
	m_BkBrush.CreateSolidBrush(m_BackColor);
	p_Font = NULL;
}
CStaticEx::~CStaticEx()
{
	if ( p_Font )    
		delete p_Font;
}
void CStaticEx::SetForeColor( COLORREF color )
{
	m_ForeColor = color;							//设置文字颜色
}
void CStaticEx::SetBkColor( COLORREF color )
{
	m_BackColor = color;
	m_BkBrush.Detach();								//分离原背景刷
	m_BkBrush.CreateSolidBrush( m_BackColor );		//建立背景刷
}
void CStaticEx::SetTextFont( int FontHight,LPCTSTR FontName )
{
	if ( p_Font )   delete p_Font;					//删除旧字体
	p_Font = new CFont;								//生成字体对象
	p_Font->CreatePointFont( FontHight, FontName ); //创建字体
	SetFont( p_Font );								//设置控件字体
}
HBRUSH CStaticEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor( m_ForeColor );         //设置文字颜色
	pDC->SetBkColor( m_BackColor );           //设置背景色
	return (HBRUSH)m_BkBrush.GetSafeHandle(); //返回背景刷
}

/*===============================================================
*	模块名：CButtonEx
*	描  述：
*			按钮按下、抬起添加位图图片
*================================================================*/
BEGIN_MESSAGE_MAP(CButtonEx,CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CButtonEx::CButtonEx()
{
}

CButtonEx::~CButtonEx()
{
}

void CButtonEx::SetBitmapEx(CString strPicDown,CString strPicUp)
{
	CRect rect;
	this->GetClientRect(rect);

	m_bitmapDown = (HBITMAP)LoadImage(NULL,strPicDown,IMAGE_BITMAP,rect.Width(),rect.Height(),LR_LOADFROMFILE);
	m_bitmapUp = (HBITMAP)LoadImage(NULL,strPicUp,IMAGE_BITMAP,rect.Width(),rect.Height(),LR_LOADFROMFILE);
	
}

void CButtonEx::OnLButtonDown( UINT nFlags, CPoint point )
{
	this->SetBitmap(m_bitmapDown);
	CButton::OnLButtonDown(nFlags,point);
}

void CButtonEx::OnLButtonUp( UINT nFlags, CPoint point )
{
	this->SetBitmap(m_bitmapUp);
	CButton::OnLButtonUp(nFlags,point);
}
/*===============================================================
*	模块名：CListBoxEx
*	描  述：
*			扩展拖动滑条
*================================================================*/
IMPLEMENT_DYNAMIC(CListBoxEx, CListBox)

CListBoxEx::CListBoxEx()
{

}

CListBoxEx::~CListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
END_MESSAGE_MAP()

int CListBoxEx::AddString( LPCTSTR lpszItem )
{
	int nResult = CListBox::AddString( lpszItem );
	RefushHorizontalScrollBar();
	return nResult;
}

int CListBoxEx::InsertString( int nIndex, LPCTSTR lpszItem )
{
	int nResult = CListBox::InsertString( nIndex, lpszItem );
	RefushHorizontalScrollBar();
	return nResult;
}

void CListBoxEx::RefushHorizontalScrollBar( void )
{
	CDC *pDC = this->GetDC();
	if ( NULL == pDC )
	{
		return;
	}

	int nCount = this->GetCount();
	if ( nCount < 1 )
	{
		this->SetHorizontalExtent( 0 );
		return;
	}

	int nMaxExtent = 0;
	CString szText;
	for ( int i = 0; i < nCount; ++i )
	{
		this->GetText( i, szText );
		CSize &cs = pDC->GetTextExtent( szText );
		if ( cs.cx > nMaxExtent )
		{
			nMaxExtent = cs.cx;
		}
	}

	this->SetHorizontalExtent( nMaxExtent );
}


/*===============================================================
*	模块名：CTreeCtrlEx
*	描  述：
*			树形控件
*================================================================*/
CTreeCtrlEx::CTreeCtrlEx()
{

}

CTreeCtrlEx::~CTreeCtrlEx()
{

}
void CTreeCtrlEx::InitTree()
{
	m_imageList.Create(20, 20, ILC_COLOR32 | ILC_MASK, 4, 4);
	//m_imageList.SetBkColor(GetSysColor(COLOR_WINDOW));

	//加载图片
	m_icon[0] = (HICON)LoadImage(NULL, _T(".\\ConfigSrc\\socket.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_icon[1] = (HICON)LoadImage(NULL, _T(".\\ConfigSrc\\tcp.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_icon[2] = (HICON)LoadImage(NULL, _T(".\\ConfigSrc\\udp.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	m_icon[3] = (HICON)LoadImage(NULL, _T(".\\ConfigSrc\\com.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	for (int i = 0; i < 4; ++i)
	{
		m_imageList.Add(m_icon[i]);
	}

	this->SetImageList(&m_imageList, TVSIL_NORMAL);
}


/*===============================================================
*	模块名：BasicExcelEx
*	描  述：
*			Excel表格基本读取、写入
*			只支持.xls格式的表格，如果是.xlsx格式的表格需要另存为.xls
*================================================================*/
BasicExcelEx::BasicExcelEx( CString strExcelPath )
{
	m_strExcelPath = strExcelPath;
}

BasicExcelEx::~BasicExcelEx()
{

}
BOOL BasicExcelEx::ReanExcelData()
{
	BasicExcel e;

	CString strLine = _T("");

	int len =WideCharToMultiByte(CP_ACP,0,m_strExcelPath,-1,NULL,0,NULL,NULL);
	char *ptxtTemp =new char[len +1];
	WideCharToMultiByte(CP_ACP,0,m_strExcelPath,-1,ptxtTemp,len,NULL,NULL );

	if(!e.Load(ptxtTemp))
	{
		AfxMessageBox(m_strExcelPath + _T(" 打开失败！"));
		return FALSE;
	}

	BasicExcelWorksheet* sheet1 = e.GetWorksheet("sheet1");
	if (NULL == sheet1)
	{
		AfxMessageBox(_T("sheet1 打开失败！"));
		return FALSE;
	}

	size_t maxRows = sheet1->GetTotalRows();
	size_t maxCols = sheet1->GetTotalCols();

	CString strName(sheet1->GetAnsiSheetName());
	strLine.Format(_T("Dimension of %s (%d,%d)"),strName,maxRows,maxCols);

	std::map<CString,std::vector<CString>> mapData;	//数据

	CMap<size_t,size_t,CString,CString&> mapTmp;//临时保存列数和key对应关系
	
	//初始化map容器
	for (size_t c = 0; c < maxCols; ++c)
	{
		BasicExcelCell* cell = sheet1->Cell(0, c);
		CString strKey=_T("");
		std::vector<CString> vecTmp;
		if (cell->Type() == 0)
		{
		}
		else if (cell->Type() == 1)
		{
			strKey.Format(_T("%d"), cell->GetInteger());
		}
		else if (cell->Type() == 2)
		{
			strKey.Format(_T("%f"), cell->GetDouble());
		}
		else if (cell->Type() == 3)
		{
			CString strTmp(cell->GetString());
			strKey = strTmp;
		}
		else if (cell->Type() == 4)
		{
			CString strTmp(cell->GetWString());
			strKey = strTmp;
		}
		
		if (!strKey.IsEmpty())
		{
			mapData[strKey] = vecTmp;
			mapTmp[c] = strKey;
		}
	}

	//表格内容
	CString strValue = _T("");
	for (size_t r = 1; r < maxRows; ++r)
	{
		CString strDbValue;
		for (size_t c = 0; c < maxCols; ++c)
		{
			BasicExcelCell* cell = sheet1->Cell(r, c);
			CString strKey = mapTmp[c];

			strValue = _T("");
			if (cell->Type() == 0)	//BasicExcelCell::UNDEFINED
			{
			}
			else if (cell->Type() == 1)//BasicExcelCell::INT
			{
				strValue.Format(_T("%d"), cell->GetInteger());
			}
			else if (cell->Type() == 2)
			{
				strValue.Format(_T("%f"), cell->GetDouble());
			}
			else if (cell->Type() == 3)
			{
				CString strTmp(cell->GetString());
				strValue = strTmp;
			}
			else if (cell->Type() == 4)
			{
				CString strTmp(cell->GetWString());
				strValue = strTmp;
			}

			mapData[strKey].push_back(strValue);	
		}
	}
	return TRUE;
}
/*===============================================================
*	模块名：HttpCtrlEx
*	描  述：
*			http客户端Get、Post请求
*================================================================*/
HttpCtrlEx::HttpCtrlEx()
{

}

HttpCtrlEx::~HttpCtrlEx()
{}
CString HttpCtrlEx::GetData( CString strUrl )
{
	CHttpClient httpClient;
	CString strRespon;
	//CString strUrl= _T("https://console.apipost.cn/register?utm_source=10009");

	/*httpClient.HttpGet(strUrl,_T(""),strRespon);*/
	httpClient.HttpPost(strUrl,_T(""),strRespon);
	
	return strRespon;
}

/*===============================================================
*	模块名：DatabaseCtrlEx
*	描  述：
*			数据库增删查改
*================================================================*/

DatabaseCtrlEx::DatabaseCtrlEx(DbPara dbPara)
{
	m_dbPara = dbPara;
	InitDatabase();
}

DatabaseCtrlEx::~DatabaseCtrlEx()
{
	DisConnDb();
}

void DatabaseCtrlEx::InitDatabase()
{
	switch (m_dbPara.m_nDbType)
	{
	case Mysql:
		//初始化mysql数据库
		if (mysql_library_init(0, nullptr, nullptr))
			g_log.OutPutLog(_T("mysql_library_init failed."));
		
		if (nullptr == mysql_init(&m_mysql))
			g_log.OutPutLog(_T("mysql_init failed."));
		
		if (mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "gbk"))
			g_log.OutPutLog(_T("mysql_options failed."));
		
		break;
	case driverSQLServer:
		//初始化SQL数据库

		break;
	}

}

BOOL DatabaseCtrlEx::ConnectDB()
{
	switch (m_dbPara.m_nDbType)
	{
	case Mysql:
		ConnMysql();
		break;
	}
	return TRUE;
}

void DatabaseCtrlEx::DisConnDb()
{
	switch (m_dbPara.m_nDbType)
	{
	case Mysql:
		mysql_close(&m_mysql);
		break;
	}
}

BOOL DatabaseCtrlEx::QueryMysql(CString strSql, vector<vector<string>>& qryResult)
{
	USES_CONVERSION;
	std::string sSql(W2A(strSql));
	const char* sql = sSql.c_str();

	MYSQL_RES* result;//保存查询结果对象

	if (mysql_real_query(&m_mysql, sql, strlen(sql)) != 0)
		return FALSE;
	
	result = mysql_store_result(&m_mysql);
	//列数(字段数)
	unsigned fields = mysql_num_fields(result);
	MYSQL_ROW row;//
	while (row = mysql_fetch_row(result))//true
	{
		vector<string> rowData;
		for (unsigned i = 0; i < fields; ++i)
		{
			if (row[i])
			{
				rowData.push_back(row[i]);
			}
			else
			{
				rowData.push_back("");
			}
		}
		qryResult.push_back(rowData);
	}

	//释放结果对象
	mysql_free_result(result);
	return TRUE;
}

BOOL DatabaseCtrlEx::ExcuteMysql(CString strSql)
{
	USES_CONVERSION;
	std::string sSql(W2A(strSql));
	const char* sql = sSql.c_str();

	if (mysql_real_query(&m_mysql, sql, strlen(sql)) != 0)
		return FALSE;

	return TRUE;
}

BOOL DatabaseCtrlEx::ConnMysql()
{
	USES_CONVERSION;
	std::string sAdress(W2A(m_dbPara.m_strAdress));
	const char* pAdress = sAdress.c_str();

	std::string sUser(W2A(m_dbPara.m_strUser));
	const char* pUser = sUser.c_str();

	std::string sPasswd(W2A(m_dbPara.m_strPasswd));
	const char* pPasswd = sPasswd.c_str();

	std::string sDbName(W2A(m_dbPara.m_strDbName));
	const char* pDbName = sDbName.c_str();

	if (mysql_real_connect(&m_mysql, pAdress, pUser, pPasswd, pDbName, m_dbPara.m_nPort, nullptr, 0) == nullptr)
	{
		AfxMessageBox(_T("数据库连接失败！"));
		return FALSE;
	}
	AfxMessageBox(_T("数据库连接成功！"));
	return TRUE;
}

BOOL DatabaseCtrlEx::ConnSQL()
{
	return TRUE;
}

/*===============================================================
*	模块名：ComCtrlEx
*	描  述：
*			串口收发数据
*================================================================*/
ComCtrlEx::ComCtrlEx()
{
	m_hComm = NULL;
	m_strComPort = _T("COM2");		//com口
	m_strBaudRate;		//波特率
	m_strByteSize;		//数据位
	m_strStopBits;		//停止位
	m_strParity;		//校验位
}
ComCtrlEx::~ComCtrlEx()
{

}
BOOL ComCtrlEx::OpenComm()
{
	//创建串口
	m_hComm = CreateFile(
		m_strComPort,				//串口名字
		GENERIC_READ|GENERIC_WRITE,	//指定串口访问的类型
		0,							//指定共享属性
		NULL,						//引用安全性属性结构体
		OPEN_EXISTING,				//创建标志
		0,							//是否可以精细异步操作
		NULL						//模板文件句柄
	);
	if (m_hComm == (HANDLE)0xFFFFFFFF)
	{
		DWORD dwError = GetLastError();
		AfxMessageBox(dwError);
		return FALSE;
	}

	SetupComm(m_hComm,1024,1024); //输入缓冲区和输出缓冲区的大小都是1024 


	COMMTIMEOUTS TimeOuts; 
	TimeOuts.ReadIntervalTimeout=1000;		//每个字符之间的时间超时
	TimeOuts.ReadTotalTimeoutMultiplier=500;
	TimeOuts.ReadTotalTimeoutConstant=5000; 
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetTimeOut(TimeOuts);

	DCB dcb;
	GetCommState(m_hComm,&dcb);
	dcb.BaudRate = _ttoi(m_strBaudRate);	//波特率为9600
	dcb.ByteSize = _ttoi(m_strByteSize);	//每个字节有8位
	/*
	#define NOPARITY            0
	#define ODDPARITY           1
	#define EVENPARITY          2
	#define MARKPARITY          3
	#define SPACEPARITY         4
	*/
	dcb.Parity= _ttoi(m_strParity);			//无奇偶校验位
	/*
	#define ONESTOPBIT          0
	#define ONE5STOPBITS        1
	#define TWOSTOPBITS         2
	*/
	dcb.StopBits=_ttoi(m_strStopBits);	//两个停止位
	SetCommDcb(dcb);

	g_log.OutPutLog(_T("串口打开成功"));

}
BOOL ComCtrlEx::SetTimeOut( COMMTIMEOUTS timeOut )
{
	return SetCommTimeouts(m_hComm,&timeOut); //设置超时
}
BOOL ComCtrlEx::SetCommDcb( DCB dcb )
{
	return SetCommState(m_hComm,&dcb);
}
void ComCtrlEx::CloseComm()
{
	if (m_hComm)
	{
		CloseHandle(m_hComm);
		g_log.OutPutLog(_T("串口关闭成功"));
	}
}
void ComCtrlEx::ReadComm( CString &strReadData,DWORD dwLen )
{
	char szBuff[1024]={0}; 
	BOOL bReadStat=ReadFile(m_hComm,szBuff,dwLen,&dwLen,NULL);
	if(!bReadStat)
	{ 
		AfxMessageBox(_T("读串口失败!")); 
	} 
	PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 

	strReadData = szBuff;
	g_log.OutPutLog(_T("串口接收：")+strReadData);
}
void ComCtrlEx::WriteComm( CString &strWriteData,DWORD dwLen )
{
	int nLen = WideCharToMultiByte(CP_ACP,0,strWriteData,-1,NULL,0,NULL,NULL);
	char *pBuff = new char[nLen + 1];
	WideCharToMultiByte(CP_ACP,0,strWriteData,-1,pBuff,nLen,NULL,NULL);

	DWORD dwBytesWrite = nLen;
	COMSTAT ComStat; 
	DWORD dwErrorFlags; 
	BOOL bWriteStat; 
	ClearCommError(m_hComm,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(m_hComm,pBuff,dwBytesWrite,& dwBytesWrite,NULL);
	if(!bWriteStat)
	{ 
		AfxMessageBox(_T("写串口失败!")); 
	}
	PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 

	delete [] pBuff;
	g_log.OutPutLog(_T("串口发送：")+strWriteData);
}

/*===============================================================
*	模块名：SocketCtrlEx
*	描  述：
*			网口收发数据（支持并发）
*================================================================*/
HWND m_hParentWnd;
CLogPrint g_log;
SocketCtrl::SocketCtrl()
{
	WSADATA wsadata;
	::WSAStartup(MAKEWORD(2, 2), &wsadata);
}
SocketCtrl::~SocketCtrl()
{
	::WSACleanup();
}
void SocketCtrl::InitServer(HWND hParentWnd,CString strIp,int nPort)
{
	m_hParentWnd = hParentWnd;
	m_strServerIp = strIp;
	m_nServerPort = nPort;
	m_nType = TCP_SERVER;

	g_log.OutPutLog(_T("InitServer"));
	ClearRecvBuff();

	//创建socket
	m_sockServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//绑定socket
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));						
	sockAddr.sin_family = PF_INET;							
	InetPton(PF_INET,m_strServerIp,&sockAddr.sin_addr);
	sockAddr.sin_port = htons(m_nServerPort);				
	::bind(m_sockServer, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//监听客户端socket
	g_log.OutPutLog(_T("监听客户端socket"));
	listen(m_sockServer, 20);

	//发送当前正在监听
	PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETLISTEN, 0);

	//接受客户端连接请求
	AfxBeginThread(Accept, this);
	AfxBeginThread(Loop, this);
}
void SocketCtrl::SendMsg(CString strMsg,size_t nSendFlag/*=0*/)
{
	USES_CONVERSION;
	char* pMsg = T2A(strMsg);
	/*int nLen = WideCharToMultiByte(CP_ACP,0,strMsg,-1,NULL,0,NULL,NULL);
	char* pMsg = new char[nLen + 1];
	WideCharToMultiByte(CP_ACP,0,strMsg,-1,pMsg,nLen,NULL,NULL);*/

	if (m_nType == TCP_CLIENT)
	{	
		int nRet = ::send(m_sockClient, pMsg, (int)strlen(pMsg)+1, 0);
		if (nRet == SOCKET_ERROR)
		{
			AfxMessageBox(_T("数据发送失败"));
			return;
		}
	}
	else if (m_nType == TCP_SERVER)
	{
		for (std::list<SOCKET>::iterator iter = m_sockList.begin();iter!=m_sockList.end();++iter)
		{
			if ((size_t)*iter == nSendFlag)
			{
				SOCKET sockSend = (SOCKET)*iter;
				int nRet = ::send(sockSend, pMsg, (int)strlen(pMsg)+1, 0);
				if (nRet == SOCKET_ERROR)
				{
					AfxMessageBox(_T("数据发送失败"));
					return;
				}
				break;
			}
		}
		
	}
}
UINT SocketCtrl::Accept(LPVOID pParam)
{
	SocketCtrl* pDlg = (SocketCtrl*)pParam;
	while (TRUE)
	{
		SOCKADDR clientAddr;
		int nSize = sizeof(SOCKADDR);
		SOCKET sockListen = ::accept(pDlg->m_sockServer, (SOCKADDR*)&clientAddr, &nSize);
		if (INVALID_SOCKET != sockListen)
		{
			bool bFound = (std::find(pDlg->m_sockList.begin(), pDlg->m_sockList.end(), sockListen) != pDlg->m_sockList.end());
			if (!bFound)
			{
				{
					sockaddr_in sockAddr;
					memcpy(&sockAddr, &clientAddr, sizeof(sockAddr));
					char buf[32] = {0,};
					pDlg->toIp(buf, 32, sockAddr);
					int nPort = ntohs(sockAddr.sin_port);
					CString strLog;
					CString strIp(buf);
					strLog.Format(_T("%s:%d"), strIp,nPort);
					g_log.OutPutLog(_T("客户端接入:")+strLog);
				}

				char szAccept[5] = {0};
				pDlg->m_nSendFlag = (size_t)sockListen;
				sprintf_s(szAccept,5, "%d", pDlg->m_nSendFlag);
				PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETACCEPT, (LPARAM)szAccept);
				pDlg->m_sockList.push_back(sockListen);
				//const char* szDefault = "server:Welcome!";
				//int resSend = send(sockListen, szDefault, strlen(szDefault), 0);
			}
		}
	}

	return 1;
}
UINT SocketCtrl::Loop(LPVOID pParam)
{
	SocketCtrl* pDlg = (SocketCtrl*)pParam;
	CString strLog;
	strLog.Format(_T("当前事件类型:%d(0客户端，1服务器)"),pDlg->m_nType);
	g_log.OutPutLog(strLog);

	if (pDlg->m_nType == TCP_SERVER)
	{
		fd_set fdRead;
		int nRet = 0;	//记录发送或者接受的字节数
		TIMEVAL tv;		//设置超时等待时间
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		while (true)
		{
			if (pDlg->m_sockList.size() == 0)
				continue;
			FD_ZERO(&fdRead);
			for (std::list<SOCKET>::iterator iter = pDlg->m_sockList.begin(); iter != pDlg->m_sockList.end(); ++iter)
			{
				FD_SET((*iter), &fdRead);
			}
			nRet = select(0, &fdRead, NULL, NULL, &tv);
			if (nRet == 0)
				continue;

			for (std::list<SOCKET>::iterator iter = pDlg->m_sockList.begin(); iter != pDlg->m_sockList.end(); ++iter)
			{
				if (FD_ISSET((*iter), &fdRead))
				{
					char szRecvBuff[256] = {0};
					nRet = recv((*iter), szRecvBuff, 256, 0);
					pDlg->m_nSendFlag = (size_t)*iter;
					if (nRet == 0 || (nRet == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
					{
						char szClose[5] = { 0 };		
						sprintf_s(szClose, 5, "%d", pDlg->m_nSendFlag);

						CString strClose((char*)szClose);
						g_log.OutPutLog(_T("客户端断开连接：") + strClose);

						PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETCLOSE, (LPARAM)szClose);
						::closesocket((*iter));
						pDlg->m_sockList.erase(iter);
						break;
					}
					
					memcpy(pDlg->m_szRecvBuff,szRecvBuff,sizeof(char)*256);
					CString strRecv((char*)pDlg->m_szRecvBuff);
					g_log.OutPutLog(_T("客户端发来数据：") + strRecv);

					PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETRECV, (LPARAM)&pDlg->m_szRecvBuff);//m_szRecvBuff

				}
			}
		}
	}
	else if (pDlg->m_nType == TCP_CLIENT)
	{
		while (TRUE)
		{
			char szRecvBuff[256] = {0};
			int nRet = recv(pDlg->m_sockClient, szRecvBuff, 256, 0);
			if (nRet == 0 || (nRet == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
			{
				g_log.OutPutLog(_T("服务器主动断开"));
				closesocket(pDlg->m_sockClient);
				PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETCLOSE, 0);
				break;
			}
			else
			{
				memcpy(pDlg->m_szRecvBuff,szRecvBuff,sizeof(char)*256);
				CString strRecv((char*)pDlg->m_szRecvBuff);
				g_log.OutPutLog(_T("服务器发来数据：")+strRecv);
				PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETRECV, (LPARAM)&pDlg->m_szRecvBuff);//m_szRecvBuff
			}
		}
	}
	return TRUE;
}
void SocketCtrl::ClearRecvBuff()
{
	memset(m_szRecvBuff, 0, 256);
}
void SocketCtrl::toIp(char* buf, size_t nSize, const struct sockaddr_in& addr)
{
	::inet_ntop(AF_INET,(PVOID) &addr.sin_addr, buf, nSize);
	int a = 0;
}
void SocketCtrl::InitClient(HWND hParentWnd,CString strServerIp, int nPort)
{
	g_log.OutPutLog(_T("初始化客户端"));
	m_hParentWnd = hParentWnd;
	m_nType = TCP_CLIENT;
	m_bConnect = FALSE;

	m_sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockClient < 0)
	{
		AfxMessageBox(_T("套接字创建失败"));
		return;
	}

	SOCKADDR_IN client_in;
	//client_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//将网络地址字符串转换成二进制形式
	InetPton(AF_INET, strServerIp, &client_in.sin_addr);
	client_in.sin_family = AF_INET;
	client_in.sin_port = htons(nPort);

	if (connect(m_sockClient, (SOCKADDR*)&client_in, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("套接字连接失败"));
		return;
	}
	m_bConnect = TRUE;
	AfxBeginThread(Loop, this);
}


