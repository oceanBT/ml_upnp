/*****************************************************************
|
|   Winamp_ScanDeviceDialog.h - Include file for Scan Device Dialog Class
|
| Copyright (c) 2010, Cédric Lallier.
| All rights reserved.
| http://mlupnp.sourceforge.net
|
| This file is part of ml_upnp.
|
| ml_upnp is free software: you can redistribute it and/or modify
| it under the terms of the GNU General Public License as published by
| the Free Software Foundation, either version 3 of the License, or
| (at your option) any later version.
| 
| ml_upnp is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with ml_upnp; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc., 
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-3.0.html
|
 ****************************************************************/

/** @file
 Winamp Scan Device Dialog
 */

#ifndef _WINAMP_SCANDEVICEDIALOG_H_
#define _WINAMP_SCANDEVICEDIALOG_H_

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "resource.h"

// Boîte de dialogue CWinamp_ScanDeviceDialog

class CWinamp_ScanDeviceDialog : public CDialog
{
	//DECLARE_DYNAMIC(CWinamp_ScanDeviceDialog)

public:
	CWinamp_ScanDeviceDialog(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CWinamp_ScanDeviceDialog();
    //void SetCurrentFile(LPCTSTR filename);

// Données de boîte de dialogue
	enum { IDD = IDD_UPNP_SCAN_DEVICE };
  //CStatic	m_CurrentFile;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV
    //virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
  //afx_msg void OnCancel(void);
  //afx_msg int OnInitDialog(void);
  

};

#endif /* _WINAMP_SCANDEVICEDIALOG_H_ */