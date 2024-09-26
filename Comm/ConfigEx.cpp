#include "ConfigEx.h"
#include <afxtempl.h> //CMap

#include "ConfigInc/BasicExcel.hpp"
#include "ConfigInc/HttpClient.h"

using namespace YExcel;

/*===============================================================
*	ģ������CLogPrint
*	��  ����
*			��־������ļ���cmd����(��־�ļ�ֻ����1��������)
*================================================================*/
CLogPrint::CLogPrint()
{
	::InitializeCriticalSection(&m_cs);

	//��ȡ��ǰexeĿ¼
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

	//ƴ���ļ�·��
	m_strPath += m_strFoder;

	//�ļ���
	CString strLogFile;
	strLogFile.Format(_T("%4d-%02d-%02d.txt"), time.wYear, time.wMonth, time.wDay);

	//��־д�뵽�ļ�
	CString strTimeLog;
	strTimeLog.Format(_T("%4d-%02d-%02d_%02d:%02d:%02d.%03d "), 
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	strTimeLog += strLog;
	strTimeLog += _T("\n");

	::EnterCriticalSection(&m_cs);
	//����Ŀ¼
	::CreateDirectory(m_strPath, NULL);
	//д��
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

	//д��ǰ��ʱɾ����־�ļ�
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
		if (strLogFile.CompareNoCase(strFileName) == 0)//��ͬ�ļ������ж�
			continue;
		//��ȡ�ļ�����
		int nIndex = strFileName.ReverseFind('.');
		CString strFileExt = strFileName.Right(strFileName.GetLength() - nIndex);
		if(strFileExt.CompareNoCase(_T(".txt")) == 0)
		{
			//�ָ��·ݳ����ж��Ƿ���ͬһ���·�
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
						//��ͬ�·�ɾ����־�ļ�
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
	//�ڴ�����
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
		//�����ڸ÷ָ����
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
*	ģ������CIniFile
*	��  ����
*			������������
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
*	ģ������CDialogExt
*	��  ����
*			�Զ���Ի�����
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
	//��¼��һ���ͻ��˵Ĵ�С
	CString strTmp;
	strTmp.Format(_T("��:%.1d, ��:%.1d"),m_ptClient.x,m_ptClient.y);
	//SetDlgItemText(IDC_ED_CURR,strTmp);

	//��ȡ��ǰ�仯��Ŀͻ��˴�С
	CRect rtTmp;
	GetClientRect(rtTmp);

	//���㵱ǰ�ͻ��˴�С
	POINT ptNew;
	ptNew.x = rtTmp.right - rtTmp.left;
	ptNew.y = rtTmp.bottom - rtTmp.top;
	strTmp.Format(_T("��:%.1d, ��:%.1d"),ptNew.x,ptNew.y);
	//SetDlgItemText(IDC_ED_CHANGE,strTmp);

	//�жϴ����Ǳ���Ǳ�С
	static BOOL bToMax = TRUE;	//���ڴ�С����仯
	if (ptNew.x > m_ptClient.x || ptNew.y > m_ptClient.y)
		bToMax = TRUE;
	else
		bToMax = FALSE;

	//�������ű���
	double dZoomX,dZoomY;	//���ű���
	//�����С�����ű���
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


	//�ӿؼ��������������
	strTmp.Format(_T("X=%.1f, Y=%.1f"),dZoomX,dZoomY);
	//SetDlgItemText(IDC_ED_ZOOM,strTmp);

	//�����ӿؼ�
	CRect rectChild;
	int	nChildId;
	POINT ptTLPoint;//�ؼ����Ͻ�
	POINT ptBRPoint;//�ؼ����½�
	HWND hWndChild = ::GetWindow(m_hWnd,GW_CHILD);
	while(hWndChild)
	{
		nChildId=::GetDlgCtrlID(hWndChild);//ȡ��ID
		CWnd* pCtrlWnd = GetDlgItem(nChildId);
		pCtrlWnd->GetWindowRect(rectChild); 

		ScreenToClient(rectChild); 
		ptTLPoint = rectChild.TopLeft();
		ptBRPoint = rectChild.BottomRight();

		//�����С������
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

		//��������
		CString strNameBuff;
		::GetClassName(GetDlgItem(nChildId)->m_hWnd,strNameBuff.GetBufferSetLength(255),255);
		if (strNameBuff.CompareNoCase(_T("Button")) == 0 ||
			strNameBuff.CompareNoCase(_T("Static")) == 0 ||
			strNameBuff.CompareNoCase(_T("Edit")) == 0
			)
		{
			//CWnd* pChildCtr = (CWnd*)GetDlgItem(nChildId);

			////��ȡ�ؼ�����
			//CFont  *pfont = pChildCtr->GetFont();  
			//LOGFONT logfont;  
			//pfont->GetLogFont(&logfont);  

			//CString strTmpFont;
			//strTmpFont.Format(_T("�仯ǰ �� %d, �� %d"),logfont.lfWidth,logfont.lfHeight);
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
	//�����С��״̬���
	bToMax = !bToMax;

	m_ptClient = ptNew;
}

BOOL CDialogExt::OnInitDialog()
{
	CDialog::OnInitDialog();

	//������󻯳�ʼ������
	SetClassLong(m_hWnd,GCL_STYLE,CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS); //ȥ����󻯺��Ӱ
	CRect rect;   
	GetClientRect(&rect);  
	m_ptClient.x = rect.right-rect.left;
	m_ptClient.y = rect.bottom-rect.top;
	CString strTmp;
	strTmp.Format(_T("��:%.1d, ��:%.1d"),m_ptClient.x,m_ptClient.y);
	//SetDlgItemText(IDC_ED_CURR,strTmp);

	return TRUE;
}

void CDialogExt::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
void CDialogExt::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CDialogExt::SetFontEx( CWnd *pWnd, int nWidth,int nHeight,CString strFontType )
{
	m_pFont = new CFont;
	m_pFont->CreateFont(nWidth,nHeight,	//������
		0,								//������ʾ�ĽǶ�
		0,								//����ĽǶ�
		FW_NORMAL,						//����İ���
		FALSE,							//��б����
		FALSE,							//���»��ߵ�����
		0,								//��ɾ���ߵ�����
		DEFAULT_CHARSET,				//�ַ���
		OUT_DEFAULT_PRECIS,				//����ľ���
		CLIP_DEFAULT_PRECIS,			//�ü��ľ���
		DEFAULT_QUALITY,				//�߼�����������豸��ʵ������֮��ľ���
		DEFAULT_PITCH | FF_SWISS,		//����������弯
		strFontType						//��������
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
	CBitmap *pbmpOld=dcMem.SelectObject(&bmpBackground); //ѡ��λͼ������װ���ڴ��豸������;
	//ʹ��BitBlt()�������ڴ��豸�������а�λͼ����������豸����������ʾ,
	//�±�����ʵ�ֽ��һ�������Ա��˾��õ�3��4������Ϊλͼ��͸�Ҳ�У�
	//dc.BitBlt(0,0, bitmap.bmWidth,bitmap.bmHeight,&dcMem,0,0,SRCCOPY);
	dc.BitBlt(0,0, rect.Width(),rect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject( pbmpOld );//��ԭ����λͼ��������
	dcMem.DeleteDC();//ɾ���ڴ�DC
}
BOOL CDialogExt::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	LPCTSTR lpMsg = (LPCTSTR)(pCopyDataStruct->lpData);
	CString strRecv(lpMsg);
	//SetDlgItemText(IDC_ST_FONT,strRecv);
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}


/*===============================================================
*	ģ������CStaticEx
*	��  ����
*			��չ��ɫ������
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
	m_ForeColor = color;							//����������ɫ
}
void CStaticEx::SetBkColor( COLORREF color )
{
	m_BackColor = color;
	m_BkBrush.Detach();								//����ԭ����ˢ
	m_BkBrush.CreateSolidBrush( m_BackColor );		//��������ˢ
}
void CStaticEx::SetTextFont( int FontHight,LPCTSTR FontName )
{
	if ( p_Font )   delete p_Font;					//ɾ��������
	p_Font = new CFont;								//�����������
	p_Font->CreatePointFont( FontHight, FontName ); //��������
	SetFont( p_Font );								//���ÿؼ�����
}
HBRUSH CStaticEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor( m_ForeColor );         //����������ɫ
	pDC->SetBkColor( m_BackColor );           //���ñ���ɫ
	return (HBRUSH)m_BkBrush.GetSafeHandle(); //���ر���ˢ
}

/*===============================================================
*	ģ������CButtonEx
*	��  ����
*			��ť���¡�̧�����λͼͼƬ
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
*	ģ������CListBoxEx
*	��  ����
*			��չ�϶�����
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
*	ģ������CTreeCtrlEx
*	��  ����
*			���οؼ�
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

	//����ͼƬ
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
*	ģ������BasicExcelEx
*	��  ����
*			Excel��������ȡ��д��
*			ֻ֧��.xls��ʽ�ı�������.xlsx��ʽ�ı����Ҫ���Ϊ.xls
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
		AfxMessageBox(m_strExcelPath + _T(" ��ʧ�ܣ�"));
		return FALSE;
	}

	BasicExcelWorksheet* sheet1 = e.GetWorksheet("sheet1");
	if (NULL == sheet1)
	{
		AfxMessageBox(_T("sheet1 ��ʧ�ܣ�"));
		return FALSE;
	}

	size_t maxRows = sheet1->GetTotalRows();
	size_t maxCols = sheet1->GetTotalCols();

	CString strName(sheet1->GetAnsiSheetName());
	strLine.Format(_T("Dimension of %s (%d,%d)"),strName,maxRows,maxCols);

	std::map<CString,std::vector<CString>> mapData;	//����

	CMap<size_t,size_t,CString,CString&> mapTmp;//��ʱ����������key��Ӧ��ϵ
	
	//��ʼ��map����
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

	//�������
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
*	ģ������HttpCtrlEx
*	��  ����
*			http�ͻ���Get��Post����
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
*	ģ������DatabaseCtrlEx
*	��  ����
*			���ݿ���ɾ���
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
		//��ʼ��mysql���ݿ�
		if (mysql_library_init(0, nullptr, nullptr))
			g_log.OutPutLog(_T("mysql_library_init failed."));
		
		if (nullptr == mysql_init(&m_mysql))
			g_log.OutPutLog(_T("mysql_init failed."));
		
		if (mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "gbk"))
			g_log.OutPutLog(_T("mysql_options failed."));
		
		break;
	case driverSQLServer:
		//��ʼ��SQL���ݿ�

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

	MYSQL_RES* result;//�����ѯ�������

	if (mysql_real_query(&m_mysql, sql, strlen(sql)) != 0)
		return FALSE;
	
	result = mysql_store_result(&m_mysql);
	//����(�ֶ���)
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

	//�ͷŽ������
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
		AfxMessageBox(_T("���ݿ�����ʧ�ܣ�"));
		return FALSE;
	}
	AfxMessageBox(_T("���ݿ����ӳɹ���"));
	return TRUE;
}

BOOL DatabaseCtrlEx::ConnSQL()
{
	return TRUE;
}

/*===============================================================
*	ģ������ComCtrlEx
*	��  ����
*			�����շ�����
*================================================================*/
ComCtrlEx::ComCtrlEx()
{
	m_hComm = NULL;
	m_strComPort = _T("COM2");		//com��
	m_strBaudRate;		//������
	m_strByteSize;		//����λ
	m_strStopBits;		//ֹͣλ
	m_strParity;		//У��λ
}
ComCtrlEx::~ComCtrlEx()
{

}
BOOL ComCtrlEx::OpenComm()
{
	//��������
	m_hComm = CreateFile(
		m_strComPort,				//��������
		GENERIC_READ|GENERIC_WRITE,	//ָ�����ڷ��ʵ�����
		0,							//ָ����������
		NULL,						//���ð�ȫ�����Խṹ��
		OPEN_EXISTING,				//������־
		0,							//�Ƿ���Ծ�ϸ�첽����
		NULL						//ģ���ļ����
	);
	if (m_hComm == (HANDLE)0xFFFFFFFF)
	{
		DWORD dwError = GetLastError();
		AfxMessageBox(dwError);
		return FALSE;
	}

	SetupComm(m_hComm,1024,1024); //���뻺����������������Ĵ�С����1024 


	COMMTIMEOUTS TimeOuts; 
	TimeOuts.ReadIntervalTimeout=1000;		//ÿ���ַ�֮���ʱ�䳬ʱ
	TimeOuts.ReadTotalTimeoutMultiplier=500;
	TimeOuts.ReadTotalTimeoutConstant=5000; 
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetTimeOut(TimeOuts);

	DCB dcb;
	GetCommState(m_hComm,&dcb);
	dcb.BaudRate = _ttoi(m_strBaudRate);	//������Ϊ9600
	dcb.ByteSize = _ttoi(m_strByteSize);	//ÿ���ֽ���8λ
	/*
	#define NOPARITY            0
	#define ODDPARITY           1
	#define EVENPARITY          2
	#define MARKPARITY          3
	#define SPACEPARITY         4
	*/
	dcb.Parity= _ttoi(m_strParity);			//����żУ��λ
	/*
	#define ONESTOPBIT          0
	#define ONE5STOPBITS        1
	#define TWOSTOPBITS         2
	*/
	dcb.StopBits=_ttoi(m_strStopBits);	//����ֹͣλ
	SetCommDcb(dcb);

	g_log.OutPutLog(_T("���ڴ򿪳ɹ�"));

}
BOOL ComCtrlEx::SetTimeOut( COMMTIMEOUTS timeOut )
{
	return SetCommTimeouts(m_hComm,&timeOut); //���ó�ʱ
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
		g_log.OutPutLog(_T("���ڹرճɹ�"));
	}
}
void ComCtrlEx::ReadComm( CString &strReadData,DWORD dwLen )
{
	char szBuff[1024]={0}; 
	BOOL bReadStat=ReadFile(m_hComm,szBuff,dwLen,&dwLen,NULL);
	if(!bReadStat)
	{ 
		AfxMessageBox(_T("������ʧ��!")); 
	} 
	PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 

	strReadData = szBuff;
	g_log.OutPutLog(_T("���ڽ��գ�")+strReadData);
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
		AfxMessageBox(_T("д����ʧ��!")); 
	}
	PurgeComm(m_hComm, PURGE_TXABORT| PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); 

	delete [] pBuff;
	g_log.OutPutLog(_T("���ڷ��ͣ�")+strWriteData);
}

/*===============================================================
*	ģ������SocketCtrlEx
*	��  ����
*			�����շ����ݣ�֧�ֲ�����
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

	//����socket
	m_sockServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//��socket
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));						
	sockAddr.sin_family = PF_INET;							
	InetPton(PF_INET,m_strServerIp,&sockAddr.sin_addr);
	sockAddr.sin_port = htons(m_nServerPort);				
	::bind(m_sockServer, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//�����ͻ���socket
	g_log.OutPutLog(_T("�����ͻ���socket"));
	listen(m_sockServer, 20);

	//���͵�ǰ���ڼ���
	PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETLISTEN, 0);

	//���ܿͻ�����������
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
			AfxMessageBox(_T("���ݷ���ʧ��"));
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
					AfxMessageBox(_T("���ݷ���ʧ��"));
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
					g_log.OutPutLog(_T("�ͻ��˽���:")+strLog);
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
	strLog.Format(_T("��ǰ�¼�����:%d(0�ͻ��ˣ�1������)"),pDlg->m_nType);
	g_log.OutPutLog(strLog);

	if (pDlg->m_nType == TCP_SERVER)
	{
		fd_set fdRead;
		int nRet = 0;	//��¼���ͻ��߽��ܵ��ֽ���
		TIMEVAL tv;		//���ó�ʱ�ȴ�ʱ��
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
						g_log.OutPutLog(_T("�ͻ��˶Ͽ����ӣ�") + strClose);

						PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETCLOSE, (LPARAM)szClose);
						::closesocket((*iter));
						pDlg->m_sockList.erase(iter);
						break;
					}
					
					memcpy(pDlg->m_szRecvBuff,szRecvBuff,sizeof(char)*256);
					CString strRecv((char*)pDlg->m_szRecvBuff);
					g_log.OutPutLog(_T("�ͻ��˷������ݣ�") + strRecv);

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
				g_log.OutPutLog(_T("�����������Ͽ�"));
				closesocket(pDlg->m_sockClient);
				PostMessage(m_hParentWnd, WM_RECVSOCKDATA, (WPARAM)WM_SOCKETCLOSE, 0);
				break;
			}
			else
			{
				memcpy(pDlg->m_szRecvBuff,szRecvBuff,sizeof(char)*256);
				CString strRecv((char*)pDlg->m_szRecvBuff);
				g_log.OutPutLog(_T("�������������ݣ�")+strRecv);
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
	g_log.OutPutLog(_T("��ʼ���ͻ���"));
	m_hParentWnd = hParentWnd;
	m_nType = TCP_CLIENT;
	m_bConnect = FALSE;

	m_sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockClient < 0)
	{
		AfxMessageBox(_T("�׽��ִ���ʧ��"));
		return;
	}

	SOCKADDR_IN client_in;
	//client_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//�������ַ�ַ���ת���ɶ�������ʽ
	InetPton(AF_INET, strServerIp, &client_in.sin_addr);
	client_in.sin_family = AF_INET;
	client_in.sin_port = htons(nPort);

	if (connect(m_sockClient, (SOCKADDR*)&client_in, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("�׽�������ʧ��"));
		return;
	}
	m_bConnect = TRUE;
	AfxBeginThread(Loop, this);
}


