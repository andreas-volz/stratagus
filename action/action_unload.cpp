//   ___________		     _________		      _____  __
//   \_	  _____/______   ____   ____ \_   ___ \____________ _/ ____\/  |_
//    |    __) \_  __ \_/ __ \_/ __ \/    \  \/\_  __ \__  \\   __\\   __\ 
//    |     \   |  | \/\  ___/\  ___/\     \____|  | \// __ \|  |   |  |
//    \___  /   |__|    \___  >\___  >\______  /|__|  (____  /__|   |__|
//	  \/		    \/	   \/	     \/		   \/
//  ______________________                           ______________________
//			  T H E   W A R   B E G I N S
//	   FreeCraft - A free fantasy real time strategy game engine
//
/**@name action_unload.c		-	The unload action. */
/*
**	(c) Copyright 1998,2000 by Lutz Sammer
**
**	$Id$
*/

//@{

#include <stdio.h>
#include <stdlib.h>

#include "freecraft.h"
#include "video.h"
#include "sound_id.h"
#include "unitsound.h"
#include "unittype.h"
#include "player.h"
#include "unit.h"
#include "actions.h"
#include "interface.h"
#include "tileset.h"
#include "map.h"

/**
**	Move to coast.
**
**	@param unit	Pointer to unit.
**	@return		-1 if unreachable, True if reached, False otherwise.
*/
local int MoveToCoast(Unit* unit)
{
    int i;

    DebugLevel3("%p\n",unit->Command.Data.Move.Goal);
    if( !(i=HandleActionMove(unit)) ) {	// reached end-point
	return 0;
    }
    DebugLevel3(__FUNCTION__" Result: %d\n",i);
    if( !CoastOnMap(unit->X,unit->Y) ) {
	DebugLevel2("COAST NOT REACHED\n");
	return -1;
    }

    unit->Command.Action=UnitActionUnload;
    return 1;
}

/**
**	Leave the transporter.
**
**	@param unit	Pointer to unit.
*/
local void LeaveTransporter(Unit* unit)
{
    int i;
    Unit* goal;

    goal=unit->Command.Data.Move.Goal;
    DebugLevel3("Goal %p\n",goal);
    if( goal ) {
	for( i=0; i<MAX_UNITS_ONBOARD; ++i ) {
	    if( goal==unit->OnBoard[i] ) {
		goal->X=unit->X;
		goal->Y=unit->Y;
#ifdef NEW_HEADING
		DropOutOnSide(goal,LookingW
			,unit->Type->TileWidth,unit->Type->TileHeight);
#else
		DropOutOnSide(goal,HeadingW
			,unit->Type->TileWidth,unit->Type->TileHeight);
#endif
		unit->OnBoard[i]=NoUnitP;
		unit->Value--;
		break;
	    }
	}
    } else {
	for( i=0; i<MAX_UNITS_ONBOARD; ++i ) {
	    if( (goal=unit->OnBoard[i]) ) {
		goal->X=unit->X;
		goal->Y=unit->Y;
#ifdef NEW_HEADING
		DropOutOnSide(goal,LookingW
			,unit->Type->TileWidth,unit->Type->TileHeight);
#else
		DropOutOnSide(goal,HeadingW
			,unit->Type->TileWidth,unit->Type->TileHeight);
#endif
		unit->OnBoard[i]=NoUnitP;
		unit->Value--;
	    }
	}
    }
    if( IsSelected(unit) ) {
	UpdateButtonPanel();
	MustRedraw|=RedrawPanels;
    }
    unit->Wait=1;
    unit->Command.Action=UnitActionStill;
    unit->SubAction=0;
}

/**
**	The transporter unloads an unit.
**
**	@param unit	Pointer to unit.
*/
global void HandleActionUnload(Unit* unit)
{
    int i;

    DebugLevel3(__FUNCTION__": %p(%Zd) SubAction %d\n"
	    ,unit,UnitNumber(unit),unit->SubAction);

    switch( unit->SubAction ) {
	//
	//	Move to transporter
	//
	case 0:
	    if( !unit->Command.Data.Move.Goal ) {
		// NOTE: the Move clears the goal!!
		if( (i=MoveToCoast(unit)) ) {
		    if( i==-1 ) {
			if( ++unit->SubAction==1 ) {
			    unit->Command.Action=UnitActionStill;
			    unit->SubAction=0;
			}
		    } else {
			unit->SubAction=1;
		    }
		}
		break;
	    }
	//
	//	Leave the transporter
	//
	case 1:
	    // FIXME: show still animations ?
	    LeaveTransporter(unit);
	    break;
    }
}

//@}
