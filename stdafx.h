/*****************************************************************
|
|   stdafx.h - Include file describes both standard system and 
|   project specific include files that are used frequently 
|   but hardly ever change.
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
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés

#if _MSC_VER > 1000
#pragma once
#pragma warning (disable : 4786)
#endif // _MSC_VER > 1000

// Must import std namespace as early as possible to avoid "ambiguous symbol"
// errors
namespace std {}
using namespace std;


#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>
#include <Strsafe.h>
#include <AtlConv.h>

#ifdef STRICT
#define PROC WNDPROC
#else
#define PROC FARPROC
#endif

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#include "gen_ml/ml.h"
#include "gen_ml/childwnd.h"
#include "gen_ml/ml_ipc_0313.h"
#include "api/service/api_service.h"
#include "api/service/waServiceFactory.h"
#include "api/memmgr/api_memmgr.h"
#include "coverdirectory/CoverDirectory.h"

#include "Platinum.h"
typedef NPT_Map<NPT_String, itemRecord>                 CWinamp_ItemMap;
typedef NPT_Map<NPT_String, itemRecord>::Entry          CWinamp_ItemMapEntry;

LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK ListViewCompareStringProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
