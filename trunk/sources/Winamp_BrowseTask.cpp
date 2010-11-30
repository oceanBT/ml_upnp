/*****************************************************************
|
|   Winamp_BrowseTask.cpp - Core file for Browse Task Class
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
 Winamp browse task
 */

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "StdAfx.h"
#include "resource.h"
#include "Winamp_BrowseTask.h"
#include "PltSyncMediaBrowser.h"
#include "proc.h"

NPT_SET_LOCAL_LOGGER("ml_upnp.Winamp_BrowseTask")

/*----------------------------------------------------------------------
|   CWinamp_BrowseTask::CWinamp_BrowseTask
+---------------------------------------------------------------------*/
CWinamp_BrowseTask::CWinamp_BrowseTask (HWND                        hwnd,
                                        NPT_Int32                   firstUse,
                                        CWinamp_MediaController*    mediaController,
                                        NPT_Int32                   nodeId /* = -1 */)
{
  NPT_LOG_FINEST("CWinamp_BrowseTask::CWinamp_BrowseTask");

  m_MediaController = mediaController;
  m_PluginUPNP = mediaController->GetPlugin();
  m_DB = mediaController->GetDatabase();
  m_Device = mediaController->GetCurrentDevice();
  m_NodeId = nodeId;
  m_FirstUse = firstUse;
  m_Hwnd = hwnd;
}

/*----------------------------------------------------------------------
|   CWinamp_BrowseTask::~CWinamp_BrowseTask
+---------------------------------------------------------------------*/
CWinamp_BrowseTask::~CWinamp_BrowseTask(void)
{
  NPT_Int8 buf[256];

  NPT_LOG_FINEST("CWinamp_BrowseTask::~CWinamp_BrowseTask");

  if (m_NodeId != -1)
  {
    MLTREEITEMINFO TreeInfo;
    TreeInfo.handle = (UINT_PTR) SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)m_NodeId, ML_IPC_TREEITEM_GETHANDLE);
    TreeInfo.mask = MLTI_TEXT;
    TreeInfo.item.size = sizeof(MLTREEITEM);
    TreeInfo.item.id = m_NodeId;

    memset(buf, 0, sizeof(buf));
    NPT_FormatString(buf, sizeof(buf), "%s\0", m_Device->GetFriendlyName().GetChars());
    TreeInfo.item.title = buf;
    TreeInfo.item.titleLen = sizeof(buf) - 1;
    SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)&TreeInfo, ML_IPC_TREEITEM_SETINFO);
  }

  if (m_Hwnd)
  {
    NPT_SetMemory(buf, 0, sizeof(buf));
    NPT_CopyString(buf, "Fin du scan.");
    SetDlgItemText(m_Hwnd, IDC_STATIC_ITEM, buf);
  }

  if (m_DB)
  {
    m_DB->UpdateNbItemMSDevice(m_Device->GetUUID(), m_NbItem);
    m_DB->UpdateScanMSDevice(m_Device->GetUUID(), 1);
    m_DB->Sync();
  }

  if (m_Hwnd)
    EndDialog(m_Hwnd, 0);
}

/*----------------------------------------------------------------------
|   CWinamp_BrowseTask::DoRun
+---------------------------------------------------------------------*/
void
CWinamp_BrowseTask::DoRun(void)
{
  NPT_Int8 buf[256];

  NPT_LOG_FINEST("CWinamp_BrowseTask::DoRun");
  NPT_CHECK_POINTER_LABEL_FATAL(m_DB, done);
  NPT_CHECK_POINTER_LABEL_FATAL(m_MediaController, done);

  m_NbItem = 0;
  
  if (m_NodeId != -1)
  {
    MLTREEITEMINFO TreeInfo;
    TreeInfo.handle = (UINT_PTR) SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)m_NodeId, ML_IPC_TREEITEM_GETHANDLE);
    TreeInfo.mask = MLTI_TEXT;
    TreeInfo.item.size = sizeof(MLTREEITEM);
    TreeInfo.item.id = m_NodeId;
    memset(buf, 0, sizeof(buf));
    NPT_FormatString(buf, sizeof(buf), "%s (scan en cours...)\0", m_Device->GetFriendlyName().GetChars());
    TreeInfo.item.title = buf;
    TreeInfo.item.titleLen = sizeof(buf) - 1;
    SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)&TreeInfo, ML_IPC_TREEITEM_SETINFO);
  }

  if (m_Hwnd)
  {
    NPT_SetMemory(buf, 0, sizeof(buf));
    NPT_CopyString(buf, "Scan en cours...");
    SetDlgItemText(m_Hwnd, IDC_STATIC_ITEM, buf);
  }

  m_Abort = 0;
  RecursiveBrowse(NPT_String::NPT_String("0"));

done:
  NPT_LOG_INFO("Impossible de démarrer la tache de scan à cause de pointeur non défini");
  return;
}

/*----------------------------------------------------------------------
|   CWinamp_BrowseTask::RecursiveBrowse
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_BrowseTask::RecursiveBrowse(NPT_String object_id)
{
  PLT_MediaObjectListReference        BrowseResults;
  NPT_Cardinal                        i = 0;
  NPT_Int8 buf[256];

  if (!IsAborting(0)) 
  {
    NPT_LOG_FINEST("CWinamp_BrowseTask::RecursiveBrowse");

    m_MediaController->BrowseSync(m_Device, object_id.GetChars(), BrowseResults);

    if (!BrowseResults.IsNull()) 
    {
      NPT_List<PLT_MediaObject*>::Iterator item = BrowseResults->GetFirstItem();
      while ((item) && (!IsAborting(0)))
      {
        if ((*item)->IsContainer()) 
        {
          if (m_Hwnd)
          {
            NPT_SetMemory(buf, 0, sizeof(buf));
            NPT_FormatString(buf, sizeof(buf), "Scan du répertoire %s", (*item)->m_Title.GetChars());
            SetDlgItemText(m_Hwnd, IDC_STATIC_ITEM, buf);
          }
          RecursiveBrowse((*item)->m_ObjectID);
        }
        else
        {
          if ((*item)->m_Resources.GetItemCount() > 0)
          {
            for (i=0;i < (*item)->m_Resources.GetItemCount(); i++) 
            {
              if ((*item)->m_Resources[i].m_ProtocolInfo.GetProtocol().Compare("http-get", true) == 0)
              {
                // On enregistre en base de données
                if (strcmp((*item)->GetUPnPClass((*item)->m_Resources[i].m_Uri), "object.item.audioItem.musicTrack") == 0)
                {
                  if (NPT_FAILED(m_DB->CheckItem(m_Device->GetUUID(), *item)))
                  {
                    if (m_Hwnd)
                    {
                      NPT_SetMemory(buf, 0, sizeof(buf));
                      NPT_FormatString(buf, sizeof(buf), "Ajout du média %s", (*item)->m_Title.GetChars());
                      SetDlgItemText(m_Hwnd, IDC_STATIC_ITEM, buf);
                    }

                    m_NbItem++;
                    m_DB->AddItem(m_Device->GetUUID(), (*item));

                    if (m_NodeId != -1)
                    {
                      //Partie Winamp
                      MLTREEITEMINFO TreeInfo;
                      TreeInfo.handle = (UINT_PTR) SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)m_NodeId, ML_IPC_TREEITEM_GETHANDLE);
                      TreeInfo.mask = MLTI_TEXT;
                      TreeInfo.item.size = sizeof(MLTREEITEM);
                      TreeInfo.item.id = m_NodeId;
                      NPT_SetMemory(buf, 0, sizeof(buf));
                      NPT_FormatString(buf, sizeof(buf), "%s (%ld)\0", m_Device->GetFriendlyName().GetChars(), m_NbItem);
                      TreeInfo.item.title = buf;
                      TreeInfo.item.titleLen = sizeof(buf) - 1;
                      SendMessage(m_PluginUPNP->hwndLibraryParent, WM_ML_IPC, (WPARAM)&TreeInfo, ML_IPC_TREEITEM_SETINFO);
                    }
                  }
                }
              }
            }
          }
        }
        ++item;
      }
      BrowseResults->Clear();
    }
  }

  return NPT_SUCCESS;
}