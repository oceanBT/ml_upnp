/*****************************************************************
|
|   targetver.h - Target Version file
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
#pragma once

// Les macros suivantes définissent la plateforme minimale requise. La plateforme minimale requise
// est la version de Windows, Internet Explorer etc. qui dispose des fonctionnalités nécessaires pour exécuter 
// votre application. Les macros fonctionnent en activant toutes les fonctionnalités disponibles sur les versions de la plateforme jusqu'à la 
// version spécifiée.

// Modifiez les définitions suivantes si vous devez cibler une plateforme avant celles spécifiées ci-dessous.
// Reportez-vous à MSDN pour obtenir les dernières informations sur les valeurs correspondantes pour les différentes plateformes.
#ifndef WINVER                          // Spécifie que la plateforme minimale requise est Windows Vista.
#define WINVER 0x0600           // Attribuez la valeur appropriée à cet élément pour cibler d'autres versions de Windows.
#endif

#ifndef _WIN32_WINNT            // Spécifie que la plateforme minimale requise est Windows Vista.
#define _WIN32_WINNT 0x0600     // Attribuez la valeur appropriée à cet élément pour cibler d'autres versions de Windows.
#endif

#ifndef _WIN32_WINDOWS          // Spécifie que la plateforme minimale requise est Windows 98.
#define _WIN32_WINDOWS 0x0410 // Attribuez la valeur appropriée à cet élément pour cibler Windows Me ou version ultérieure.
#endif

#ifndef _WIN32_IE                       // Spécifie que la plateforme minimale requise est Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Attribuez la valeur appropriée à cet élément pour cibler d'autres versions d'Internet Explorer.
#endif
