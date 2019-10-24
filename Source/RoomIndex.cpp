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
#include "Area.h"
#include "MUD.h"
#include "Object.h"
#include "Person.h"
#include "RoomIndex.h"
#include "StringMagic.h"
#include "Tables.h"
#include "TEditor.h"


RoomIndex::RoomIndex()
    : mOwner        (0)
    , mInArea       (NULL)
    , mLights       (0)
    , mNumPlayers   (0)
    , mLastAmbience (0)
    , mClan         (0)
    , mFlags        (0)
    , mTerrain      (0)
    , mCapacity     (0)
    , mAmbFreq      (60)
    , mHitBonus     (0)
    , mEnergyBonus  (0)
    , mMoveBonus    (0)
{
    for(SHORT i = 0; i < kDirMax; ++i)
        mExits[i] = NULL;
}




RoomIndex::~RoomIndex()
{
    for(SHORT i = 0; i < kDirMax; ++i)
    if(mExits[i])
        delete mExits[i];
}



RoomIndex* RoomIndex::Get(VNUM hVnum)
{
    return MUD::Get()->GetRoomIndex(hVnum);
}


RoomIndex* RoomIndex::Get(VREF hVRef)
{
    return MUD::Get()->GetRoomIndex(hVRef);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::GetTextForCodeTo
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

STRINGCW RoomIndex::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    STRINGCW nText;
    
    
    if(hCode == '?')
    {
        if(hTo->IsImm())
            nText.SPrintF("%s [%s]", *mName, *GetIdentTo(hTo));
        else
            nText = mName;
    }
    else
    {
        nText.SPrintF("{Unrecognized_Code(%c)_On_RoomIndex}", hCode);
    }
    
    
    return nText;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::IsDark
//
//   Desc:: Checks the natural state of the room, plus any lights to see if it
//          is dark. kRoomDark/kRoomLight override time of day/weather, but not
//          player lights. If the room is indoors and kRoomDark/kRoomLight are
//          not set, it is dark if no player is carrying a light. If the room
//          is outdoors the light is determined by time of day, and then by
//          player light.
//
// Output:: True if the room currently appears to be dark.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool RoomIndex::IsDark()
{
    bool nIsDark = false;
    
    //Check time out day & weather
    if(IsIndoors())
        nIsDark = true;
    else
    {
        //weather stuff here
    }
    
    //Check kRoomDark/kRoomLight
    if(IsSet(mFlags, kRoomDark))
        nIsDark = true;
    if(IsSet(mFlags, kRoomLight))
        nIsDark = false;
        
    if(mLights)
        nIsDark = false;
    
    return nIsDark;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::IsIndoors
//
//   Desc:: Checks if the RoomIndex is currently sheltered by walls and a 
//          ceiling, considered being indoors. Elements of storms and such
//          wouldn't hurt any occupants if this is true.
//
// Output:: True if the room currently appears to be sheltered/indoors.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool RoomIndex::IsIndoors()
{
    return  mTerrain == kTerInside || IsSet(mFlags, kRoomIndoors);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::IsPrivate
//
//   Desc:: Checks if the RoomIndex is currently private, a privacy shield
//          object has been dropped inside of it.
//
// Output:: True if the room is currently private.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool RoomIndex::IsPrivate()
{
    return RetrieveObjectAs(NULL, kVRef_Object_Privacy);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::IsWater
//
//   Desc:: Checks if this room is a water terrain room.
//
// Output:: True if it's water, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool RoomIndex::IsWater()
{
    return (mTerrain == kTerWaterSwim || mTerrain == kTerWaterNoSwim);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::ShowTo
//
//   Desc:: Show this RoomIndex to Person hTo. Shows description and any other
//          important things. If you set hEnteringRoom to true it shows the
//          autoscan which wouldn't normally be seen if you just did a plain
//          "look" command.
//
//  Input:: hTo: The Person to show this Room to.
//          hEnteringRoom: If they just entered the room (show AutoScan).
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void RoomIndex::ShowTo(Person* hTo, bool hEnteringRoom)
{
    ASSERT(hTo);
    bool nCanSeeRoom = hTo->CanSee(this);
    
    
    //** Show Room Name **//
    if(hTo->IsImm())
        hTo->SendF("^@%s [Room %s]^x\n", *mName, *GetIdentTo(hTo)); 
    else
    {
        if(nCanSeeRoom)
            hTo->SendF("^@%s^x\n", *mName); else hTo->Send("^@Pitch Black Darkness^x\n");
    }

    //** Show Room Exits **//
    if(hTo->IsOpt(kOptAutoExits))
        ShowExitsTo(hTo);

    //** Show Description **//
    if(nCanSeeRoom)
        hTo->SendF("\n^%%%s^x\n", *mDescription);
    else
        hTo->Send("\n^%It's pitch black, you can't see a thing!^x\n");


    //** Show AutoScan **//
    if(nCanSeeRoom && hEnteringRoom && hTo->IsOpt(kOptAutoScan))
    {
        hTo->Send("^.");
        ShowScanTo(hTo, true);
        hTo->Send("^x");
    }
    
    //** Show People **//
    if(nCanSeeRoom)
    {
        hTo->Send("\n");
        RoomIndex::ShowPeopleTo(hTo);
    }
    
    //** Show Objects **//
    if(nCanSeeRoom)
        RoomIndex::ShowObjectsTo(hTo);
}



void RoomIndex::ShowPeopleTo(Person* hTo)
{
    const CHAR* sPlayerCode = "^(";
    const CHAR* sMobileCode = "^*";
    const CHAR* nColor;
    LONG        nAmt;
    
    PersonHolder::CombineTo(hTo, PersonLookShortSame);
    if(!PersonHolder::CombinedSize())
        return;
        
    for(ULONG i = 0; i < PersonHolder::CombinedSize(); ++i)
    {
        CombinedPeople* nScan = PersonHolder::CombinedAt(i);
        
        if(nScan->mPerson->mOnObject)
            continue;
        if(!hTo->CanSee(nScan->mPerson))
            continue;

        nColor = nScan->mPerson->isPlayer ? sPlayerCode : sMobileCode;
        
        if((nAmt = nScan->mAmount) == 1)
            hTo->SendF("%s%s^x\n", nColor, *nScan->mPerson->GetShortDescTo(hTo));
        else
            hTo->SendF("%s%s^x\n", nColor, *nScan->mPerson->GetPShortDescTo(hTo, nAmt));
    }
}

void RoomIndex::ShowObjectsTo(Person* hTo)
{
    LONG nAmt;

    ObjectHolder::CombineTo(hTo, ObjectLookShortSame);
    if(!ObjectHolder::CombinedSize())
        return;
        
    for(ULONG i = 0; i < ObjectHolder::CombinedSize(); ++i)
    {
        CombinedObjects* nScan = ObjectHolder::CombinedAt(i);
        
        if((nAmt = nScan->mAmount) == 1)
            hTo->SendF("%s\n", *nScan->mObject->GetShortDescTo(hTo));
        else
            hTo->SendF("%s\n", *nScan->mObject->GetPShortDescTo(hTo, nAmt));
            
        if(nScan->mObject->ItemType() != kItemFurniture)
            continue;
        
        PersonHolder nUsers;
        if(nScan->mObject->GetUsers(&nUsers))
            nUsers.ShowPeopleToAs(hTo);
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::ShowExitsTo
//
//   Desc:: Show this RoomIndex's exits to the person. This is what is shown
//          just below the room name when you look at the room.
//
//  Input:: The Person to show it to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void RoomIndex::ShowExitsTo(Person* hTo)
{
    static CHAR nBuf[400];
    STRINGCW    nOpenStr, nClosedStr;
    bool        nFoundAny = false;
    Exit*       nExit;
    
    
    for(SHORT nDir = 0; nDir < kDirMax; ++nDir)
    if((nExit = mExits[nDir]) != NULL)
    {
        nFoundAny = true;
        if(nExit->IsWater())
            sprintf(nBuf, " ^b%s^#", *gDirName[nDir]);
        else
            sprintf(nBuf, " %s", *gDirName[nDir]);
        
        if(nExit->IsClosed())
            nClosedStr += nBuf; else nOpenStr += nBuf;
    }
    
    if(!nFoundAny)
    {
        hTo->Send("^#[Exits: none]^x\n");
        return;
    }
    if(!nOpenStr.Empty() && !nClosedStr.Empty())
    {
        hTo->SendF("^#[Exits:%s] [Closed:%s]^x\n", *nOpenStr, *nClosedStr);
        return;
    }
    if(!nOpenStr.Empty())
    {
        hTo->SendF("^#[Exits:%s]^x\n", *nOpenStr);
        return;
    }
    if(!nClosedStr.Empty())
    {
        hTo->SendF("^#[Closed:%s]^x\n", *nClosedStr);
        return;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::ShowScanTo
//
//   Desc:: Show a "scan" from this RoomIndex. It shows all the people in
//          adjacent rooms. If it's not an autoscan then it also shows who's
//          in this room.
//
//  Input:: hTo: The Person to show it to.
//          hAutoScan: False if they purposefully typed "scan".
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void RoomIndex::ShowScanTo(Person* hTo, bool hAutoScan)
{
    ASSERT(hTo != NULL);
    PersonHolder    nPeople;
    bool            nFoundAnyone = false;
    RoomIndex*      nScanRoom;
    LONG            nAmt;
    CHAR            nDirBuf[255];
    
    
    for(CHAR i = !hAutoScan ? -1 : 0; i < kDirMax; ++i)
    {
        if(i == -1)
            nScanRoom = this;
        else
        {
            if(mExits[i] == NULL)
                continue;
            if((nScanRoom = mExits[i]->mToRoom) == NULL)
                continue;
            if(mExits[i]->IsClosed())
                continue;
        }
        if(!nScanRoom->RetrievePeopleAs(hTo, "all", nPeople))
            continue;
        
        
        nFoundAnyone = true;
        nPeople.CombineTo(hTo, PersonLookShortSame);
        if(!nPeople.CombinedSize())
            return;     
        
        STRINGCW nOutStr;
        for(ULONG p = 0; p < nPeople.CombinedSize(); ++p)
        {
            if(!nOutStr.Empty())
                nOutStr += ", ";

            if((nAmt = nPeople.CombinedAt(p)->mAmount) == 1)
                nOutStr += nPeople.CombinedAt(p)->mPerson->GetNameTo(hTo);
            else
                nOutStr += nPeople.CombinedAt(p)->mPerson->GetPNameTo(hTo, nAmt);
        }
        
        TEditor nFormat(nOutStr);
        nFormat.FormatWidth(80-14);
        nFormat.Update();
        nFormat.PrefixLines("              ");
        nOutStr = nFormat.mAfter.c_str();
        
        sprintf(nDirBuf, "%12s: ", i == -1 ? "here" : *gDirName[i]);
        memcpy(nOutStr.CStr(), nDirBuf, strlen(nDirBuf));
        
        hTo->Send("\n");
        hTo->Send(nOutStr);
        nPeople.ClearPeople();
    }
    
    if(nFoundAnyone)
        hTo->Send("\n");
    
    if(!nFoundAnyone && !hAutoScan) 
        hTo->Send("You see nothing in the vicinity.\n");
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::GetExitsCharsTo
//
//   Desc:: Quick way to grab the "NEsW" exit chars you'd see in the prompt or
//          whatnot. It colors the water exits blue so don't expect any ANSI
//          codes to bleed all the way through this text.
//
//  Input:: The Person who's looking for exits.
//
// Output:: Reference to the static string output. Copy or use right away, the
//          next call to this function invalidates previous results.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW& RoomIndex::GetExitsCharsTo(Person* hTo)
{
    ASSERT(hTo != NULL);
    static STRINGCW sExitChars;
    static bool     sIsClosed;
    static bool     sDoWater;
    
    sExitChars.Clear();
    
    for(SHORT i = 0; i < kDirMax; ++i)
    if(mExits[i] && hTo->CanSee(mExits[i]))
    {
        sDoWater    = mExits[i]->IsWater();
        sIsClosed   = mExits[i]->IsClosed();
        
        //If it's closed it only shows up as water if we're already in a water room
        if(sDoWater && sIsClosed)
            sDoWater = IsWater();
        
        if(sDoWater)
            sExitChars += "^b";
            
        
        if(sIsClosed)
            sExitChars += LOWER(gDirName[i][0]);
        else
            sExitChars += UPPER(gDirName[i][0]);
            
        if(sDoWater)
            sExitChars += "^x";
    }
    
    
    return sExitChars;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::StatTo
//
//   Desc:: Sends all the statistics about a RoomIndex to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void RoomIndex::StatTo(Person* hTo)
{
    STRINGCW nBuf, nTitleBuf;

    nTitleBuf.SPrintF("(( ^T%s^x :: ^T%s^x ))", *GetIdentTo(hTo), *mName);
    nTitleBuf = CenterLine(nTitleBuf.Capitalize(true)); 
    const CHAR* nCapBuf = mCapacity ? NumToWord(mCapacity) : "infinite";

    nBuf.CSPrintF(
    //////////////////////////////////////////////////////////////////////////////////////////
    kGfxSep"\n"
    "%s\n"
    kGfxSep"\n" 
    "\n"
    "Inside Area : %-30s      Terrain : %s\n"
    "Room Number : %-30d    Hit Regen : %+d%%\n"
    "      Exits : %-30s Energy Regen : %+d%%\n"
    "   Capacity : %-30s   Move Regen : %+d%%\n"
    "      Owner : %-30s     Ambience : %d seconds\n"
    "       Clan : %s\n"
    "      Flags : %s\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *nTitleBuf              ,
    *mInArea->mName         , gTerrainTable[mTerrain].mName ,
    GetVnum(mVRef)          , mHitBonus                     ,
    *GetExitsCharsTo(hTo)   , mEnergyBonus                  ,
    nCapBuf                 , mMoveBonus                    ,
    "n/a"                   , mAmbFreq                      ,
    "n/a"                   ,
    *gRoomFlags.ListBitNames(mFlags, hTo));
    hTo->Send(nBuf);
    
    //Spots
    SpottedThing::StatListTo(hTo);
    
    //Scripts
    ScriptedThing::StatListTo(hTo);
    
    hTo->Send(kGfxLine"\n");
}






