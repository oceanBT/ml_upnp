/*****************************************************************
|
|   tools.h - Include file
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

#include "Platinum.h"

#ifndef _ML_TOOLS_H_
#define _ML_TOOLS_H_
#pragma once

void  FormatTimeStamp(NPT_String& out, NPT_UInt32 seconds)
{
  int hours = seconds/3600;
  if (hours == 0) {
      out += "";
  } else {
      out += NPT_String::FromInteger(hours) + ":";
  }

  int minutes = (seconds/60)%60;
  if (minutes == 0) {
      out += "0:";
  } else {
      out += NPT_String::FromInteger(minutes) + ":";
  }

  int secs = seconds%60;
  if (secs == 0) {
      out += "00";
  } else {
      if (secs < 10) {
          out += '0';
      }
      out += NPT_String::FromInteger(secs);
  }
}

#endif /* _ML_TOOLS_H_ */