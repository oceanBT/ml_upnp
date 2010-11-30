/*****************************************************************
|
|   Winamp_Database.h - Include file for Database Class
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
 Winamp Database
 */

#ifndef _WINAMP_DATABASE_H_
#define _WINAMP_DATABASE_H_

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#undef ASSERT
#include <nde/nde.h>
#include "winamp_listview.h"

/*----------------------------------------------------------------------
|   Define
+---------------------------------------------------------------------*/
#define CONFIG_DATABASE     "upnp_DataBase"
#define WINAMP_DB_DEVICE    "device"
#define WINAMP_DB_ITEM      "main"

/*----------------------------------------------------------------------
|   Enumeration
+---------------------------------------------------------------------*/
enum {
  TABLE_MSDEVICE,
  TABLE_ITEM,
  TABLE_MAXNUM
};


/*----------------------------------------------------------------------
 |   CWinamp_Database
 +---------------------------------------------------------------------*/
class CWinamp_Database
{
public:
  /**
    Constructeur
  */
  CWinamp_Database(winampMediaLibraryPlugin *plugin); //OK

  /**
    Destructeur
  */
  virtual ~CWinamp_Database(void); //OK

  /**
    Verifie la présence d'un serveur media enregistré en base
  */
  NPT_Result  CheckMSDevice(NPT_String uuid_ref); //OK
  NPT_Result  CheckMSDevice(NPT_String uuid_ref, NPT_Int32 *id_tree); //OK
  NPT_Result  CheckMSDevice(NPT_Int32 id_tree_ref, NPT_String *uuid_ref); //OK

  /**
    Verifie si le serveur a déjà été scanné
  */
  NPT_Int16  CheckFirstScanMSDevice(NPT_String uuid_ref); //OK

  /**
    Ajoute un serveur media
  */
  NPT_Result  AddMSDevice(PLT_DeviceDataReference& device); //OK

  /**
    Mise à jour de l'identifiant dans le treeview pour l'appareil indiqué
  */
  NPT_Result  UpdateIdTreeMSDevice(NPT_String uuid_ref, NPT_Int32 id_tree); //OK

  /**
    Mise à jour du nombre d'item pour l'appareil indiqué
  */
  NPT_Result  UpdateNbItemMSDevice(NPT_String uuid_ref, NPT_Int32 nb_item); //OK

  /**
    Mise à jour du scan pour l'appareil indiqué
  */
  NPT_Result  UpdateScanMSDevice(NPT_String uuid_ref, NPT_Int32 scanned); //OK

  /**
    Suppression d'un serveur media
  */
  NPT_Result  RemoveMSDevice(NPT_String uuid_ref); //OK

  /**
    Compte le nombre de serveurs media
  */
  NPT_Int32  GetActifDeviceCount(void); //OK

  /**
    Verifie la présence d'un élément media enregistré en base par rapport à l'URL
  */
  NPT_Result  CheckItem(NPT_String uuid_ref, PLT_MediaObject *item);  //OK

  /** 
    Ajout d'un element media
  */
  NPT_Int32   AddItem(NPT_String uuid_ref, PLT_MediaObject *item);  //OK

  /** 
    Ouverture d'une table
  */
  NPT_Int32   OpenTable(NPT_Int32  table);  // OK

  /** 
    Fermeture d'une table
  */
  NPT_Int32   CloseTable(NPT_Int32  table); //OK

  /** 
    Chargement de la listView
  */
  NPT_Int32   LoadData(CWinamp_ListView* lv, NPT_String uuid_ref);  // OK

  /**
    Chargement de la liste des éléments à lire
  */
  NPT_Int32   SelectItem(NPT_String Object_id_ref, itemRecord* item); //OK

  /**
    Synchronisation de la bas de données
  */
  NPT_Result  Sync(void);

private:
  /**
    Ajoute d'une valeur dans un champ
  */
  NPT_Result  SetValue(NPT_Int32 nom_table, Scanner * scanner, NPT_Int32 id, NPT_Int32 value); //OK
  NPT_Result  SetValue(NPT_Int32 nom_table, Scanner * scanner, NPT_Int32 id, const NPT_Int8* value); //OK

  /** 
    Recherche de la ligne concernée pour l'appareil indiqué
  */
  Scanner*   GetScannerForDevice(NPT_String uuid_ref); //OK

  /** 
    Recherche de la ligne concernée pour l'item indiqué
  */
  Scanner*   GetScannerForItem(NPT_String Object_id); //OK

private:

  // Adresse du plugin
  winampMediaLibraryPlugin            *m_PluginUPNP;

  // Base de données
  Database                            *m_DB;

  // Table des serveurs media 
  Table                               *m_MSTable;

  // Nom de la table des serveurs media
  NPT_String                          m_MSTableName;

  // Nom de l'index de la table des serveurs media
  NPT_String                          m_MSTableIndexName;

  // Table des chansons
  Table                               *m_ItemTable;

  // Nom de la table des chansons
  NPT_String                          m_ItemTableName;

  // Nom de l'index de la table des chansons
  NPT_String                          m_ItemTableIndexName;
};

#endif /* _WINAMP_DATABASE_H_ */