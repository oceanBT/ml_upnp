// AproposDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ml_upnp.h"
#include "AproposDialog.h"

// CAproposDialog dialog

IMPLEMENT_DYNAMIC(CAproposDialog, CDialog)

CAproposDialog::CAproposDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAproposDialog::IDD, pParent)
{

}

CAproposDialog::~CAproposDialog()
{
}

void CAproposDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAproposDialog, CDialog)
	ON_BN_CLICKED(IDC_OK_BUTTON, &CAproposDialog::OnBnClickedOkButton)
END_MESSAGE_MAP()


// CAproposDialog message handlers

void CAproposDialog::OnBnClickedOkButton()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

//void CAproposDialog::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Do not call CDialog::OnPaint() for painting messages
//}

//int CAproposDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  Add your specialized creation code here
//
//	return 0;
//}

//void CAproposDialog::OnDestroy()
//{
//	CDialog::OnDestroy();
//
//	// TODO: Add your message handler code here
//}
