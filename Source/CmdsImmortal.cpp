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
#include "Area.h"
#include "Immortal.h"
#include "Interpret.h"
#include "MapGenerator.h"
#include "Mobile.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "OrangeScript.h"
#include "RoomIndex.h"
#include "StringMagic.h"
#include "StringUtils.h"





void Do_At(Immortal* hCh, STRINGCW& hArg)
{
    STRINGCW nAtArg;
    hArg = hArg.OneArgument(nAtArg);
    RoomIndex* hAtRoom = hCh->FindLocation(nAtArg);
    
    if(!hAtRoom)
    {
        hCh->SendF("No locations matching \"%s\".\n", *nAtArg);
        return;
    }
    
    RoomIndex* nOriginalRoom = hCh->mInRoom;
    hCh->RemoveFrom();
    hCh->PlaceIn(hAtRoom);
    hCh->Interpret(hArg, true);
    hCh->RemoveFrom();
    hCh->PlaceIn(nOriginalRoom);
}




void Do_Compile(Immortal* hCh, STRINGCW& hArg)
{
    static const CHAR*  sMenu[] = { "room", "object", "mobile", "\n" };
    SHORT               nChoice;
    LONG                nScriptNum;
    OrangeScript*       nScript;
    STRINGCW            nArg;

    hArg = hArg.OneArgument(nArg);
    if((nChoice = SearchMenu(nArg, sMenu, false)) == -1)
    {
        hCh->Send("Usage: compile room|object|mobile <vref> <script#>\n");
        return;
    }
    hArg = hArg.OneArgument(nArg);
    
    if(nChoice == 0)
    {
        RoomIndex* nIndex;
        if(!(nIndex = MUD::Get()->ResolveRoomVRef(hCh, nArg)))
            return;
        if(!hArg.ResolveLong(hCh, nScriptNum, 1))
            return;
        if(!(nScript = nIndex->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *nIndex->GetIdentTo(hCh));
            return;
        }
        if(nIndex->CompileTo(nScript, hCh))
            hCh->SendF("Compilation Successful!\n");
    }
    else
    if(nChoice == 1)
    {
        ObjectIndex* nIndex;
        if(!(nIndex = MUD::Get()->ResolveObjectVRef(hCh, nArg)))
            return;
        if(!hArg.ResolveLong(hCh, nScriptNum, 1))
            return;
        if(!(nScript = nIndex->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *nIndex->GetIdentTo(hCh));
            return;
        }
        if(nIndex->CompileTo(nScript, hCh))
            hCh->SendF("Compilation Successful!\n");
    }
    else
    if(nChoice == 2)
    {
        MobileIndex* nIndex;
        if(!(nIndex = MUD::Get()->ResolveMobileVRef(hCh, nArg)))
            return;
        if(!hArg.ResolveLong(hCh, nScriptNum, 1))
            return;
        if(!(nScript = nIndex->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *nIndex->GetIdentTo(hCh));
            return;
        }
        if(nIndex->CompileTo(nScript, hCh))
            hCh->SendF("Compilation Successful!\n");
    }
}




void Do_Force(Immortal* hCh, STRINGCW& hArg)
{
    SHORT           nChTrust = hCh->GetTrust();
    PersonHolder    nFound, nForced;
    STRINGCW        nWhoArg;    
    
    
    hArg = hArg.OneArgument(nWhoArg);
    if(nWhoArg.Empty() || hArg.Empty())
    {
        hCh->Send("Force who to do what?\n");
        return;
    }
    
    
    //Find People
    PersonIter nScan, nScanNext;
    hCh->mInRoom->RetrievePeopleAs(hCh, nWhoArg, nFound);
    if(nFound.Exists(hCh, &nScan))
        nFound.RemovePerson(nScan);
    if(nFound.Size() == 0)
    {
        hCh->SendF("There's no \"%s\" around here.\n", *nWhoArg);
        return;
    }
    
    
    //Filter People We Cant Force
    Person* nPerson;
    for(nScan = nFound.Begin(); nScan != nFound.End(); nScan = nScanNext)
    {
        ++(nScanNext = nScan);
        nPerson = *nScan;
        if(nPerson->GetTrust() > nChTrust)
            continue;
        
        nFound.RemovePerson(nScan);
        nForced.AddPerson(nPerson);
    }
    if(nForced.Empty())
    {
        hCh->Send("You weren't powerful enough to force them.\n");
        return;
    }


    //Force Them
    ActStr nActArg(hArg);

    Act("You force $N to \"$t\".\n", hCh, nActArg, &nForced, ToChar);
    for(nScan = nForced.Begin(); nScan != nForced.End(); ++nScan)
    {
        nPerson = *nScan;
        Act("$n forces you to \"$t\".\n", hCh, nActArg, nPerson, ToVict, kPosDead);
        STRINGCW nCopyOfCommand = hArg;
        nPerson->Interpret(nCopyOfCommand);
    }
}




void Do_Goto(Immortal* hCh, STRINGCW& hArg)
{
    RoomIndex* hToLoc = hCh->FindLocation(hArg);
    
    if(!hToLoc)
    {
        hCh->SendF("No locations matching \"%s\".\n", *hArg);
        return;
    }
    
    static STRINGCW sGenericPoofIn  = "$n appears in a swirling mist.\n";
    static STRINGCW sGenericPoofOut = "$n vanishes.\n";
    
    STRINGCW nPoofIn    = hCh->mPoofIn.Empty()  ? sGenericPoofIn  : hCh->mPoofIn  + "\n";
    STRINGCW nPoofOut   = hCh->mPoofOut.Empty() ? sGenericPoofOut : hCh->mPoofOut + "\n";

    Act(nPoofOut, hCh, NULL, NULL, ToRoom);
    hCh->RemoveFrom();
    hCh->PlaceIn(hToLoc);
    Act(nPoofIn, hCh, NULL, NULL, ToRoom);
    
    
    STRINGCW nNoArgument;
    Do_Look(hCh, nNoArgument);
}




void Do_Invis(Immortal* hCh, STRINGCW& hArg)
{
    BYTE nGoToLevel;
    if(!hArg.Empty())
    {
        LONG nTmpLevel;
        if(!hArg.ResolveLong(hCh, nTmpLevel, 1, hCh->GetTrust()))
            return;
        nGoToLevel = nTmpLevel;
    }else
        nGoToLevel = hCh->mInvisLevel ? 0 : hCh->GetTrust();
    
    
    //** Going Visible **//
    if(nGoToLevel == 0)
    {
        hCh->mInvisLevel = nGoToLevel;
        STRINGCW nGenericGoVisSelf("You flicker and fade into view.");
        STRINGCW nGenericGoVisRoom("$n flickers and fades into view.");
        STRINGCW& nToSelf = hCh->mGoVisSelf.Empty() ? nGenericGoVisSelf : hCh->mGoVisSelf;
        STRINGCW& nToRoom = hCh->mGoVisRoom.Empty() ? nGenericGoVisRoom : hCh->mGoVisRoom;
        Act(nToSelf+"\n", hCh, NULL, NULL, ToChar, kPosDead);
        Act(nToRoom+"\n", hCh, NULL, NULL, ToRoom);
    } 
    else
    //** Going Invisible **//
    {
        STRINGCW nGenericGoInvisSelf("You vanish into thin air.");
        STRINGCW nGenericGoInvisRoom("$n vanishes into thin air.");
        STRINGCW& nToSelf = hCh->mGoInvisSelf.Empty() ? nGenericGoInvisSelf : hCh->mGoInvisSelf;
        STRINGCW& nToRoom = hCh->mGoInvisRoom.Empty() ? nGenericGoInvisRoom : hCh->mGoInvisRoom;
        Act(nToSelf+"\n", hCh, NULL, NULL, ToChar, kPosDead);
        Act(nToRoom+"\n", hCh, NULL, NULL, ToRoom);
        hCh->mInvisLevel = nGoToLevel;
    }
}




void Do_Map(Immortal* hCh, STRINGCW& hArg)
{
    SHORT nRadius = 3;
    if(!hArg.Empty())
    {
        if(!hArg.ResolveShort(hCh, nRadius, 1, 100))
            return;
    }
    
    MapGenerator mapGen(hCh->mInRoom, nRadius, nRadius);
    STRINGCW mapString;
    
    mapGen.Generate(mapString);
    hCh->Send(mapString);
}




void Do_MCreate(Immortal* hCh, STRINGCW& hArg)
{
    LONG        nStartWhere, nWantAmount;
    STRINGCW    nMatchArg;
    bool        nAll;
    
    
    //** Check Input **//
    ParseMatching(hArg, nStartWhere, nWantAmount, nMatchArg, nAll);
    if(nAll || nStartWhere != 1)
    {
        hCh->SendF("Invalid argument \"%s\", all and #. may not be used.\n", *hArg);
        return;
    }
    
    
    //** Locate Index **//
    MobileIndex* nIndex;
    if(nMatchArg.Empty() && hCh->mOLC && hCh->mOLC->GetOpenMobile())
        nIndex = hCh->mOLC->GetOpenMobile();
    else
    if(!(nIndex = MUD::Get()->ResolveMobileVRef(hCh, nMatchArg)))
        return;
    
    
    //** Create Mobiles **//
    PersonHolder nCreated;
    for(LONG i = 0; i < nWantAmount; ++i)
    {
        Mobile* nNewMobile = new Mobile(nIndex);
        
        nNewMobile->PlaceInGame(hCh->mInRoom, false);
        nCreated.AddPerson(nNewMobile);
    }
    
    //** Show Creation **//
    static STRINGCW sGenericOneSelf     = "$N coalesces in front of you.\n";
    static STRINGCW sGenericOneRoom     = "$N coalesces in front of $n.\n";
    static STRINGCW sGenericMultiSelf   = "$N coalesce in front of you.\n";
    static STRINGCW sGenericMultiRoom   = "$N coalesce in front of $n.\n";

    if(nCreated.Size() == 1)
    {       
        STRINGCW nOneSelf   = hCh->mMakeMobOneSelf.Empty() ? sGenericOneSelf : hCh->mMakeMobOneSelf + "\n";
        STRINGCW nOneRoom   = hCh->mMakeMobOneRoom.Empty() ? sGenericOneRoom : hCh->mMakeMobOneRoom + "\n";

        Act(nOneSelf, hCh, NULL, &nCreated, ToChar);
        Act(nOneRoom, hCh, NULL, &nCreated, ToRoom);
    }else{
        STRINGCW nMultiSelf = hCh->mMakeMobMultiSelf.Empty() ? sGenericMultiSelf : hCh->mMakeMobMultiSelf + "\n";
        STRINGCW nMultiRoom = hCh->mMakeMobMultiRoom.Empty() ? sGenericMultiRoom : hCh->mMakeMobMultiRoom + "\n";

        Act(nMultiSelf, hCh, NULL, &nCreated, ToChar);
        Act(nMultiRoom, hCh, NULL, &nCreated, ToRoom);
    }
}




void Do_OCreate(Immortal* hCh, STRINGCW& hArg)
{
    LONG        nStartWhere, nWantAmount;
    STRINGCW    nMatchArg;
    bool        nAll;
    
    
    //** Check Input **//
    ParseMatching(hArg, nStartWhere, nWantAmount, nMatchArg, nAll);
    if(nAll || nStartWhere != 1)
    {
        hCh->SendF("Invalid argument \"%s\", all and #. may not be used.\n", *hArg);
        return;
    }
    
    
    //** Locate Index **//
    ObjectIndex* nIndex;    
    if(nMatchArg.Empty() && hCh->mOLC && hCh->mOLC->GetOpenObject())
        nIndex = hCh->mOLC->GetOpenObject();
    else
    if(!(nIndex = MUD::Get()->ResolveObjectVRef(hCh, nMatchArg)))
        return;
    
    
    //** Create Objects **//
    ObjectHolder nCreated;
    for(LONG i = 0; i < nWantAmount; ++i)
        nCreated.AddObject(new Object(nIndex));
    
    
    //** Show Creation **//
    static STRINGCW sGenericOneSelf     = "$P coalesces in your hand.\n";
    static STRINGCW sGenericOneRoom     = "$P coalesces in $n's hand.\n";
    static STRINGCW sGenericMultiSelf   = "$P coalesce in your hand.\n";
    static STRINGCW sGenericMultiRoom   = "$P coalesce in $n's hand.\n";

    if(nCreated.Size() == 1)
    {       
        STRINGCW nOneSelf   = hCh->mMakeObjOneSelf.Empty() ? sGenericOneSelf : hCh->mMakeObjOneSelf + "\n";
        STRINGCW nOneRoom   = hCh->mMakeObjOneRoom.Empty() ? sGenericOneRoom : hCh->mMakeObjOneRoom + "\n";

        Act(nOneSelf, hCh, NULL, &nCreated, ToChar);
        Act(nOneRoom, hCh, NULL, &nCreated, ToRoom);
    }else{
        STRINGCW nMultiSelf = hCh->mMakeObjMultiSelf.Empty() ? sGenericMultiSelf : hCh->mMakeObjMultiSelf + "\n";
        STRINGCW nMultiRoom = hCh->mMakeObjMultiRoom.Empty() ? sGenericMultiRoom : hCh->mMakeObjMultiRoom + "\n";

        Act(nMultiSelf, hCh, NULL, &nCreated, ToChar);
        Act(nMultiRoom, hCh, NULL, &nCreated, ToRoom);
    }
    hCh->AddObjects(&nCreated, true);
}




void Do_Repop(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Usage: repop <area>\n");
        return;
    }
    
    Area* nArea = MUD::Get()->FindArea(hArg);
    if(!nArea)
    {
        hCh->Send("No such area.\n");
        return;
    }
    
    nArea->RePopulate();
    hCh->SendF("Repopulating %s, %s.\n", *nArea->mFileName, *nArea->mName);
}




void Do_Shutdown(Immortal* hCh, STRINGCW& hArg)
{
    MUD::Get()->mMUDAlive = false;
}




void Do_Strings(Immortal* hCh, STRINGCW& hArg)
{
    ///////////////////////////////////////////////////////////////////////////
    static const CHAR* sOutputFormat =
    "%s\n"
    "\n"
    " PoofIn      : %s\n"
    " PoofOut     : %s\n"
    " GoInvisSelf : %s\n"
    " GoInvisRoom : %s\n"
    " GoVisSelf   : %s\n"
    " GoVisRoom   : %s\n"
    " ObjOneSelf  : %s\n"
    " ObjOneRoom  : %s\n"
    " ObjMultiSelf: %s\n"
    " ObjMultiRoom: %s\n"
    " MobOneSelf  : %s\n"
    " MobOneRoom  : %s\n"
    " MobMultiSelf: %s\n"
    " MobMultiRoom: %s\n"
    "\n"
    kGfxThinSep"\n";
    ///////////////////////////////////////////////////////////////////////////
    
    //** Display Current Strings **//
    if(hArg.Empty())
    {
        hCh->SendF(sOutputFormat,
        OverlayCenterLine("[ ^TYour Immortal Message Strings^x ]", kGfxSep),
        *hCh->mPoofIn,
        *hCh->mPoofOut,
        *hCh->mGoInvisSelf,
        *hCh->mGoInvisRoom,
        *hCh->mGoVisSelf,
        *hCh->mGoVisRoom,
        *hCh->mMakeObjOneSelf,
        *hCh->mMakeObjOneRoom,
        *hCh->mMakeObjMultiSelf,
        *hCh->mMakeObjMultiRoom,
        *hCh->mMakeMobOneSelf,
        *hCh->mMakeMobOneRoom,
        *hCh->mMakeMobMultiSelf,
        *hCh->mMakeMobMultiRoom);
        return;     
    }
    
    
    //** Handle Options **//
    enum
    {
        kChoicePoofIn = 0   , kChoicePoofOut, 
        kChoiceGoInvisSelf  , kChoiceGoInvisRoom, 
        kChoiceGoVisSelf    , kChoiceGoVisRoom,
        kChoiceObjOneSelf   , kChoiceObjOneRoom, 
        kChoiceObjMultiSelf , kChoiceObjMultiRoom,
        kChoiceMobOneSelf   , kChoiceMobOneRoom, 
        kChoiceMobMultiSelf , kChoiceMobMultiRoom
    };
    static const CHAR* sMenu[] = 
    {
        "PoofIn"        , "PoofOut", 
        "GoInvisSelf"   , "GoInvisRoom", 
        "GoVisSelf"     , "GoVisRoom", 
        "ObjOneSelf"    , "ObjOneRoom", 
        "ObjMultiSelf"  , "ObjMultiRoom", 
        "MobOneSelf"    , "MobOneRoom", 
        "MobMultiSelf"  , "MobMultiRoom", 
        "\n" 
    };
    STRINGCW    nWhichArg;
    SHORT       nChoice;
    
    
    hArg = hArg.OneArgument(nWhichArg);
    if((nChoice = SearchMenu(nWhichArg, sMenu, false)) == -1)
    {
        hCh->SendF("No such string '%s' to set.\n", *nWhichArg);
        return;
    }
    
    
    //** Various Checks **//
    if(nChoice != kChoiceGoInvisSelf && nChoice != kChoiceGoVisSelf
    && nChoice != kChoiceObjOneSelf  && nChoice != kChoiceObjMultiSelf
    && nChoice != kChoiceMobOneSelf  && nChoice != kChoiceMobMultiSelf)
    if(!hArg.Empty() && hArg.Find("$n") == cwsNotFound)
    {
        hCh->SendF("%s must contain $n for your name.\n", sMenu[nChoice]);
        return;
    }
    
    if(nChoice == kChoiceObjOneSelf || nChoice == kChoiceObjOneRoom)
    if(!hArg.Empty() && hArg.Find("$P") == cwsNotFound && hArg.Find("$O") == cwsNotFound)
    {
        hCh->SendF("%s must contain $P or $O for the object.\n", sMenu[nChoice]);
        return;
    }
    
    if(nChoice == kChoiceObjMultiSelf || nChoice == kChoiceObjMultiRoom)
    if(!hArg.Empty() && hArg.Find("$P") == cwsNotFound)
    {
        hCh->SendF("%s must contain $P for the objects.\n", sMenu[nChoice]);
        return;
    }

    if(nChoice == kChoiceMobOneSelf || nChoice == kChoiceMobOneRoom)
    if(!hArg.Empty() && hArg.Find("$N") == cwsNotFound)
    {
        hCh->SendF("%s must contain $N for the mobile.\n", sMenu[nChoice]);
        return;
    }
    
    if(nChoice == kChoiceMobMultiSelf || nChoice == kChoiceMobMultiRoom)
    if(!hArg.Empty() && hArg.Find("$N") == cwsNotFound)
    {
        hCh->SendF("%s must contain $N for the mobiles.\n", sMenu[nChoice]);
        return;
    }

    
    //** Set The String **//
    switch(nChoice)
    {
        case kChoicePoofIn:         hCh->mPoofIn = hArg;            break;
        case kChoicePoofOut:        hCh->mPoofOut = hArg;           break;
        case kChoiceGoInvisSelf:    hCh->mGoInvisSelf = hArg;       break;
        case kChoiceGoInvisRoom:    hCh->mGoInvisRoom = hArg;       break;
        case kChoiceGoVisSelf:      hCh->mGoVisSelf = hArg;         break;
        case kChoiceGoVisRoom:      hCh->mGoVisRoom = hArg;         break;
        case kChoiceObjOneSelf:     hCh->mMakeObjOneSelf = hArg;    break;
        case kChoiceObjOneRoom:     hCh->mMakeObjOneRoom = hArg;    break;
        case kChoiceObjMultiSelf:   hCh->mMakeObjMultiSelf = hArg;  break;
        case kChoiceObjMultiRoom:   hCh->mMakeObjMultiRoom = hArg;  break;
        case kChoiceMobOneSelf:     hCh->mMakeMobOneSelf = hArg;    break;
        case kChoiceMobOneRoom:     hCh->mMakeMobOneRoom = hArg;    break;
        case kChoiceMobMultiSelf:   hCh->mMakeMobMultiSelf = hArg;  break;
        case kChoiceMobMultiRoom:   hCh->mMakeMobMultiRoom = hArg;  break;
        default:
            ASSERT(!"Unhandled Choice"); break;
    }

    hCh->SendF("Set %s: %s\n", sMenu[nChoice], *hArg);
}




void Do_Trigger(Immortal* hCh, STRINGCW& hArg)
{
    LONG                nScriptNum;
    OrangeScript*       nScript;
    STRINGCW            nTargetArg, nScriptArg, nPersonArg;
    Person*             nAsPerson;
    
    hArg = hArg.OneArgument(nTargetArg);
    hArg = hArg.OneArgument(nScriptArg);
    hArg = hArg.OneArgument(nPersonArg);
    if(nTargetArg.Empty() || nScriptArg.Empty())
    {
        hCh->Send("Usage: trigger <target> <script#> [<as person>]\n");
        return;
    }

    if(!nScriptArg.ResolveLong(hCh, nScriptNum, 1))
        return;
        
    if(nPersonArg.Empty())
        nAsPerson = hCh;
    else if(!(nAsPerson = hCh->ResolvePerson(nPersonArg)))
        return;

    //** Run A Room Script **//
    if(nTargetArg == "here")
    {
        if(!(nScript = hCh->mInRoom->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *hCh->mInRoom->GetIdentTo(hCh));
            return;
        }
        
        MUD::Get()->RunScript(nScript, hCh->mInRoom, nAsPerson, NULL, NULL, NULL);
        return;
    }
    

    //** Run A Mobile Script **//
    PersonHolder nPeopleTargets;
    if(hCh->mInRoom->RetrievePeopleAs(hCh, nTargetArg, nPeopleTargets))
    {
        if(nPeopleTargets.Size() != 1)
        {
            hCh->Send("One person at a time!\n");
            return;
        }
        Mobile* nMobile = nPeopleTargets.Front()->isMobile;
        if(!nMobile)
            return;
        if(!(nScript = nMobile->mIndex->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *nMobile->mIndex->GetIdentTo(hCh));
            return;
        }
        
        MUD::Get()->RunScript(nScript, hCh->mInRoom, nAsPerson, nMobile, NULL, NULL);
        return;
    }


    //** Run An Object Script **//
    ObjectHolder nObjectTargets;
    if(hCh->RetrieveObjectsHere(nTargetArg, nObjectTargets, kFromInvWearRoom))
    {
        if(nObjectTargets.Size() != 1)
        {
            hCh->Send("One object at a time!\n");
            return;
        }
        Object* nObject = nObjectTargets.Front();
        if(!(nScript = nObject->mIndex->GetScript(nScriptNum)))
        {
            hCh->SendF("There is no script #%ld on %s.\n", nScriptNum, *nObject->mIndex->GetIdentTo(hCh));
            return;
        }
        
        MUD::Get()->RunScript(nScript, hCh->mInRoom, nAsPerson, NULL, nObject, NULL);
        return;
    }
    
    
    hCh->Send("No such target here.\n");
}




