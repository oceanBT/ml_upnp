// Winamp_ScanDeviceDialog.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "ml_upnp.h"
#include "Winamp_ScanDeviceDialog.h"
#include "Winamp_BrowseTask.h"


// Boîte de dialogue CWinamp_ScanDeviceDialog

//IMPLEMENT_DYNAMIC(CWinamp_ScanDeviceDialog, CDialog)

CWinamp_ScanDeviceDialog::CWinamp_ScanDeviceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWinamp_ScanDeviceDialog::IDD, pParent)
{

}

CWinamp_ScanDeviceDialog::~CWinamp_ScanDeviceDialog()
{
}

void CWinamp_ScanDeviceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  //DDX_Control(pDX, IDC_SCAN_CURRENT_FILE, m_CurrentFile);
}


BEGIN_MESSAGE_MAP(CWinamp_ScanDeviceDialog, CDialog)
  
END_MESSAGE_MAP()


// Gestionnaires de messages de CWinamp_ScanDeviceDialog
//
//void CWinamp_ScanDeviceDialog::OnCancel(void)
//{
//  // TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
//  CDialog::OnCancel();
//}
//
//BOOL CWinamp_ScanDeviceDialog::OnInitDialog(void)
//{
//  CDialog::OnInitDialog();
//
//  return TRUE;
//}
//
//void CWinamp_ScanDeviceDialog::PostNcDestroy() 
//{
//  CDialog::PostNcDestroy();
//  delete this;
//}
//
//void CWinamp_ScanDeviceDialog::SetCurrentFile(LPCTSTR filename)
//{
//  m_CurrentFile.SetWindowText(filename);
//}