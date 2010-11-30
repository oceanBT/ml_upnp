#pragma once

#include "resource.h"

// CAproposDialog dialog

class CAproposDialog : public CDialog
{
	DECLARE_DYNAMIC(CAproposDialog)

public:
	CAproposDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAproposDialog();

// Dialog Data
	enum { IDD = IDD_APROPOS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOkButton();

};
