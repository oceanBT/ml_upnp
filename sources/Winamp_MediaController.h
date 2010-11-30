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

/** @file
 UPnP Media Controller
 */

#ifndef _WINAMP_MEDIACONTROLLER_H_
#define _WINAMP_MEDIACONTROLLER_H_
#pragma once

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Platinum.h"
#include "PltSyncMediaBrowser.h"
#include "PltMediaController.h"
#include "Winamp_Database.h"
#include "Winamp_ListView.h"

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
class CWinamp_MediaController : public PLT_SyncMediaBrowser,
                                public PLT_MediaController,
                                public PLT_MediaControllerDelegate
{
public:

  /**
    Constructeur : Creer le controleur A/V UPnP.
	@pointeur du Plugin
    @param Point de controle
  */
  CWinamp_MediaController(winampMediaLibraryPlugin *plugin, 
                          PLT_CtrlPointReference& ctrlPoint);

  /**
    Destructeur
  */
  virtual     ~CWinamp_MediaController(void);

  /**
    Gestion des messages
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  NPT_Result  MessageProc(NPT_Int32 message_type, NPT_Int32 param1, NPT_Int32 param2, NPT_Int32 param3);

  /**
    Gestion des process
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  LRESULT CALLBACK  MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  /**
    Gestion des process pour la fenetre Root
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  LRESULT CALLBACK  MainRootDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  /**
   Joue le morceau selectionné
   @param hwnd Handle de la fenetre
   @uuid Identifiant du device
  */
  void PlayTo(NPT_Int32 id_device, PLT_MediaObject* track);

  /**
    Fonction retournant l'adresse du plugin
  */
  winampMediaLibraryPlugin* GetPlugin(void) { return m_PluginUPNP; } //OK

  /**
    Fonction retournant l'adresse de la base de donnnées
  */
  CWinamp_Database* GetDatabase(void) { return m_DB; } //OK

  /**
    Fonction retournant l'adresse de la base de donnnées
  */
  PLT_DeviceDataReference GetCurrentDevice(void) { return m_CurrentMediaServer; } //OK

  /**
    Fonction retournant l'adresse du gestionnaire de tache
  */
  PLT_TaskManager* GetTaskManager(void) { return m_TaskManager; } //OK

  /**
    Fonction retournant l'adresse de la base de donnnées
  */
  NPT_Int32 GetCurrentId(void) { return m_CurrentItemId; } //OK

private:
  /**
   Fonction de création de la liste d'éléments à jouer
  */
  void        GetItemRecordList(itemRecordList *list);

  /**
    Fonction pour jouer un morceau
  */
  void        play(HWND hwnd, INT_PTR enqueue);

  /**
    Fonction appelée lors de la decouverte d'un Serveur A/V
  */
  bool        OnMSAdded(PLT_DeviceDataReference& device); //OK

  /**
    Fonction appelée lors de l'arret d'un Serveur A/V
  */
  void        OnMSRemoved(PLT_DeviceDataReference& device); //OK

  /**
   Supprime/Ajoute le menu "(- No Device Found -)" en cas
   de Présence/Absence de Serveur A/V
  */
  void        CheckMSDeviceFound(void); //OK

  /**
    Fonction appelée lors de la decouverte d'un Renderer A/V
  */
  bool        OnMRAdded(PLT_DeviceDataReference& device);

  /**
    Fonction appelée lors de l'arret d'un Renderer A/V
  */
  void        OnMRRemoved(PLT_DeviceDataReference& device);

  
  /**
   Supprime/Ajoute le menu "(- No MediaRender Device Found -)" en cas
   de Présence/Absence de Renderer A/V
  */
  void        CheckMRDeviceFound(void);

  /* Pointeur des fonctions Winamp pour la librairie ML */
  void          (*childresize_init)(HWND hwndDlg, ChildWndResizeItem *list, NPT_Cardinal num);
  void          (*childresize_resize)(HWND hwndDlg, ChildWndResizeItem *list, NPT_Cardinal num);
  NPT_Int32     (*WADlg_getColor)(NPT_Cardinal idx);
  NPT_Int32     (*WADlg_handleDialogMsgs)(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam); 
  void          (*WADlg_DrawChildWindowBorders)(HWND hwndDlg, NPT_Cardinal *tab, NPT_Cardinal tabsize);

private:
  // Adresse du plugin
  winampMediaLibraryPlugin            *m_PluginUPNP;

  // Nom du fichier ini de Winamp
  NPT_String                          m_WAConfFile;

  // Nom du fichier de Configuration de ml_upnp
  TCHAR                               m_MLFile[128];

  // UUID en cours d'utilsation
  NPT_String                          m_CurrentUUID;

  // Base de données
  CWinamp_Database                    *m_DB;

  // Indice de la racine du TreeView de la ML de Winamp
  NPT_Int32                           m_RootItemId;

  // Identifiant de l'item "No device Found"
  NPT_Int32                           m_NoDeviceItemId;

  // Gestion de tache pour la lecture asynchrone des appareils
  PLT_TaskManager*                    m_TaskManager;

  // Appareil en cours d'utilisation
  PLT_DeviceDataReference             m_CurrentMediaServer;

  // Identifiant de l'Indice en cours de selection
  NPT_Int32                           m_CurrentItemId;

  // ListView de la fenetre
  CWinamp_ListView                    m_ListView;

  // Menu Popup
  HMENU                               m_PopupMenu;
};

#endif /* _WINAMP_MEDIACONTROLLER_H_ */