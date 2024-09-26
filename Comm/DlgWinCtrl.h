#pragma once

#include <winuser.h>

class DlgWinCtrl:public CDialogEx
{
public:
	DlgWinCtrl();
	~DlgWinCtrl();
	enum {
		SOCKET_TCP_WIN,
		SOCKET_DUP_WIN,
		SERIAL_WIN
	};
	void ClearWin();
	CDialogEx* m_pWinCtrl;
	void CreateWin(int nRoot,UINT uID,CWnd* parent, CRect* rectParent);
};

