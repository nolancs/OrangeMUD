/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: October 2001                                          [Crossplatform]
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
#include "Immortal.h"
#include "Interpret.h"
#include "MUD.h"
#include "ObjectIndex.h"
#include "OLC.h"
#include "OrangeScript.h"
#include "RoomIndex.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"
//
//ToDo: REditDelete
//      RSpot
//





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_RList
//
//   Desc:: Room listing function.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_RList(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid rlist options are...\n"
        "rlist <area>         : List rooms from an area file.\n"
        "rlist <keyword>      : List rooms partially matching the keyword(s).\n"
        "rlist <area/keyword> : List rooms in area(s) matching the keyword(s).\n"
        "rlist ^!all^x            : List ALL rooms.\n";
        hCh->Send(nShortHelp);
        return;
    }


    //Fill nCheckAreas and nMatchArg for checking
    STRINGCW    nAreaArg, nMatchArg, nTitle;
    bool        nMatchAll = false;
    MUD*        nMUD = MUD::Get();
    Area*       nArea = NULL;
    List<Area*> nCheckAreas;

    if(StrSuffix(hArg, ".are"))
    {
        nMatchAll = true;
        if(!(nArea = MUD::Get()->FindArea(hArg)))
        {
            hCh->SendF("No areas found matching \"%s\".\n", *hArg);
            return;
        }
        nCheckAreas.push_back(nArea);
        nTitle.SPrintF("[ ^TRooms In The %s Area^x ]", *nArea->mName);
    }
    else if(hArg.IsVRef(&nAreaArg, &nMatchArg))
    {
        if(!(nArea = MUD::Get()->FindArea(nAreaArg)))
        {
            hCh->SendF("No areas found matching \"%s\".\n", *nAreaArg);
            return;
        }
        nCheckAreas.push_back(nArea);
        if(nMatchArg == "all" || nMatchArg == "*" || nMatchArg.Empty())
        {
            nMatchAll = true;
            nTitle.SPrintF("[ ^TRooms In The %s Area^x ]", *nArea->mName);
        }else{
            nTitle.SPrintF("[ ^TRooms Matching \"%s\" In The %s Area^x ]", *nMatchArg, *nArea->mName);
        }           
    }
    else
    {
        nMatchArg = hArg;
        if(nMatchArg == "all" || nMatchArg == "*")
        {
            nMatchAll = true;
            nTitle.SPrintF("[ ^TAll Rooms^x ]", *nMatchArg);
        }else{
            nTitle.SPrintF("[ ^TRooms Matching \"%s\"^x ]", *nMatchArg);
        }
        for(AreaIter i = nMUD->GetAreas().begin(); i != nMUD->GetAreas().end(); ++i)
            nCheckAreas.push_back(i->second);

    }

    
    //Match every ObjectIndex in nCheckAreas against nMatchArg
    ULONG nMatched = 0;
    bool nLeft = true;
    STRINGCW nBuf;
    
    hCh->SendPaged(OverlayCenterLine(nTitle, kGfxSep));
    hCh->SendPaged("\n\n^UVRef   ^x  ^URoom Name...                 ^x    ^UVRef   ^x  ^URoom Name...                 ^x");
    for(List<Area*>::iterator i = nCheckAreas.begin(); i != nCheckAreas.end(); ++i)
    for(IRoomIter j = (*i)->GetRooms().begin(); j != (*i)->GetRooms().end(); ++j)
    {
        RoomIndex* nIndex = j->second;
        if(nMatchAll || nIndex->mName.IsNamed(nMatchArg))
        {
            ++nMatched;
            if(nLeft)
                nBuf.CSPrintF("\n#%-6s  %-29s^x", *nIndex->GetIdentTo(hCh), CTrunc(nIndex->mName, 29));
            else
                nBuf.CSPrintF("    #%-6s  %s^x"   , *nIndex->GetIdentTo(hCh), CTrunc(nIndex->mName, 29));
            hCh->SendPaged(nBuf);
            nLeft = !nLeft;
        }
    }
    hCh->SendPagedF("\n\nFound: %lu.\n%s\n", nMatched, kGfxLine);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_RStat
//
//   Desc:: Room information command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_RStat(Immortal* hCh, STRINGCW& hArg)
{
    RoomIndex* nIndex;
    STRINGCW nArg1, nArg2, nArg3;
    
    hArg = hArg.OneArgument(nArg1);
    hArg = hArg.OneArgument(nArg2);
    hArg = hArg.OneArgument(nArg3);
    if(nArg1 == "script" || nArg1 == "spot")
    {
        nArg3 = nArg2;
        nArg2 = nArg1;
        nArg1.Clear();
    }
    
    if(nArg1.Empty() && hCh->mInRoom)
        nIndex = hCh->mInRoom;
    else
    if(!(nIndex = MUD::Get()->ResolveRoomVRef(hCh, nArg1)))
        return;
    
    if(nArg2.Empty())
    {
        nIndex->StatTo(hCh);
        return;
    }
    else if(nArg2 == "script")
    {
        OrangeScript* nScript;
        if(!nIndex->ResolveScript(hCh, nArg3, nScript))
            return;
        nScript->StatTo(hCh);
    }
    else if(nArg2 == "spot")
    {
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_REdit
//
//   Desc:: Room editing command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_REdit(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid redit options are...\n"
        "redit ^!new^x [vnum] [dir] : Create a room. Optional vnum and direction.\n"
        "redit ^!delete^x           : Delete a room.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = {"new", "delete", "\n"};
    switch(hCh->OLCCheckMenu(kOLC_Check_Area, nMenu, hArg, true))
    {
        case -1: return;
        case 0: hCh->mOLC->REditNew(hArg);      break;
        case 1: hCh->mOLC->REditDelete(hArg);   break;
    }
}



void OLC::REditNew(STRINGCW& hArg)
{
    VNUM nUseVNum = -1;
    CHAR nInDir = -1;
    STRINGCW nScanArg;
    

    hArg = hArg.OneArgument(nScanArg);
    while(nScanArg.Empty() == false)
    {
        if(nScanArg.IsNumber())
        {
            if(!nScanArg.ResolveShort(&mBuilder, nUseVNum, 0))
                return;
                
            if(mArea->GetRoomIndex(nUseVNum))
            {
                mBuilder.SendF("Room %d already in use for %s.\n", nUseVNum, *mArea->mName);
                return;
            }
            
            hArg = hArg.OneArgument(nScanArg);
            continue;
        }
        
        if((nInDir = SearchMenu(nScanArg, gDirNameStrs, false)) == -1)
        {
            mBuilder.Send("Invalid direction. North, east, south, west, up, down.\n");
            return;
        }
        else
        {
            if(mBuilder.mInRoom->mExits[nInDir]
            && mBuilder.mInRoom->mExits[nInDir]->mToRoom)
            {
                mBuilder.SendF("The %s exit already leads to '%s'.\n", gDirNameStrs[nInDir],
                *mBuilder.mInRoom->mExits[nInDir]->mToRoom->mName);
                return;
            }
        }
        
        hArg = hArg.OneArgument(nScanArg);
        continue;
    }
    
    
    //** Pick An Unused Vnum **//
    if(nUseVNum == -1)
    {
        for(nUseVNum = 0; nUseVNum < SHRT_MAX; ++nUseVNum)
        if(!GetOpenArea()->GetRoomIndex(nUseVNum))
            break;
    
        if(nUseVNum == SHRT_MAX)
        {
            mBuilder.SendF("The area is full and has no more room vnums left.\n");
            return;
        }
    }


    //** Room Used? **//
    if(GetOpenArea()->GetRoomIndex(nUseVNum))
    {
        mBuilder.SendF("The %hd room vnum is already taken.\n", nUseVNum);
        return;
    }


    //** Create Room **//
    RoomIndex* nRoom = new RoomIndex();
    nRoom->mName        = "Creation";
    nRoom->mDescription = "Under construction.";
    nRoom->mAmbFreq     = 60;
    nRoom->mVRef        = MakeVRef(GetOpenArea()->mANum, nUseVNum);
    
    GetOpenArea()->AddRoomIndex(nRoom);
    Msg("New room #%d created in %s.", nUseVNum, *GetOpenArea()->mName);
    AreaChanged();
    

    //**Link Up **//
    if(nInDir != -1)
    {
        nScanArg.SPrintF("%s link %d/%d", gDirNameStrs[nInDir], GetOpenArea()->mANum, nUseVNum);
        RSetExit(nScanArg);
        nScanArg.SPrintF("%d/%d rset exit %s link %d/%d", GetOpenArea()->mANum, nUseVNum, 
        gDirNameStrs[gRevDir[nInDir]], mBuilder.mInRoom->ANum(), mBuilder.mInRoom->VNum());
        Do_At(&mBuilder, nScanArg);
    }
}



void OLC::REditDelete(STRINGCW& hArg)
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_RSet
//
//   Desc:: Room setting command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_RSet(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid rset options are...\n"
        "rset ^!name^x <string>   : Name the room.\n"
        "rset ^!desc^x <editor>   : Edit the room description.\n"
        "rset ^!exit^x <dir> ***  : Edit the room exits.\n"
        "rset ^!flag^x <flags>    : Toggle room flags on or off.\n"
        "rset ^!terrain^x <type>  : Change the terrain type.\n"
        "rset ^!hit/energy/move^x <%>: Add % bonus regeneration rate.\n"
        "rset ^!ambience^x <seconds> : Set the frequency of ambience in seconds.\n"
        "rset ^!capacity^x <people>  : Set max number of people the room can fit.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = {"name", "description", "exit", "flag", "terrain",
    "terrain", "hit", "energy", "move", "ambience", "capacity", "\n"};
    switch(hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg))
    {
        case -1: return;
        case 0: hCh->mOLC->RSetName(hArg);          break;
        case 1: hCh->mOLC->RSetDescription(hArg);   break;
        case 2: hCh->mOLC->RSetExit(hArg);          break;
        case 3: hCh->mOLC->RSetFlags(hArg);         break;
        case 4: hCh->mOLC->RSetTerrain(hArg);       break;
        case 5: hCh->mOLC->RSetHit(hArg);           break;
        case 6: hCh->mOLC->RSetEnergy(hArg);        break;
        case 7: hCh->mOLC->RSetMove(hArg);          break;
        case 8: hCh->mOLC->RSetAmbience(hArg);      break;
        case 9: hCh->mOLC->RSetCapacity(hArg);      break;
    }
}



void OLC::RSetName(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    nRoom->mName = hArg.Replace("^x", "^x^@");
    Msg("The room name is now: ^@%s^x", *nRoom->mName);
    AreaChanged();
}


void OLC::RSetDescription(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    if(mBuilder.DoEditor(hArg, nRoom->mDescription, "[ Room Description Cleared ]"))
        AreaChanged();
}



void OLC::RSetFlags(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    if(gRoomFlags.FlagSet(&mBuilder, hArg, nRoom->mFlags))
    {
        AreaChanged();
        Msg("The room flags are now: %s.", *gRoomFlags.ListBitNames(nRoom->mFlags, &mBuilder));     
    }
}



void OLC::RSetTerrain(STRINGCW& hArg)
{
    RoomIndex* nRoom = GetOpenRoom();
    SHORT nTerrain;
    
    if((nTerrain = TableLookup(gTerrainTable, *hArg, false)) == -1)
    {
        CHAR* nShortHelp = 
        "Invalid terrain. Choose from the following types...\n"
        "   inside, city, field, forest, hills, mountain\n"
        "   shallow_water, deep_water, air, desert, cavern\n";
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;
    
    nRoom->mTerrain = nTerrain;
    Msg("The room's terrain type is now %s.", gTerrainTable[nTerrain].mName);
    AreaChanged();
}



void OLC::RSetHit(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    SHORT nBonus;

    if(!hArg.ResolveShort(&mBuilder, nBonus, -10000, 10000))
        return;
    
    nRoom->mHitBonus = nBonus;
    Msg("%s now grants a %hd%% hitpoint regen bonus.", *nRoom->mName, nBonus);
    AreaChanged();
}



void OLC::RSetEnergy(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    SHORT nBonus;

    if(!hArg.ResolveShort(&mBuilder, nBonus, -10000, 10000))
        return;
    
    nRoom->mEnergyBonus = nBonus;
    Msg("%s now grants a %hd%% energy regen bonus.", *nRoom->mName, nBonus);
    AreaChanged();
}



void OLC::RSetMove(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    SHORT nBonus;

    if(!hArg.ResolveShort(&mBuilder, nBonus, -10000, 10000))
        return;
    
    nRoom->mMoveBonus = nBonus;
    Msg("%s now grants a %hd%% move regen bonus.", *nRoom->mName, nBonus);
    AreaChanged();
}



void OLC::RSetAmbience(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    LONG nAmbFreq;

    if(!hArg.ResolveLong(&mBuilder, nAmbFreq, 0))
        return;
    
    nRoom->mAmbFreq = nAmbFreq;
    if(nRoom->mAmbFreq)
        Msg("%s will show ambience about every %ld seconds.", *nRoom->mName, nRoom->mAmbFreq);
    else
        Msg("%s will show no ambience.", *nRoom->mName);
    AreaChanged();
}



void OLC::RSetCapacity(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    RoomIndex* nRoom = GetOpenRoom();
    LONG nCapacity;
    
    if(!hArg.ResolveLong(&mBuilder, nCapacity))
        return;
    
    if(nCapacity < 0)
        nCapacity = 0;
    nRoom->mCapacity = nCapacity;

    if(nCapacity)
        Msg("%s now holds %ld people.", *nRoom->mName, nCapacity);
    else
        Msg("%s now holds unlimited people.", *nRoom->mName);
        
    AreaChanged();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



#define kExitSet_New        0
#define kExitSet_Desc       1
#define kExitSet_Link       2
#define kExitSet_Key        3
#define kExitSet_Keywords   4
#define kExitSet_Delete     5
#define kExitSet_Flags      6
#define kExitSet_Name       7

#define kExitSet_DirStat    6



void OLC::RSetExit(STRINGCW& hArg)
{
    //** Short Help **//
    if(hArg.Empty())
    {
        CHAR* nShortHelp =
        "Valid options for rset exit...\n"
        "^!stat^x                    : Show room exit stats.\n"
        "<dir> ^!new/delete^x        : Create or delete an exit.\n"
        "<dir> ^!link^x <#>          : Link an exit to a room.\n"
        "<dir> ^!key^x  <#>          : Set the vnum of the door key.\n"
        "<dir> ^!name^x     <string> : Set the full name of the door.\n"
        "<dir> ^!kw^x       <string> : Set the keywords for door.\n"
        "<dir> ^!descript^x <string> : Set exit direction description.\n"
        "<dir> ^!flags^x <flags>     : Toggle exit flags. Help exit flags for more info.\n";

        mBuilder.Send(nShortHelp);
        return;
    }
    

    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    
    const CHAR* nExitMenu[] = {"new", "descript","link", "key", "kw", "delete", "flags","name", "\n"};
    const CHAR* nDirMenu[] = {"north", "east","south", "west", "up", "down", "stat", "\n"};
    RoomIndex* nRoom = GetOpenRoom();
    SHORT nInDir, nOption;
    STRINGCW nArg1;
    Exit* nExit;
    

    hArg = hArg.OneArgument(nArg1);
    if((nInDir = SearchMenu(nArg1, nDirMenu, false)) == -1)
    {
        mBuilder.Send("Invalid direction. North east south west up down or stat.\n");
        return;
    }
    
    if(nInDir == kExitSet_DirStat)
    {
        RSetExitStat(hArg);
        return;
    }


    hArg = hArg.OneArgument(nArg1);
    if((nOption = SearchMenu(nArg1, nExitMenu, false)) == -1)
    {
        mBuilder.Send("Invalid option. Type rset exit for a list of valid options.\n");
        return;
    }

    nExit = nRoom->mExits[nInDir];
    if(!nExit && nOption != kExitSet_New && nOption != kExitSet_Link)
    {
        mBuilder.Send("There is no exit there.\n");
        return;
    }

    switch(nOption)
    {
        case kExitSet_New:
            RSetExitNew(hArg, nInDir);
            break;
        case kExitSet_Delete:
            RSetExitDelete(hArg, nInDir, nExit);
            break;
        case kExitSet_Link:
            RSetExitLink(hArg, nInDir, nExit);
            break;
        case kExitSet_Key:
            RSetExitKey(hArg, nInDir, nExit);
            break;
        case kExitSet_Name:
            RSetExitName(hArg, nInDir, nExit);
            break;
        case kExitSet_Keywords:
            RSetExitKeywords(hArg, nInDir, nExit);
            break;
        case kExitSet_Desc:
            RSetExitDesc(hArg, nInDir, nExit);
            break;
        case kExitSet_Flags:
            RSetExitFlags(hArg, nInDir, nExit);
            break;
            
        default:
            mBuilder.Send("Error. No code to handle this option.\n");
            break;
    }
}



void OLC::RSetExitStat(STRINGCW& hArg)
{
    RoomIndex* nRoom = GetOpenRoom();
    STRINGCW nTitleBuf;
    
    nTitleBuf.SPrintF("[ ^TExits from [%s] %s^x ]", *nRoom->GetIdentTo(&mBuilder), *nRoom->mName);
    nTitleBuf.Capitalize(true);
    nTitleBuf = OverlayCenterLine(nTitleBuf, kGfxSep);
    mBuilder.SendF("%s\n", *nTitleBuf);

    bool nFound = false;
    for(USHORT i = 0; i < kDirMax; ++i)
    {
        Exit* nExit = nRoom->mExits[i];
        if(nExit != NULL)
        {
            nExit->StatTo(&mBuilder);
            nFound = true;
        }
    }

    if(!nFound)
        mBuilder.Send("\n   No Exits.\n");

    mBuilder.Send(kGfxLine"\n");
}



void OLC::RSetExitNew(STRINGCW& hArg, SHORT hInDir)
{
    RoomIndex* nRoom = GetOpenRoom();
    Exit* nExit;
    
    if(nRoom->mExits[hInDir])
    {
        mBuilder.Send("There is already an exit there.\n");
        return;
    }
    
    nExit               = new Exit(nRoom);
    nExit->mDirection   = hInDir;
    nRoom->mExits[hInDir] = nExit;

    Msg("Exit %s created.", gDirNameStrs[hInDir]);
    AreaChanged();
}



void OLC::RSetExitDesc(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    if(mBuilder.DoEditor(hArg, hExit->mDescription, "[ Exit Description Cleared ]"))
        AreaChanged();
}



void OLC::RSetExitLink(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    RoomIndex* nRoom = GetOpenRoom();
    RoomIndex* nToRoom = MUD::Get()->ResolveRoomVRef(&mBuilder, hArg);
    

    if(nToRoom == NULL)
        return;
    
    if(hExit == NULL)
    {
        STRINGCW nNoArg;
        RSetExitNew(nNoArg, hInDir);
        if(!(hExit = nRoom->mExits[hInDir]))
            return;
    }


    //** Link It **//
    hExit->mToVRef  = nToRoom->VRef();
    hExit->mToRoom  = nToRoom;

    Msg("Exit %s now linked to [%s] %s.", gDirNameStrs[hInDir], 
    *nToRoom->GetIdentTo(&mBuilder), *nToRoom->mName);
    AreaChanged();
}



void OLC::RSetExitKey(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    if(hArg.Empty() || hArg == "none" || hArg == "0/0")
    {   
        hExit->mKey = kNoKey;
        Msg("There is no longer any key for the %s exit.", gDirNameStrs[hInDir]);
        return;
    }
    
    ObjectIndex* nKey = MUD::Get()->ResolveObjectVRef(&mBuilder, hArg);
    if(nKey == NULL)
        return;
    
    hExit->mKey = nKey->VRef();
    Msg("Key for the %s exit is now item [%s] %s.", gDirNameStrs[hInDir],
    *nKey->GetIdentTo(&mBuilder), *nKey->mName);
    AreaChanged();
}



void OLC::RSetExitName(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    hExit->mName = hArg;
    Msg("The %s exit's name is now \"%s\".", gDirNameStrs[hInDir], *hArg);
    AreaChanged();
}



void OLC::RSetExitKeywords(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    hExit->mKeywords = hArg;
    Msg("The %s exit's keywords are now \"%s\".", gDirNameStrs[hInDir], *hArg);
    AreaChanged();
}



void OLC::RSetExitDelete(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    RoomIndex* nRoom = GetOpenRoom();
    Msg("Exit %s deleted.", gDirNameStrs[hInDir]);
    nRoom->mExits[hInDir] = NULL;
    delete hExit;
    AreaChanged();
}



void OLC::RSetExitFlags(STRINGCW& hArg, SHORT hInDir, Exit* hExit)
{
    if(gExitFlags.FlagSet(&mBuilder, hArg, hExit->mFlags))
    {
        AreaChanged();
        Msg("The exit flags are now: %s.", *gExitFlags.ListBitNames(hExit->mFlags, &mBuilder));     
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_RSpot(Immortal* hCh, STRINGCW& hArg)
{
    //Msg("Not implemented.");
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_RScript(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid rscript options...\n"
        "rscript ^!list^x             : List all the scripts on the room.\n"
        "rscript ^!open^x <id>        : Open a script by the id number, or close it.\n"
        "rscript ^!close^x            : Close the room script you're working with.\n"
        "rscript ^!new^x              : Create a new room script.\n"
        "rscript ^!delete^x <id num>  : Delete a room script by its id number.\n"
        "rscript ^!code^x <editor>    : Modify the code for the script (also rcode).\n"
        "rscript ^!trigger^x <type>   : What triggers the script; enter, leave, phrase, etc.\n"
        "rscript ^!commands^x <words> : Set command words to trigger the phrase script.\n"
        "rscript ^!keywords^x <words> : Set the keywords that work with command words.\n"
        "rscript ^!compile^x          : Try to compile the script you're working on.\n"
        "rscript ^!text^x ...         : Edit text variables through the editor interface.\n"
        "rstat ^!script^x <id num>    : Info on a script.\n";
        hCh->Send(nShortHelp);
        return;
    }
    
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithRoomScripts();
    Do_Script(hCh, hArg, hCh->mOLC ? hCh->mOLC->GetOpenRoom() : NULL, kOLC_Check_RoomIndex);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//** Room Editing Shortcuts **//
void Do_RNew(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_Area))
        return;
    hCh->mOLC->REditNew(hArg);
}


//** Room Setting Shortcuts **//
void Do_RName(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_OLC))
        return;

    hCh->mOLC->RSetName(hArg);
}


void Do_RDesc(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_OLC))
        return;

    hCh->mOLC->RSetDescription(hArg);
}


void Do_RExit(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_OLC))
        return;

    hCh->mOLC->RSetExit(hArg);
}


//** Room Scripting Shortcuts **//
void Do_RCode(Immortal* hCh, STRINGCW& hArg)
{
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithRoomScripts();
    if(!hCh->OLCCheck(kOLC_Check_Script))
        return;
    
    hCh->mOLC->ScriptCode(hArg);            
}


void Do_RText(Immortal* hCh, STRINGCW& hArg)
{
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithRoomScripts();
    if(!hCh->OLCCheck(kOLC_Check_ScriptText))
        return;
    
    hCh->mOLC->ScriptTextEdit(hArg);
}
