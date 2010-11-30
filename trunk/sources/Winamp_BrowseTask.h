/*****************************************************************
|
|   Winamp_BrowseTask.h - Include file for Browse Task Class
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
 Winamp browse task
 */

#ifndef _WINAMP_BROWSETASK_H_
#define _WINAMP_BROWSETASK_H_

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltThreadTask.h"
#include "Winamp_Database.h"
#include "Winamp_MediaController.h"

/*----------------------------------------------------------------------
 |   CWinamp_BrowseTask
 +---------------------------------------------------------------------*/
class CWinamp_BrowseTask : public PLT_ThreadTask
{
public:
  /**
    Constructeur
  */
  CWinamp_BrowseTask (HWND                        hwnd,
                      NPT_Int32                   firstUse,
                      CWinamp_MediaController*    mediaController,
                      NPT_Int32                   nodeId = -1);


protected:
  /**
    Destructeur
  */
   ~CWinamp_BrowseTask(void);

  /**
    Fonction principale
  */
  void DoRun(void);

private:
	NPT_Result RecursiveBrowse(NPT_String object_id);

private:
  // Adresse du plugin
  winampMediaLibraryPlugin            *m_PluginUPNP;

  // Handle de la fenetre
  HWND                                m_Hwnd;

  // Base de données
  CWinamp_Database                    *m_DB;

  // Serveur media
  PLT_DeviceDataReference		      m_Device;

  // Le controleur
  CWinamp_MediaController*            m_MediaController;

  // Identifiant du noeud dans la ML
  NPT_Int32                           m_NodeId;

  // Nombre d'éléments
  NPT_Int32                           m_NbItem;

  // Premiere Utilisation?
  NPT_Int32                           m_FirstUse; //TODO pour rescan à developper

  // Arret de l'application
  NPT_Int32                           m_Abort;

};

#endif /* _WINAMP_BROWSETASK_H_ */
