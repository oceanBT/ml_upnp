/*****************************************************************
|
|   Winamp_UPnPEngine.h - Moteur UPnP comprenant la gestion de 
|                         l'ensemble des outils A/V UPnP en lien avec
|                         Winamp
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
 UPnP Engine
 */

#ifndef _WINAMP_UPNP_ENGINE_H_
#define _WINAMP_UPNP_ENGINE_H_
#pragma once

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
/* Entete Winamp SDK */
#include "wa_ipc.h"

/* Entete Plugin UPnP */
#include "Winamp_MediaController.h"
#include "Winamp_MediaRenderer.h"

#include "proc.h"

/*----------------------------------------------------------------------
|   CWinamp_UPnPEngine class
+---------------------------------------------------------------------*/
/**
  La Classe CWinamp_UPnPEngine est le moteur du plugin permettant la 
  gestion de l'ensemble des outils A/V UPnP en lien avec Winamp
*/

class CWinamp_UPnPEngine
{
public:
  /**
    Constructeur : Creer le moteur de gestion du plugin.
    @param Adresse du plugin
  */
                CWinamp_UPnPEngine(winampMediaLibraryPlugin *plugin);

  /**
    Destructeur
  */
                ~CWinamp_UPnPEngine(void);


  /**
    Retourne l'adresse du Plugin
  */
  winampMediaLibraryPlugin*    GetPlugin(void) { return m_PluginUPNP; }

  /**
    Indique dans le LOG les versions des différents logiciels utilisés :
    Version de l'OS, de Winamp, du plugin et du SDK de Platinum, ainsi que la langue.
  */
  NPT_Result    GetVersion(void); /**/

  /**
    Verifie la version de Winamp et autorise ou non le fonctionnement du plugin
  */
  NPT_Result    CheckWinampVersion(void); /**/

  /**
    Charge des préférences du plugin présentes dans Winamp
  */
  NPT_Result    LoadPreferences(void);

  /**
    Enregistre les préférences du plugin dans Winamp
  */
  NPT_Result    SavePreferences(void);

  /**
    Enregistre une préférence du plugin dans Winamp
    @param variable à enregistrer
    @param valeur de la variable (Chaine de caractère)
  */
  NPT_Result    SavePreference(TCHAR *variable, TCHAR *valeur);

  /**
    Enregistre une préférence du plugin dans Winamp
    @param variable à enregistrer
    @param valeur de la variable (Entier)
  */
  NPT_Result    SavePreference(TCHAR *variable, NPT_Int16 valeur);

  /**
    Creation de la boite de dialogue des préferences
  */
  NPT_Result    CreatePreferencePage(void);

  /**
    SplashScreen du plugin
  */
  NPT_Result    SplashScreen(void); /**/

  /**
    Controle nouvelle version du plugin
  */
  NPT_Result    UpdateCheck(void); /**/

  /**
    Execution de la gestion des outils A/V UPnP
  */
  NPT_Result    Start(void);

  /**
    Arret de la gestion des outils A/V UPnP
  */
  NPT_Result    Stop(void);

  /**
    Retourne le controlleur en cours d'utilisation
  */
  CWinamp_MediaController*    GetController(void) { return m_controller; } 

  /**
    Gestion des messages
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  INT_PTR MessageProc(int message_type, INT_PTR param1, INT_PTR param2, INT_PTR param3);

    /**
    Gestion des process pour la fenetre Root
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  LRESULT CALLBACK  MainRootDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  /**
    Gestion des fenetres
    @param ???
    @param ???
    @param ???
    @param ???

    @return ???
  */
  LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

  // Adresse du plugin
  winampMediaLibraryPlugin  *m_PluginUPNP;

  //Fichier ini de Winamp
  NPT_String                *m_WAConfFile;

  //Version de Winamp
  NPT_Int32                 m_WAVersion;

  //Indice d'enregistrement de la version de Winamp
  NPT_Int32                 m_isWinampPro;

  //Controleur A/V Winamp UPnP 
  CWinamp_MediaController   *m_controller;

  //Server A/V Winamp UPnP 
  /*TMP CLA CWinamp_MediaServer       *m_server; */

  /*******************************************************/
  /* Renderer                                            */
  /*******************************************************/
  PLT_DeviceHostReference   m_RendererDevice; //Renderer A/V Winamp UPnP 
  NPT_Int32                 m_RendererActive; //Indice d'enregistrement de la version de Winamp
  TCHAR                     m_FriendlyName[32];   // Nom du Renderer
  /*******************************************************/

  // Moteur UPnP PLATINUM
  PLT_UPnP                  *m_upnp;

  // Pages des préférences
  prefsDlgRec               m_prefpageUPnPSupport;
  prefsDlgRec               m_prefpageUPnPMediaServer;
  prefsDlgRec               m_prefpageUPnPMediaRenderer;
  prefsDlgRec               m_prefpageUPnPAbout;

  // Valeurs des préférences
  NPT_Int16                 m_UpdateCheck;      /* Indice de controle de MAJ du plugin */
  NPT_Int16                 m_SplashScreen;     /* Indice pour l'affichage du SplashScreen */
  NPT_Int16                 m_MediaServer;      /* Indice pour activer ou non le Server A/V */
  NPT_Int16                 m_MediaRenderer;    /* Indice pour activer ou non le Renderer A/V */
};

#endif // _WINAMP_UPNP_ENGINE_H_