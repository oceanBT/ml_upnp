/*****************************************************************
|
|   ml_upnp.cpp - Core file
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
#include "gen_ml/ml.h"
#include "ml_upnp.h"
#include "resource.h"
#include "wa_dlg.h"
#include "PltDidl.h"

#include "Winamp_ListView.h"

ChildWndResizeItem resize_rlist[]={
  {IDC_TEXT_QUICKSEARCH,0x0010},
  {IDC_LISTVIEW,0x0011},
  {IDC_BUTTON_REFRESH,0x0101},
  {IDC_BUTTON_PLAY,0x0101},
  {IDC_BUTTON_ENQUEUE,0x0101},
  {IDC_STATIC_STATUS,0x0110},
  {IDC_STATIC_SEARCH,0x0010},
};

LRESULT CALLBACK EmptyDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return 0;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CWinamp_MediaController
+---------------------------------------------------------------------*/
CWinamp_MediaController::CWinamp_MediaController(PLT_CtrlPointReference& ctrlPoint) :
    PLT_SyncMediaBrowser(ctrlPoint),
    PLT_MediaController(ctrlPoint)
{
  /* Initialisation des valeurs */
  m_RootItemId=0;
  m_NoDeviceItemId = 0;
  m_CurrentItemId=0;
  m_WAConfFile=NULL;
  m_WASABI_API_SVC=NULL;
  m_WASABI_API_MEMMGR=NULL;

  UnloadMLFunctions();

  NPT_AutoLock lock(m_MediaRepositories);
  m_MediaRepositories.Clear();

  NPT_AutoLock lock2(m_UUIDs);
  m_UUIDs.Clear();

  PLT_MediaController::SetDelegate(this);

  m_IsAlive = false;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CWinamp_MediaController
+---------------------------------------------------------------------*/
CWinamp_MediaController::~CWinamp_MediaController()
{

  this->SavePreference();

  UnloadMLFunctions();
  ServiceRelease(m_WASABI_API_MEMMGR, memMgrApiServiceGuid);
  SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) &m_RootItemId, ML_IPC_TREEITEM_DELETE);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::LoadPreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_MediaController::LoadPreference(void)
{
  if (m_WAConfFile != NULL)
  {
    if (NPT_SUCCEEDED(m_ListView.LoadPreference(CONFIG_SECTION, m_WAConfFile)))
      return NPT_SUCCESS;
  }
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::SavePreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_MediaController::SavePreference(void)
{
  if (m_WAConfFile != NULL)
  {
    if (NPT_SUCCEEDED(m_ListView.SavePreference(CONFIG_SECTION, m_WAConfFile)))
      return NPT_SUCCESS;
  }
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::ServiceBuild
+---------------------------------------------------------------------*/
template <class api_T>
void CWinamp_MediaController::ServiceBuild(api_T *&api_t, GUID factoryGUID_t)
{
  if (this->m_WASABI_API_SVC)
  {
    waServiceFactory *factory = this->m_WASABI_API_SVC->service_getServiceByGuid(factoryGUID_t);
    if (factory)
      api_t = (api_T *)factory->getInterface();
  }
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::ServiceRelease
+---------------------------------------------------------------------*/
template <class api_T>
void CWinamp_MediaController::ServiceRelease(api_T *api_t, GUID factoryGUID_t)
{
  if (this->m_WASABI_API_SVC)
  {
    waServiceFactory *factory = this->m_WASABI_API_SVC->service_getServiceByGuid(factoryGUID_t);
    if (factory)
      factory->releaseInterface(api_t);
  }
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::Start
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::Start()
{
  /* Controle de la version de Winamp */
  if(SendMessage(PluginUPNP.hwndWinampParent, WM_WA_IPC, 0, IPC_GETVERSION) < 0x5050)
  {
#ifdef UNICODE
    MessageBox(PluginUPNP.hwndLibraryParent,L"The ml_upnp plug-in requires Winamp v5.50 and up for it to work.\t\n"
  								  L"Please upgrade your Winamp client to be able to use this.",
								  CA2T(PluginUPNP.description),MB_OK|MB_ICONINFORMATION);
#else
    MessageBox(PluginUPNP.hwndLibraryParent,"The ml_upnp plug-in requires Winamp v5.50 and up for it to work.\t\n"
  								  "Please upgrade your Winamp client to be able to use this.",
								  PluginUPNP.description,MB_OK|MB_ICONINFORMATION);
#endif
	return NPT_FAILURE;
  }

  //starting point for wasabi, where services are shared
  m_WASABI_API_SVC = (api_service *)SendMessage(PluginUPNP.hwndWinampParent, WM_WA_IPC, 0, IPC_GET_API_SERVICE);
  if (m_WASABI_API_SVC == (api_service*)1) 
  {
    m_WASABI_API_SVC = NULL;
    return NPT_FAILURE;
  }

  ServiceBuild(m_WASABI_API_MEMMGR, memMgrApiServiceGuid);

  /* Récupération du nom de fichier de conf de Winamp */
  m_WAConfFile = (char*)SendMessage(PluginUPNP.hwndWinampParent, WM_WA_IPC, 0, IPC_GETINIFILE);
  if (m_WAConfFile == NULL)
    return NPT_FAILURE;
  else
  {
    this->LoadPreference();
  }

  /* Chargement des fonctions Winamp */
  if (LoadMLFunctions() == NPT_FAILURE)
    return NPT_FAILURE;

  /* Création du menu */
  if ((m_PopupMenu = LoadMenu(PluginUPNP.hDllInstance, MAKEINTRESOURCE(IDR_TREEITEMMENU))) == NULL) 
    return NPT_FAILURE;

  /* Création de la racine */
  MLTREEITEM NewTree;
  NewTree.size        = sizeof(MLTREEITEM);     // size of this struct
  NewTree.id          = 0;                      // depends on contxext
  NewTree.parentId    = 0;                      // 0 = root, or ML_TREEVIEW_ID_*
  NewTree.title       = TITLE_ROOT_ITEM;        // pointer to the buffer contained item name. 
  NewTree.titleLen    = sizeof(TITLE_ROOT_ITEM);// used for GetInfo 
  NewTree.hasChildren = TRUE;                   // TRUE - has children
  NewTree.imageIndex  = MLTREEIMAGE_BRANCH;

  SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADD);
  m_RootItemId = NewTree.id;

  this->CheckDeviceFound();

  m_IsAlive = true;

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CheckDeviceFound
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::CheckDeviceFound(void)
{
  if (m_MediaRepositories.GetEntryCount() == 0)
  {
    if (m_NoDeviceItemId == 0)
    {
      // Add an item
      MLTREEITEM NewTree;
      NewTree.size        = sizeof(MLTREEITEM);
      NewTree.id          = 0;
      NewTree.parentId    = m_RootItemId;
      NewTree.title       = WINAMP_NO_DEVICE_FOUND;
      NewTree.titleLen    = sizeof(WINAMP_NO_DEVICE_FOUND);
      NewTree.hasChildren = TRUE;
      NewTree.imageIndex  = MLTREEIMAGE_BRANCH;
      SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADD);
      m_NoDeviceItemId = NewTree.id;
    }
  }
  else
  {
    if (m_NoDeviceItemId != 0)
    {
      // Suppression de la vue Winamp
      SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) m_NoDeviceItemId, ML_IPC_DELTREEITEM);
      m_NoDeviceItemId = 0;
    }
  }
}
/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMSAdded
+---------------------------------------------------------------------*/
bool 
CWinamp_MediaController::OnMSAdded(PLT_DeviceDataReference& device) 
{
  CWinamp_Repository    Repository;

  // test if it's a media server
  PLT_Service* service;
  if (NPT_SUCCEEDED(device->FindServiceByType("urn:schemas-upnp-org:service:ContentDirectory:*", service))) 
  {
    // Issue special action upon discovering MediaConnect server
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

    //Partie Winamp
    if (m_IsAlive == true)
    {
      NPT_String uuid = device->GetUUID();

      // Add an item
      MLTREEITEM NewTree;
      NewTree.size        = sizeof(MLTREEITEM);
      NewTree.id          = 0;
      NewTree.parentId    = m_RootItemId;
      NewTree.title       = (char*) device->GetFriendlyName();
      NewTree.titleLen    = device->GetFriendlyName().GetLength();
      NewTree.hasChildren = TRUE;
      NewTree.imageIndex  = MLTREEIMAGE_BRANCH;
      SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADD);

      // Et on l'ajoute à notre liste 
      Repository.SetDeviceDataReference(device);
      Repository.SetId("0");
      Repository.SetMLId(NewTree.id);
      NPT_AutoLock lock(m_MediaRepositories);
      m_MediaRepositories.Put(uuid, Repository);

      NPT_AutoLock lock2(m_UUIDs);
      m_UUIDs.Put(NewTree.id, uuid);
    }

    this->CheckDeviceFound();
  }

  return true; 
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMSRemoved
+---------------------------------------------------------------------*/
void
CWinamp_MediaController::OnMSRemoved(PLT_DeviceDataReference& device)
{
  NPT_Result                    Res             = NPT_FAILURE   ;
  CWinamp_Repository*           Ptr_repository  = NULL          ;

  if (m_IsAlive == true)
  {
    INT_PTR id = 0;

    Res = FindMediaRepository(device->GetUUID(), Ptr_repository);
    if (Res == NPT_SUCCESS)
    {
      // Suppression de la vue Winamp
      SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) Ptr_repository->GetMLId(), ML_IPC_DELTREEITEM);

      // Et on le supprimer à notre liste 
      NPT_AutoLock lock(m_MediaRepositories);
      m_MediaRepositories.Erase(device->GetUUID());

      NPT_AutoLock lock2(m_UUIDs);
      m_UUIDs.Erase(Ptr_repository->GetMLId());
    }
    this->CheckDeviceFound();
  }
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMRAdded
+---------------------------------------------------------------------*/
bool
CWinamp_MediaController::OnMRAdded(PLT_DeviceDataReference& device)
{
  NPT_String uuid = device->GetUUID();

  // test if it's a media renderer
  PLT_Service* service;
  if (NPT_SUCCEEDED(device->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:*", service))) 
  {
    NPT_AutoLock lock(m_MediaRenderers);
    m_MediaRenderers.Put(uuid, device);
  }
  return true;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnMRRemoved
+---------------------------------------------------------------------*/
void
CWinamp_MediaController::OnMRRemoved(PLT_DeviceDataReference& device)
{
  NPT_String uuid = device->GetUUID();
  NPT_AutoLock lock(m_MediaRenderers);
  m_MediaRenderers.Erase(uuid);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::FindMediaRepository
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_MediaController::FindMediaRepository(NPT_String uuid, CWinamp_Repository*& repository)
{
  if (m_IsAlive != true)
    return NPT_FAILURE;

  NPT_AutoLock lock(m_MediaRepositories);
  if (m_MediaRepositories.Get(uuid, repository) != NPT_SUCCESS)
    return NPT_FAILURE;

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::FindUUID
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_MediaController::FindUUID(NPT_Cardinal id, NPT_String* uuid)
{
  NPT_String* uuid_tmp;

  if (m_IsAlive != true)
    return NPT_FAILURE;

  NPT_AutoLock lock(m_UUIDs);
  if (m_UUIDs.Get(id, uuid_tmp) != NPT_SUCCESS)
    return NPT_FAILURE;

  uuid->Erase(0, uuid->GetLength());
  uuid->Append(uuid_tmp->GetChars());
  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::OnDeviceRemoved
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::BrowseDevice(NPT_Cardinal id, 
                  HWND parent,
                  const char* object_id /* = NULL*/,
                  bool metadata /* = false*/)
{
  NPT_Result                    Res             = NPT_FAILURE   ;
  CWinamp_Repository*           Ptr_repository  = NULL          ;
  CWinamp_Repository            Repository                      ;
  PLT_MediaObjectListReference  BrowseResults                   ;
  MLTREEITEM                    NewTree                         ;
  PLT_DeviceDataReference       device                          ;
  NPT_String                    uuid                            ;
  NPT_String                    sId                             ;
  char                          tmp_id[256]     = {'\0'}        ;

  if (m_IsAlive != true)
    return NPT_FAILURE;

  Res = FindUUID(id, &uuid);

  if (Res == NPT_SUCCESS)
  {
    Res = FindMediaRepository(uuid, Ptr_repository);
  }

  if (Res == NPT_SUCCESS)
  {
    if (!Ptr_repository->GetDeviceDataReference().IsNull()) 
    {
      device = Ptr_repository->GetDeviceDataReference();

      // send off the browse packet and block
      Res = BrowseSync(
        Ptr_repository->GetDeviceDataReference(), 
        (const char*) Ptr_repository->GetId().GetChars(), 
        BrowseResults, 
        metadata);
    }

    if (!BrowseResults.IsNull()) 
    {
      NPT_List<PLT_MediaObject*>::Iterator item = BrowseResults->GetFirstItem();
      m_ListView.ClearAll();

      while (item) {
        if ((*item)->IsContainer()) 
        {
          // Vérifier si l'élément existe avant de l'ajouter
          if (!Ptr_repository->IsUpdated())
          {
            /* Ajout d'un Repertoire */
            NewTree.size          = sizeof(MLTREEITEM);
            NewTree.id            = 0;
            NewTree.parentId      = id;
            NewTree.title         = (char*) (*item)->m_Title.GetChars();
            NewTree.titleLen      = (*item)->m_Title.GetLength();
            NewTree.hasChildren   = TRUE;
            NewTree.imageIndex    = MLTREEIMAGE_BRANCH;
            SendMessage(PluginUPNP.hwndLibraryParent, WM_ML_IPC, (WPARAM) &NewTree, ML_IPC_TREEITEM_ADD);

            Repository.SetDeviceDataReference(device);
            Repository.SetId((*item)->m_ObjectID);
            Repository.SetMLId(NewTree.id);
            NPT_AutoLock lock(m_MediaRepositories);

            NPT_SetMemory(tmp_id, 0 , sizeof(tmp_id));
            sprintf_s(tmp_id, sizeof(tmp_id), "%s-%ld\0", device->GetUUID().GetChars(), NewTree.id);
            sId.Erase(0, sId.GetLength());
            sId.Append(tmp_id);
            m_MediaRepositories.Put(sId, Repository);

            NPT_AutoLock lock2(m_UUIDs);
            m_UUIDs.Put(NewTree.id, sId);
          }
        }
        else
        {
          // Ce n'est pas un repertoire!!!
          itemRecord ItemRecord ={0};

          if ((*item)->m_People.artists.GetItemCount() > 0)
          {
            NPT_List<PLT_PersonRole>::Iterator artist = (*item)->m_People.artists.GetFirstItem();
            ItemRecord.artist = (char*)calloc((*artist).name.GetLength()+1, sizeof(char));
            NPT_SetMemory(ItemRecord.artist, 0, sizeof(ItemRecord.artist));
            NPT_CopyString(ItemRecord.artist, (*artist).name.GetChars());
          }
          else
          {
            ItemRecord.artist = (char*)calloc(1, sizeof(char));
            NPT_SetMemory(ItemRecord.artist, 0, sizeof(ItemRecord.artist));
          }

          ItemRecord.title = (char*)calloc((*item)->m_Title.GetLength()+1, sizeof(char));
          NPT_SetMemory(ItemRecord.title, 0, sizeof(ItemRecord.title));
          NPT_CopyString(ItemRecord.title, (*item)->m_Title.GetChars());

          ItemRecord.album = (char*)calloc((*item)->m_Affiliation.album.GetLength()+1, sizeof(char));
          NPT_SetMemory(ItemRecord.album, 0, sizeof(ItemRecord.album));
          NPT_CopyString(ItemRecord.album, (*item)->m_Affiliation.album.GetChars());

          if ((*item)->m_Affiliation.genre.GetItemCount() > 0)
          {
            NPT_List<NPT_String>::Iterator genre = (*item)->m_Affiliation.genre.GetFirstItem();
            ItemRecord.genre = (char*)calloc((*genre).GetLength()+1, sizeof(char));
            NPT_SetMemory(ItemRecord.genre, 0, sizeof(ItemRecord.genre));
            NPT_CopyString(ItemRecord.genre, (*genre).GetChars());
          }
          else
          {
            ItemRecord.genre = (char*)calloc(1, sizeof(char));
            NPT_SetMemory(ItemRecord.genre, 0, sizeof(ItemRecord.genre));
          }

          if ((*item)->m_Resources.GetItemCount() > 0)
            ItemRecord.length = (*item)->m_Resources[0].m_Duration;

          if ((*item)->m_Resources.GetItemCount() > 0)
          {
            NPT_Cardinal i = 0;
            for (i=0;i<(*item)->m_Resources.GetItemCount(); i++) 
            {
              if ((*item)->m_Resources[i].m_ProtocolInfo.GetProtocol().Compare("http-get", true) == 0)
              {
                break;
              }
            }
            ItemRecord.filename = (char*)calloc((*item)->m_Resources[i].m_Uri.GetLength()+1, sizeof(char));
            NPT_CopyString(ItemRecord.filename, (*item)->m_Resources[i].m_Uri.GetChars());
          }

          NPT_AutoLock lock(m_MediaItems);
          m_MediaItems.Put((*item)->m_ObjectID, ItemRecord);
          m_ListView.AddItem((*item)->m_ObjectID, ItemRecord);
        }
        ++item;
      }

      TCHAR tmp[512];
      sprintf_s(tmp, sizeof(tmp), "There were %d items (%d files)\n", BrowseResults->GetItemCount(), m_ListView.CountItem());
      SetDlgItemText(parent, IDC_STATIC_STATUS, tmp);

      /* Mise à jour de l'information que le pere a été MAJ */
      Ptr_repository->UpdateTrueRepository();
      BrowseResults = NULL;
    }
  }

  return Res;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::MessageProc
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::MessageProc (NPT_Cardinal message_type, 
                                      NPT_Cardinal param1, 
                                      NPT_Cardinal param2, 
                                      NPT_Cardinal param3)
{
  NPT_Result            Res             = NPT_FAILURE ;
  CWinamp_Repository*   Ptr_repository  = NULL        ;
  NPT_String            uuid                          ;

  if (m_IsAlive != true)
    return NPT_FAILURE;

  // Controler que le menu concerné correspond bien à un menu de notre plugin
  if (param1 != m_RootItemId)
  {
    Res = FindUUID(param1, &uuid);
  }
  else
  {
    Res = NPT_SUCCESS;
  }

  // Le menu concerné appartient à notre plugin
  if (Res == NPT_SUCCESS)
  {
    // check for any global messages here
    if (message_type >= ML_MSG_TREE_BEGIN && message_type <= ML_MSG_TREE_END)
    {
      switch(message_type)
      {
      case ML_MSG_TREE_ONCREATEVIEW:
        if (param1 != m_RootItemId)
        {
          m_CurrentItemId = param1;
          return (NPT_Result) CreateDialogParam(PluginUPNP.hDllInstance, MAKEINTRESOURCE(IDD_VIEW_UPNP_ROOT), (HWND)param2, (DLGPROC)::MainDlgProc, (LPARAM)param1);
        }
        else
        {
          /* Creation d'une fenetre simple pour la racine */
          /* à faire */
        }
      break;
      case ML_MSG_TREE_ONCLICK:
        if (param2 == ML_ACTION_RCLICK) 
        {
          HMENU menu=GetSubMenu(m_PopupMenu,0);
          POINT p;
          GetCursorPos(&p);
          int r = TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,(HWND)param3,NULL);
          switch(r) 
          { //items that do not require the db to be loaded
            case ID_UPNPGENERALTREEITEMCONTEXT_AIDE:
              MessageBox((HWND)param3,PLUGIN_ABOUT,"About", MB_OK);
              break;
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
|   CWinamp_MediaController::LoadMLFunctions
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::LoadMLFunctions()
{
  *(void **)&WADlg_getColor=(void*)SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,1,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&WADlg_handleDialogMsgs=(void*)SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,2,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&WADlg_DrawChildWindowBorders=(void*)SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,3,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&childresize_init=(void*)SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,32,ML_IPC_SKIN_WADLG_GETFUNC);
  *(void **)&childresize_resize=(void*)SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,33,ML_IPC_SKIN_WADLG_GETFUNC);

  if ((WADlg_getColor == NULL) || (WADlg_handleDialogMsgs == NULL)
    || (WADlg_DrawChildWindowBorders == NULL)|| (childresize_init == NULL)
    || (childresize_resize == NULL))
  {
    return NPT_FAILURE;
  }
  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::UnloadMLFunctions
+---------------------------------------------------------------------*/
void 
CWinamp_MediaController::UnloadMLFunctions()
{
  *(void **)&WADlg_getColor=NULL;
  *(void **)&WADlg_handleDialogMsgs=NULL;
  *(void **)&WADlg_DrawChildWindowBorders=NULL;
  *(void **)&childresize_init=NULL;
  *(void **)&childresize_resize=NULL;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::ReloadLoadMLFunctions
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_MediaController::ReloadLoadMLFunctions()
{
  UnloadMLFunctions();
  return LoadMLFunctions();
}


/*----------------------------------------------------------------------
|   CWinamp_MediaController::playFiles
+---------------------------------------------------------------------*/
void CWinamp_MediaController::playFiles(HWND hwnd, INT_PTR enqueue)
{
  HWND listWnd = GetDlgItem(hwnd,IDC_LISTVIEW);
  if (m_ListView.CountSelectedItem() == 0) return;

  itemRecordList obj={0,};
  obj.Alloc=0;
  obj.Size=0;
  obj.Items=0;

  m_ListView.GetItemRecordList(&obj, m_MediaItems);

  if (obj.Size)
  {
    mlSendToWinampStruct s={ML_TYPE_ITEMRECORDLIST, (void*)&obj, !!enqueue};
    SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,(WPARAM)&s,ML_IPC_SENDTOWINAMP);
  }

  free(obj.Items);
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::MainDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CWinamp_MediaController::MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_Result                    Res               = NPT_SUCCESS   ;
  CWinamp_Repository*           Ptr_repository    = NULL          ;
  bool                          RecallMLFunctions = false         ;

  if (this->WADlg_handleDialogMsgs)
  {
    int ret = this->WADlg_handleDialogMsgs(hwnd,uMsg,wParam,lParam);
    if (ret) return ret;
  }
  else
    RecallMLFunctions=true;

  switch (uMsg)
  {
    case WM_INITDIALOG: 
    {
      /* skin dialog */
	  MLSKINWINDOW sw;
	  sw.skinType = SKINNEDWND_TYPE_DIALOG;
	  sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	  sw.hwndToSkin = hwnd;
	  MLSkinWindow(PluginUPNP.hwndLibraryParent, &sw);

      m_ListView.SetHwnd(GetDlgItem(hwnd,IDC_LISTVIEW));
      m_ListView.AddColumn("Object ID", -1);
      m_ListView.AddColumn("Artist", 200);
      m_ListView.AddColumn("Title", 300);
      m_ListView.AddColumn("Album", 200);
      m_ListView.AddColumn("Genre", 150);
      m_ListView.AddColumn("Duration", 70);
      m_ListView.SkinWindow();

      /* skin button */
	  sw.skinType = SKINNEDWND_TYPE_BUTTON;
	  sw.style = SWS_USESKINCOLORS | SWS_USESKINCURSORS | SWS_USESKINFONT;
	  sw.hwndToSkin = GetDlgItem(hwnd,IDC_BUTTON_REFRESH);
	  MLSkinWindow(PluginUPNP.hwndLibraryParent, &sw);
      sw.hwndToSkin = GetDlgItem(hwnd,IDC_BUTTON_PLAY);
	  MLSkinWindow(PluginUPNP.hwndLibraryParent, &sw);
	  sw.hwndToSkin = GetDlgItem(hwnd,IDC_BUTTON_ENQUEUE);
	  MLSkinWindow(PluginUPNP.hwndLibraryParent, &sw);

      // récupération de l'identifiant selectionné
      this->BrowseDevice ((NPT_Cardinal)lParam, hwnd);

      if (this->childresize_init)
        this->childresize_init(hwnd, resize_rlist, sizeof(resize_rlist)/sizeof(resize_rlist[0]));
      else
        RecallMLFunctions=true;
    }
    break;
    case WM_SIZE:
    if (wParam != SIZE_MINIMIZED) 
    {
      if (this->childresize_resize)
        this->childresize_resize(hwnd, resize_rlist, sizeof(resize_rlist)/sizeof(resize_rlist[0]));
      else
        RecallMLFunctions=true;
    }
    break;
    case WM_PAINT:
    {
      int tabs[] = { IDC_TEXT_QUICKSEARCH|DCW_SUNKENBORDER, IDC_LISTVIEW|DCW_SUNKENBORDER };
      if (this->WADlg_DrawChildWindowBorders)
        this->WADlg_DrawChildWindowBorders(hwnd, tabs, sizeof(tabs)/sizeof(tabs[0]));
      else
        RecallMLFunctions=true;
    }
    break;
    case WM_NOTIFY:
    {
      if (((LPNMHDR)lParam)->idFrom==IDC_LISTVIEW)
      {
        if (((LPNMHDR)lParam)->code == NM_RCLICK)
        {
          HMENU menu=GetSubMenu(m_PopupMenu,1);
          POINT p;
          GetCursorPos(&p);
          int r = TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON|TPM_LEFTBUTTON|TPM_NONOTIFY,p.x,p.y,0,hwnd,NULL);
          switch(r) 
          { //items that do not require the db to be loaded
            case ID_UPNPITEMCONTEXT_PLAY:
              this->playFiles(hwnd, CWinamp_PLAY);
              break;
            case ID_UPNPITEMCONTEXT_ENQUEUE:
              this->playFiles(hwnd, CWinamp_ENQUEUE);
              break;
          }
        }
        else if (((LPNMHDR)lParam)->code == NM_DBLCLK)
        {
          this->playFiles(hwnd, !!(GetAsyncKeyState(VK_SHIFT)&0x8000));
        }
        else if (((LPNMHDR)lParam)->code == LVN_BEGINDRAG)
        {
          SetCapture(hwnd);
        }
        else if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)
        {
          m_ListView.Sort(lParam);
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
          this->playFiles(hwnd, !!(GetAsyncKeyState(VK_SHIFT)&0x8000));
        }
      }
    }
    break;
    case WM_COMMAND:
      switch(LOWORD(wParam)) 
      {
        case IDC_BUTTON_REFRESH:
          this->BrowseDevice (m_CurrentItemId, hwnd);
        break;
        case IDC_BUTTON_PLAY:
          this->playFiles(hwnd, CWinamp_PLAY);
        break;
        case IDC_BUTTON_ENQUEUE:
          this->playFiles(hwnd, CWinamp_ENQUEUE);
        break;
        case IDC_TEXT_QUICKSEARCH:
          TCHAR buffer[256];
          NPT_SetMemory(buffer, 0, sizeof(buffer));
          if (HIWORD(wParam) == EN_UPDATE)
          {
            GetDlgItemText(hwnd,IDC_TEXT_QUICKSEARCH, buffer, sizeof(buffer));
            m_ListView.Search(buffer, m_MediaItems);
          }
          else if(HIWORD(wParam) == EN_SETFOCUS)
          {
            SetDlgItemText(hwnd, IDC_TEXT_QUICKSEARCH, "");
            m_ListView.Search(buffer, m_MediaItems);
          }
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
          mlDropItemStruct m={ML_TYPE_ITEMRECORDLIST,NULL,0};
          m.p=p;
          m.flags=ML_HANDLEDRAG_FLAG_NOCURSOR;

          SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDRAG);
          if (m.result>0)
          {
            itemRecordList obj={0,}; 
            m_ListView.GetItemRecordList(&obj, m_MediaItems);
            if (obj.Size)
            {
              //fill in this itemCacheObject if you want to provide drag&drop out of the window
              m.flags=0;
              m.result=0;
              m.data=(void*)&obj;
              SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDROP);
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
        mlDropItemStruct m={ML_TYPE_ITEMRECORDLIST,NULL,0};
        m.p=p;
        HWND h=WindowFromPoint(p);
        if (!h || h == hwnd || IsChild(hwnd,h))
        {
          SetCursor(LoadCursor(NULL,IDC_NO));
        }
        else 
        {
          SetCursor(LoadCursor(GetModuleHandle("gen_ml.dll"), MAKEINTRESOURCE(ML_IDC_DRAGDROP)));
          SendMessage(PluginUPNP.hwndLibraryParent,WM_ML_IPC,(WPARAM)&m,ML_IPC_HANDLEDRAG);
        }
      }
    break;
    case WM_DESTROY:
      m_CurrentItemId = 0;

      MLUnskinWindow(PluginUPNP.hwndLibraryParent, GetDlgItem(hwnd,IDC_BUTTON_ENQUEUE));
      MLUnskinWindow(PluginUPNP.hwndLibraryParent, GetDlgItem(hwnd,IDC_BUTTON_PLAY));
      MLUnskinWindow(PluginUPNP.hwndLibraryParent, GetDlgItem(hwnd,IDC_BUTTON_REFRESH));

      MLUnskinWindow(PluginUPNP.hwndLibraryParent, GetDlgItem(hwnd,IDC_LISTVIEW));
      MLUnskinWindow(PluginUPNP.hwndLibraryParent, hwnd);
    break;
  }

  if (RecallMLFunctions == true)
      Res = this->ReloadLoadMLFunctions();

  return Res;
}
