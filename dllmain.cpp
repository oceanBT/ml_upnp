/*****************************************************************
|
|   dllmain.cpp - Définit le point de départ de la DLL
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

#include "stdafx.h"
#include "ml_upnp.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

PLT_UPnP* m_upnp;
CWinamp_MediaController * m_controller;

/* gen_ml calls this function when it loads your plugin.  return non-zero to abort loading your plugin */
int Init()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Create control point
	PLT_CtrlPointReference ctrlPoint(new PLT_CtrlPoint());

	// Create controller
    m_controller = new CWinamp_MediaController(ctrlPoint);

	//Create Engine
	m_upnp = new PLT_UPnP();

	//Add control point to upnp engine and start it
    m_upnp->AddCtrlPoint(ctrlPoint);
	m_upnp->Start();

	if (m_controller->Start() == NPT_FAILURE)
    {
      m_upnp->Stop();
      return NPT_FAILURE;
    }

    return NPT_SUCCESS;
}

void Quit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_upnp->Stop();
	delete m_controller;
	delete m_upnp;
}

INT_PTR MessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_controller->IsAlive())
		return m_controller->MessageProc(message_type, param1, param2, param3);

	return 0;
}

winampMediaLibraryPlugin PluginUPNP =
{
	MLHDR_VER,
	PLUGIN_NAME " " PLUGIN_VERSION,
	Init,
	Quit,
	MessageProc,
	0,
	0,
	0,
};

LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

  if (m_controller->IsAlive())
    return m_controller->MainDlgProc(hwnd,uMsg,wParam,lParam);

  return 0;
}

extern "C" 	__declspec(dllexport) winampMediaLibraryPlugin *winampGetMediaLibraryPlugin()
{
	return &PluginUPNP;
}

