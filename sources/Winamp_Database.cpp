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

/* Entete Winamp SDK */
#include "wa_ipc.h"

#include "NptTime.h"

#undef ASSERT

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
  const NPT_Int8  *name;
  NPT_UInt8       type;
  NPT_Cardinal    use;
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
  NPT_Int8 DefaultDBDirectory[128];
  NPT_Int8 DBDirectory[128];

  NPT_LOG_FINEST("CWinamp_Database::CWinamp_Database");

  m_PluginUPNP = plugin;

  //Fichier ini de Winamp
  NPT_String WAConfFile((char*)SendMessage(m_PluginUPNP->hwndWinampParent, WM_WA_IPC, 0, IPC_GETINIFILE));

  // Récupération du paramétrage
  NPT_FormatString(DefaultDBDirectory, sizeof(DefaultDBDirectory), "%s\\ml_upnp", (char*)SendMessage(m_PluginUPNP->hwndWinampParent,WM_WA_IPC,0,IPC_GETPLUGINDIRECTORY));
  GetPrivateProfileString(CONFIG_SECTION, CONFIG_DATABASE, DefaultDBDirectory, DBDirectory, sizeof(DBDirectory), WAConfFile.GetChars());

  // Enregistrement du paramétrage
  WritePrivateProfileString(CONFIG_SECTION,CONFIG_DATABASE,DBDirectory,WAConfFile.GetChars());

  // Création du repertoire
  NPT_File::CreateDir(DBDirectory);

  //Nommage des tables et des index
  NPT_String DataBaseURL(DBDirectory);

  m_MSTableName.Erase(0, m_MSTableName.GetLength());
  m_MSTableName = DataBaseURL;
  m_MSTableName.Append("\\");
  m_MSTableName.Append(WINAMP_DB_DEVICE);
  m_MSTableName.Append(".dat");
  
  m_MSTableIndexName.Erase(0, m_MSTableIndexName.GetLength());
  m_MSTableIndexName = DataBaseURL;
  m_MSTableIndexName.Append("\\");
  m_MSTableIndexName.Append(WINAMP_DB_DEVICE);
  m_MSTableIndexName.Append(".idx");

  m_ItemTableName.Erase(0, m_ItemTableName.GetLength());
  m_ItemTableName = DataBaseURL;
  m_ItemTableName.Append("\\");
  m_ItemTableName.Append(WINAMP_DB_ITEM);
  m_ItemTableName.Append(".dat");

  m_ItemTableIndexName.Erase(0, m_ItemTableIndexName.GetLength());
  m_ItemTableIndexName = DataBaseURL;
  m_ItemTableIndexName.Append("\\");
  m_ItemTableIndexName.Append(WINAMP_DB_ITEM);
  m_ItemTableIndexName.Append(".idx");

  // Création de la base de données
  NPT_LOG_INFO("Création de la base de données en cours...");
  m_DB = new Database;
  if (m_DB)
  {
    m_MSTable = m_DB->OpenTable(m_MSTableName.UseChars(), m_MSTableIndexName.UseChars(), NDE_OPEN_ALWAYS, NDE_CACHE);
    if (m_MSTable == NULL)
    {
      NPT_LOG_INFO_2("Creation table %s(%s) impossible", m_MSTableName.GetChars(), m_MSTableIndexName.GetChars());
    }
    else
    {
      NPT_LOG_INFO_2("table %s(%s) créée", m_MSTableName.GetChars(), m_MSTableIndexName.GetChars());
      for (int i=0; i < FID_MS_MAXNUM; i++) 
      {
        if (MSFieldData[i].use) 
        {
        m_MSTable->NewColumn(i, (char*)MSFieldData[i].name, MSFieldData[i].type, 0);
        }
      }
      m_MSTable->AddIndexById(FID_MS_UUID, "UUID");
      m_MSTable->PostColumns();
    }

    m_ItemTable = m_DB->OpenTable(m_ItemTableName.UseChars(), m_ItemTableIndexName.UseChars(), NDE_OPEN_ALWAYS, NDE_CACHE);
    if (m_ItemTable == NULL)
    {
      NPT_LOG_INFO_2("Creation table %s(%s) impossible", m_ItemTableName.GetChars(), m_ItemTableIndexName.GetChars());
    }
    else
    {
      NPT_LOG_INFO_2("table %s(%s) créée", m_ItemTableName.GetChars(), m_ItemTableIndexName.GetChars());
      for (int i=0; i < FID_ITEM_MAXNUM; i++) 
      {
        if (ItemFieldData[i].use) 
        {
          m_ItemTable->NewColumn(i, (char*)ItemFieldData[i].name, ItemFieldData[i].type, 0);
        }
      }
      m_ItemTable->PostColumns();
      m_ItemTable->AddIndexById(FID_OBJECT_ID, "Item");
    }
  }
  else
  {
    NPT_LOG_FATAL("Impossible de créer la base de données!");
  }
  NPT_LOG_INFO("Création terminée");

  // MAJ de la table des appareils pour rendre tous les appareils en base
  // intactifs.
  NPT_LOG_FATAL("MAJ des données...");
  Scanner *scanner = m_MSTable->GetDefaultScanner();
  if (scanner != NULL)
  {
    for (scanner->First();!scanner->Eof();scanner->Next())
    {
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
      scanner->Post();
    }
  }

  // Synchronisation des tables
  Sync();

  NPT_LOG_FATAL("MAJ des données terminée.");
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
      NPT_LOG_INFO_1("Ouverture de la table %s", m_MSTableName.GetChars());
      m_MSTable = m_DB->OpenTable(m_MSTableName.UseChars(), m_MSTableIndexName.UseChars(), NDE_OPEN_EXISTING, NDE_CACHE);
      if (m_MSTable == NULL)
      {
        NPT_LOG_INFO_2("Ouverture table %s(%s) impossible", m_MSTableName.GetChars(), m_MSTableIndexName.GetChars());
        return NPT_FAILURE;
      }
    }
    break;
  case TABLE_ITEM:
    if (!m_ItemTable)
    {
      NPT_LOG_INFO_1("Ouverture de la table %s", m_ItemTableName.GetChars());
      m_ItemTable = m_DB->OpenTable(m_ItemTableName.UseChars(), m_ItemTableIndexName.UseChars(), NDE_OPEN_EXISTING, NDE_CACHE);
      if (m_ItemTable == NULL)
      {
        NPT_LOG_INFO_2("Ouverture table %s(%s) impossible", m_ItemTableName.GetChars(), m_ItemTableIndexName.GetChars());
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
    NPT_LOG_INFO_1("Fermeture de la table %s", m_MSTableName.GetChars());
    if (m_MSTable)
      m_DB->CloseTable(m_MSTable);
    m_MSTable = NULL;
    break;
  case TABLE_ITEM:
    NPT_LOG_INFO_1("Fermeture de la table %s", m_ItemTableName.GetChars());
    if (m_ItemTable)
      m_DB->CloseTable(m_ItemTable);
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

  delete m_DB;
  
  m_MSTableName.Erase(0, m_MSTableName.GetLength());
  m_MSTableIndexName.Erase(0, m_MSTableIndexName.GetLength());
  m_ItemTableName.Erase(0, m_ItemTableName.GetLength());
  m_ItemTableIndexName.Erase(0, m_ItemTableIndexName.GetLength());
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

  m_MSTable->Sync();
  m_ItemTable->Sync();
  
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

  Scanner *scanner = GetScannerForDevice(uuid_ref);
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
  NPT_LOG_FINEST("CWinamp_Database::CheckMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'UUID : %s.", uuid_ref.GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  Scanner *scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  IntegerField *id = (IntegerField *)scanner->GetFieldById(FID_MS_IDTREE);
  NPT_CHECK_POINTER_LABEL_FATAL(id, done);
  *id_tree = id->GetValue();
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
  NPT_LOG_FINEST("CWinamp_Database::CheckMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'identifiant : %ld.", id_tree_ref);

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  Scanner *scanner = m_MSTable->NewScanner(0);
  NPT_CHECK_POINTER_FATAL(scanner);

  for (scanner->First();!scanner->Eof();scanner->Next())
  {
    IntegerField *id_tree = (IntegerField *)scanner->GetFieldById(FID_MS_IDTREE);
    NPT_CHECK_POINTER_FATAL(id_tree);
    if (id_tree->GetValue() == id_tree_ref)
    {
      StringField *uuid = (StringField *)scanner->GetFieldById(FID_MS_UUID);

      NPT_CHECK_POINTER_FATAL(uuid);
      NPT_CHECK_POINTER_FATAL(uuid->GetString());

      uuid_ref->Erase(0, uuid_ref->GetLength());
      uuid_ref->Append(uuid->GetString());

      NPT_LOG_INFO("Enregistrement trouvé.");
      m_MSTable->DeleteScanner(scanner);
      return NPT_SUCCESS;
    }
  }

  NPT_LOG_INFO("Enregistrement non trouvé.");
  m_MSTable->DeleteScanner(scanner);
  return NPT_FAILURE;
}


/*----------------------------------------------------------------------
|   CWinamp_Database::CheckFirstScanMSDevice
+---------------------------------------------------------------------*/
NPT_Int16
CWinamp_Database::CheckFirstScanMSDevice(NPT_String uuid_ref)
{
  NPT_LOG_FINEST("CWinamp_Database::CheckFirstScanMSDevice");
  NPT_LOG_INFO_1("Recherche de l'enregistrement avec l'UUID : %s.", uuid_ref.GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  Scanner *scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  NPT_LOG_INFO("Enregistrement trouvé.");

  IntegerField *scanned = (IntegerField *)scanner->GetFieldById(FID_MS_SCANNED);
  NPT_CHECK_POINTER_FATAL(scanned);
  return scanned->GetValue();

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
  Scanner *scanner=NULL;

  NPT_LOG_FINEST("CWinamp_Database::CheckItem");

  NPT_CHECK_LABEL_FATAL(m_ItemTable==NULL, pt_error);

  if (item->m_People.artists.GetItemCount() > 0)
  {
    artist_ref = item->m_People.artists.GetFirstItem();
    artistName = (*artist_ref).name;
    NPT_CHECK_POINTER_FATAL(artist_ref);
  }

  NPT_LOG_INFO_1("Recherche de l'enregistrement : %s", item->m_ObjectID.GetChars());

  scanner = m_ItemTable->NewScanner(0);
  NPT_CHECK_LABEL_FATAL(scanner==NULL, pt_error);
  scanner->setBlocking(TRUE);

  for (scanner->First();!scanner->Eof();scanner->Next())
  {
    StringField *uuid = (StringField *)scanner->GetFieldById(FID_ITEM_MS_UUID);
    StringField *title = (StringField *)scanner->GetFieldById(FID_ITEM_TITLE);
    StringField *artist = (StringField *)scanner->GetFieldById(FID_ITEM_ARTIST);
    StringField *album = (StringField *)scanner->GetFieldById(FID_ITEM_ALBUM);

    NPT_CHECK_LABEL_FATAL(uuid==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(uuid->GetString()==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(title==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(title->GetString()==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(artist==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(artist->GetString()==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(album==NULL, pt_error);
    NPT_CHECK_LABEL_FATAL(album->GetString()==NULL, pt_error);

    if ((uuid_ref.Compare(uuid->GetString())==0) &&
      (item->m_Title.Compare(title->GetString())==0) &&
      (artistName.Compare(artist->GetString())==0) &&
      (item->m_Affiliation.album.Compare(album->GetString())==0))
    {
      NPT_LOG_INFO("Enregistrement trouvé.");
      m_ItemTable->DeleteScanner(scanner);
      return NPT_SUCCESS;
    }
  }

  NPT_LOG_INFO("Enregistrement non trouvé.");
  m_ItemTable->DeleteScanner(scanner);
  return NPT_FAILURE;

pt_error:
  NPT_LOG_FATAL("Erreur de pointeur!!!");
  if (scanner != NULL)
    m_ItemTable->DeleteScanner(scanner);
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::AddMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::AddMSDevice(PLT_DeviceDataReference& device)
{
  NPT_LOG_FINEST("CWinamp_Database::AddMSDevice");
  NPT_LOG_INFO_2("Ajout de l'appareil %s(%s).", device->GetFriendlyName().GetChars(), device->GetUUID().GetChars());

  NPT_CHECK_POINTER_FATAL(m_MSTable);

  Scanner * scanner = m_MSTable->GetDefaultScanner();
  NPT_CHECK_POINTER_FATAL(scanner);

  scanner->New();

  SetValue(TABLE_MSDEVICE, scanner, FID_MS_UUID, device->GetUUID().GetChars());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_PARENTUUID, device->GetParentUUID().GetChars());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_URLBASEIP, device->GetURLBase().GetHost().GetChars());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_URLBASEPORT, device->GetURLBase().GetPort());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_DEVICETYPE, 0); // TODO : Serveur ou Renderer???
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_FRIENDLYNAME, device->GetFriendlyName().GetChars());
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_IDTREE, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_NB_ITEM, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
  SetValue(TABLE_MSDEVICE, scanner, FID_MS_SCANNED, 0);

  scanner->Post();

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
  NPT_Int8      year[4+1];

  NPT_LOG_FINEST("CWinamp_Database::AddItem");
  NPT_LOG_INFO_1("Ajout de l'element audio : %s.", item->m_Title.GetChars());

  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  Scanner * scanner = m_ItemTable->GetDefaultScanner();
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
      scanner->New();

      SetValue(TABLE_ITEM, scanner, FID_ITEM_MS_UUID, uuid_ref.GetChars());
      SetValue(TABLE_ITEM, scanner, FID_OBJECT_ID, item->m_ObjectID.GetChars());
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILENAME, item->m_Resources[i].m_Uri.GetChars());

      SetValue(TABLE_ITEM, scanner, FID_ITEM_BITRATE, item->m_Resources[i].m_Bitrate);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILETIME, item->m_Resources[i].m_Duration);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_FILESIZE, (NPT_Int32)item->m_Resources[i].m_Size);

      SetValue(TABLE_ITEM, scanner, FID_ITEM_TITLE, item->m_Title.GetChars());

      if (item->m_People.artists.GetItemCount() > 0)
      {
        NPT_List<PLT_PersonRole>::Iterator artist = item->m_People.artists.GetFirstItem();
        NPT_CHECK_POINTER_FATAL(artist);
        SetValue(TABLE_ITEM, scanner, FID_ITEM_ARTIST, (*artist).name.GetChars());
      }
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_ARTIST, " ");

      SetValue(TABLE_ITEM, scanner, FID_ITEM_ALBUM, item->m_Affiliation.album.GetChars());

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
        SetValue(TABLE_ITEM, scanner, FID_ITEM_GENRE, (*genre).GetChars());
      }
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_ARTIST, " ");

      SetValue(TABLE_ITEM, scanner, FID_ITEM_TRACKNO, item->m_MiscInfo.original_track_number); //??? Not sure
      SetValue(TABLE_ITEM, scanner, FID_ITEM_LENGTH, item->m_Resources[i].m_Duration); //TODO

      if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.audioItem.musicTrack") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 0); // Audio
      else if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.videoItem") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 1); // Video
      else if (strcmp(item->GetUPnPClass(item->m_Resources[i].m_Uri), "object.item.imageItem.photo") == 0)
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, 2); // Photo
      else
        SetValue(TABLE_ITEM, scanner, FID_ITEM_TYPE, " ");

      NPT_TimeStamp time;
      NPT_System::GetCurrentTimeStamp(time);
      SetValue(TABLE_ITEM, scanner, FID_ITEM_LASTUPD, (NPT_Int32) time.ToSeconds()); // OK

      SetValue(TABLE_ITEM, scanner, FID_ITEM_LASTPLAY, -1);   // OK
      SetValue(TABLE_ITEM, scanner, FID_ITEM_RATING, 0);     // OK
      SetValue(TABLE_ITEM, scanner, FID_ITEM_PLAYCOUNT, 0);  // OK

      scanner->Post();

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
  NPT_LOG_FINEST("CWinamp_Database::UpdateIdTreeMSDevice");
  Scanner * scanner = GetScannerForDevice(uuid_ref);
  if (scanner) {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_IDTREE, id_tree);
    if (id_tree == -1)
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 0);
    else
      SetValue(TABLE_MSDEVICE, scanner, FID_MS_ACTIF, 1);
    scanner->Post();
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::UpdateNbItemMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::UpdateNbItemMSDevice(NPT_String uuid_ref, NPT_Int32 nb_item)
{
  NPT_LOG_FINEST("CWinamp_Database::UpdateNbItemMSDevice");
  Scanner * scanner = GetScannerForDevice(uuid_ref);
  if (scanner) {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_NB_ITEM, nb_item);
    scanner->Post();
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::UpdateScanMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::UpdateScanMSDevice(NPT_String uuid_ref, NPT_Int32 scanned)
{
  NPT_LOG_FINEST("CWinamp_Database::UpdateScanMSDevice");
  Scanner * scanner = GetScannerForDevice(uuid_ref);
  if (scanner) {
    SetValue(TABLE_MSDEVICE, scanner, FID_MS_SCANNED, scanned);
    scanner->Post();
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::RemoveMSDevice
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::RemoveMSDevice(NPT_String uuid_ref)
{
  NPT_LOG_FINEST("CWinamp_Database::RemoveMSDevice");
  NPT_LOG_INFO_1("Suppression de l'appareil %s.", uuid_ref);

  Scanner * scanner = GetScannerForDevice(uuid_ref);
  NPT_CHECK_POINTER_LABEL_FATAL(scanner, done);
  scanner->Delete();
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
  NPT_Int32 nb=0;

  NPT_LOG_FINEST("CWinamp_Database::GetDeviceCount");
  NPT_CHECK_POINTER_FATAL(m_MSTable);

  Scanner *scanner = m_MSTable->NewScanner(0);
  NPT_CHECK_POINTER_FATAL(scanner);

  for (scanner->First();!scanner->Eof();scanner->Next())
  {
    IntegerField *actif = (IntegerField *)scanner->GetFieldById(FID_MS_ACTIF);
    NPT_CHECK_POINTER_FATAL(actif);

    if (actif->GetValue() == 1)
      nb++;
  }

  m_MSTable->DeleteScanner(scanner);
  return nb;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::SelectItem
+---------------------------------------------------------------------*/
NPT_Int32
CWinamp_Database::SelectItem(NPT_String Object_id_ref, itemRecord* item)
{
  NPT_LOG_FINEST("CWinamp_Database::SelectItem");
  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  Scanner * scanner = m_ItemTable->GetDefaultScanner();
  StringField f(Object_id_ref.GetChars());
  if (scanner->LocateById(FID_OBJECT_ID, FIRST_RECORD, &f, 0)) {
    NPT_CHECK_POINTER_FATAL(item);

    StringField *filename= (StringField *)scanner->GetFieldById(FID_ITEM_FILENAME);
    StringField *title= (StringField *)scanner->GetFieldById(FID_ITEM_TITLE);
    StringField *album= (StringField *)scanner->GetFieldById(FID_ITEM_ALBUM);
    StringField *artist= (StringField *)scanner->GetFieldById(FID_ITEM_ARTIST);
    StringField *genre= (StringField *)scanner->GetFieldById(FID_ITEM_GENRE);
    IntegerField *year= (IntegerField *)scanner->GetFieldById(FID_ITEM_YEAR);
    IntegerField *length= (IntegerField *)scanner->GetFieldById(FID_ITEM_LENGTH);
    IntegerField *track= (IntegerField *)scanner->GetFieldById(FID_ITEM_TRACKNO);

    if (filename && filename->GetString())
      item->filename = _strdup(filename->GetString());
    if (title && title->GetString())
      item->title = _strdup(title->GetString());
    if (album && album->GetString())
      item->album = _strdup(album->GetString());
    if (artist && artist->GetString())
      item->artist = _strdup(artist->GetString());
    if (genre && genre->GetString())
      item->genre = _strdup(genre->GetString());
    if (length)
      item->length = length->GetValue();
    if (track)
      item->track = track->GetValue();
    if (year)
      item->year = year->GetValue();
  }

  return NPT_FAILURE;
}

//////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------
|   CWinamp_Database::SetValue
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::SetValue(NPT_Int32 nom_table, Scanner * scanner, NPT_Int32 id, const NPT_Int8* value)
{
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

  Field *f = scanner->GetFieldById(id);
  if(!f) 
    f = scanner->NewFieldById(id, PERM_READWRITE);

  if (f) 
  {
    StringField *fs = static_cast<StringField*>(f);
    if (strcmp(value, "") !=0)
      fs->SetString(value);
    else
      fs->SetString(" ");
    return NPT_SUCCESS;
  }

  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::SetValue
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::SetValue(NPT_Int32 nom_table, Scanner * scanner, NPT_Int32 id, NPT_Int32 value)
{
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

  Field *f = scanner->GetFieldById(id);
  if(!f) 
    f = scanner->NewFieldById(id, PERM_READWRITE);

  if (f) {
    IntegerField *fs = static_cast<IntegerField*>(f);
    fs->SetValue(value);
    return NPT_SUCCESS;
  }

  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::GetScannerForDevice
+---------------------------------------------------------------------*/
Scanner *
CWinamp_Database::GetScannerForDevice(NPT_String uuid_ref)
{
  NPT_LOG_FINEST("CWinamp_Database::GetScannerForDevice");

  if (m_MSTable != NULL)
  {
    Scanner * scanner = m_MSTable->GetDefaultScanner();
    if (scanner != NULL)
    {
      StringField f(uuid_ref.GetChars());
      if (scanner->LocateById(FID_MS_UUID, FIRST_RECORD, &f, 0))
        return scanner;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::GetScannerForItem
+---------------------------------------------------------------------*/
Scanner *
CWinamp_Database::GetScannerForItem(NPT_String Object_id)
{
  NPT_LOG_FINEST("CWinamp_Database::GetScannerForItem");

  if (m_ItemTable != NULL)
  {
    Scanner * scanner = m_ItemTable->GetDefaultScanner();
    if (scanner != NULL)
    {
      StringField f(Object_id.GetChars());
      if (scanner->LocateById(FID_OBJECT_ID, FIRST_RECORD, &f, 0))
        return scanner;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------
|   CWinamp_Database::LoadData
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_Database::LoadData(CWinamp_ListView* lv, NPT_String uuid_ref)
{
  NPT_LOG_FINEST("CWinamp_Database::LoadData");
  NPT_CHECK_POINTER_FATAL(m_ItemTable);

  Scanner *scanner = m_ItemTable->NewScanner(0);
  NPT_CHECK_POINTER_FATAL(scanner);

  for (scanner->First();!scanner->Eof();scanner->Next())
  {
    StringField *ObjectID=(StringField *)scanner->GetFieldById(FID_OBJECT_ID);
    StringField *filename= (StringField *)scanner->GetFieldById(FID_ITEM_FILENAME);
    StringField *title= (StringField *)scanner->GetFieldById(FID_ITEM_TITLE);
    StringField *album= (StringField *)scanner->GetFieldById(FID_ITEM_ALBUM);
    StringField *artist= (StringField *)scanner->GetFieldById(FID_ITEM_ARTIST);
    StringField *genre= (StringField *)scanner->GetFieldById(FID_ITEM_GENRE);
    IntegerField *year= (IntegerField *)scanner->GetFieldById(FID_ITEM_YEAR);
    IntegerField *length= (IntegerField *)scanner->GetFieldById(FID_ITEM_LENGTH);
    IntegerField *track= (IntegerField *)scanner->GetFieldById(FID_ITEM_TRACKNO);
    
    itemRecord item = {0,};

    if (filename && filename->GetString())
      item.filename = filename->GetString();
    if (title && title->GetString())
      item.title = title->GetString();
    if (album && album->GetString())
      item.album = album->GetString();
    if (artist && artist->GetString())
      item.artist = artist->GetString();
    if (genre && genre->GetString())
      item.genre = genre->GetString();
    if (length)
      item.length = length->GetValue();
    if (track)
      item.track = track->GetValue();
    if (year)
      item.year = year->GetValue();
    lv->AddItem(ObjectID->GetString(), &item);
  }

  m_ItemTable->DeleteScanner(scanner);
  return NPT_SUCCESS;
}