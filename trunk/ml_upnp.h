/*****************************************************************
|
|   ml_upnp.h - Include file
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

#include "Platinum.h"
#include "PltSyncMediaBrowser.h"
#include "PltMediaController.h"
#include "Winamp_ListView.h"

#ifndef _ML_UPNP_H_
#define _ML_UPNP_H_
#pragma once

#define PLUGIN_NAME      "UPnP MediaServer ControlPoint for Winamp"
#define PLUGIN_VERSION   "v0.6"
#define PLUGIN_AUTHOR    "Cédric Lallier"
#define PLUGIN_AUTHOR_MAIL    "c.lallier@gmail.com"
#define PLUGIN_ABOUT     PLUGIN_NAME " " PLUGIN_VERSION " by " PLUGIN_AUTHOR " (" PLUGIN_AUTHOR_MAIL ")"
#define TITLE_ROOT_ITEM  "Network Devices"

#define WINAMP_NO_DEVICE_FOUND  "No device found"

#define CWinamp_PLAY     0
#define CWinamp_ENQUEUE  1

// configuration section in winamp.ini
#ifdef UNICODE
#define CONFIG_SECTION  L"ml_upnp"
#else
#define CONFIG_SECTION  "ml_upnp"
#endif

/*----------------------------------------------------------------------
|   CWinamp_Repository
+---------------------------------------------------------------------*/
class CWinamp_Repository
{
public:
                            CWinamp_Repository() { m_IsUpdated = false; }
                            ~CWinamp_Repository() {};

  void                      SetId(NPT_String id) { m_id = id; }
  void                      SetMLId(NPT_Cardinal id) { m_mlId = id; }
  void                      SetDeviceDataReference(PLT_DeviceDataReference device) { m_device = device; }
  void                      UpdateTrueRepository() { m_IsUpdated = true; }
  void                      UpdateFalseRepository() { m_IsUpdated = false; }
  void                      UpdateRepository(bool value) { m_IsUpdated = value; }

  NPT_String                GetId() { return m_id; }
  NPT_Cardinal              GetMLId() { return m_mlId; }
  PLT_DeviceDataReference   GetDeviceDataReference() { return m_device; }
  bool                      IsUpdated() { return m_IsUpdated; }

  void                      SetHwnd(HWND hwnd) { m_Hwnd = hwnd; }
  HWND                      GetHwnd() { return m_Hwnd; }

  LRESULT CALLBACK          SpecificDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  NPT_String                m_id;
  NPT_Cardinal              m_mlId;
  PLT_DeviceDataReference   m_device;
  HWND                      m_Hwnd;
  bool                      m_IsUpdated;
};

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
typedef NPT_Map<NPT_Cardinal, NPT_String>               CWinamp_UUIDsMap;
typedef NPT_Map<NPT_String, CWinamp_Repository>         CWinamp_RepositoriesMap;
typedef NPT_Map<NPT_String, CWinamp_Repository>::Entry  CWinamp_RepositoriesMapEntry;

/*----------------------------------------------------------------------
|   CWinamp_MediaController
+---------------------------------------------------------------------*/
class CWinamp_MediaController : public PLT_SyncMediaBrowser,
                                public PLT_MediaController,
                                public PLT_MediaControllerDelegate
{
public:
              CWinamp_MediaController(PLT_CtrlPointReference& ctrlPoint);
  virtual     ~CWinamp_MediaController();

  NPT_Result  Start();
  NPT_Result  MessageProc(NPT_Cardinal message_type, NPT_Cardinal param1, NPT_Cardinal param2, NPT_Cardinal param3);
  bool        IsAlive() { return m_IsAlive; }

  LRESULT CALLBACK  MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  int CALLBACK ListViewCompareStringProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:

  bool        OnMSAdded(PLT_DeviceDataReference& device);
  void        OnMSRemoved(PLT_DeviceDataReference& device);

  bool        OnMRAdded(PLT_DeviceDataReference& device);
  void        OnMRRemoved(PLT_DeviceDataReference& device);
  void        OnMRStateVariablesChanged(PLT_Service* /* service */, 
                                   NPT_List<PLT_StateVariable*>* /* vars */) {};

  NPT_Result  BrowseDevice(NPT_Cardinal id, HWND parent, const char* object_id = NULL, bool metadata = false);
  NPT_Result  FindMediaRepository(NPT_String uuid, CWinamp_Repository*& repository);
  NPT_Result  FindUUID(NPT_Cardinal id, NPT_String* uuid);

  NPT_Result  LoadMLFunctions();
  void        UnloadMLFunctions();
  NPT_Result  ReloadLoadMLFunctions();

  void        playFiles(HWND hwnd, INT_PTR enqueue);

  void        CheckDeviceFound();

  NPT_Result  LoadPreference();
  NPT_Result  SavePreference();

  template <class api_T>
  void        ServiceBuild(api_T *&api_t, GUID factoryGUID_t);

  template <class api_T>
  void        ServiceRelease(api_T *api_t, GUID factoryGUID_t);

  /* Fonction Winamp-ML */
  void  (*childresize_init)(HWND hwndDlg, ChildWndResizeItem *list, int num);
  void  (*childresize_resize)(HWND hwndDlg, ChildWndResizeItem *list, int num);
  int   (*WADlg_getColor)(int idx);
  int   (*WADlg_handleDialogMsgs)(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); 
  void  (*WADlg_DrawChildWindowBorders)(HWND hwndDlg, int *tab, int tabsize);


private:
  NPT_Lock<CWinamp_UUIDsMap>          m_UUIDs;
  NPT_Lock<CWinamp_RepositoriesMap>   m_MediaRepositories;
  NPT_Lock<CWinamp_ItemMap>           m_MediaItems;

  NPT_Lock<PLT_DeviceMap>             m_MediaRenderers;
  
  int                                 m_RootItemId;
  int                                 m_NoDeviceItemId;
  int                                 m_CurrentItemId;

  bool                                m_IsAlive;

  char*                               m_WAConfFile;

  HMENU                               m_PopupMenu;
  CWinamp_ListView                    m_ListView;

  api_memmgr*                         m_WASABI_API_MEMMGR;
  api_service*                        m_WASABI_API_SVC;
  
};


extern winampMediaLibraryPlugin PluginUPNP;

#endif /* _ML_UPNP_H_ */
