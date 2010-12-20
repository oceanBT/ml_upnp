/*****************************************************************
|
|   Winamp_UPnPEngine.cpp - Moteur UPnP comprenant la gestion de 
|                         l'ensemble des outils A/V UPnP en lien avec
|                         Winamp
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
#include "wa_dlg.h"
#include "resource.h"
#include "Winamp_UPnPEngine.h"

NPT_SET_LOCAL_LOGGER("ml_upnp.UPnPEngine")

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::CWinamp_UPnPEngine
+---------------------------------------------------------------------*/
CWinamp_UPnPEngine::CWinamp_UPnPEngine(winampMediaLibraryPlugin *plugin)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::CWinamp_UPnPEngine");
  m_WAVersion   = 0;
  m_isWinampPro = 0;
  m_SplashScreen = 0;

  m_PluginUPNP = plugin;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::~CWinamp_UPnPEngine
+---------------------------------------------------------------------*/
CWinamp_UPnPEngine::~CWinamp_UPnPEngine(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::~CWinamp_UPnPEngine");
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::GetVersion
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::GetVersion(void)
{
  NPT_Result os2k = NPT_FAILURE;

  NPT_LOG_FINEST("CWinamp_UPnPEngine::GetVersion");

  OSVERSIONINFO osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
  if (osvi.dwPlatformId > 1 && osvi.dwMajorVersion > 4) os2k = NPT_SUCCESS;

  NPT_LOG_INFO("== Controle des versions ==");
  NPT_LOG_INFO_1("Version de ml_upnp --> %s", PLUGIN_VERSION);
  NPT_LOG_INFO_2("Version de Winamp --> %ld(%s)", m_WAVersion, (m_isWinampPro ? "PRO": "Basic"));
  NPT_LOG_INFO_5("Version de Windows --> %ld.%ld build(%ld) platformId = %ld os2k=%ld",
    osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId, os2k);
  NPT_LOG_INFO_1("Version de Neptune --> %s", NPT_NEPTUNE_VERSION_STRING);
  NPT_LOG_INFO_1("Version de Platinum --> %s", PLT_PLATINUM_SDK_VERSION_STRING);

  if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if (osvi.dwMajorVersion > 6)
      NPT_LOG_INFO("OS = ?");
    if (osvi.dwMajorVersion == 6)
      NPT_LOG_INFO("OS = Vista");
    else if (osvi.dwMajorVersion == 5)
    {
      if (osvi.dwMinorVersion == 0)
        NPT_LOG_INFO("OS = Win2000");
      else if (osvi.dwMinorVersion == 1)
        NPT_LOG_INFO("OS = WinXP");
      else 
        NPT_LOG_INFO("OS = Win Server 2003 (?)");
    }
    else if (osvi.dwMajorVersion == 4)
      NPT_LOG_INFO("OS = Win NT 4");
    else 
      NPT_LOG_INFO("OS = Win NT (<4)");
  }
  else 
  {
    if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
      NPT_LOG_INFO("OS = Win 95");
    else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
      NPT_LOG_INFO("OS = Win 98");
    else 
      NPT_LOG_INFO("OS = Win ME (?)");
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::CheckWinampVersion
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::CheckWinampVersion(void)
{
  HKEY    hkResult;
  WCHAR   resStr[200];
  DWORD   strLen = 200;

  NPT_LOG_FINEST("CWinamp_UPnPEngine::CheckWinampVersion");

  NPT_CHECK_POINTER_FATAL(m_PluginUPNP);

  m_WAVersion = SendMessage(m_PluginUPNP->hwndWinampParent, WM_WA_IPC, 0, IPC_GETVERSION);

  if(m_WAVersion < 0x5060)
  {
    MessageBoxW(m_PluginUPNP->hwndLibraryParent,L"The ml_upnp plug-in requires Winamp v5.50 and up for it to work.\t\n"
  								  L"Please upgrade your Winamp client to be able to use this.",
								  PLUGIN_ABOUT,MB_OK|MB_ICONINFORMATION);
	return NPT_FAILURE;
  }

  /* Version PRO de Winamp */
  if((RegOpenKeyExW(HKEY_LOCAL_MACHINE,(L"SOFTWARE\\Nullsoft\\Winamp"),0,KEY_READ,&hkResult)) == ERROR_SUCCESS)
  {
    if((RegQueryValueEx(hkResult,(L"regkey"),NULL,NULL,(unsigned char *)resStr,&strLen)) == ERROR_SUCCESS)
    {
      if (wcslen(resStr) > 8)
        m_isWinampPro=1;
    }
    RegCloseKey(hkResult);
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::LoadPreferences
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::LoadPreferences(void)
{
  LPWSTR  WAConfFile = NULL;

  NPT_LOG_FINEST("CWinamp_UPnPEngine::LoadPreferences");

  NPT_CHECK_POINTER_FATAL(m_PluginUPNP);
  WAConfFile = (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,IPC_GETINIFILE);
  NPT_CHECK_POINTER_FATAL(WAConfFile);

  m_SplashScreen=GetPrivateProfileIntW(CONFIG_SECTION, CONFIG_SPLASH, 0, WAConfFile);
  if (m_SplashScreen < 0) m_SplashScreen = 0;

  m_RendererActive=GetPrivateProfileIntW(CONFIG_SECTION, CONFIG_RENDERER, 0, WAConfFile);
  if (m_RendererActive < 0) m_RendererActive = 0;

  GetPrivateProfileStringW(CONFIG_SECTION, CONFIG_FRIENDLYNAME, DEFAUT_FRIENDLYNAME, m_FriendlyName, sizeof(m_FriendlyName), WAConfFile);

  NPT_LOG_INFO_3("m_SplashScreen = %ld\nm_RendererActive = %d\nm_FriendlyName = %s\n", 
      m_SplashScreen, m_RendererActive, m_FriendlyName);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::SavePreferences
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::SavePreferences(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::SavePreferences");

  SavePreference(CONFIG_SPLASH, m_SplashScreen);
  SavePreference(CONFIG_RENDERER, m_MediaRenderer);
  SavePreference(CONFIG_FRIENDLYNAME, m_FriendlyName);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::SavePreference
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::SavePreference(LPCWSTR variable, TCHAR *valeur)
{
  LPWSTR  WAConfFile = NULL;

  NPT_LOG_FINEST("CWinamp_UPnPEngine::SavePreference");

  NPT_CHECK_POINTER_FATAL(m_PluginUPNP);
  WAConfFile = (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETINIFILEW*/ 1334);
  NPT_CHECK_POINTER_FATAL(WAConfFile);
  WritePrivateProfileString(CONFIG_SECTION, variable, valeur, WAConfFile);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::SavePreference
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::SavePreference(LPCWSTR variable, DWORD valeur)
{
  LPCWSTR  WAConfFile = NULL;
  WCHAR   s[32] = {0};

  NPT_LOG_FINEST("CWinamp_UPnPEngine::SavePreference");

  NPT_CHECK_POINTER_FATAL(m_PluginUPNP);
  WAConfFile = (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETINIFILEW*/ 1334);
  NPT_CHECK_POINTER_FATAL(WAConfFile);

  StringCchPrintfW(s, sizeof(s), L"%d", valeur);
  WritePrivateProfileStringW(CONFIG_SECTION, variable, s, WAConfFile);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::CreatePreferencePage
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::CreatePreferencePage(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::CreatePreferencePage");

  NPT_CHECK_POINTER_FATAL(m_PluginUPNP);

  // Page support
  m_prefpageUPnPSupport.hInst = m_PluginUPNP->hDllInstance;
  m_prefpageUPnPSupport.dlgID = IDD_UPNP_CONFIG_SUPPORT;
  m_prefpageUPnPSupport.name=L"UPnP Support";
  m_prefpageUPnPSupport.where=0;
  m_prefpageUPnPSupport.proc=CfgSupportDlgProc;

  //// Page Media Server
  //m_prefpageUPnPMediaServer.hInst = m_PluginUPNP->hDllInstance;
  //m_prefpageUPnPMediaServer.dlgID = IDD_UPNP_CONFIG_MEDIA_SERVER;
  //m_prefpageUPnPMediaServer.name="Media Server";
  //m_prefpageUPnPMediaServer.where=(intptr_t)&m_prefpageUPnPSupport;
  //m_prefpageUPnPMediaServer.proc=CfgMediaServerDlgProc;

  //// Page Media Renderer
  //m_prefpageUPnPMediaRenderer.hInst = m_PluginUPNP->hDllInstance;
  //m_prefpageUPnPMediaRenderer.dlgID = IDD_UPNP_CONFIG_MEDIA_RENDERER;
  //m_prefpageUPnPMediaRenderer.name="Media Renderer";
  //m_prefpageUPnPMediaRenderer.where=(intptr_t)&m_prefpageUPnPSupport;
  //m_prefpageUPnPMediaRenderer.proc=CfgMediaRendererDlgProc;

  //// Page à propos
  //m_prefpageUPnPAbout.hInst = m_PluginUPNP->hDllInstance;
  //m_prefpageUPnPAbout.dlgID = IDD_UPNP_CONFIG_ABOUT;
  //m_prefpageUPnPAbout.name="About";
  //m_prefpageUPnPAbout.where=(intptr_t)&m_prefpageUPnPSupport;
  //m_prefpageUPnPAbout.proc=CfgAboutDlgProc;

  SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,(int)&m_prefpageUPnPSupport,IPC_ADD_PREFS_DLGW);
  //SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,(int)&m_prefpageUPnPMediaServer,IPC_ADD_PREFS_DLG);
  //SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,(int)&m_prefpageUPnPMediaRenderer,IPC_ADD_PREFS_DLG);
  //SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,(int)&m_prefpageUPnPAbout,IPC_ADD_PREFS_DLG);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::SplashScreen
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::SplashScreen(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::SplashScreen");
  if (!m_SplashScreen)
  {
    m_SplashScreen=!m_SplashScreen;
    SavePreference(CONFIG_SPLASH, m_SplashScreen);
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::Start
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::Start(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::Start");

  // Création du point de controle
  PLT_CtrlPointReference ctrlPoint(new PLT_CtrlPoint());

  // Création du controleur UPnP Winamp
  m_controller = new CWinamp_MediaController(m_PluginUPNP, ctrlPoint);

  // Création du moteur PLATINUM UPnP
  m_upnp = new PLT_UPnP();

  // Ajout du point de controle au moteur PLATINUM UPnP
  m_upnp->AddCtrlPoint(ctrlPoint);

  //// Création du Renderer. Il ne sera actif que s'il est attaché au moteur PLATINUM UPnP
  //m_RendererDevice = new CWinamp_MediaRenderer(m_FriendlyName, false);

  //if (m_RendererActive == 1)
  //  m_upnp->AddDevice(m_RendererDevice);
  //else
  //  m_upnp->RemoveDevice(m_RendererDevice);

  //On démarre le moteur PLATINUM UPnP
  m_upnp->Start();

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::Stop
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_UPnPEngine::Stop(void)
{
  NPT_LOG_FINEST("CWinamp_UPnPEngine::Stop");

  this->SavePreferences();

  delete m_controller;

  m_upnp->Stop();
  delete m_upnp;

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::MessageProc
+---------------------------------------------------------------------*/
INT_PTR 
CWinamp_UPnPEngine::MessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3)
{
  NPT_CHECK_POINTER_FATAL(m_controller);
  return m_controller->MessageProc(message_type, param1, param2, param3);
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::MainDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK 
CWinamp_UPnPEngine::MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_CHECK_POINTER_FATAL(m_controller);
  return m_controller->MainDlgProc(hwnd, uMsg, wParam, lParam);
}

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine::MainRootDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK 
CWinamp_UPnPEngine::MainRootDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_CHECK_POINTER_FATAL(m_controller);
  return m_controller->MainRootDlgProc(hwnd, uMsg, wParam, lParam);
}