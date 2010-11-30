#pragma once

#include "stdafx.h"
#include "resource.h"

// CViewUpnpRoot form view

class CViewUpnpRoot : public CFormView
{
	DECLARE_DYNCREATE(CViewUpnpRoot)

protected:
	CViewUpnpRoot();           // protected constructor used by dynamic creation
	virtual ~CViewUpnpRoot();

public:
	enum { IDD = IDD_VIEW_UPNP_ROOT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};


