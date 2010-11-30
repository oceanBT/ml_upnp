/*****************************************************************
|
|   proc.h - Fichier d'entete des proc�dures Windows
|
| Copyright (c) 2010, C�dric Lallier.
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
 Windows Procedures
 */

#ifndef _PROC_H_
#define _PROC_H_
#pragma once

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "stdafx.h"



/* Fonction vide */
LRESULT CALLBACK EmptyDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AbortDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Fonctions pour les �crans de configuration */
LRESULT CALLBACK CfgSupportDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CfgAboutDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CfgMediaServerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CfgMediaRendererDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Fonction pour la fenetre Root du plugin */
LRESULT CALLBACK MainRootDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Fonctions pour l'�cran principal */
LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Fonction pour l'�cran de scan d'appareil */
LRESULT CALLBACK ScanDeviceDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


#endif /* _PROC_H_ */