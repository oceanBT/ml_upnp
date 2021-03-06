/*****************************************************************
|
|   stdafx.h - Include file describes both standard system and 
|   project specific include files that are used frequently 
|   but hardly ever change.
|
| Copyright (c) 2010, C�dric Lallier.
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

#ifndef _STDAFX_H_
#define _STDAFX_H_
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclure les en-t�tes Windows rarement utilis�s

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
#include <Strsafe.h>

#ifdef STRICT
#define PROC WNDPROC
#else
#define PROC FARPROC
#endif

#define PLUGIN_NAME         L"UPnP A/V ControlPoint for Winamp"
#define PLUGIN_VERSION      L"v0.7.1"
#define PLUGIN_AUTHOR       L"C�dric Lallier"
#define PLUGIN_AUTHOR_MAIL  L"c.lallier@gmail.com"
#define PLUGIN_ABOUT        PLUGIN_NAME L" " PLUGIN_VERSION L" by " PLUGIN_AUTHOR L" (" PLUGIN_AUTHOR_MAIL L")"
#define PLUGIN_DESCRIPTION  PLUGIN_NAME L" " PLUGIN_VERSION

#define WINAMP_ROOT_ITEM                L"Network Devices"
#define WINAMP_NO_DEVICE_FOUND          L"No device found"
#define WINAMP_NO_RENDER_DEVICE_FOUND   L"(- No MediaRender Device Found -)"

// configuration section in winamp.ini
#define CONFIG_SECTION      L"ml_upnp"
#define CONFIG_SPLASH       L"upnp_Splash"
#define CONFIG_RENDERER     L"upnp_Renderer"
#define CONFIG_FRIENDLYNAME L"upnp_FriendlyName"
#define CONFIG_MLFILE       L"upnp_ini"
#define DEFAUT_FRIENDLYNAME L"Winamp Media Renderer"

// Define
#define TIMERID_QUICKSEARCH     500
#define TIMERID_FIRSTUSE        600

// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#include "gen_ml/ml.h"
#include "gen_ml/childwnd.h"
#include "gen_ml/ml_ipc_0313.h"

#include "Platinum.h"
#include "PltMediaItem.h"
#include "nde/nde.h"

extern winampMediaLibraryPlugin PluginUPNP;

#endif /* _STDAFX_H_ */
