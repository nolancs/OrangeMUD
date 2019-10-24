/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: January 2001                                          [Crossplatform]
 License: MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Copyright 2000-2019 Matthew Nolan, All Rights Reserved
******************************************************************************/
#include "CommonTypes.h"
#include "Act.h"
#include "Exit.h"
#include "MUD.h"
#include "ObjectIndex.h"
#include "Person.h"
#include "RoomIndex.h"
#include "Tables.h"



Exit::Exit(RoomIndex* hInRoom)
    : mToRoom(NULL)
    , mInRoom(hInRoom)
    , mToVRef(kNoRoom)
    , mFlags(0)
    , mKey(kNoKey)
    , mDirection(0)
    , mToOldVnum(0) //Outdated
    , mOldKey(0)    //Outdated
{
}



Exit::~Exit()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::GetTextForCodeTo
//
//   Desc:: An ActThing function overriden so that whenever a $n, $m, etc is
//          encountered in relation to this class that it returns a string you
//          decide upon. The code is the 'n' part of a $n. Every ActThing
//          object is required to respond to at least the '?' code. 
//
//  Input:: hTo: Get the text for hCode relative to this person.
//          hCode: The code.
//
// Output:: The string results.
//
//   Note:: The codes n,e,m,s,v,c are reserved for Person/Player/Mobile classes
//          ONLY so you cannot use those as codes for any other classes to
//          respond to, but all other characters are usable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Exit::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    STRINGCW nText;
    
    
    if(hCode == 'x' || hCode == '?')
    {
        nText = hTo->CanSee(this) ? *mName : "something";
    }
    else
    if(hCode == 'i')
    {
        if(hTo->CanSee(this))
            nText = IsSet(mFlags, kExPlural) ? "are" : "is";
        else
            nText = "is";
    }
    else
    if(hCode == 'd')    //Direction
    {
        nText = gDirName[mDirection];
    }
    else    
    if(hCode == 'w')    //North of here, Above Here
    {
        switch(mDirection)
        {
            case kDirNorth:
            case kDirEast:
            case kDirSouth:
            case kDirWest:
                nText = gDirName[mDirection] + " of here";
                break;
            case kDirUp:
                nText = "above here";
                break;
            case kDirDown:
                nText = "below here";
                break;
            default:
                ASSERT(!"Unknown direction");
        }
    }
    else
    {
        nText.SPrintF("{Unrecognized_Code(%c)_On_Exit}", hCode);
    }
    
    
    return nText;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::GetOtherSideExit
//
//   Desc:: Returns the exit that points back to the room this exit originated
//          from. It may return NULL if there is no matching exit that points
//          back on the other side.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Exit* Exit::GetOtherSideExit()
{
    if(!mToRoom)
        mToRoom = RoomIndex::Get(mToVRef);
    
    if(!mToRoom)
        return NULL;
    
    
    Exit* nRevExit = mToRoom->mExits[gRevDir[mDirection]];
    
    if(!nRevExit)
        return NULL;
    
    if(!nRevExit->mToRoom)
        nRevExit->mToRoom = RoomIndex::Get(nRevExit->mToVRef);
    
    if(mInRoom != nRevExit->mToRoom)
        return NULL;
    
    return nRevExit;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::IsClosed
//
//   Desc:: Returns true if this exit is closed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Exit::IsClosed()
{
    return IsSet(mFlags, kExClosed);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::IsLocked
//
//   Desc:: Returns true if this exit is a door.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Exit::IsDoor()
{
    return IsSet(mFlags, kExIsDoor);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::IsLocked
//
//   Desc:: Returns true if this exit is locked.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Exit::IsLocked()
{
    return IsSet(mFlags, kExLocked);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::IsWater
//
//   Desc:: Returns true if this exits leads to a water room.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Exit::IsWater()
{
    if(!mToRoom)
        return false;
    
    return mToRoom->IsWater();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::IsHidden
//
//   Desc:: Returns true if this is a hidden exit and should not appear on exit
//          scans. Hidden exits are only hidden when they are closed.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Exit::IsHidden()
{
    return IsSet(mFlags, kExHidden) && IsClosed();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::ShowTo
//
//   Desc:: Show this Exit to Person hTo. Shows description and any other
//          important things.
//
//  Input:: The Person to show this Exit to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Exit::ShowTo(Person* hTo)
{
    if(!mDescription.Empty())
    {
        hTo->SendF("%s\n", *mDescription);
    }else{
        Act("You see nothing special $w.\n", hTo, this, NULL, ToChar, kPosDead);
    }
    
    if(IsDoor())
    {
        if(IsClosed())
            Act("$x $i closed.\n", hTo, this, NULL, ToChar, kPosDead);
        else
            Act("$x $i open.\n", hTo, this, NULL, ToChar, kPosDead);
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::StatTo
//
//   Desc:: Sends all the statistics about an Exit to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Exit::StatTo(Person* hTo)
{
    const CHAR* nDirShow[] = { "^![^xNorth^!]^x", "^![^xEast^!]^x ","^![^xSouth^!]^x", 
    "^![^xWest^!]^x ", "^![^xUp^!]^x   ", "^![^xDown^!]^x ", "\n" };
    STRINGCW nBuf;

    nBuf.SPrintF(
    //////////////////////////////////////////////////////////////////////////
    "\n%s\n"
    "   ...Flags : %s\n"
    "   ...Leads : [%hd/%hd] %s\n",
    //////////////////////////////////////////////////////////////////////////
    nDirShow[mDirection],
    *gExitFlags.ListBitNames(mFlags, hTo),
    GetArea(mToVRef), GetVnum(mToVRef), mToRoom ? *mToRoom->mName : "NULL ROOM!");
    hTo->Send(nBuf);

    if(IsDoor())
    {
        if(mKey != kNoKey)
        {
            ObjectIndex* nKeyObj = MUD::Get()->GetObjectIndex(mKey);
            nBuf.SPrintF("     ...Key : [%hd/%hd] %s\n", GetArea(mKey), GetVnum(mKey), nKeyObj ? *nKeyObj->mName : "NON EXISTANT OBJECT!");
            hTo->Send(nBuf);
        }
        nBuf.SPrintF( 
        "...Keywords : %s\n"
        "    ...Name : %s\n",
        *mKeywords, *mName);
        hTo->Send(nBuf);
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::Open
//
//   Desc:: Open this exit.
//
//  Input:: Open both sides or just this side?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Exit::Open(bool hBothSides)
{
    Exit* nExit = NULL;
    RemoveBit(mFlags, kExClosed);
    if(hBothSides && (nExit = GetOtherSideExit()) != NULL)
        RemoveBit(nExit->mFlags, kExClosed);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Exit::Close
//
//   Desc:: Close this exit.
//
//  Input:: Close both sides or just this side?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Exit::Close(bool hBothSides)
{
    Exit* nExit = NULL;
    SetBit(mFlags, kExClosed);
    if(hBothSides && (nExit = GetOtherSideExit()) != NULL)
        SetBit(nExit->mFlags, kExClosed);
}


