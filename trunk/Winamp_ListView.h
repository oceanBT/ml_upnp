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

#ifdef UNICODE
#define CONFIG_SORT_DIR  L"upnp_SortDir"
#define CONFIG_SORT_COL  L"upnp_SortCol"
#else
#define CONFIG_SORT_DIR  "upnp_SortDir"
#define CONFIG_SORT_COL  "upnp_SortCol"
#endif

enum {
    COL_OBJECT_ID,
    COL_ARTIST,
    COL_TITLE,
    COL_ALBUM,
    COL_GENRE,
    COL_DURATION,
 };

/*----------------------------------------------------------------------
|   CWinamp_ListView
+---------------------------------------------------------------------*/
class CWinamp_ListView
{
public:
  CWinamp_ListView();
  CWinamp_ListView(HWND hwnd);
  ~CWinamp_ListView(void);

  NPT_Result  SkinWindow();

  HWND        SetHwnd(HWND hwnd) { return m_Hwnd = hwnd; }
  HWND        GetHwnd() { return m_Hwnd; }

  NPT_Int32   GetSubItem() { return m_SubItem; }
  NPT_Int32   GetSortDirection() { return m_SortDirection; }
  NPT_Int32   GetSortColumn() { return m_SortColumn; }

  NPT_Result  AddColumn(char* text, NPT_Int32 width);
  NPT_Result  InsertColumn(NPT_Int32 column, char* text, NPT_Int32 width);

  void        SelectAll();

  NPT_Result  ClearAll() { m_Row = 0; return (NPT_Result) ListView_DeleteAllItems(m_Hwnd);}
  void        AddItem(NPT_String object_id, itemRecord item);

  NPT_Int32   CountItem()  { return m_Row; }
  NPT_Int32   CountSelectedItem()  { return ListView_GetSelectedCount(m_Hwnd); }
  NPT_Result  Sort(LPARAM lParam);
  NPT_Result  Search(TCHAR* text, CWinamp_ItemMap MediaItems);
  void        GetItemRecordList(itemRecordList *list, CWinamp_ItemMap MediaItems);

  NPT_Result  SavePreference(TCHAR* section, TCHAR* filename);
  NPT_Result  LoadPreference(TCHAR* section, TCHAR* filename);

private:
  static int CALLBACK CompareStringProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

private:
  HWND            m_Hwnd;
  NPT_Int32       m_Column;
  NPT_Int32       m_Row;

  NPT_Int32       m_SortColumn;
  NPT_Int32       m_SortDirection;
  NPT_Int32       m_SubItem;
};

#endif /* _CWINAMP_LISTVIEW_H_ */