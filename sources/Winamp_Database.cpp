/*****************************************************************
|
|   Winamp_Database.cpp - Core file for Database Class
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

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "StdAfx.h"
#include "Winamp_Database.h"
#include "NptTime.h"

/* Entete Winamp SDK */
#include "wa_ipc.h"

/*----------------------------------------------------------------------
|   Enumeration
+---------------------------------------------------------------------*/
enum {
  FID_MS_UUID,
  FID_MS_PARENTUUID,
  FID_MS_URLBASEIP,
  FID_MS_URLBASEPORT,
  FID_MS_DEVICETYPE,
  FID_MS_FRIENDLYNAME,
  FID_MS_IDTREE,
  FID_MS_ACTIF,
  FID_MS_NB_ITEM,
  FID_MS_SCANNED,
  FID_MS_MAXNUM
};

enum {
  FID_OBJECT_ID,
  FID_ITEM_MS_UUID,
  FID_ITEM_FILENAME,
  FID_ITEM_TITLE,
  FID_ITEM_ARTIST,
  FID_ITEM_ALBUM,
  FID_ITEM_YEAR,
  FID_ITEM_GENRE,
  FID_ITEM_TRACKNO,
  FID_ITEM_LENGTH,
  FID_ITEM_TYPE,
  FID_ITEM_LASTUPD,
  FID_ITEM_LASTPLAY,
  FID_ITEM_RATING,
  FID_ITEM_PLAYCOUNT,
  FID_ITEM_FILETIME,
  FID_ITEM_FILESIZE,
  FID_ITEM_BITRATE,
  FID_ITEM_MAXNUM
};

/*----------------------------------------------------------------------
|   Structure
+---------------------------------------------------------------------*/
struct SFieldData {
  const char    *name;
  NPT_UInt8     type;
  NPT_Cardinal  use;
};

/*----------------------------------------------------------------------
|   Data
+---------------------------------------------------------------------*/
SFieldData MSFieldData[] = {
  { "UUID", FIELD_STRING, 1 },
  { "ParentUUID", FIELD_STRING, 1 },
  { "URLBaseIP", FIELD_STRING, 1 },
  { "URLBasePort", FIELD_INTEGER, 1 },
  { "DeviceType", FIELD_INTEGER, 1 },
  { "FriendlyName", FIELD_STRING, 1 },
  { "IDTree", FIELD_INTEGER, 1 },
  { "NbItem", FIELD_INTEGER, 1 },
  { "Scanned", FIELD_INTEGER, 1 },
  { "Actif", FIELD_INTEGER, 1 },
};

SFieldData ItemFieldData[] = {
  { "ObjectID", FIELD_STRING, 1 },
  { "MS_UUID", FIELD_STRING, 1 },
  { "filename", FIELD_STRING, 1 },
  { "title", FIELD_STRING, 1 },
  { "artist", FIELD_STRING, 1 },
  { "album", FIELD_STRING, 1 },
  { "year", FIELD_INTEGER, 1 },
  { "genre", FIELD_STRING, 1 },
  { "trackno", FIELD_INTEGER, 1 },
  { "length", FIELD_INTEGER, 1 },
  { "type", FIELD_INTEGER, 1 },
  { "lastupd", FIELD_INTEGER, 1 },
  { "lastplay", FIELD_INTEGER, 1 },
  { "rating", FIELD_INTEGER, 1 },
  { "playCount", FIELD_INTEGER, 1 },
  { "fileTime", FIELD_INTEGER, 1 },
  { "fileSize", FIELD_INTEGER, 1 },
  { "bitRate", FIELD_INTEGER, 1 },
  { "Actif", FIELD_INTEGER, 1 },
};

NPT_SET_LOCAL_LOGGER("ml_upnp.Winamp_Database")

/*----------------------------------------------------------------------
|   CWinamp_Database::CWinamp_Database
+---------------------------------------------------------------------*/
CWinamp_Database::CWinamp_Database(winampMediaLibraryPlugin *plugin)
{
  TCHAR         DefaultDBDirectory[128] = {0};
  TCHAR         DBDirectory[128] = {0};
  LPWSTR        WAConfFile = NULL;
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::CWinamp_Database");

  m_PluginUPNP = plugin;
  m_DB = NULL;
  m_MSTable = NULL;
  memset(m_MSTableName, 0, sizeof(m_MSTableName));
  memset(m_MSTableIndexName, 0, sizeof(m_MSTableIndexName));
  m_ItemTable = NULL;
  memset(m_ItemTableName, 0, sizeof(m_ItemTableName));
  memset(m_ItemTableIndexName, 0, sizeof(m_ItemTableIndexName));

#ifdef DEBUG
  NDE_Init();
#endif

  //Fichier ini de Winamp
  WAConfFile = (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETINIFILEW*/ 1334);

  // Récupération du paramétrage
  StringCchPrintfW(DefaultDBDirectory, sizeof(DefaultDBDirectory), L"%s\\ml_upnp", (LPWSTR)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,/*IPC_GETPLUGINDIRECTORY*/ 1336));
  GetPrivateProfileStringW(CONFIG_SECTION, CONFIG_DATABASE, DefaultDBDirectory, DBDirectory, sizeof(DBDirectory), WAConfFile);

  // Enregistrement du paramétrage
  WritePrivateProfileStringW(CONFIG_SECTION,CONFIG_DATABASE,DBDirectory,WAConfFile);

  // Création du repertoire
  CreateDirectoryW(DBDirectory, NULL);

  //Nommage des tables et des index
  StringCchPrintfW(m_MSTableName, sizeof(m_MSTableName), L"%s\\%s.dat", DBDirectory, WINAMP_DB_DEVICE);
  StringCchPrintfW(m_MSTableIndexName, sizeof(m_MSTableIndexName), L"%s\\%s.idx", DBDirectory, WINAMP_DB_DEVICE);

  StringCchPrintfW(m_ItemTableName, sizeof(m_ItemTableName), L"%s\\%s.dat", DBDirectory, WINAMP_DB_ITEM);
  StringCchPrintfW(m_ItemTableIndexName, sizeof(m_ItemTableIndexName), L"%s\\%s.idx", DBDirectory, WINAMP_DB_ITEM);

  // Création de la base de données
  NPT_LOG_INFO("Création de la base de données en cours...");
  m_DB = NDE_CreateDatabase(m_PluginUPNP->hDllInstance);
  if (m_DB)
  {
    m_MSTable = NDE_Database_OpenTable(m_DB, m_MSTableName, m_MSTableIndexName, NDE_OPEN_ALWAYS, NDE_CACHE);
    if (m_MSTable == NULL)
      NPT_LOG_INFO_2("Creation table %s(%s) impossible", m_MSTableName, m_MSTableIndexName);
    else
    {
      NPT_LOG_INFO_2("table %s(%s) créée", m_MSTableName, m_MSTableIndexName);
      for (int i=0; i < FID_MS_MAXNUM; i++) 
      {
        if (MSFieldData[i].use)
          NDE_Table_NewColumn(m_MSTable, i, (char*)MSFieldData[i].name, MSFieldData[i].type);
      }
      NDE_Table_AddIndexByID(m_MSTable, FID_MS_UUID, "UUID");
      NDE_Table_PostColumns(m_MSTable);
    }

    m_ItemTable = NDE_Database_OpenTable(m_DB, m_ItemTableName, m_ItemTableIndexName, NDE_OPEN_ALWAYS, NDE_CACHE);
    if (m_ItemTable == NULL)
      NPT_LOG_INFO_2("Creation table %s(%s) impossible", m_ItemTableName, m_ItemTableIndexName);
    else
    {
      NPT_LOG_INFO_2("table %s(%s) créée", m_ItemTableName, m_ItemTableIndexName);
      for (int i=0; i < FID_ITEM_MAXNUM; i++) 
      {
        if (ItemFieldData[i].use) 
          NDE_Table_NewColumn(m_ItemTable, i, (char*)ItemFieldData[i].name, ItemFieldData[i].type);
      }
      NDE_Table_AddIndexByID(m_ItemTable, FID_OBJECT_ID, "Item");
      NDE_Table_PostColumns(m_ItemTable);
    }

    NPT_LOG_INFO("Création terminée");
  }
  else
    NPT_LOG_FATAL("Impossible de créer la base de données!");

  // MAJ de la table des appareils pour rendre tous les appareils en base
  // inactifs.
  NPT_LOG_INFO("MAJ des données...");
  scanner = NDE_Table_CreateScanner(m_MSTable);
  if (scanner != NULL)
  {
    for (NDE_Scanner_First(scanner);!(NDE_Scanner_EOF(scanner));NDE_Scanner_Next(scanner))
    {
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
      NDE_Scanner_Post(scanner);
    }
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  // Synchronisation des tables
  Sync();

  NPT_LOG_INFO("MAJ des données terminée.");
}

/*----------------------------------------------------------------------
|   CWinamp_Database::OpenTable
+---------------------------------------------------------------------*/
NPT_Int32
CWinamp_Database::OpenTable(NPT_Int32 table)
{
  NPT_LOG_FINEST("CWinamp_Database::OpenTable");

  switch (table)
  {
  case TABLE_MSDEVICE:
    if (!m_MSTable)
    {
      NPT_LOG_INFO_1("Ouverture de la table %s", m_MSTableName);
      m_MSTable = NDE_Database_OpenTable(m_DB, m_MSTableName, m_MSTableIndexName, NDE_OPEN_EXISTING, NDE_CACHE);
      if (m_MSTable == NULL)
      {
        NPT_LOG_INFO_2("Ouverture table %s(%s) impossible", m_MSTableName, m_MSTableIndexName);
        return NPT_FAILURE;
      }
    }
    break;
  case TABLE_ITEM:
    if (!m_ItemTable)
    {
      NPT_LOG_INFO_1("Ouverture de la table %s", m_ItemTableName);
      m_ItemTable = NDE_Database_OpenTable(m_DB, m_ItemTableName, m_ItemTableIndexName, NDE_OPEN_EXISTING, NDE_CACHE);
      if (m_ItemTable == NULL)
      {
        NPT_LOG_INFO_2("Ouverture table %s(%s) impossible", m_ItemTableName, m_ItemTableIndexName);
        return NPT_FAILURE;
      }
    }
    break;
  default:
    NPT_LOG_INFO("Incohérence dans le choix de la table");
    return NPT_FAILURE;
  }
  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CloseTable
+---------------------------------------------------------------------*/
NPT_Int32
CWinamp_Database::CloseTable(NPT_Int32 table)
{
  NPT_LOG_FINEST("CWinamp_Database::CloseTable");

  switch (table)
  {
  case TABLE_MSDEVICE:
    NPT_LOG_INFO_1("Fermeture de la table %s", m_MSTableName);
    if (m_MSTable)
      NDE_Database_CloseTable(m_DB, m_MSTable);
    m_MSTable = NULL;
    break;
  case TABLE_ITEM:
    NPT_LOG_INFO_1("Fermeture de la table %s", m_ItemTableName);
    if (m_ItemTable)
      NDE_Database_CloseTable(m_DB, m_ItemTable);
    m_ItemTable = NULL;
    break;
  default:
    NPT_LOG_INFO("Incohérence dans le choix de la table");
    break;
  }
  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::~CWinamp_Database
+---------------------------------------------------------------------*/
CWinamp_Database::~CWinamp_Database(void)
{
  NPT_LOG_FINEST("CWinamp_Database::~CWinamp_Database");
  if (m_DB)
  {
    CloseTable(TABLE_MSDEVICE);
    CloseTable(TABLE_ITEM);
  }

  NDE_DestroyDatabase(m_DB);

  memset(m_MSTableName, 0, sizeof(m_MSTableName));
  memset(m_MSTableIndexName, 0, sizeof(m_MSTableIndexName));
  memset(m_ItemTableName, 0, sizeof(m_ItemTableName));
  memset(m_ItemTableIndexName, 0, sizeof(m_ItemTableIndexName));

#ifdef DEBUG
  NDE_Quit();
#endif
}

/*----------------------------------------------------------------------
|   CWinamp_Database::Sync
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::Sync(void)
{
  NPT_LOG_FINEST("CWinamp_Database::Sync");

  NPT_CHECK_POINTER_FATAL(m_MSTable);
  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  NDE_Table_Sync(m_MSTable);
  NDE_Table_Sync(m_ItemTable);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CheckMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::CheckMSDevice(NPT_String uuid_ref)
{
  NPT_LOG_FINEST("CWinamp_Database::CheckMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'UUID : %s.", uuid_ref.GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  nde_scanner_t scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  NPT_LOG_INFO("Enregistrement trouvé.");
  return NPT_SUCCESS;

done:
  NPT_LOG_INFO("Enregistrement non trouvé.");
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CheckMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::CheckMSDevice(NPT_String uuid_ref, NPT_Int32 *id_tree)
{
  nde_scanner_t scanner = NULL;
  nde_field_t   id      = NULL;

  NPT_LOG_FINEST("CWinamp_Database::CheckMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'UUID : %s.", uuid_ref.GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);

  id = NDE_Scanner_GetFieldByID(scanner, FID_MS_IDTREE);
  NPT_CHECK_POINTER_LABEL_FATAL(id, done);

  *id_tree = NDE_IntegerField_GetValue(id);
  NPT_LOG_INFO_1("Enregistrement trouvé avec identifiant %ld.", id_tree);
  return NPT_SUCCESS;

done:
  NPT_LOG_INFO("Enregistrement non trouvé.");
  *id_tree = -1;
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CheckMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::CheckMSDevice(NPT_Int32 id_tree_ref, NPT_String *uuid_ref)
{
  nde_scanner_t scanner = NULL;
  nde_field_t   uuid    = NULL;

  NPT_LOG_FINEST("CWinamp_Database::CheckMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'identifiant : %ld.", id_tree_ref);

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  scanner = NDE_Table_CreateScanner(m_MSTable);
  if (scanner != NULL)
  {
    if (NDE_Scanner_LocateInteger(scanner, FID_MS_IDTREE, FIRST_RECORD, id_tree_ref))
    {
      uuid = NDE_Scanner_GetFieldByID(scanner, FID_MS_UUID);
      NPT_CHECK_POINTER_FATAL(uuid);

      uuid_ref->Erase(0, uuid_ref->GetLength());
      uuid_ref->Append(CW2A(NDE_StringField_GetString(uuid)));

      NPT_LOG_INFO("Enregistrement trouvé.");
      NDE_Table_DestroyScanner(m_MSTable, scanner);
      return NPT_SUCCESS;
    }

    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  NPT_LOG_INFO("Enregistrement non trouvé.");
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CheckFirstScanMSDevice
+---------------------------------------------------------------------*/
NPT_Int16
CWinamp_Database::CheckFirstScanMSDevice(NPT_String uuid_ref)
{
  nde_scanner_t scanner = NULL;
  nde_field_t   scanned = NULL;

  NPT_LOG_FINEST("CWinamp_Database::CheckFirstScanMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'UUID : %s.", uuid_ref.GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  NPT_LOG_INFO("Enregistrement trouvé.");

  scanned = NDE_Scanner_GetFieldByID(scanner, FID_MS_SCANNED);
  NPT_CHECK_POINTER_FATAL(scanned);
  return NDE_IntegerField_GetValue(scanned);

done:
  NPT_LOG_INFO("Enregistrement non trouvé.");
  return -1;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::CheckItem
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::CheckItem(NPT_String uuid_ref, PLT_MediaObject *item)
{
  NPT_List<PLT_PersonRole>::Iterator artist_ref;
  NPT_String artistName;
  nde_scanner_t scanner = NULL;

  nde_field_t uuid = NULL;
  nde_field_t title = NULL;
  nde_field_t artist = NULL;
  nde_field_t album = NULL;

  NPT_LOG_FINEST("CWinamp_Database::CheckItem");

  NPT_CHECK_POINTER_LABEL_FATAL(m_ItemTable, pt_error);

  if (item->m_People.artists.GetItemCount() > 0)
  {
    artist_ref = item->m_People.artists.GetFirstItem();
    artistName = (*artist_ref).name;
    NPT_CHECK_POINTER_FATAL(artist_ref);
  }

  NPT_LOG_INFO_1("Recherche de l'enregistrement : %s", item->m_ObjectID.GetChars());

  scanner = NDE_Table_CreateScanner(m_ItemTable);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, pt_error);
  for (NDE_Scanner_First(scanner);!(NDE_Scanner_EOF(scanner));NDE_Scanner_Next(scanner))
  {
    uuid = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_MS_UUID);
    title = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_TITLE);
    artist = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ARTIST);
    album = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ALBUM);

    NPT_CHECK_POINTER_LABEL_FATAL(uuid, pt_error);
    NPT_CHECK_POINTER_LABEL_FATAL(title, pt_error);
    NPT_CHECK_POINTER_LABEL_FATAL(artist, pt_error);
    NPT_CHECK_POINTER_LABEL_FATAL(album, pt_error);

    if ((uuid_ref.Compare(CW2A(NDE_StringField_GetString(uuid)))==0) &&
      (item->m_Title.Compare(CW2A(NDE_StringField_GetString(title)))==0) &&
      (artistName.Compare(CW2A(NDE_StringField_GetString(artist)))==0) &&
      (item->m_Affiliation.album.Compare(CW2A(NDE_StringField_GetString(album)))==0))
    {
      NPT_LOG_INFO("Enregistrement trouvé.");
      NDE_Table_DestroyScanner(m_ItemTable, scanner);
      return NPT_SUCCESS;
    }
  }

  NPT_LOG_INFO("Enregistrement non trouvé.");
  NDE_Table_DestroyScanner(m_ItemTable, scanner);
  return NPT_FAILURE;

pt_error:
  NPT_LOG_FATAL("Allocation en erreur pour les variables dynamique");
  if (scanner != NULL)
    NDE_Table_DestroyScanner(m_ItemTable, scanner);
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::AddMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::AddMSDevice(PLT_DeviceDataReference& device)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::AddMSDevice");
  NPT_LOG_INFO_2("Ajout de l'appareil %s(%s).", device->GetFriendlyName().GetChars(), device->GetUUID().GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  scanner = NDE_Table_CreateScanner(m_MSTable);
  NPT_CHECK_POINTER_FATAL(scanner);

  NDE_Scanner_New(scanner);

  SetValue(TABLE_MSDEVICE, scanner, FID_MS_UUID, CA2T(device->GetUUID().GetChars()));
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_PARENTUUID, CA2T(device->GetParentUUID().GetChars()));
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_URLBASEIP, CA2T(device->GetURLBase().GetHost().GetChars()));
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_URLBASEPORT, device->GetURLBase().GetPort());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_DEVICETYPE, 0); // TODO : Serveur ou Renderer???
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_FRIENDLYNAME, CA2T(device->GetFriendlyName().GetChars()));
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_IDTREE, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_NB_ITEM, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_SCANNED, 0);

  NDE_Scanner_Post(scanner);
  NDE_Table_DestroyScanner(m_MSTable, scanner);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::AddItem
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::AddItem(NPT_String uuid_ref, PLT_MediaObject *item)
{
  NPT_Cardinal  trouve = 0;
  NPT_Cardinal  i = 0;
  NPT_Int32     y = 0;
  NPT_Int8      year[4+1] = {0};

  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::AddItem");
  NPT_LOG_INFO_1("Ajout de l'element audio : %s.", item->m_Title.GetChars());

  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  scanner = NDE_Table_CreateScanner(m_ItemTable);
  NPT_CHECK_POINTER_FATAL(scanner);

  if (item->m_Resources.GetItemCount() > 0)
  {
    for (i=0;i<item->m_Resources.GetItemCount(); i++) 
    {
      if (item->m_Resources[i].m_ProtocolInfo.GetProtocol().Compare("http-get", true) == 0)
      {
        trouve = 1;
        break;
      }
    }

    if (trouve == 1)
    {
      NDE_Scanner_New(scanner);

      SetValue(TABLE_ITEM, scanner, FID_ITEM_MS_UUID, CA2T(uuid_ref.GetChars()));
      SetValue(TABLE_ITEM, scanner, FID_OBJECT_ID, CA2T(item->m_ObjectID.GetChars()));
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILENAME, CA2T(item->m_Resources[i].m_Uri.GetChars()));

      SetValue(TABLE_ITEM, scanner, FID_ITEM_BITRATE, item->m_Resources[i].m_Bitrate);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILETIME, item->m_Resources[i].m_Duration);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILESIZE, (NPT_Int32)item->m_Resources[i].m_Size);

      SetValue(TABLE_ITEM, scanner, FID_ITEM_TITLE, CA2T(item->m_Title.GetChars()));

      if (item->m_People.artists.GetItemCount() > 0)
      {
        NPT_List<PLT_PersonRole>::Iterator artist = item->m_People.artists.GetFirstItem();
        NPT_CHECK_POINTER_FATAL(artist);
        SetValue(TABLE_ITEM, scanner, FID_ITEM_ARTIST, CA2T((*artist).name.GetChars()));
      }
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_ARTIST, L" ");

      SetValue(TABLE_ITEM, scanner, FID_ITEM_ALBUM, CA2T(item->m_Affiliation.album.GetChars()));

      /* Ruse de sioux de merde pour ne trouver que l'année */
      for (y = NPT_DATETIME_YEAR_MIN; y < NPT_DATETIME_YEAR_MAX ; y++)
      {
        NPT_SetMemory(year, 0, sizeof(year));
        NPT_FormatString(year, sizeof(year), "%ld", y);
        if (item->m_Date.Find(year) >= 0)
          break;
      }
      if (y >= NPT_DATETIME_YEAR_MAX) y = NPT_DATETIME_YEAR_MIN;
      SetValue(TABLE_ITEM, scanner, FID_ITEM_YEAR, y);

      if (item->m_Affiliation.genres.GetItemCount() > 0)
      {
        NPT_List<NPT_String>::Iterator genre = item->m_Affiliation.genres.GetFirstItem();
        NPT_CHECK_POINTER_FATAL(genre);
        SetValue(TABLE_ITEM, scanner, FID_ITEM_GENRE, CA2T((*genre).GetChars()));
      }
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_GENRE, L" ");

      SetValue(TABLE_ITEM, scanner, FID_ITEM_TRACKNO, item->m_MiscInfo.original_track_number); //??? Not sure
      SetValue(TABLE_ITEM, scanner, FID_ITEM_LENGTH, item->m_Resources[i].m_Duration); //TODO

      if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.audioItem.musicTrack") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 0); // Audio
      else if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.videoItem") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 1); // Video
      else if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.imageItem.photo") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 2); // Photo
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, L" ");

      NPT_TimeStamp time;
      NPT_System::GetCurrentTimeStamp(time);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_LASTUPD, (NPT_Int32) time.ToSeconds()); // OK

      SetValue(TABLE_ITEM, scanner, FID_ITEM_LASTPLAY, -1);   // OK
      SetValue(TABLE_ITEM, scanner, FID_ITEM_RATING, 0);     // OK
      SetValue(TABLE_ITEM, scanner, FID_ITEM_PLAYCOUNT, 0);  // OK

      NDE_Scanner_Post(scanner);
    }
    else
    {
      // Pas d'URL, ça sert à rien de prendre en compte cet élément
      return NPT_FAILURE;
    }
  }
  else
  {
    // Pas d'URL, ça sert à rien de prendre en compte cet élément
    return NPT_FAILURE;
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::UpdateIdTreeMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::UpdateIdTreeMSDevice(NPT_String uuid_ref, NPT_Int32 id_tree)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::UpdateIdTreeMSDevice");
  scanner = GetScannerForDevice(uuid_ref);
  if (scanner) {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_IDTREE, id_tree);

    if (id_tree == -1)
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
    else
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 1);

    NDE_Scanner_Post(scanner);
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::UpdateNbItemMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::UpdateNbItemMSDevice(NPT_String uuid_ref, NPT_Int32 nb_item)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::UpdateNbItemMSDevice");
  scanner = GetScannerForDevice(uuid_ref);
  if (scanner) 
  {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_NB_ITEM, nb_item);
    NDE_Scanner_Post(scanner);
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::UpdateScanMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::UpdateScanMSDevice(NPT_String uuid_ref, NPT_Int32 scanned)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::UpdateScanMSDevice");
  scanner = GetScannerForDevice(uuid_ref);
  if (scanner) 
  {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_SCANNED, scanned);
    NDE_Scanner_Post(scanner);
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::RemoveMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::RemoveMSDevice(NPT_String uuid_ref)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::RemoveMSDevice");
  NPT_LOG_INFO_1("Suppression de l'appareil %s.", uuid_ref);

  scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  NDE_Scanner_Delete(scanner);
  return NPT_SUCCESS;

done:
  NPT_LOG_INFO("Suppression impossible");
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::GetActifDeviceCount
+---------------------------------------------------------------------*/
NPT_Int32
CWinamp_Database::GetActifDeviceCount(void)
{
  nde_scanner_t scanner = NULL;
  NPT_Int32     nb      = 0;

  NPT_LOG_FINEST("CWinamp_Database::GetDeviceCount");
  NPT_CHECK_POINTER_FATAL(m_MSTable);

  scanner = NDE_Table_CreateScanner(m_MSTable);
  if (scanner != NULL)
  {
    for (NDE_Scanner_First(scanner);!(NDE_Scanner_EOF(scanner));NDE_Scanner_Next(scanner))
    {
      nde_field_t actif = NULL;
      actif = NDE_Scanner_GetFieldByID(scanner, FID_MS_ACTIF);
      NPT_CHECK_POINTER_FATAL(actif);
      if (NDE_IntegerField_GetValue(actif) == 1)
        nb++;
    }
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  return nb;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::SelectItem
+---------------------------------------------------------------------*/
NPT_Int32
CWinamp_Database::SelectItem(NPT_String Object_id_ref, itemRecordW* item)
{
  nde_scanner_t scanner = NULL;
  nde_field_t   filename= NULL;
  nde_field_t   title   = NULL;
  nde_field_t   album   = NULL;
  nde_field_t   artist  = NULL;
  nde_field_t   genre   = NULL;
  nde_field_t   year    = NULL;
  nde_field_t   length  = NULL;
  nde_field_t   track   = NULL;

  NPT_LOG_FINEST("CWinamp_Database::SelectItem");
  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  scanner = NDE_Table_CreateScanner(m_ItemTable);
  if (NDE_Scanner_LocateString(scanner, FID_OBJECT_ID, FIRST_RECORD, CA2T(Object_id_ref.GetChars())))
  {
    NPT_CHECK_POINTER_FATAL(item);

    filename = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_FILENAME);
    title = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_TITLE);
    album = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ALBUM);
    artist = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ARTIST);
    genre = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_GENRE);
    year = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_YEAR);
    length = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_LENGTH);
    track = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_TRACKNO);

    NPT_CHECK_POINTER_FATAL(filename);
    NPT_CHECK_POINTER_FATAL(title);
    NPT_CHECK_POINTER_FATAL(album);
    NPT_CHECK_POINTER_FATAL(artist);
    NPT_CHECK_POINTER_FATAL(genre);
    NPT_CHECK_POINTER_FATAL(year);
    NPT_CHECK_POINTER_FATAL(length);
    NPT_CHECK_POINTER_FATAL(track);

    item->filename = _wcsdup(NDE_StringField_GetString(filename));
    item->title = _wcsdup(NDE_StringField_GetString(title));
    item->album = _wcsdup(NDE_StringField_GetString(album));
    item->artist = _wcsdup(NDE_StringField_GetString(artist));
    item->genre = _wcsdup(NDE_StringField_GetString(genre));
    item->length = NDE_IntegerField_GetValue(length);
    item->track = NDE_IntegerField_GetValue(track);
    item->year = NDE_IntegerField_GetValue(year);
  }

  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::LoadData
+---------------------------------------------------------------------*/
NPT_Result  
CWinamp_Database::LoadData(CWinamp_ListView* lv, NPT_String uuid_ref, LPCWSTR filter /* = NULL */)
{
  nde_scanner_t scanner = NULL;
  nde_field_t   objectID= NULL;
  nde_field_t   uuid    = NULL;
  nde_field_t   filename= NULL;
  nde_field_t   title   = NULL;
  nde_field_t   album   = NULL;
  nde_field_t   artist  = NULL;
  nde_field_t   genre   = NULL;
  nde_field_t   year    = NULL;
  nde_field_t   length  = NULL;
  nde_field_t   track   = NULL;

  NPT_LOG_FINEST("CWinamp_Database::LoadData");
  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  scanner = NDE_Table_CreateScanner(m_ItemTable);
  NPT_CHECK_POINTER_FATAL(scanner);

  lv->ClearAll();

  for (NDE_Scanner_First(scanner);!(NDE_Scanner_EOF(scanner));NDE_Scanner_Next(scanner))
  {
    uuid=NDE_Scanner_GetFieldByID(scanner, FID_ITEM_MS_UUID);
    NPT_CHECK_POINTER_LABEL_FATAL(uuid, done);

    if (uuid_ref.Compare(CW2A(NDE_StringField_GetString(uuid)))==0)
    {
      title = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_TITLE);
      album = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ALBUM);
      artist = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_ARTIST);
  
      NPT_String s_artist(CW2A(NDE_StringField_GetString(artist)));
      NPT_String s_title(CW2A(NDE_StringField_GetString(title)));
      NPT_String s_album(CW2A(NDE_StringField_GetString(album)));

      if ((filter == NULL) || (wcscmp(filter, L"") == 0) 
        || (s_artist.Find(CW2A(filter),0,true) != -1) 
        || (s_album.Find(CW2A(filter),0,true) != -1) || (s_title.Find(CW2A(filter),0,true) != -1))
      {
        objectID = NDE_Scanner_GetFieldByID(scanner, FID_OBJECT_ID);
        filename = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_FILENAME);
        genre = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_GENRE);
        year = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_YEAR);
        length = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_LENGTH);
        track = NDE_Scanner_GetFieldByID(scanner, FID_ITEM_TRACKNO);

        itemRecordW item = {0,};

        item.filename = NDE_StringField_GetString(filename);
        item.title = NDE_StringField_GetString(title);
        item.album = NDE_StringField_GetString(album);
        item.artist = NDE_StringField_GetString(artist);
        item.genre = NDE_StringField_GetString(genre);
        item.length = NDE_IntegerField_GetValue(length);
        item.track = NDE_IntegerField_GetValue(track);
        item.year = NDE_IntegerField_GetValue(year);

        lv->AddItem(NDE_StringField_GetString(objectID), &item);
      }
    }
  }

  NDE_Table_DestroyScanner(m_ItemTable, scanner);
  return NPT_SUCCESS;

done:
  NPT_LOG_INFO("Erreur au chargement de la Listview.");
  NDE_Table_DestroyScanner(m_ItemTable, scanner);
  return NPT_FAILURE;
}

//////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------
|   CWinamp_Database::SetValue
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::SetValue(NPT_Int32 nom_table, nde_scanner_t scanner, NPT_Int32 id, const LPCWSTR value)
{
  nde_field_t   f = NULL;

  NPT_LOG_FINEST("CWinamp_Database::SetValue");
  NPT_CHECK_POINTER_FATAL(scanner);

  switch (nom_table)
  {
  case TABLE_MSDEVICE:
    if ((id >= FID_MS_MAXNUM) || (MSFieldData[id].type != FIELD_STRING))
      return NPT_FAILURE;
    break;
  case TABLE_ITEM:
    if ((id >= FID_ITEM_MAXNUM) || (ItemFieldData[id].type != FIELD_STRING))
      return NPT_FAILURE;
    break;
  default:
    return NPT_FAILURE;
  }

  if (value==NULL)
    return NPT_FAILURE;

  f = NDE_Scanner_GetFieldByID(scanner, id);
  if(!f) 
    f = NDE_Scanner_NewFieldByID(scanner, id);

  if (f) 
  {
    if (wcscmp(value, L"") !=0)
      NDE_StringField_SetString(f, value);
    else
      NDE_StringField_SetString(f, L"");
    return NPT_SUCCESS;
  }

  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::SetValue
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::SetValue(NPT_Int32 nom_table, nde_scanner_t scanner, NPT_Int32 id, NPT_Int32 value)
{
  nde_field_t   f = NULL;

  NPT_LOG_FINEST("CWinamp_Database::SetValue");
  NPT_CHECK_POINTER_FATAL(scanner);

  switch (nom_table)
  {
  case TABLE_MSDEVICE:
    if (id >= FID_MS_MAXNUM)
      return NPT_FAILURE;
    break;
  case TABLE_ITEM:
    if (id >= FID_ITEM_MAXNUM)
      return NPT_FAILURE;
    break;
  default:
    return NPT_FAILURE;
  }

  f = NDE_Scanner_GetFieldByID(scanner, id);
  if(!f) 
    f = NDE_Scanner_NewFieldByID(scanner, id);

  if (f) 
  {
    NDE_IntegerField_SetValue(f, value);
    return NPT_SUCCESS;
  }

  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::GetScannerForDevice
+---------------------------------------------------------------------*/
nde_scanner_t
CWinamp_Database::GetScannerForDevice(NPT_String uuid_ref)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::GetScannerForDevice");

  if (m_MSTable != NULL)
  {
    scanner = NDE_Table_CreateScanner(m_MSTable);
    if (scanner != NULL)
    {
      if (NDE_Scanner_LocateString(scanner, FID_MS_UUID, FIRST_RECORD, CA2T(uuid_ref.GetChars())))
        return scanner;
    }
    NDE_Table_DestroyScanner(m_MSTable, scanner);
  }

  return NULL;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::GetScannerForItem
+---------------------------------------------------------------------*/
nde_scanner_t
CWinamp_Database::GetScannerForItem(NPT_String Object_id)
{
  nde_scanner_t scanner = NULL;

  NPT_LOG_FINEST("CWinamp_Database::GetScannerForItem");

  if (m_ItemTable != NULL)
  {
    scanner = NDE_Table_CreateScanner(m_ItemTable);
    if (scanner != NULL)
    {
      if (NDE_Scanner_LocateString(scanner, FID_OBJECT_ID, FIRST_RECORD, CA2T(Object_id.GetChars())))
        return scanner;
    }
    NDE_Table_DestroyScanner(m_ItemTable, scanner);
  }

  return NULL;
}
