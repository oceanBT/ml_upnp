/*****************************************************************
|
|   Winamp_ListView.cpp - Core file
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
CWinamp_ListView::SkinWindow(winampMediaLibraryPlugin *plugin)
{
  return (m_Handle = (UINT_PTR) SendMessage(plugin->hwndLibraryParent,WM_ML_IPC,(WPARAM)m_Hwnd,ML_IPC_SKIN_LISTVIEW));
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::UnskinWindow
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::UnskinWindow(winampMediaLibraryPlugin *plugin)
{
  return (SendMessage(plugin->hwndLibraryParent,WM_ML_IPC,(WPARAM)m_Handle,ML_IPC_UNSKIN_LISTVIEW));
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::InsertColumn
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::InsertColumn(NPT_Int32 column, LPWSTR text, NPT_Int32 width=0)
{
  /* add listview columns */
  LVCOLUMNW lvc = {0, };
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;
  lvc.pszText = text;
  lvc.cx = width;

  m_Column++;

  return (NPT_Result) ListView_InsertColumn(m_Hwnd, column, &lvc);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::AddColumn
+---------------------------------------------------------------------*/
NPT_Result 
CWinamp_ListView::AddColumn(LPWSTR text, NPT_Int32 width=0)
{
  /* add listview columns */
  LVCOLUMNW lvc = {0, };
  lvc.mask = LVCF_TEXT | LVCF_WIDTH;
  lvc.pszText = text;
  lvc.cx = width;

  m_Column++;

  return (NPT_Result) ListView_InsertColumn(m_Hwnd, m_Column-1, &lvc);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::AddItem
+---------------------------------------------------------------------*/
void 
CWinamp_ListView::AddItem(LPWSTR ObjectId, itemRecordW* item)
{
  TCHAR         length[10+1] = {0};
  NPT_String    s_length;

  LVITEMW lvi = {0};
  lvi.mask = LVIF_TEXT;
  lvi.iItem = m_Row;
  lvi.iSubItem = LW_OBJECT_ID;

  lvi.pszText = ObjectId;
  lvi.cchTextMax = wcslen(ObjectId);
  lvi.iItem = ListView_InsertItem(m_Hwnd, &lvi);

  if (lvi.iItem >= 0) {
    /* Artist */
    lvi.iSubItem = LW_ARTIST;
    if (item->artist != NULL)
    {
      lvi.pszText = item->artist;
      lvi.cchTextMax = wcslen(item->artist);
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    /* Title */
    lvi.iSubItem = LW_TITLE;
    lvi.pszText = item->title;
    lvi.cchTextMax = wcslen(item->title);
    ListView_SetItem(m_Hwnd, &lvi);

    /* Album */
    lvi.iSubItem = LW_ALBUM;
    if (item->artist != NULL)
    {
      lvi.pszText = item->album;
      lvi.cchTextMax = wcslen(item->album);
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    /* Genre */
    lvi.iSubItem = LW_GENRE;
    if (item->genre != NULL)
    {
      lvi.pszText = item->genre;
      lvi.cchTextMax = wcslen(item->genre);
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    /* Year */
    lvi.iSubItem = LW_YEAR;
    if (item->year != NPT_DATETIME_YEAR_MIN)
    {
      lvi.pszText = (LPTSTR)calloc(4+1, sizeof(LPTSTR));
      StringCchPrintfW(lvi.pszText, 4+1, L"%ld", item->year);
      lvi.cchTextMax = 4+1;
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    /* Track */
    lvi.iSubItem = LW_TRACKNO;
    if (item->track != 0)
    {
      lvi.pszText = (LPTSTR)calloc(10+1, sizeof(LPTSTR));
      StringCchPrintfW(lvi.pszText, 10+1, L"%ld", item->track);
      lvi.cchTextMax = 10+1;
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    /* length */
    lvi.iSubItem = LW_LENGTH;
    if (item->length != 0)
    {
      FormatTimeStamp(s_length, item->length);
      lvi.pszText = (LPTSTR)calloc(10+1, sizeof(LPTSTR));
      StringCchPrintfW(lvi.pszText, 10+1, L"%s", CA2T(s_length.GetChars()));
      lvi.cchTextMax = 10+1;
    }
    else
    {
      lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
      StringCchCopyW(lvi.pszText, 1, L"");
      lvi.cchTextMax = 1;
    }
    ListView_SetItem(m_Hwnd, &lvi);

    ///* FileName */
    //lvi.iSubItem = LW_FILENAME;
    //if (item->filename != NULL)
    //{
    //  lvi.pszText = item->filename;
    //  lvi.cchTextMax = strlen(item->filename);
    //}
    //else
    //{
    //  lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
    //  StringCchCopyW(lvi.pszText, 1, "");
    //  lvi.cchTextMax = 1;
    //}
    //ListView_SetItem(m_Hwnd, &lvi);

    //LW_LASTPLAY,
  //LW_RATING,
  //LW_PLAYCOUNT,

  ////  /* Duration */
  ////  lvi.iSubItem = LW_DURATION;
  ////  if (item->m_Resources.GetItemCount() > 0)
  ////  {
  ////    NPT_String duration;
  ////    FormatTimeStamp(duration, item->m_Resources[0].m_Duration);
  ////    lvi.pszText = (LPTSTR)calloc(duration.GetLength()+1, sizeof(LPTSTR));
  ////    StringCchCopyW(lvi.pszText, duration.GetLength()+1, CA2T(duration.GetChars()));
  ////    lvi.cchTextMax = duration.GetLength()+1;
  ////  }
  ////  else
  ////  {
  ////    lvi.pszText = (LPTSTR)calloc(1, sizeof(LPTSTR));
  ////    StringCchCopyW(lvi.pszText, 1, "");
  ////    lvi.cchTextMax = 1;
  ////  }
  ////  ListView_SetItem(m_Hwnd, &lvi);
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
    return(_tcsicmp(szBuf1, szBuf2) * -1);
  return(_tcsicmp(szBuf1, szBuf2));
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::Sort
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_ListView::Sort(winampMediaLibraryPlugin* plugin)
{
  LV_SKIN_SORT lv_skin_sort = {0,};
  lv_skin_sort.listView = m_Handle;
  lv_skin_sort.columnIndex = m_SortColumn;
  if (m_SortDirection == HDF_SORTUP)
    lv_skin_sort.ascending = 1;
  else
    lv_skin_sort.ascending = 0;

  m_SubItem = m_SortColumn;
  SendMessage(plugin->hwndLibraryParent, WM_ML_IPC, (WPARAM)&lv_skin_sort, ML_IPC_LISTVIEW_SORT);
  return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::Sort
+---------------------------------------------------------------------*/
NPT_Result
CWinamp_ListView::Sort(LPARAM lParam)
{
  HDITEMW hdi;
  LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
  HWND HwndLVHeader = (HWND) ListView_GetHeader(m_Hwnd);
            
  hdi.mask = HDI_FORMAT;
  Header_GetItem(HwndLVHeader, pnmv->iSubItem, &hdi);

  if(m_SortColumn > -1) // un colonne est déja sélectionnée
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
      hdi.fmt |= HDF_SORTUP;
      m_SortDirection = HDF_SORTUP;
      Header_SetItem(HwndLVHeader, m_SortColumn, &hdi); 
    }
  }
  else
  {
    Header_GetItem(HwndLVHeader, m_SortColumn = pnmv->iSubItem, &hdi);
    hdi.fmt |= HDF_SORTUP;
    m_SortDirection = HDF_SORTUP;
    Header_SetItem(HwndLVHeader, pnmv->iSubItem, &hdi); 
  }

  m_SubItem = pnmv->iSubItem;
  return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this);
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::GetItemText
+---------------------------------------------------------------------*/
NPT_String  
CWinamp_ListView::GetItemText(NPT_Int32 value)
{
  NPT_String        IdObject;
  TCHAR             szBuf1[1024];

  ListView_GetItemText(m_Hwnd, value, 0, szBuf1, sizeof(szBuf1));
  IdObject.Erase(0, IdObject.GetLength());
  IdObject.Append(CW2A(szBuf1));

  return IdObject;
}

///*----------------------------------------------------------------------
//|   CWinamp_ListView::Search
//+---------------------------------------------------------------------*/
//NPT_Result  
//CWinamp_ListView::Search(TCHAR* text, Winamp_MediaObjectsMap MediaObjects)
//{
//  NPT_String s_artist;
//
//  NPT_List<Winamp_MediaObjectsMapEntry*> entries = MediaObjects.GetEntries();
//  if (entries.GetItemCount() != 0) 
//  {
//    ClearAll();
//    NPT_List<Winamp_MediaObjectsMapEntry*>::Iterator entry = entries.GetFirstItem();
//    while (entry) 
//    {
//      PLT_MediaObject MediaObject = (*entry)->GetValue();
//      
//      if (MediaObject.m_People.artists.GetItemCount() > 0)
//      {
//        NPT_List<PLT_PersonRole>::Iterator artist = MediaObject.m_People.artists.GetFirstItem();
//        s_artist = artist->name;
//      }
//
//      if ((strcmp(text, "") == 0) || (s_artist.Find(text,0,true) != -1) 
//          || (MediaObject.m_Affiliation.album.Find(text,0,true) != -1) || (MediaObject.m_Title.Find(text,0,true) != -1))
//      {
//        NPT_String s((NPT_String)(*entry)->GetKey());
//        //AddItem(&MediaObject);
//      }
//      ++entry;
//    }
//  }
//
//  if (m_SubItem != -1)
//    return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this);
//
//  return NPT_SUCCESS;
//}

/*----------------------------------------------------------------------
|   CWinamp_ListView::SavePreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_ListView::SavePreference(LPWSTR section, LPWSTR filename)
{
  TCHAR s[32] = {0};
  if (filename != NULL)
  {
    StringCchPrintfW(s, sizeof(s), L"%d", m_SortDirection);
    WritePrivateProfileStringW(section, CONFIG_SORT_DIR, s, filename);
    StringCchPrintfW(s, sizeof(s), L"%d", m_SortColumn);
    WritePrivateProfileStringW(section, CONFIG_SORT_COL, s, filename);
    return NPT_SUCCESS;
  }
  return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   CWinamp_ListView::LoadPreference
+---------------------------------------------------------------------*/
NPT_Result CWinamp_ListView::LoadPreference(LPWSTR section, LPWSTR filename)
{
  if (filename != NULL)
  {
    m_SortDirection=GetPrivateProfileInt(section, CONFIG_SORT_DIR, 0, filename);
    if (m_SortDirection < 0) m_SortDirection = 0;

    m_SortColumn=GetPrivateProfileInt(section, CONFIG_SORT_COL, HDF_SORTDOWN, filename);
    //if (m_SortColumn < HDF_SORTDOWN) m_SortColumn = HDF_SORTDOWN;

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
    ListView_SetItemState(m_Hwnd,nItem, LVIS_SELECTED, LVIS_SELECTED);
  } while ((nItem = ListView_GetNextItem(m_Hwnd, nItem, LVNI_ALL)) >= 0);
}
