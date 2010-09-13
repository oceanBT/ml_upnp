#pragma once

#include "Platinum.h"

/*----------------------------------------------------------------------
|   CWinamp_View
+---------------------------------------------------------------------*/
class CWinamp_View
{
public:
              CWinamp_View();
              ~CWinamp_View();

  inline NPT_Result  MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
  HWND        m_WND;
};
