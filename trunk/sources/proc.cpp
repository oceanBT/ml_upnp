/*****************************************************************
|
|   proc.c - Corps des procédures Windows
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

#include "stdafx.h"
#include "proc.h"
#include "resource.h"
#include "Winamp_UPnPEngine.h"
#include "Winamp_MediaController.h"
#include "Winamp_BrowseTask.h"

extern CWinamp_UPnPEngine  *m_UPnPEngine;

NPT_SET_LOCAL_LOGGER("ml_upnp.proc")


/*----------------------------------------------------------------------
|   EmptyDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK EmptyDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("EmptyDlgProc");
  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   AbortDlgProc
+---------------------------------------------------------------------*/
NPT_Int32 g_Aborted;
LRESULT CALLBACK AbortDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
  switch(uMsg)
  {
    case WM_INITDIALOG:
      g_Aborted = 0;
     break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case ID_SCAN_CANCEL:
          g_Aborted = 1;
          break;
        case WM_DESTROY:
          g_Aborted = 1;
      }
      break;
  }
  return NPT_SUCCESS;
}


/*----------------------------------------------------------------------
|   CfgSupportDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CfgSupportDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("CfgSupportDlgProc");

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CfgMediaServerDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CfgMediaServerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("CfgMediaServerDlgProc");

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CfgMediaRendererDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CfgMediaRendererDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("CfgMediaRendererDlgProc");

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   CfgAboutDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK CfgAboutDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("CfgAboutDlgProc");

  switch(uMsg)
  {
    case WM_INITDIALOG:
    break;
    case WM_COMMAND:
    break;
    case WM_NOTIFY:
    break;
    case WM_DESTROY:
    break;
  }

  return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   MainDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("MainDlgProc");
  NPT_CHECK_POINTER_FATAL(m_UPnPEngine);
  return m_UPnPEngine->MainDlgProc(hwnd,uMsg,wParam,lParam);
}

/*----------------------------------------------------------------------
|   MainRootDlgProc
+---------------------------------------------------------------------*/
LRESULT CALLBACK MainRootDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  NPT_LOG_FINEST("MainRootDlgProc");
  NPT_CHECK_POINTER_FATAL(m_UPnPEngine);
  return m_UPnPEngine->MainRootDlgProc(hwnd,uMsg,wParam,lParam);
}

/*----------------------------------------------------------------------
|   ScanDeviceDlgProc
+---------------------------------------------------------------------*/
CWinamp_MediaController *g_MediaController;
NPT_Int16               g_Cancel;
CWinamp_BrowseTask      *g_Task;

LRESULT CALLBACK ScanDeviceDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  TCHAR buf[256];

  NPT_LOG_FINEST("ScanDeviceDlgProc");

  switch(uMsg)
  {
    case WM_INITDIALOG:
      g_MediaController = (CWinamp_MediaController*) lParam;
      g_Task=NULL;
      g_Cancel = 0;
     break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case ID_SCAN_DEVICE:
          NPT_CHECK_POINTER_FATAL(g_MediaController);
          if (g_Cancel == 0)
          {
            g_Task = new CWinamp_BrowseTask(hwnd, 1, g_MediaController);
            g_MediaController->GetTaskManager()->StartTask(g_Task);

            StringCchPrintfW(buf, sizeof(buf), L"Cancel");
            SetDlgItemText(hwnd, ID_SCAN_DEVICE, buf);
            g_Cancel = 1;
            EnableWindow(GetDlgItem(hwnd,IDC_CHECK), FALSE);
            EnableWindow(GetDlgItem(hwnd,ID_CLOSE), FALSE);
          }
          else
          {
            g_Cancel = 0;
            g_MediaController->GetTaskManager()->StopAllTasks();
            EndDialog(hwnd, 0);
          }
          break;
        case ID_CLOSE:
          NPT_CHECK_POINTER_FATAL(g_MediaController);
          if(IsDlgButtonChecked(hwnd, IDC_CHECK) == BST_CHECKED)
            g_MediaController->GetDatabase()->UpdateScanMSDevice(g_MediaController->GetCurrentDevice()->GetUUID(), 1);
          EndDialog(hwnd, 0);
          break;
        case WM_DESTROY:
          g_Task=NULL;
          g_MediaController=NULL;
          break;
      }
      break;
  }
  return NPT_SUCCESS;
}
