/*
 * Copyright (C) 1993-2006 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 * 
 * $Id: main-rig.cpp,v 1.5 2006/01/25 21:54:00 jeremy_laine Exp $
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sailcutqt.h"
#include "formrig.h"

int main(int argc, char *argv[])
{
    int retCode = 1;

    CSailApp app( argc, argv );
    app.readPrefs();

    CFormRig rig(&app);
    app.setMainWidget( &rig );

    rig.show();
    retCode = app.exec();

    app.writePrefs();
    return retCode;
}

