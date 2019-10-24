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
#include "Interpret.h"
#include "ObjectHolder.h"
#include "Act.h"
#include "RoomIndex.h"
#include "Object.h"
#include "StringMagic.h"
#include "Tables.h"


//## Local Prototypes ##//
bool DetermineWearLocation(Person *hCh, Object* hItem, STRINGCW& hWhereArg, bool hReplace, LONG& hTryLoc);






void Do_Drop(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Drop what?\n");
        return;
    }
    
    
    ObjectHolder nObjects;
    if(!hCh->RetrieveObjectsAs(hCh, hArg, nObjects))
    {
        hCh->SendF("You aren't carrying any %s.\n", *hArg);
        return;
    }
    
    
    hCh->RemoveObjects(&nObjects, true);
    hCh->mInRoom->AddObjects(&nObjects, true);
    
    Act("You drop $p.\n", hCh, &nObjects, NULL, ToChar);
    Act("$n drops $p.\n", hCh, &nObjects, NULL, ToRoom);
}




void Do_Get(Person* hCh, STRINGCW& hArg)
{
    STRINGCW nTargetArg, nContArg;
    
    hArg = hArg.OneArgument(nTargetArg);
    hArg = hArg.OneArgument(nContArg);
    
    
    
    //** Get From Room **//
    if(nContArg.Empty())
    {
        ObjectHolder nObjects;
        if(!hCh->mInRoom->RetrieveObjectsAs(hCh, nTargetArg, nObjects))
        {
            hCh->SendF("You don't see any %s.\n", *nTargetArg);
            return;
        }
    
        
        hCh->mInRoom->RemoveObjects(&nObjects, true);
        hCh->AddObjects(&nObjects, true);
        
        Act("You get $p.\n", hCh, &nObjects, NULL, ToChar);
        Act("$n gets $p.\n", hCh, &nObjects, NULL, ToRoom);     
        return;
    }
    
    
    //** Get From Container(s) **//
    ObjectHolder nContainers;

    if(!hCh->RetrieveObjectsHere(nContArg, nContainers, kFromInvWearRoom))
    {
        hCh->SendF("You don't see any %s around here.\n", *nContArg);
        return;
    }
    
    if(nContainers.Size() > 1)
    {
        hCh->Send("You can only get from one thing at a time.\n");
        return;
    }
    
    Object*         nContObject = *nContainers.Begin();
    ObjectHolder    nObjects;
    
    if(!nContObject->IsContainer())
    {
        Act("$p is not a container.\n", hCh, nContObject, NULL, ToChar);
        return;
    }
    else if(!nContObject->RetrieveObjectsAs(hCh, nTargetArg, nObjects))
    {
        Act("You don't see any $t inside ($W) $P.\n", hCh, ActStr(nTargetArg), nContObject, ToChar);
        return;
    }


    nContObject->RemoveObjects(&nObjects, true);
    hCh->AddObjects(&nObjects, true);
    
    Act("You get $p from $P.\n", hCh, &nObjects, nContObject, ToChar);
    Act("$n gets $p from $P.\n", hCh, &nObjects, nContObject, ToRoom);      
}




void Do_Put(Person* hCh, STRINGCW& hArg)
{
    STRINGCW nTargetArg, nContArg;
    
    
    hArg = hArg.OneArgument(nTargetArg);
    hArg = hArg.OneArgument(nContArg);
    
    if(nTargetArg.Empty() || nContArg.Empty())
    {
        hCh->Send("Put what where?\n");
        return;
    }
    
    
    //Find The Container Object In Inventory/Wearing/Room
    ObjectHolder nContainers;
    
    if(!hCh->RetrieveObjectsHere(nContArg, nContainers, kFromInvWearRoom))
    {
        hCh->SendF("You don't see any %s around here.\n", *nContArg);
        return;
    }
    
    if(nContainers.Size() > 1)
    {
        hCh->Send("You can only put into one thing at a time.\n");
        return;
    }
    
    Object* nContObject = *nContainers.Begin();
    if(!nContObject->IsContainer())
    {
        Act("$p is not a container.\n", hCh, nContObject, NULL, ToChar);
        return;
    }

        
    //Retrieve The Objects From The Container
    ObjectHolder    nObjects;
    
    if(!hCh->RetrieveObjectsAs(hCh, nTargetArg, nObjects))
    {
        hCh->SendF("You aren't carrying any %s.\n", *nTargetArg);
        return;
    }


    //Check The Objects
    ObjectIter  nScan, nScanNext;
    Object*     nObjScan;
    
    for(nScan = nObjects.Begin(); nScan != nObjects.End(); nScan = nScanNext)
    {
        ++(nScanNext = nScan);
        nObjScan = *nScan;      
        
        if(nObjScan == nContObject)
        {
            if(nObjects.Size() == 1)
            {
                Act("You can't put $P into itself.\n", hCh, NULL, nContObject, ToChar);
                return;
            }
            
            nObjects.RemoveObject(nScan);
        }
    }
    
    
    //Move Them Into The Container
    hCh->RemoveObjects(&nObjects, true);
    nContObject->AddObjects(&nObjects, true);
    
    Act("You put $p into $P.\n", hCh, &nObjects, nContObject, ToChar);
    Act("$n puts $p into $P.\n", hCh, &nObjects, nContObject, ToRoom);      
}



void Do_Give(Person* hCh, STRINGCW& hArg)
{
    STRINGCW nObjectArg, nPersonArg;
    
    
    hArg = hArg.OneArgument(nObjectArg);
    hArg = hArg.OneArgument(nPersonArg);
    if(nObjectArg.Empty() || nPersonArg.Empty())
    {
        hCh->Send("Give <Objects> <Person>\n");
        return;
    }
    
    
    //** Find Person **//
    PersonHolder nPeople;
    if(!hCh->mInRoom->RetrievePeopleAs(hCh, nPersonArg, nPeople))
    {
        hCh->SendF("There's no \"%s\" person around here.\n", *nPersonArg);
        return;
    }
    if(nPeople.Size() != 1)
    {
        hCh->Send("You can only give to one person at a time.\n");
        return;
    }
    
    
    //** Check Person **//
    Person* nPerson = *nPeople.Begin();
    if(nPerson == hCh)
    {
        hCh->Send("How very generous of you to give things to yourself.\n");
        return;
    }
    if(!nPerson->IsAwake())
    {
        Act("$N is sleeping right now.\n", hCh, NULL, nPerson, ToChar);
        return;
    }
    
    
    //** Find Objects **//
    ObjectHolder nObjects;
    if(!hCh->RetrieveObjectsAs(hCh, nObjectArg, nObjects))
    {
        hCh->SendF("You aren't carrying any \"%s\".\n", *nObjectArg);
        return;
    }
    
    
    //** Check The Objects Can Be Given **//
    ObjectIter  nScan, nScanNext;
    Object*     nObjScan;
    
    for(nScan = nObjects.Begin(); nScan != nObjects.End(); nScan = nScanNext)
    {
        ++(nScanNext = nScan);
        nObjScan = *nScan;      
        
        if(!nPerson->CanHandle(nObjScan))
        {
            if(nObjects.Size() == 1)
            {
                Act("$N can't handle $p, $s hands are full.\n", hCh, nObjScan, nPerson, ToChar);
                Act("$N tried to give you $p but your hands are full.\n", hCh, nObjScan, nPerson, ToVict);
                return;
            }
            
            nObjects.RemoveObject(nScan);
        }
        
        //Give Them The Object
        hCh->RemoveObject(nObjScan, true);
        nPerson->AddObject(nObjScan, true);
    }
    
    
    //** Show The Wonderful Gift They Gave **//
    Act("You give $N $p.\n" , hCh, &nObjects, nPerson, ToChar);
    Act("$n gives you $p.\n", hCh, &nObjects, nPerson, ToVict);
    Act("$n gives $N $p.\n" , hCh, &nObjects, nPerson, ToNotVict);
}


void Do_Sacrifice(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Sacrifice what?\n");
        return;
    }
    
    
    ObjectHolder nFound, nCannot;   
    if(!hCh->RetrieveObjectsHere(hArg, nFound, kFromRoom))
    {
        hCh->SendF("You don't see any %s in the room.\n", *hArg);
        return;
    }
    for(ObjectIter nScanNext, nScan = nFound.Begin(); nScan != nFound.End(); nScan = nScanNext)
    {
        ++(nScanNext = nScan);
        Object* nObject = *nScan;
        if(!nObject->IsTakeable())
        {
            nFound.RemoveObject(nScan);
            nCannot.AddObject(nObject);
        }
    }
    if(nCannot.Size())
        Act("$p can not be sacrificed.\n", hCh, &nCannot, NULL, ToChar);
    
    
    if(nFound.Size())
    {
        ObjectHolder nSacrificed;
        nFound.RemoveFromOwners();
        nSacrificed.AddObjects(&nFound, true);
        Act("You sacrifice $p to the gods.\n", hCh, &nSacrificed, NULL, ToChar);
        Act("$n sacrifices $p to the gods.\n", hCh, &nSacrificed, NULL, ToRoom);
        nSacrificed.ClearObjects();
    }
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Do_Wear(Person* hCh, STRINGCW& hArg)
{
    ObjectHolder    nWearObjects;
    STRINGCW        nWearArg, nWhereArg;
    

    hArg = hArg.OneArgument(nWearArg);
    hArg = hArg.OneArgument(nWhereArg);
    if(!nWhereArg.Empty() && TableLookup(gWearTable, nWhereArg, false) <= 0)
    {
        hCh->SendF("There is no such wear location as \"%s\".\n", *nWhereArg);
        return;
    }
    

    //** Find All The Objects **//
    if(!hCh->RetrieveObjectsAs(hCh, nWearArg, nWearObjects))
    {
        hCh->SendF("You're not carrying any \"%s\".\n", *nWearArg);
        return;
    }
    
    
    //** Want Just A Single Object (Allows AutoReplace) **//
    LONG        nStartAt, nWantAmount;
    bool        nWantAll, nReplace;
    STRINGCW    nTempStr;

    ParseMatching(nWearArg, nStartAt, nWantAmount, nTempStr, nWantAll); 
    nReplace = nWantAmount == 1;

    
    //** Attempt To Wear All The Objects **//
    bool        nShowedResponse = false;
    Object*     nItem, *nRItem;
    LONG        nWearLocItem;
    ObjectIter  nScan;
    
    for(nScan = nWearObjects.Begin(); nScan != nWearObjects.End(); ++nScan)
    {
        nItem = *nScan;
        
        if(!DetermineWearLocation(hCh, nItem, nWhereArg, nReplace, nWearLocItem))
        {
            if(nReplace)
                nShowedResponse = true;
            continue;
        }
        
        //Check For Equipment First
        nRItem = hCh->GetEquipAt(nWearLocItem);
        if(nRItem && !nReplace)
            continue;

        //Can We Use Object To Begin With
        nShowedResponse = true;
        if(!hCh->CanWear(nItem, nWearLocItem))
            continue;
                
        //Replace If We Can
        if(nRItem)
        if(!hCh->CanRemove(nRItem) || !hCh->TryRemove(nRItem))
            continue;
        
        //Finally Try And Wear It
        ASSERT(hCh->GetEquipAt(nWearLocItem) == NULL);
        hCh->TryWear(nItem, nWearLocItem);
    }
    
    
    if(!nShowedResponse)
        hCh->Send("You couldn't wear any of the objects.\n");
}



void Do_Remove(Person* hCh, STRINGCW& hArg)
{
    ObjectHolder    nRemoveObjects;
    STRINGCW        nRemoveArg;
    
    
    hArg.OneArgument(nRemoveArg);
    if(!hCh->mWearing.RetrieveObjectsAs(hCh, nRemoveArg, nRemoveObjects))
    {
        hCh->SendF("You're not wearing any \"%s\".\n", *nRemoveArg);
        return;
    }


    //** Attempt To Remove All The Objects **//
    ObjectIter  nScan;
    Object*     nItem;
    
    for(nScan = nRemoveObjects.Begin(); nScan != nRemoveObjects.End(); ++nScan)
    {
        nItem = *nScan;
        
        if(!hCh->CanRemove(nItem))
            continue;
        
        hCh->TryRemove(nItem);
    }   
}




void Do_Wield(Person* hCh, STRINGCW& hArg)
{
    STRINGCW nTargetArg, nWearArg;
    
    hArg.OneArgument(nWearArg);
    
    nTargetArg += "\"";
    nTargetArg += nWearArg;
    nTargetArg += "\" wield";
    Do_Wear(hCh, nTargetArg);
}




void Do_Draw(Person* hCh, STRINGCW& hArg)
{
    Object *nWield      = hCh->GetEquipAt(kLocWield);
    Object *nSheathed   = hCh->GetEquipAt(kLocSheathed);
    
    if(!nSheathed)
    {
        hCh->Send("You're don't have anything sheathed.\n");
        return;
    }
    
    if(nWield)
    {
        Act("You sheath your $o.\n" , hCh, nWield, NULL, ToChar);
        Act("$n sheathes $s $o.\n"  , hCh, nWield, NULL, ToRoom);
        nWield->mWearLoc = kLocSheathed;
    }

    Act("You draw your $o.\n"   , hCh, nSheathed, NULL, ToChar);
    Act("$n draws $s $o.\n" , hCh, nSheathed, NULL, ToRoom);
    nSheathed->mWearLoc = kLocWield;
}



void Do_Sheath(Person* hCh, STRINGCW& hArg)
{
    Object *nWield      = hCh->GetEquipAt(kLocWield);
    Object *nSheathed   = hCh->GetEquipAt(kLocSheathed);
    
    if(!nWield)
    {
        hCh->Send("You're not wielding anything.\n");
        return;
    }
    
    if(nSheathed)
    {
        Act("You draw your $o.\n", hCh, nSheathed, NULL, ToChar);
        Act("$n draws $s $o.\n", hCh, nSheathed, NULL, ToRoom);
        nSheathed->mWearLoc = kLocWield;
    }

    Act("You sheath your $o.\n", hCh, nWield, NULL, ToChar);
    Act("$n sheathes $s $o.\n", hCh, nWield, NULL, ToRoom);
    nWield->mWearLoc = kLocSheathed;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool DetermineWearLocation(Person *hCh, Object* hItem, STRINGCW& hWhereArg, bool hReplace, LONG& hTryLoc)
{
    //** Wearable At All **//
    LONG nScanWearFlags = hItem->WearFlags() & ~kWearTake;
    if(nScanWearFlags == 0)
    {
        if(hReplace)
            Act("$P cannot be wielded, worn, or held.\n", hCh, NULL, hItem, ToChar, kPosDead);
        return false;
    }
    
    
    //** Find kWear Index In The Wear Table **//
    ULONG nIndex;
    if(!hWhereArg.Empty())
    {
        if((nIndex = TableLookup(gWearTable, hWhereArg, false)) <= 0)
        {
            hCh->SendF("There is no such wear location as \"%s\".\n", *hWhereArg);
            return false;
        }
    }
    else
    {
        for(nIndex = 1; nIndex < gWearTable.size(); ++nIndex)
        if(IsSet(hItem->WearFlags(), gWearTable[nIndex].mWearBit))
            break;
        
        ASSERT(nIndex != gWearTable.size()); //Not wearable anywhere
    }
    
    
    //** Find An Empty Wear Location **//
    for(LONG w = 0; w < kMaxWearTypeLocs; ++w)
    if(gWearTable[nIndex].mWearLocs[w] != kLocNone)
    if(!hCh->GetEquipAt(gWearTable[nIndex].mWearLocs[w]))
    {
        hTryLoc = gWearTable[nIndex].mWearLocs[w];
        return true;
    }
            
    if(!hReplace)
        return false;
    
    //** None Empty, Replace First One  **//
    ASSERT(gWearTable[nIndex].mWearLocs[0] != kLocNone);
    hTryLoc = gWearTable[nIndex].mWearLocs[0];
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::UseFurniture
//
//   Desc:: Given a position and a "on|at|in object" or "object" argument this
//          function will attempt to let the Person use the furniture.
//
//  Input:: hPosition: kPosStanding, kPosSitting, kPosRestings, kPosSleeping.
//          hArg: "on|at|in object" or "object" argument.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::UseFurniture(SHORT hPosition, STRINGCW& hArg)
{
    STRINGCW    nUseArg, nObjectArg;
    BYTE        nHow = kUseNone;
    
    nObjectArg = hArg.OneArgument(nUseArg);
    if(nUseArg == "at")
    {
        nHow = kUseAt;
        hArg = nObjectArg;
    }
    else if(nUseArg == "in")
    {
        nHow = kUseIn;
        hArg = nObjectArg;
    }
    else if(nUseArg == "on")
    {
        nHow = kUseOn;
        hArg = nObjectArg;
    }
    hArg.OneArgument(nObjectArg);
    Object* nObject = ResolveObject(nObjectArg, kFromRoom);
    if(!nObject)
        return;
    if(nObject->ItemType() != kItemFurniture)
    {
        Act("$p is not furniture.\n", this, nObject, NULL, ToChar, kPosDead);
        return;
    }


    //Determine how to use the furniture
    LONG nFurnFlags = nObject->Value(kFurnFlags);
    if(nHow == kUseNone)
    {
        switch(hPosition)
        {
            case kPosStanding:
                     if(IsSet(nFurnFlags, kStandAt))
                    nHow = kUseAt;
                else if(IsSet(nFurnFlags, kStandOn))
                    nHow = kUseOn;
                else if(IsSet(nFurnFlags, kStandIn))
                    nHow = kUseIn;
                else{
                    Act("$p cannot be used with stand.\n", this, nObject, NULL, ToChar, kPosDead);
                    return;
                }
                break;
            case kPosSitting:
                     if(IsSet(nFurnFlags, kSitAt))
                    nHow = kUseAt;
                else if(IsSet(nFurnFlags, kSitOn))
                    nHow = kUseOn;
                else if(IsSet(nFurnFlags, kSitIn))
                    nHow = kUseIn;
                else{
                    Act("$p cannot be used with sit.\n", this, nObject, NULL, ToChar, kPosDead);
                    return;
                }
                break;
            case kPosResting:
                     if(IsSet(nFurnFlags, kRestAt))
                    nHow = kUseAt;
                else if(IsSet(nFurnFlags, kRestOn))
                    nHow = kUseOn;
                else if(IsSet(nFurnFlags, kRestIn))
                    nHow = kUseIn;
                else{
                    Act("$p cannot be used with rest.\n", this, nObject, NULL, ToChar, kPosDead);
                    return;
                }
                break;
            case kPosSleeping:
                     if(IsSet(nFurnFlags, kSleepAt))
                    nHow = kUseAt;
                else if(IsSet(nFurnFlags, kSleepOn))
                    nHow = kUseOn;
                else if(IsSet(nFurnFlags, kSleepIn))
                    nHow = kUseIn;
                else{
                    Act("$p cannot be used with sleep.\n", this, nObject, NULL, ToChar, kPosDead);
                    return;
                }
                break;
            default:
                ASSERT(!"Unhandled position in UseFurniture()");
                break;
        }
    }
    
    
    //Attempt use of furniture object
    switch(hPosition)   
    {
        case kPosStanding:
            if(CanStand(nObject, nHow))
                TryStand(nObject, nHow);
            break;
        case kPosSitting:
            if(CanSit(nObject, nHow))
                TrySit(nObject, nHow);
            break;
        case kPosResting:
            if(CanRest(nObject, nHow))
                TryRest(nObject, nHow);
            break;
        case kPosSleeping:
            if(CanSleep(nObject, nHow))
                TrySleep(nObject, nHow);
            break;
        default:
            ASSERT(!"Unhandled position in UseFurniture()");
            break;
    }
}


