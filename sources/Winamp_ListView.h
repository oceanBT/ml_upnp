/*****************************************************************
|
|   Winamp_ListView.h - Include file
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

#ifndef _CWINAMP_LISTVIEW_H_
#define _CWINAMP_LISTVIEW_H_
#pragma once

#include "Platinum.h"
#include "gen_ml/ml.h"

#define CONFIG_SORT_DIR  L"upnp_SortDir"
#define CONFIG_SORT_COL  L"upnp_SortCol"

enum {
  LW_OBJECT_ID,
  LW_ARTIST,
  LW_TITLE,
  LW_ALBUM,
  LW_GENRE,
  LW_YEAR,
  LW_TRACKNO,
  LW_LENGTH,
  /*LW_FILENAME,*/
  /*LW_LASTPLAY,*/
  /*LW_RATING,*/
  /*LW_PLAYCOUNT,*/
  LW_MAXNUM
};

/*----------------------------------------------------------------------
|   CWinamp_ListView
+---------------------------------------------------------------------*/
class CWinamp_ListView
{
public:
  CWinamp_ListView();
  ~CWinamp_ListView(void);

  NPT_Result  SkinWindow(winampMediaLibraryPlugin *plugin);  // OK
  NPT_Result  UnskinWindow(winampMediaLibraryPlugin *plugin);  // OK

  HWND        SetHwnd(HWND hwnd) { return m_Hwnd = hwnd; }  // OK
  HWND        GetHwnd() { return m_Hwnd; }  // OK

  NPT_Int32   GetSubItem() { return m_SubItem; }  // OK
  NPT_Int32   GetSortDirection() { return m_SortDirection; }  // OK
  NPT_Int32   GetSortColumn() { return m_SortColumn; }  // OK

  NPT_Result  AddColumn(LPWSTR text, NPT_Int32 width);  // OK
  NPT_Result  InsertColumn(NPT_Int32 column, LPWSTR text, NPT_Int32 width);  // OK

  void        SelectAll();  // OK

  NPT_Result  ClearAll() { m_Row = 0; return (NPT_Result) ListView_DeleteAllItems(m_Hwnd);}  // OK
  void        AddItem(LPWSTR ObjectId, itemRecordW* item);  // OK

  NPT_Int32   CountItem()  { return m_Row; }  // OK
  NPT_Int32   CountSelectedItem()  { return ListView_GetSelectedCount(m_Hwnd); }  // OK
  NPT_Int32   GetNextSelectedItem(NPT_Int32 value) { return ListView_GetNextItem(m_Hwnd, value, LVNI_SELECTED); } //OK
  NPT_String  GetItemText(NPT_Int32 value); //OK

  NPT_Result  Sort(LPARAM lParam);  // OK
  NPT_Result  Sort(winampMediaLibraryPlugin *plugin);
  NPT_Result  Sort() { return (NPT_Result) ListView_SortItemsEx(m_Hwnd, CompareStringProc, (LPARAM)this); } // OK

  NPT_Int32   GetColumnWidth(NPT_Int32 column) { return ListView_GetColumnWidth(m_Hwnd, column); }  //OK

  NPT_Result  SavePreference(LPWSTR section, LPWSTR filename);
  NPT_Result  LoadPreference(LPWSTR section, LPWSTR filename);

private:
  static int CALLBACK CompareStringProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

private:
  // Adresse du plugin
  winampMediaLibraryPlugin            *m_PluginUPNP;

  HWND            m_Hwnd;
  UINT_PTR        m_Handle;

  NPT_Int32       m_Column;
  NPT_Int32       m_Row;

  NPT_Int32       m_SortColumn;
  NPT_Int32       m_SortDirection;
  NPT_Int32       m_SubItem;
};

#endif /* _CWINAMP_LISTVIEW_H_ */