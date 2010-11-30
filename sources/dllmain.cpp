/*****************************************************************
|
|   dllmain.cpp : Définit le point d'entrée pour l'application DLL.
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

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "stdafx.h"
#include "Winamp_UPnPEngine.h"

CWinamp_UPnPEngine  *m_UPnPEngine;

NPT_SET_LOCAL_LOGGER("ml_upnp.dllmain")

/* gen_ml calls this function when it loads your plugin.  
return non-zero to abort loading your plugin */
int Init()
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

#ifdef DEBUG
  // setup Neptune logging
  NPT_LogManager::GetDefault().Configure("plist:.level=INFO;.handlers=FileHandler;.FileHandler.filename=ml_upnp.log");
#else
  NPT_LogManager::GetDefault().Configure("plist:.level=OFF;.handlers=NullHandler");
#endif

  NPT_LOG_FINEST("Init");
  NPT_LOG_INFO("ml_upnp plugin: starting...");

  // Creation du moteur.
  m_UPnPEngine = new CWinamp_UPnPEngine(&PluginUPNP);

  // Controle de la version de Winamp.
  if (NPT_SUCCEEDED(m_UPnPEngine->CheckWinampVersion()))
  {

    // LOG des versions.
    m_UPnPEngine->GetVersion();

    // Chargement des préférences.
    m_UPnPEngine->LoadPreferences();

    // Creation de l'écran des préférences.
    m_UPnPEngine->CreatePreferencePage();

    // Un petit message de présentation pour la première utilisation du plugin.
    m_UPnPEngine->SplashScreen();

    // Démarrage de la gestion.
    if (NPT_SUCCEEDED(m_UPnPEngine->Start()))
    {
      NPT_LOG_INFO("ml_upnp plugin is started.");
      return NPT_SUCCESS;
    }
    else
    {
      NPT_LOG_INFO("ml_upnp plugin can't be started.");
      return NPT_FAILURE;
    }
  }

  return NPT_FAILURE;
}

void Quit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    NPT_LOG_FINEST("Quit");
    NPT_LOG_INFO("ml_upnp plugin: stopping...");

    if (NPT_FAILED(m_UPnPEngine->Stop()))
      NPT_LOG_WARNING("ml_upnp plugin din't stop correctly.");

    // Destruction du moteur UPnP
    delete m_UPnPEngine;

    NPT_LOG_INFO("ml_upnp plugin is stopped.\nbye.");
}

INT_PTR MessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    NPT_LOG_FINEST("MessageProc");

    NPT_CHECK_POINTER_FATAL(m_UPnPEngine);
	return m_UPnPEngine->MessageProc(message_type, param1, param2, param3);
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

extern "C" 	__declspec(dllexport) winampMediaLibraryPlugin *winampGetMediaLibraryPlugin()
{
  return &PluginUPNP;
}

