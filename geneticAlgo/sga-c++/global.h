/***************************************************************************
 *   Copyright (C) 2005 by Tian-Li Yu,,,                                   *
 *   tianliyu@fishlaptop.ytgroup                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "chromosome.h"
#include "myrand.h"

extern bool SHOW_POPULATION;
extern bool SHOW_REPLACEMENT;
extern bool SHOW_SELECTION_INDEX;

extern MyRand myRand;
extern void outputErrMsg (const char *errMsg);

#endif
