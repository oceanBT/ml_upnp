/*****************************************************************
|
|   Winamp_MediaController.h - Media Controleur UPnP
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
#include "StdAfx.h"
#include "resource.h"
#include "wa_dlg.h"
#include "Winamp_MediaController.h"
#include "Winamp_BrowseTask.h"
#include "proc.h"

/*----------------------------------------------------------------------
|   define
+---------------------------------------------------------------------*/
#define MENU_MEDIARENDERER_OFFSET 41000

#define IDT_TIMER 1;

ChildWndResizeItem resize_rlist[]={
  {IDC_TEXT_QUICKSEARCH,0x0010},
  {IDC_LISTVIEW,0x0011},
  {IDC_BUTTON_PLAY,0x0101},
  {IDC_BUTTON_ENQUEUE,0x0101},
  {IDC_STATIC_STATUS,0x0110},
  {IDC_STATIC_SEARCH,0x0010},
};

ChildWndResizeItem root_resize_rlist[]={
  {IDC_STATIC_ROOT,0x0011},
};

#define CWinamp_PLAY     0
#define CWinamp_ENQUEUE  1



/*----------------------------------------------------------------------
|   Structure
+---------------------------------------------------------------------*/
struct SColumnData {
  const LPWSTR    name;
  NPT_Int32       size;
  NPT_Cardinal    use;
};

/*----------------------------------------------------------------------
|   Data
+---------------------------------------------------------------------*/
SColumnData LWColumnData[] = {
  { L"IdObject", 0, 1 },
  { L"Artist", 140, 1 },
  { L"Title", 150, 1 },
  { L"Album", 130, 1 },
  { L"Genre", 100, 1 },
  { L"Year", 50, 1 },
  { L"Track #", 50, 1 },
  { L"Length", 40, 1 },
  { L"FileName", 200, 1 },
  { L"Last Play", 80, 1 },
  { L"Rating", 50, 1 },
  { L"Play Count", 50, 1 },
};

NPT_SET_LOCAL_LOGGER("ml_upnp.MediaController")

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CWinamp_MediaController
+---------------------------------------------------------------------*/
CWinamp_MediaController::CWinamp_MediaController(winampMediaLibraryPlugin *plugin, 
                                                PLT_CtrlPointReference& ctrlPoint) :
    PLT_SyncMediaBrowser(ctrlPoint, true),
    PLT_MediaController(ctrlPoint)
{
  TCHAR   DefaultIniDirectory[128] = {0};

  NPT_LOG_FINEST("CWinamp_MediaController::CWinamp_MediaController");

  /* Initialisation des variables */
  m_NoDeviceItemId = 0;
  m_PluginUPNP = plugin;
  m_DB = new CWinamp_Database(m_PluginUPNP);
  m_TaskManager = new PLT_TaskManager();
  m_CurrentMediaServer = NULL;

  *(void **)&WADlg_getColor=(void*)SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,1,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&WADlg_handleDialogMsgs=(void*)SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,2,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&WADlg_DrawChildWindowBorders=(void*)SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,3,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&childresize_init=(void*)SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,32,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&childresize_resize=(void*)SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,33,ML_IPC_SKIN_WADLG_GETFUNC);

  /* Création du menu */
  m_PopupMenu = LoadMenu(m_PluginUPNP->hDllInstance, MAKEINTRESOURCE(IDR_TREEITEMMENU));

  /* Création de la racine */
  MLTREEITEMW NewTree;
  NewTree.size        = sizeof(MLTREEITEMW);     // size of this struct
  NewTree.id          = 0;                      // depends on contxext
  NewTree.parentId    = 0;                      // 0 = root, or ML_TREEVIEW_ID_*
  NewTree.title       = WINAMP_ROOT_ITEM;        // pointer to the buffer contained item name. 
  NewTree.titleLen    = sizeof(WINAMP_ROOT_ITEM);// used for GetInfo 
  NewTree.hasChildren = TRUE;                   // TRUE - has children
  NewTree.imageIndex  = MLTREEIMAGE_BRANCH;

  SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADDW);
  m_RootItemId = NewTree.id;

  // Récupération du paramétrage
  m_WAConfFile = (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETINIFILEW*/ 1334);
  StringCchPrintfW(DefaultIniDirectory, sizeof(DefaultIniDirectory), L"%s\\ml_upnp\\upnp.ini", (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETPLUGINDIRECTORYW*/ 1336));
  GetPrivateProfileStringW(CONFIG_SECTION, CONFIG_MLFILE, DefaultIniDirectory, m_MLFile, sizeof(m_MLFile), m_WAConfFile);

  // Enregistrement du paramétrage
  WritePrivateProfileStringW(CONFIG_SECTION, CONFIG_MLFILE, m_MLFile, m_WAConfFile);

  /* Initialisation du Treeview */
  CheckMSDeviceFound();

//  /* Initialisation du menu */
//  this->CheckMRDeviceFound();

  PLT_MediaController::SetDelegate(this);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CWinamp_MediaController
+---------------------------------------------------------------------*/
CWinamp_MediaController::~CWinamp_MediaController(void)
{
  NPT_LOG_FINEST("CWinamp_MediaController::~CWinamp_MediaController");

  if (m_TaskManager != NULL)
    m_TaskManager->StopAllTasks();

  if (m_DB)
  {
    delete m_DB;
    m_DB = NULL;
  }

  SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) &m_RootItemId, ML_IPC_TREEITEM_DELETE);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CheckMSDeviceFound
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::CheckMSDeviceFound(void)
{
  NPT_LOG_FINEST("CWinamp_MediaController::CheckMSDeviceFound");

  if (m_DB->GetActifDeviceCount() <= 0)
  {
    if (m_NoDeviceItemId == 0)
    {
      // Add an item
      MLTREEITEMW NewTree;
      NewTree.size        = sizeof(MLTREEITEMW);
      NewTree.id          = 0;
      NewTree.parentId    = m_RootItemId;
      NewTree.title       = WINAMP_NO_DEVICE_FOUND;
      NewTree.titleLen    = sizeof(WINAMP_NO_DEVICE_FOUND);
      NewTree.hasChildren = TRUE;
      NewTree.imageIndex  = MLTREEIMAGE_BRANCH;
      SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADDW);
      m_NoDeviceItemId = NewTree.id;
    }
  }
  else
  {
    if (m_NoDeviceItemId != 0)
    {
      // Suppression de la vue Winamp
      SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) m_NoDeviceItemId, ML_IPC_DELTREEITEM);
      m_NoDeviceItemId = 0;
    }
  }
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CheckMRDeviceFound
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::CheckMRDeviceFound(void)
{
  //HMENU menu=GetSubMenu(m_PopupMenu,1);
  //HMENU playToMenu=GetSubMenu(menu,3);

  //MENUINFO lpcmi;
  //lpcmi.cbSize = sizeof(MENUINFO);

  //GetMenuInfo(playToMenu, &lpcmi);

  //if (m_MediaRenderers.GetEntryCount() == 0)
  //{
  //  AppendMenu(playToMenu, MF_GRAYED | MF_DISABLED, ID_UPNPITEMCONTEXT_PLAYTO_RENDER_NULL, WINAMP_NO_RENDER_DEVICE_FOUND);
  //}
  //else
  //  DeleteMenu(playToMenu, ID_UPNPITEMCONTEXT_PLAYTO_RENDER_NULL, MF_BYCOMMAND);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMSAdded
+---------------------------------------------------------------------*/
bool 
CWinamp_MediaController::OnMSAdded(PLT_DeviceDataReference& device) 
{
  NPT_Cardinal  AppareilAjoute = 0;

  NPT_LOG_FINEST("CWinamp_MediaController::OnMSAdded");

  // Teste si c'est un serveur Media
  PLT_Service* service;
  if (NPT_SUCCEEDED(device->FindServiceByType("urn:schemas-upnp-org:service:ContentDirectory:*", service))) 
  {
    /***********************/
    // Actions particulières pour des services Microsoft
    if (NPT_SUCCEEDED(device->FindServiceByType("urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:*", service))) 
    {
      PLT_ActionReference action;
      PLT_SyncMediaBrowser::m_CtrlPoint->CreateAction(
          device, 
          "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1", 
          "IsAuthorized", 
          action);
      if (!action.IsNull()) PLT_SyncMediaBrowser::m_CtrlPoint->InvokeAction(action, 0);

      PLT_SyncMediaBrowser::m_CtrlPoint->CreateAction(
          device, 
          "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1", 
          "IsValidated", 
          action);
      if (!action.IsNull()) PLT_SyncMediaBrowser::m_CtrlPoint->InvokeAction(action, 0);
    }
    /***********************/

    // Un serveur Media a été trouvé. Fait il parti de la liste déjà en base ?
    if (NPT_SUCCEEDED(m_DB->CheckMSDevice(device->GetUUID())))
      AppareilAjoute = 1;
    else
    {
      // L'appareil n'existe pas dans la base de données
      // On propose à l'utilisateur de l'ajouter
      TCHAR   msg[128] = {0};
      StringCchPrintfW(msg, sizeof(msg), L"L'appareil [%s] vient d'être découvert. Voulez vous l'ajouter à Winamp?", 
          CA2T(device->GetFriendlyName().GetChars()));
      NPT_Int32 reponse = MessageBoxW(m_PluginUPNP->hwndLibraryParent, msg,
							    PLUGIN_DESCRIPTION,MB_YESNO|MB_ICONINFORMATION);
      if (reponse == IDYES)
      {
        m_DB->AddMSDevice(device);
        AppareilAjoute = 1;
      }
    }

    if (AppareilAjoute == 1)
    {
      //Partie Winamp
      MLTREEITEMW NewTree;
      NewTree.size        = sizeof(MLTREEITEMW);
      NewTree.id          = 0;
      NewTree.parentId    = m_RootItemId;

      size_t nb = 0;
      StringCchLengthW(CA2T(device->GetFriendlyName().GetChars()), STRSAFE_MAX_CCH, &nb);
      NewTree.title       = (PWCHAR)  calloc (nb+1, sizeof(TCHAR));
      StringCchCopyW(NewTree.title, nb+1, CA2T(device->GetFriendlyName().GetChars()));
      NewTree.titleLen    = device->GetFriendlyName().GetLength();
      NewTree.hasChildren = TRUE;
      NewTree.imageIndex  = MLTREEIMAGE_BRANCH;
      SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADDW);
      m_DB->UpdateIdTreeMSDevice(device->GetUUID(), NewTree.id);

      m_CurrentMediaServer = device;
    }
  }

  CheckMSDeviceFound();
  return true; 
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMSRemoved
+---------------------------------------------------------------------*/
void
CWinamp_MediaController::OnMSRemoved(PLT_DeviceDataReference& device)
{
  NPT_Int32 IdTree=-1;

  NPT_LOG_FINEST("CWinamp_MediaController::OnMSRemoved");

  // Un serveur Media a été trouvé. Fait il parti de la liste en base ?
  if (NPT_SUCCEEDED(m_DB->CheckMSDevice(device->GetUUID(), &IdTree)))
  {
    TCHAR   msg[256] = {0};
    StringCchPrintfW(msg, sizeof(msg), 
      L"L'appareil [%s] vient d'être arreté.\nVoulez vous le supprimer definitivement de Winamp?", 
      CA2T(device->GetFriendlyName().GetChars()));
    NPT_Int32 reponse = MessageBoxW(m_PluginUPNP->hwndLibraryParent, msg,
							    PLUGIN_DESCRIPTION,MB_YESNO|MB_ICONINFORMATION);
    if (reponse == IDYES)
      m_DB->RemoveMSDevice(device->GetUUID());
    else
      m_DB->UpdateIdTreeMSDevice(device->GetUUID(), -1); //MAJ de la base pour indiquer que le serveur n'est plus actif

    SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM) IdTree, ML_IPC_DELTREEITEM);
  }
  CheckMSDeviceFound();
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMRAdded
+---------------------------------------------------------------------*/
bool
CWinamp_MediaController::OnMRAdded(PLT_DeviceDataReference& device)
{
 // NPT_Int32 new_value = MENU_MEDIARENDERER_OFFSET + m_MediaRenderers.GetEntryCount() + 1;

 // // test if it's a media renderer
 // PLT_Service* service;
 // if (NPT_SUCCEEDED(device->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:*", service))) 
 // {
 //   // Ajout du menu
 //   HMENU menu=GetSubMenu(m_PopupMenu,1);
 //   HMENU playToMenu=GetSubMenu(menu,3);
 //   AppendMenu(playToMenu, 0, new_value, device->GetFriendlyName().GetChars());

 //   NPT_AutoLock lock(m_MediaRenderers);
 //   m_MediaRenderers.Put(new_value, WinampDevice);

	//this->CheckMRDeviceFound();
 // }

 return true;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMRRemoved
+---------------------------------------------------------------------*/
void
CWinamp_MediaController::OnMRRemoved(PLT_DeviceDataReference& device)
{
 // //NPT_AutoLock lock(m_MediaRenderers);
 // NPT_List<Winamp_DevicesMapEntry*> entries = m_MediaRenderers.GetEntries();
 // if (entries.GetItemCount() != 0) 
 // {
 //   NPT_List<Winamp_DevicesMapEntry*>::Iterator entry = entries.GetFirstItem();
 //   while (entry) 
 //   {
 //     CWinamp_Device WinampDevice = (*entry)->GetValue();
 //     if (WinampDevice.GetDevice()->GetUUID() == device->GetUUID())
 //     {
 //       // Suppression du menu
 //       HMENU menu=GetSubMenu(m_PopupMenu,1);
 //       HMENU playToMenu=GetSubMenu(menu,3);
 //       DeleteMenu(playToMenu, (NPT_UInt32) (*entry)->GetKey(), MF_BYCOMMAND);
 //       m_MediaRenderers.Erase((*entry)->GetKey());
 //       break;
 //     }
 //     ++entry;
 //   }

	//this->CheckMRDeviceFound();
 // }
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::GetItemRecordList
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::GetItemRecordList(itemRecordListW *list)
{
  NPT_String IdObject;

  int nItem = m_ListView.GetNextSelectedItem(-1); 
  do {
    allocRecordList(list, list->Size+1);
    if (!list->Items) break;

    memset(&list->Items[list->Size],0,sizeof(itemRecordW));

    IdObject = m_ListView.GetItemText(nItem);
    m_DB->SelectItem(IdObject, &list->Items[list->Size]);
    list->Size++;
  } while ((nItem = m_ListView.GetNextSelectedItem(nItem)) >= 0);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::play
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::play(HWND hwnd, INT_PTR enqueue)
{
  HWND listWnd = GetDlgItem(hwnd,IDC_LISTVIEW);
  if (m_ListView.CountSelectedItem() == 0) return;

  itemRecordListW obj={0,};
  obj.Alloc=0;
  obj.Size=0;
  obj.Items=0;

  GetItemRecordList(&obj);

  if (obj.Size)
  {
    mlSendToWinampStruct s={ML_TYPE_ITEMRECORDLISTW, (void*)&obj, !!enqueue};
    SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,(WPARAM)&s,ML_IPC_SENDTOWINAMP);
  }

  free(obj.Items);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::MessageProc
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::MessageProc (NPT_Int32 message_type, 
                                      NPT_Int32 param1, 
                                      NPT_Int32 param2, 
                                      NPT_Int32 param3)
{
  NPT_Result                Res = NPT_FAILURE;
  NPT_String                UUID;

  // check for any global messages here
  if ((message_type >= ML_MSG_TREE_BEGIN) && (message_type <= ML_MSG_TREE_END))
  {
    if ((param1 == m_RootItemId) || (param1 == m_NoDeviceItemId))
    {
      Res = NPT_SUCCESS;
    }
    else
    {
      if (NPT_SUCCEEDED(m_DB->CheckMSDevice(param1, &UUID)))
      {
        Res = NPT_SUCCESS;
      }
    }
    
    // Le menu concerné appartient à notre plugin
    if (Res == NPT_FAILURE)
    {
      m_CurrentItemId = -1;
    }
    else
    {
      switch(message_type)
      {
      case ML_MSG_TREE_ONCREATEVIEW:
        m_CurrentItemId = param1;
        if ((param1 == m_RootItemId) || (param1 == m_NoDeviceItemId))
        {
          return (NPT_Result) CreateDialogParam(m_PluginUPNP->hDllInstance, MAKEINTRESOURCE(IDD_UPNP_ROOT), (HWND)param2, (DLGPROC)::MainRootDlgProc, (LPARAM)0);
        }
        else
        {
          return (NPT_Result) CreateDialogParam(m_PluginUPNP->hDllInstance, MAKEINTRESOURCE(IDD_UPNP_ROOT1), (HWND)param2, (DLGPROC)::MainDlgProc, (LPARAM)UUID.GetChars());
        }
      break;
      case ML_MSG_TREE_ONCLICK:
        if (param2 == ML_ACTION_RCLICK) 
        {
          if (param1 == m_RootItemId)
          {
            HMENU menu=GetSubMenu(m_PopupMenu,0);
            POINT p;
            GetCursorPos(&p);
            NPT_Cardinal r = TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,(HWND)param3,NULL);
            switch(r) 
            {
              case ID_UPNPGENERALTREEITEMCONTEXT_AIDE:
                MessageBoxW((HWND)param3, PLUGIN_ABOUT,L"About", MB_OK);
                break;
            }
          }
          else
          {
          }
        }
      break;
      case ML_MSG_TREE_ONDROPTARGET:
      break;
      case ML_MSG_TREE_ONDRAG:
      break;
      case ML_MSG_TREE_ONDROP:
      break;
      }
    }
  }

  return 0;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::MainDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CWinamp_MediaController::MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (this->WADlg_handleDialogMsgs)
  {
    NPT_Int32 ret = this->WADlg_handleDialogMsgs(hwnd,uMsg,wParam,lParam);
    if (ret) return ret;
  }

  switch (uMsg)
  {
    case WM_INITDIALOG: 
      {
        m_CurrentUUID.Erase(0, m_CurrentUUID.GetLength());
        m_CurrentUUID.Append((char*)lParam);

        /* skin dialog */
	    MLSKINWINDOW sw;
	    sw.skinType = SKINNEDWND_TYPE_DIALOG;
	    sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	    sw.hwndToSkin = hwnd;
	    MLSkinWindow(m_PluginUPNP->hwndLibraryParent, &sw);

        /* Creation de la listView */
        m_ListView.SetHwnd(GetDlgItem(hwnd,IDC_LISTVIEW));

        for (int i=0; i < LW_MAXNUM; i++) 
        {
          TCHAR column[32] = {'\0'};
          TCHAR column_name[32] = {'\0'};
          TCHAR size[32] = {'\0'};
          DWORD column_size = 0;
          TCHAR s_column_size[32] = {'\0'};

          StringCchPrintfW(column, sizeof(column), L"column_%ld", i);
          StringCchPrintfW(size, sizeof(size), L"size_%ld", i);

          if (LWColumnData[i].use)
          {
            GetPrivateProfileStringW(CONFIG_SECTION, column, LWColumnData[i].name, column_name, sizeof(column_name), m_MLFile);
            column_size = GetPrivateProfileIntW(CONFIG_SECTION, size, LWColumnData[i].size, m_MLFile);
            m_ListView.AddColumn(column_name, column_size);
            StringCchPrintfW(s_column_size, sizeof(s_column_size), L"%ld\0", column_size);
            WritePrivateProfileStringW(CONFIG_SECTION, column, column_name, m_MLFile);
            WritePrivateProfileStringW(CONFIG_SECTION, size, s_column_size, m_MLFile);
          }
        }

        m_ListView.SkinWindow(m_PluginUPNP);
        m_ListView.LoadPreference(CONFIG_SECTION, m_WAConfFile);

        NPT_String UUID((char*)lParam);
        if (NPT_SUCCEEDED(m_DB->LoadData(&m_ListView, UUID)))
          m_ListView.Sort(m_PluginUPNP);

        /* skin button */
        sw.skinType = SKINNEDWND_TYPE_BUTTON;
        sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
        sw.hwndToSkin = GetDlgItem(hwnd,IDC_BUTTON_PLAY);
        MLSkinWindow(m_PluginUPNP->hwndLibraryParent, &sw);
        sw.hwndToSkin = GetDlgItem(hwnd,IDC_BUTTON_ENQUEUE);
        MLSkinWindow(m_PluginUPNP->hwndLibraryParent, &sw);

        // récupération de l'identifiant selectionné
        childresize_init(hwnd, resize_rlist, sizeof(resize_rlist)/sizeof(resize_rlist[0]));
        SetTimer(hwnd, TIMERID_FIRSTUSE, 1000, (TIMERPROC) NULL);
      }
      break;
    case WM_TIMER:
      if (wParam == TIMERID_QUICKSEARCH)
      {
        KillTimer(hwnd,TIMERID_QUICKSEARCH);
        TCHAR buffer[256];
        memset(buffer, 0, sizeof(buffer));
        GetDlgItemTextW(hwnd,IDC_TEXT_QUICKSEARCH, buffer, sizeof(buffer));
        if (NPT_SUCCEEDED(m_DB->LoadData(&m_ListView, m_CurrentUUID, buffer)))
          m_ListView.Sort(m_PluginUPNP);
      }
      else if (wParam == TIMERID_FIRSTUSE)
      {
        /* Controle de la premiere utilisation */
        KillTimer(hwnd, TIMERID_FIRSTUSE);
        if (m_DB->CheckFirstScanMSDevice(m_CurrentUUID) == 0)
        {
          DialogBoxParamW(m_PluginUPNP->hDllInstance,
                                      MAKEINTRESOURCE(IDD_UPNP_SCAN_DEVICE_DIALOG),
                                      m_PluginUPNP->hwndLibraryParent,
                                      (DLGPROC)::ScanDeviceDlgProc,
                                      (LPARAM)this);

          if (NPT_SUCCEEDED(m_DB->LoadData(&m_ListView, m_CurrentUUID)))
            m_ListView.Sort(m_PluginUPNP);
        }
      }
      break;
    case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
      childresize_resize(hwnd, resize_rlist, sizeof(resize_rlist)/sizeof(resize_rlist[0]));
      break;
    case WM_PAINT:
      {
      NPT_Cardinal tabs[] = { IDC_TEXT_QUICKSEARCH|DCW_SUNKENBORDER, IDC_LISTVIEW|DCW_SUNKENBORDER };
      WADlg_DrawChildWindowBorders(hwnd, tabs, sizeof(tabs)/sizeof(tabs[0]));
      }
      break;
    case WM_NOTIFY:
    {
      if (((LPNMHDR)lParam)->idFrom==IDC_LISTVIEW)
      {
        if (((LPNMHDR)lParam)->code == NM_RCLICK)
        {
          /* Création du menu */
          HMENU PopupMenu = LoadMenu(m_PluginUPNP->hDllInstance, MAKEINTRESOURCE(IDR_TREEITEMMENU));
          HMENU menu=GetSubMenu(PopupMenu,1);
          POINT p;
          GetCursorPos(&p);
          NPT_Cardinal r = TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,hwnd,NULL);
          switch(r) 
          { 
            //items that do not require the db to be loaded
			case 0:
				/* Nothing */
				break;
            case ID_UPNPITEMCONTEXT_PLAY:
              play(hwnd, CWinamp_PLAY);
              break;
            case ID_UPNPITEMCONTEXT_ENQUEUE:
              play(hwnd, CWinamp_ENQUEUE);
              break;
//            default:
//              {
//                if (m_ListView.CountSelectedItem() == 0) break;;
//                if (m_ListView.CountSelectedItem() > 1) 
//                {
//                  MessageBox(m_PluginUPNP->hwndLibraryParent,
//                      "The ml_upnp plug-in don't support multiselection of track",
//					  m_PluginUPNP->description,MB_OK|MB_ICONINFORMATION);
//                  break;
//                }
//                TCHAR szBuf1[1024];
//                NPT_Cardinal nItem = ListView_GetNextItem(GetDlgItem(hwnd,IDC_LISTVIEW), -1, LVNI_SELECTED);
//                ListView_GetItemText(GetDlgItem(hwnd,IDC_LISTVIEW), nItem, 0, szBuf1, sizeof(szBuf1));
//                NPT_String IdObject(szBuf1);
////                PLT_MediaObject* track;
////                m_MediaObjects.Get(IdObject, track);
////                m_UPnPEngine->GetController()->PlayTo(r, track);
//              break;
//              }
          }
        }
        else if (((LPNMHDR)lParam)->code == NM_DBLCLK)
        {
          play(hwnd, !!(GetAsyncKeyState(VK_SHIFT)&0x8000));
        }
        else if (((LPNMHDR)lParam)->code == LVN_BEGINDRAG)
        {
          SetCapture(hwnd);
        }
        else if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)
        {
          m_ListView.Sort(lParam);
          m_ListView.SavePreference(CONFIG_SECTION, m_WAConfFile);
        }
        else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
        {
          // key strokes that shall be processed in ALL subpanes
          LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam;
          switch(pnkd->wVKey) 
          {
            case 0x41: //A
            {
              if(GetAsyncKeyState(VK_CONTROL)) 
              {
                // Hotkey Ctrl-A: Select all
                m_ListView.SelectAll();
              }
              break;
            }
          }
        }
        else if (((LPNMHDR)lParam)->code == NM_RETURN)
        {
          this->play(hwnd, !!(GetAsyncKeyState(VK_SHIFT)&0x8000));
        }
      }
    }
    break;
    case WM_COMMAND:
      switch(LOWORD(wParam)) 
      {
        case IDC_BUTTON_PLAY:
          play(hwnd, CWinamp_PLAY);
        break;
        case IDC_BUTTON_ENQUEUE:
          play(hwnd, CWinamp_ENQUEUE);
        break;
        case IDC_TEXT_QUICKSEARCH:
          if (HIWORD(wParam) == EN_CHANGE)
          {
            KillTimer(hwnd,TIMERID_QUICKSEARCH);
            if(HIWORD(wParam) == EN_SETFOCUS)
              SetDlgItemTextW(hwnd, IDC_TEXT_QUICKSEARCH, L"");
            SetTimer(hwnd,TIMERID_QUICKSEARCH,150,NULL);
          }
          break;

          
        break;
      }
    break;
    case WM_LBUTTONUP:
      if (GetCapture() == hwnd)
      {
        ReleaseCapture();

        POINT p;
        p.x=GET_X_LPARAM(lParam);
        p.y=GET_Y_LPARAM(lParam);
        ClientToScreen(hwnd,&p);

        HWND h=WindowFromPoint(p);
        if (h != hwnd && !IsChild(hwnd,h))
        {
          mlDropItemStruct m={ML_TYPE_ITEMRECORDLISTW,NULL,0};
          m.p=p;
          m.flags=ML_HANDLEDRAG_FLAG_NOCURSOR;

          SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDRAG);
          if (m.result>0)
          {
            itemRecordListW obj={0,}; 
            GetItemRecordList(&obj);
            if (obj.Size)
            {
              //fill in this itemCacheObject if you want to provide drag&drop out of the window
              m.flags=0;
              m.result=0;
              m.data=(void*)&obj;
              SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDROP);
            }
            free(obj.Items);
          }
        }
      }
    break;
    case WM_MOUSEMOVE:
      if (GetCapture()==hwnd)
      {
        POINT p;
        p.x=GET_X_LPARAM(lParam);
        p.y=GET_Y_LPARAM(lParam);
        ClientToScreen(hwnd,&p);
        mlDropItemStruct m={ML_TYPE_ITEMRECORDLISTW,NULL,0};
        m.p=p;
        HWND h=WindowFromPoint(p);
        if (!h || h == hwnd || IsChild(hwnd,h))
        {
          SetCursor(LoadCursor(NULL,IDC_NO));
        }
        else 
        {
          SetCursor(LoadCursor(GetModuleHandleW(L"gen_ml.dll"), MAKEINTRESOURCE(ML_IDC_DRAGDROP)));
          SendMessage(m_PluginUPNP->hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDRAG);
        }
      }
    break;
    case WM_DESTROY:
      KillTimer(hwnd, 1);

      for (int i=0; i < LW_MAXNUM; i++) 
      {
        TCHAR size[32] = {'\0'};
        NPT_Int32 column_size = 0;
        TCHAR s_column_size[32] = {'\0'};

        StringCchPrintfW(size, sizeof(size), L"size_%ld", i);

        if (LWColumnData[i].use)
        {
          column_size = m_ListView.GetColumnWidth(i);
          StringCchPrintfW(s_column_size, sizeof(s_column_size), L"%ld", column_size);
          WritePrivateProfileStringW(CONFIG_SECTION, size, s_column_size, m_MLFile);
        }
      }

      MLUnskinWindow(m_PluginUPNP->hwndLibraryParent, GetDlgItem(hwnd,IDC_BUTTON_ENQUEUE));
      MLUnskinWindow(m_PluginUPNP->hwndLibraryParent, GetDlgItem(hwnd,IDC_BUTTON_PLAY));
      m_ListView.UnskinWindow(m_PluginUPNP);
      MLUnskinWindow(m_PluginUPNP->hwndLibraryParent, hwnd);
    break;
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::MainRootDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CWinamp_MediaController::MainRootDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (WADlg_handleDialogMsgs)
  {
    NPT_Int32 ret = WADlg_handleDialogMsgs(hwnd,uMsg,wParam,lParam);
    if (ret) return ret;
  }

  switch (uMsg)
  {
    case WM_INITDIALOG: 
      /* skin dialog */
      MLSKINWINDOW sw;
      sw.skinType = SKINNEDWND_TYPE_DIALOG;
      sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
      sw.hwndToSkin = hwnd;
      MLSkinWindow(m_PluginUPNP->hwndLibraryParent, &sw);

      childresize_init(hwnd, root_resize_rlist, sizeof(root_resize_rlist)/sizeof(root_resize_rlist[0]));
      break;
    case WM_SIZE:
    if (wParam != SIZE_MINIMIZED)
      childresize_resize(hwnd, root_resize_rlist, sizeof(root_resize_rlist)/sizeof(root_resize_rlist[0]));
      break;
    case WM_PAINT:
      {
        NPT_Cardinal tabs[] = { IDC_STATIC_ROOT|DCW_SUNKENBORDER };
        WADlg_DrawChildWindowBorders(hwnd, tabs, sizeof(tabs)/sizeof(tabs[0]));
      }
      break;
    case WM_DESTROY:
      MLUnskinWindow(m_PluginUPNP->hwndLibraryParent, hwnd);
    break;
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::playTo
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::PlayTo(NPT_Int32 id_device, PLT_MediaObject* track)
{
//  CWinamp_Device            *device=NULL;
//
//  NPT_AutoLock lock(m_MediaRenderers);
//  m_MediaRenderers.Get(id_device, device);
//
//  if (track->m_Resources.GetItemCount() > 0) 
//  {
//    NPT_Cardinal resource_index = 0;
//    if (NPT_SUCCEEDED(FindBestResource(device->GetDevice(), *track, resource_index))) 
//    {
//      SetAVTransportURI(device->GetDevice(), 0, track->m_Resources[resource_index].m_Uri, track->m_Didl, NULL);
//      Play(device->GetDevice(), 0, "1", NULL);
//    }
//    else
//    {
//#ifdef UNICODE
//      MessageBox(m_PluginUPNP->hwndLibraryParent,L"No matching resource found",
//						  CA2T(m_PluginUPNP->description),MB_OK|MB_ICONINFORMATION);
//#else
//      MessageBox(m_PluginUPNP->hwndLibraryParent,"No matching resource found",
//						  m_PluginUPNP->description,MB_OK|MB_ICONINFORMATION);
//#endif
//    }
//  }
}