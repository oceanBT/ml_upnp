/*****************************************************************
|
|   Winamp_MediaRenderer.cpp - Renderer A/V pour Winamp
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

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "StdAfx.h"
#include "Winamp_MediaRenderer.h"

NPT_SET_LOCAL_LOGGER("ml_upnp.Winamp_MediaRenderer")

CWinamp_MediaRenderer::CWinamp_MediaRenderer(const char*  friendly_name,
                                              bool         show_ip,
                                              const char*  uuid,
                                              unsigned int port,
                                              bool         port_rebind) :
  PLT_MediaRenderer(friendly_name, show_ip, uuid, port, port_rebind)
{
}

CWinamp_MediaRenderer::~CWinamp_MediaRenderer(void)
{
}


// AVTransport
NPT_Result 
CWinamp_MediaRenderer::OnNext(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnPause(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnPlay(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnPrevious(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnSeek(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnStop(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnSetAVTransportURI(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnSetPlayMode(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}


// RenderingControl
NPT_Result 
CWinamp_MediaRenderer::OnSetVolume(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnSetVolumeDB(PLT_ActionReference &action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnGetVolumeDBRange(PLT_ActionReference &action)
{
  return NPT_SUCCESS;
}

NPT_Result 
CWinamp_MediaRenderer::OnSetMute(PLT_ActionReference& action)
{
  return NPT_SUCCESS;
}
