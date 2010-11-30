/*****************************************************************
|
|   Winamp_MediaRenderer.h - Renderer A/V pour Winamp
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
 Renderer A/V pour Winamp
 */

#ifndef _WINAMP_MEDIA_RENDERER_H_
#define _WINAMP_MEDIA_RENDERER_H_
#pragma once

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
/* Entete Platinum */
#include "PltUPnP.h"
#include "PltMediaRenderer.h"

class CWinamp_MediaRenderer :
  public PLT_MediaRenderer
{
public:
  CWinamp_MediaRenderer(const char*  friendly_name,
                      bool         show_ip = false,
                      const char*  uuid = NULL,
                      unsigned int port = 0,
                      bool         port_rebind = false);
  ~CWinamp_MediaRenderer(void);

  // AVTransport
  NPT_Result OnNext(PLT_ActionReference& action);
  NPT_Result OnPause(PLT_ActionReference& action);
  NPT_Result OnPlay(PLT_ActionReference& action);
  NPT_Result OnPrevious(PLT_ActionReference& action);
  NPT_Result OnSeek(PLT_ActionReference& action);
  NPT_Result OnStop(PLT_ActionReference& action);
  NPT_Result OnSetAVTransportURI(PLT_ActionReference& action);
  NPT_Result OnSetPlayMode(PLT_ActionReference& action);

  // RenderingControl
  NPT_Result OnSetVolume(PLT_ActionReference& action);
  NPT_Result OnSetVolumeDB(PLT_ActionReference &action);
  NPT_Result OnGetVolumeDBRange(PLT_ActionReference &action);
  NPT_Result OnSetMute(PLT_ActionReference& action);
};

#endif /* _WINAMP_MEDIA_RENDERER_H_ */