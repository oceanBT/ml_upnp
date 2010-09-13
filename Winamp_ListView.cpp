/*****************************************************************
|
|   Winamp_ListView.cpp - ListView file
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

#include "StdAfx.h"
#include "Winamp_ListView.h"
#include "ml_upnp.h"
#include "tools.h"

/*----------------------------------------------------------------------
|   CWinamp_ListView::CWinamp_ListView
+---------------------------------------------------------------------*/
CWinamp_ListView::CWinamp_ListView()
{
  m_Hwnd          = NULL;
  m_Column        = 0;
  m_Row           = 0;
  m_SortColumn    = 0;
  m_SortDirection = 0;
  m_SubItem       = -1;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::CWinamp_ListView
+---------------------------------------------------------------------*/
CWinamp_ListView::CWinamp_ListView(HWND hwnd)
{
  CWinamp_ListView::CWinamp_ListView();
  CWinamp_ListView::SetHwnd(hwnd);

  this->SkinWindow();
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::~CWinamp_ListView
+---------------------------------------------------------------------*/
CWinamp_ListView::~CWinamp_ListView(void)
{
  m_Hwnd = NULL;
  m_Column = 0;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::SkinWindow
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::SkinWindow()
{
  /* skin listview */
  MLSKINWINDOW sw;
  sw.hwndToSkin = m_Hwnd;
  sw.skinType = SKINNEDWND_TYPE_LISTVIEW;
  sw.style = SWLVS_FULLROWSELECT | SWLVS_DOUBLEBUFFER | SWS_USESKINFONT | SWS_USESKINCOLORS | SWS_USESKINCURSORS;
  return (NPT_Result) MLSkinWindow(PluginUPNP.hwndLibraryParent, &sw);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::InsertColumn
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::InsertColumn(NPT_Int32 column, char* text, NPT_Int32 width=0)
{
  /* add listview columns */
  LVCOLUMN lvc = {0, };
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;
  lvc.pszText = TEXT(text);
  lvc.cx = width;

  m_Column++;

  return (NPT_Result) ListView_InsertColumn(m_Hwnd, column, &lvc);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::AddColumn
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::AddColumn(char* text, NPT_Int32 width=0)
{
  /* add listview columns */
  LVCOLUMN lvc = {0, };
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;
  lvc.pszText = TEXT(text);
  lvc.cx = width;

  m_Column++;

  return (NPT_Result) ListView_InsertColumn(m_Hwnd, m_Column-1, &lvc);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::AddItem
+---------------------------------------------------------------------*/
void 
CWinamp_ListView::AddItem(NPT_String object_id, itemRecord item)
{
  LVITEM lvi = {0};
  lvi.mask = LVIF_TEXT;
  lvi.iItem = m_Row;
  lvi.iSubItem = COL_OBJECT_ID;

  lvi.pszText = (LPTSTR)calloc(object_id.GetLength()+1, sizeof(LPTSTR));
  StringCchCopy(lvi.pszText, object_id.GetLength()+1, CA2T(object_id.GetChars()));
  lvi.cchTextMax = object_id.GetLength()+1;
  lvi.iItem = ListView_InsertItem(m_Hwnd, &lvi);

  if (lvi.iItem >= 0) {
    /* Artist */
    lvi.iSubItem = COL_ARTIST;
    lvi.pszText = item.artist;
    lvi.cchTextMax = strlen(item.artist)+1;
    ListView_SetItem(m_Hwnd, &lvi);

    /* Title */
    lvi.iSubItem = COL_TITLE;
    lvi.pszText = item.title;
    lvi.cchTextMax = strlen(item.title)+1;
    ListView_SetItem(m_Hwnd, &lvi);

    /* Album */
    lvi.iSubItem = COL_ALBUM;
    lvi.pszText = item.album;
    lvi.cchTextMax = strlen(item.album)+1;
    ListView_SetItem(m_Hwnd, &lvi);

    /* Genre */
    lvi.iSubItem = COL_GENRE;
    lvi.pszText = item.genre;
    lvi.cchTextMax = strlen(item.genre)+1;
    ListView_SetItem(m_Hwnd, &lvi);

    /* Duration */
    lvi.iSubItem = COL_DURATION;
    NPT_String duration;
    FormatTimeStamp(duration, item.length);
    lvi.pszText = CA2T(duration.GetChars());
    lvi.cchTextMax = duration.GetLength()+1;
    ListView_SetItem(m_Hwnd, &lvi);
  }

  m_Row++;
}

/*----------------------------------------------------------------------
|   CWinamp_MediaController::CompareStringProc
+---------------------------------------------------------------------*/
int CALLBACK CWinamp_ListView::CompareStringProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  TCHAR szBuf1[1024], szBuf2[1024];

  CWinamp_ListView* listview = (CWinamp_ListView*) lParamSort;
  ListView_GetItemText(listview->GetHwnd(), lParam1, listview->GetSubItem(), szBuf1, sizeof(szBuf1));
  ListView_GetItemText(listview->GetHwnd(), lParam2, listview->GetSubItem(), szBuf2, sizeof(szBuf2));

  if (listview->GetSortDirection() == HDF_SORTDOWN) 
    return(_tcscmp(szBuf1, szBuf2) * -1);
  return(_tcscmp(szBuf1, szBuf2));
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::Sort
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_ListView::Sort(LPARAM lParam)
{
  HDITEM hdi;
  LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
  HWND HwndLVHeader = (HWND) ListView_GetHeader(m_Hwnd);
            
  hdi.mask = HDI_FORMAT;
  Header_GetItem(HwndLVHeader, pnmv->iSubItem, &hdi);

  if(m_SortColumn > -1) // un colonne est déja séléctionnée
  { 
    if (pnmv->iSubItem == m_SortColumn)
    {
      if(m_SortDirection == HDF_SORTDOWN)
      {
        m_SortDirection = HDF_SORTUP;
        hdi.fmt &= ~HDF_SORTDOWN;
        hdi.fmt |= HDF_SORTUP;
      }
      else
      {
        m_SortDirection = HDF_SORTDOWN;
        hdi.fmt &= ~HDF_SORTUP;
        hdi.fmt |= HDF_SORTDOWN;
      }
      Header_SetItem(HwndLVHeader, pnmv->iSubItem, &hdi); 
    }
    else
    {
      Header_GetItem(HwndLVHeader, m_SortColumn, &hdi);
      hdi.fmt &= ~HDF_SORTUP & ~HDF_SORTDOWN;
      Header_SetItem(HwndLVHeader, m_SortColumn, &hdi);
      Header_GetItem(HwndLVHeader, m_SortColumn = pnmv->iSubItem, &hdi);
      hdi.fmt |= HDF_SORTDOWN;
      m_SortDirection = HDF_SORTDOWN;
      Header_SetItem(HwndLVHeader, m_SortColumn, &hdi); 
    }
  }
  else
  {
    Header_GetItem(HwndLVHeader, m_SortColumn = pnmv->iSubItem, &hdi);
    hdi.fmt |= HDF_SORTDOWN;
    m_SortDirection = HDF_SORTDOWN;
    Header_SetItem(HwndLVHeader, pnmv->iSubItem, &hdi); 
  }

  m_SubItem = pnmv->iSubItem;
  return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::GetItemRecordList
+---------------------------------------------------------------------*/
void CWinamp_ListView::GetItemRecordList(itemRecordList *list, CWinamp_ItemMap mediaItems)
{
  NPT_String  IdObject;
  TCHAR       szBuf1[1024];

  int nItem = ListView_GetNextItem(m_Hwnd, -1, LVNI_SELECTED);
  do {
    allocRecordList(list, list->Size+1);
    if (!list->Items) break;

    ListView_GetItemText(m_Hwnd, nItem, 0, szBuf1, sizeof(szBuf1));
    IdObject.Erase(0, IdObject.GetLength());
    IdObject.Append(szBuf1);

    itemRecord* ItemRecord;
    NPT_SetMemory(&list->Items[list->Size], 0, sizeof(itemRecord));

    mediaItems.Get(IdObject, ItemRecord);

    list->Items[list->Size].artist = ItemRecord->artist;
    list->Items[list->Size].title = ItemRecord->title;
    list->Items[list->Size].filename = ItemRecord->filename;
    list->Items[list->Size].album = ItemRecord->album;
    list->Items[list->Size].genre = ItemRecord->genre;
    list->Items[list->Size].length = ItemRecord->length;
    list->Size++;

  } while ((nItem = ListView_GetNextItem(m_Hwnd, nItem, LVNI_SELECTED)) >= 0);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::Search
+---------------------------------------------------------------------*/
NPT_Result  
CWinamp_ListView::Search(TCHAR* text, CWinamp_ItemMap MediaItems)
{
  NPT_List<CWinamp_ItemMapEntry*> entries = MediaItems.GetEntries();
  if (entries.GetItemCount() != 0) 
  {
    ClearAll();
    NPT_List<CWinamp_ItemMapEntry*>::Iterator entry = entries.GetFirstItem();
    while (entry) 
    {
      itemRecord ItemRecord = (*entry)->GetValue();
      NPT_String artist(ItemRecord.artist);
      NPT_String title(ItemRecord.title);
      NPT_String album(ItemRecord.album);

      if ((strcmp(text, "") == 0) || (artist.Find(text,0,true) != -1) 
          || (album.Find(text,0,true) != -1) || (title.Find(text,0,true) != -1))
      {
        NPT_String s((NPT_String)(*entry)->GetKey());
        AddItem((NPT_String)(*entry)->GetKey(), ItemRecord);
      }
      ++entry;
    }
  }

  if (m_SubItem != -1)
    return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this);

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::SavePreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_ListView::SavePreference(TCHAR* section, TCHAR* filename)
{
  TCHAR s[32];
  if (filename != NULL)
  {
    NPT_SetMemory(s, 0, sizeof(s));
    sprintf_s(s, sizeof(s), "%d", m_SortDirection);
    WritePrivateProfileString(section,CONFIG_SORT_DIR,s,filename);

    NPT_SetMemory(s, 0, sizeof(s));
    sprintf_s(s, sizeof(s), "%d", m_SortColumn);
    WritePrivateProfileString(section,CONFIG_SORT_COL,s,filename);
    return NPT_SUCCESS;
  }
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::LoadPreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_ListView::LoadPreference(TCHAR* section, TCHAR* filename)
{
  if (filename != NULL)
  {
    m_SortDirection=GetPrivateProfileInt(section, CONFIG_SORT_DIR, 0, filename);
    if (m_SortDirection < 0) m_SortDirection = 0;

    m_SortColumn=GetPrivateProfileInt(section, CONFIG_SORT_COL, HDF_SORTDOWN, filename);
    if (m_SortColumn < HDF_SORTDOWN) m_SortColumn = HDF_SORTDOWN;

    return NPT_SUCCESS;
  }
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::SelectAll
+---------------------------------------------------------------------*/
void CWinamp_ListView::SelectAll()
{
  int nItem = ListView_GetNextItem(m_Hwnd, -1, LVNI_ALL);
  do {
    ListView_SetSelectionMark(m_Hwnd, nItem);
  } while ((nItem = ListView_GetNextItem(m_Hwnd, nItem, LVNI_ALL)) >= 0);
}