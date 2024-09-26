#include "pch.h"
#include <afxwin.h>
#include "DlgWinCtrl.h"
#include "CSerialCtrl.h"
#include "CSocketWinCtrl.h"

DlgWinCtrl::DlgWinCtrl()
{
	m_pWinCtrl = NULL;
}

DlgWinCtrl::~DlgWinCtrl()
{
	ClearWin();
}

void DlgWinCtrl::ClearWin()
{
	if (NULL != m_pWinCtrl)
	{
		delete m_pWinCtrl;
		m_pWinCtrl = NULL;
	}
}

void DlgWinCtrl::CreateWin(int nRoot, UINT uID, CWnd* parent,CRect* rectParent)
{
	ClearWin();
	switch (nRoot)
	{
	case SOCKET_TCP_WIN:
		m_pWinCtrl = new CSocketWinCtrl();
		break;
	case SOCKET_DUP_WIN:
		break;
	case SERIAL_WIN:
		m_pWinCtrl = new CSerialCtrl();
		break;
	}
	
	m_pWinCtrl->Create(uID, parent);

	m_pWinCtrl->ShowWindow(SW_SHOW);
}