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
#include "ObjectIndex.h"
#include "Object.h"
#include "Act.h"
#include "StringMagic.h"
#include "Person.h"
#include "RoomIndex.h"
#include "Tables.h"
#include "MUD.h"



Object::Object(ObjectIndex* hIndex)
    : mPopulation(NULL)
{
    if(hIndex == NULL)
        hIndex = ObjectIndex::Get(kVRef_Object_Null);
    
    ASSERT(hIndex != NULL);
    
    mIndex      = hIndex;
    mWearLoc    = kLocNone;
    mContFlags  = hIndex->mContFlags;
    
    //Sets up our mObject to point to the ObjectIndexes mObjectFlyweight
    ObjectProxy::Revert();
    MUD::Get()->PlaceInGame(this);
}




Object::~Object()
{
    MUD::Get()->RemoveFromGame(this);
    ASSERT(mPopulation == NULL);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ObjectIndex* Object::GetIndex() const
{
    return mIndex;
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




STRINGCW Object::GetNameTo(Person* hTo)
{
    return Name();
}



STRINGCW Object::GetPNameTo(Person* hTo, LONG hAmount)
{
    return STRINGCW().SPrintF("%s %s", NumToWord(hAmount), *PName());
}



STRINGCW Object::GetKeywordsTo(Person* hTo)
{
    return Keywords();
}



STRINGCW Object::GetShortDescTo(Person* hTo)
{
    STRINGCW nText = ShortDesc();

        
    return ActParseTo(hTo, nText, NULL, this, NULL);
}



STRINGCW Object::GetPShortDescTo(Person* hTo, LONG hAmount)
{
    STRINGCW nText = PShortDesc();

    
    return ActParseTo(hTo, nText, NULL, ActStr(NumToWord(hAmount)), this);
}



STRINGCW Object::GetDescriptionTo(Person* hTo)
{
    return Description();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::GetTextForCodeTo
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

STRINGCW Object::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    static STRINGCW sSomething = "something";
    STRINGCW nText;
    
    
    //Full Object Name
    if(hCode == 'p' || hCode == '?')
    {
        nText = hTo->CanSee(this) ? GetNameTo(hTo) : sSomething;
    }else
    
    //Object Name Without A/An/The Before It
    if(hCode == 'o')
    {
        nText = hTo->CanSee(this) ? GetNameTo(hTo) : sSomething;
        if(!nText.NCmp("A ", 2))
            nText.Erase(0, 2);
        else 
        if(!nText.NCmp("An ", 3))
            nText.Erase(0, 3);
        else 
        if(!nText.NCmp("The ", 4))
            nText.Erase(0, 4);
        
    }else
    
    //Where The Object Is Residing Currently
    if(hCode == 'w')
    {
        Person*     nInPerson   = dynamic_cast<Person*>(mInsideThis);
        Object*     nInObject   = dynamic_cast<Object*>(mInsideThis);
        RoomIndex*  nInRoom     = dynamic_cast<RoomIndex*>(mInsideThis);
        
        
        if(nInPerson && mWearLoc == kLocNone)
            nText = "Carried";
        else 
        if(mWearLoc != kLocNone)
            nText = "Worn";
        else
        if(nInRoom)
            nText = "Ground";
        else
        if(nInObject)
            nText = "InsideContainer";
        else
            nText = "NoWhere!?";
    }else
    
    //Name Of Liquid Within
    if(hCode == 'l')
    {
        nText = gLiqTable[Value(kDContLiq)].mName;
    }else
    
    //Color Of Liquid Within
    if(hCode == 'k')
    {
        nText = gLiqTable[Value(kDContLiq)].mColor;
    }
    
    //Unknown Code
    else
    {
        nText.SPrintF("{Unrecognized_Code(%c)_On_Object}", hCode);
    }
    
    return nText;
}




bool Object::GetUsers(PersonHolder* hUsers)
{
    RoomIndex* nInRoom = dynamic_cast<RoomIndex*>(mInsideThis);
    if(!nInRoom)
        return false;

    for(PersonIter i = nInRoom->PersonHolder::Begin(); i != nInRoom->PersonHolder::End(); ++i)
    if((*i)->mOnObject == this)
    {
        if(hUsers)
            hUsers->AddPerson(*i);
        else
            return true;
    }
    
    return hUsers ? !hUsers->Empty() : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::CountUsers
//
//   Desc:: Returns the number of people currently on this Object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT Object::CountUsers()
{
    SHORT nUsers = 0;
    RoomIndex* nInRoom = dynamic_cast<RoomIndex*>(mInsideThis);

    if(nInRoom)
    {
        for(PersonIter i = nInRoom->PersonHolder::Begin(); i != nInRoom->PersonHolder::End(); ++i)
        if((*i)->mOnObject == this)
            ++nUsers;
    }
        
    return nUsers;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


void Object::RemoveFrom()
{
    if(!mInsideThis)
        return;
        
    mInsideThis->RemoveObject(this, true);
}



bool Object::IsContainer()
{
    return IsSet(mIndex->mObjectFlags, kObjectIsContainer);
}



bool Object::IsClosed()
{
    ASSERT(IsContainer());
    return IsSet(mContFlags, kContClosed);
}



bool Object::IsLocked()
{
    ASSERT(IsContainer());
    return IsSet(mContFlags, kContLocked);
}



bool Object::IsUsableHow(Person* hTo, BYTE hPos, BYTE hHow)
{
    LONG nFurnFlag = CvtPosHowToFurnFlag(hPos, hHow);
    if(!IsSet(Value(kFurnFlags), nFurnFlag))
    {
        if(!hTo)
            return false;
    
        const CHAR* nOnPosStrs[] = { "stood", "sat", "rested", "slept" };
        const CHAR* nOnHowStrs[] = { "", "at", "on", "in" };
        const CHAR* nOnPosStr = NULL;
        const CHAR* nOnHowStr = nOnHowStrs[hHow];
        switch(hPos)
        {
            case kPosStanding: nOnPosStr = nOnPosStrs[0]; break;
            case kPosSitting : nOnPosStr = nOnPosStrs[1]; break;
            case kPosResting : nOnPosStr = nOnPosStrs[2]; break;
            case kPosSleeping: nOnPosStr = nOnPosStrs[3]; break;
            default: ASSERT(!"Unhandled position"); break;
        }
        CHAR nBuf[30];
        sprintf(nBuf, "%s %s", nOnPosStr, nOnHowStr);
        Act("$p cannot be $T.\n", hTo, this, ActStr(nBuf), ToChar, kPosDead);
        return false;
    }


    if(CountUsers() >= Value(kFurnPeople))
    {
        Act("$p is full.\n", hTo, this, NULL, ToChar, kPosDead);
        return false;
    }
    
    return true;
}



bool Object::IsWearable(LONG hPos)
{
    return IsSet(WearFlags(), hPos);
}



bool Object::IsTakeable()
{
    return IsSet(WearFlags(), kWearTake);
}



bool Object::IsLighting()
{
    return ItemType() == kItemLight && Value(kLightLife);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::ShowTo
//
//   Desc:: Show this Object to Person hTo. Shows description and any other
//          important things.
//
//  Input:: The Person to show this Object to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Object::ShowTo(Person* hTo)
{
    STRINGCW nDescription = GetDescriptionTo(hTo);
    
    if(!nDescription.Empty())
    {
        Act("You examine $p closely...\n", hTo, this, NULL, ToChar, kPosDead);
        hTo->SendF("%s\n", *nDescription);
    }else{
        Act("You see nothing special about $p.\n", hTo, this, NULL, ToChar, kPosDead);
    }
    
    if(!IsSet(WearFlags(), kWearTake))
        return;
        
    /*
    if(iObj->durability >= 0)
        to->SendQ("\nCondition: %s", Condition(to, true, false));
    */
    

    if(WearFlags() != kWearTake)
        hTo->SendF("\n   Wearable: %s", *gWearTable.ListBitNames(WearFlags(), false));

    //** Item Specifics **//
    if(ItemType() == kItemLight)
    {
        LONG nLightLife = Value(kLightLife);
        if(nLightLife < 0)
            hTo->Send("\n   Item gives off light forever when worn.\n");
        else if(nLightLife == 0)
            hTo->Send("\n   Item is a burnt-out light source.\n");
        else if(nLightLife <= 10)
            hTo->Send("\n   Item gives off light for a couple minutes when worn.\n");
        else if(nLightLife <= 20)
            hTo->Send("\n   Item gives off light for a quarter hour or so when worn.\n");
        else if(nLightLife <= 45)
            hTo->Send("\n   Item gives off light for a half hour or so when worn.\n");
        else if(nLightLife <= 90)
            hTo->Send("\n   Item gives off light for around an hour when worn.\n");
        else if(nLightLife <= 300 )
            hTo->Send("\n   Item gives off light for hours and hours when worn.\n");
        else if(nLightLife <= 600)
            hTo->Send("\n   Item gives off light for half a day or so when worn.\n");
        else if(nLightLife <= 2150)
            hTo->Send("\n   Item gives off light for an entire day or so when worn.\n");
        else
            hTo->Send("\n   Item gives off light for days and days when worn.\n");
    }
    else if(IsContainer())
    {
        float nWeightTotal = TotalWeight();
        float nWeightObject = Weight();
        float nWeightContents = nWeightTotal-nWeightObject;
        hTo->SendF("\n   Weight: %.1f lbs, contains %.1f of %.1f lbs", 
        nWeightTotal, nWeightContents, ContCapacity());
    }
    /*else if(IsDrink())
    {
        hTo->SendF("\n   Weight: %.1f lbs, filled %d/%d mL", Weight(),
        Value(DCONT_CURCAP), Value(DCONT_MAXCAP));  
    }*/
    else
    {
        hTo->SendF("\n   Weight: %.1f lbs", Weight());
    }
    
    
    hTo->Send("\n");    
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::ShowContentsTo
//
//   Desc:: Show this Object's contents to Person hTo. Shows the thing you see
//          if you do "look in object", showing object name at top and then
//          a condensed output of everything inside.
//
//  Input:: The Person to show this Object's contents to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Object::ShowContentsTo(Person* hTo)
{
    Act("You look inside ($w) $p:\n", hTo, this, NULL, ToChar, kPosDead);
    
    ObjectHolder nObjectsInside;
    if(!RetrieveObjectsAs(hTo, "all", nObjectsInside))
    {
        hTo->Send("\n    <Empty>\n");
    }else{
        Act("    $p\n", hTo, &nObjectsInside, NULL, ToChar, kPosDead);
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::TotalWeight
//
//   Desc:: Returns a floating point number corresponding to the total weight
//          of this object and its contents (if any). The multiplier is applied
//          to the weight of any objects inside this one.
//
//  Input:: The Person to show this Object's contents to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

float Object::TotalWeight()
{
    float nTotalWeight = 0;
    for(ObjectIter i = ObjectHolder::Begin(); i != ObjectHolder::End(); ++i)
    {
        Object* nContainedObject = *i;
        nTotalWeight += nContainedObject->TotalWeight();
    }
    float nMultiplier = IsContainer() ? Value(kContMulti)/100.0f : 1.0;
    nTotalWeight *= nMultiplier;
    nTotalWeight += Weight();
    return nTotalWeight;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::ContCapacity
//
//   Desc:: Returns the capacity of this container.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

float Object::ContCapacity()
{
    return mIndex->mContCapacity;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::ContKey
//
//   Desc:: Returns the container multiplier.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

float Object::ContMultiplier()
{
    return mIndex->mContMultiplier;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Object::ContKey
//
//   Desc:: Returns the VREF of the key of this container. kNoKey is returned
//          if the container does not have a keyhole.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

VREF Object::ContKey()
{
    return mIndex->mContKey;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void CvtFurnFlagToPosHow(LONG hFurnFlag, BYTE* hPos, BYTE* hHow)
{
    switch(hFurnFlag)
    {
        case kStandAt: if(hHow) *hHow = kUseAt; if(hPos) *hPos = kPosStanding; break;
        case kStandOn: if(hHow) *hHow = kUseOn; if(hPos) *hPos = kPosStanding; break;
        case kStandIn: if(hHow) *hHow = kUseIn; if(hPos) *hPos = kPosStanding; break;
        case kSitAt  : if(hHow) *hHow = kUseAt; if(hPos) *hPos = kPosSitting ; break;
        case kSitOn  : if(hHow) *hHow = kUseOn; if(hPos) *hPos = kPosSitting ; break;
        case kSitIn  : if(hHow) *hHow = kUseIn; if(hPos) *hPos = kPosSitting ; break;
        case kRestAt : if(hHow) *hHow = kUseAt; if(hPos) *hPos = kPosResting ; break;
        case kRestOn : if(hHow) *hHow = kUseOn; if(hPos) *hPos = kPosResting ; break;
        case kRestIn : if(hHow) *hHow = kUseIn; if(hPos) *hPos = kPosResting ; break;
        case kSleepAt: if(hHow) *hHow = kUseAt; if(hPos) *hPos = kPosSleeping; break;
        case kSleepOn: if(hHow) *hHow = kUseOn; if(hPos) *hPos = kPosSleeping; break;
        case kSleepIn: if(hHow) *hHow = kUseIn; if(hPos) *hPos = kPosSleeping; break;
        default:
            ASSERT(!"Bad hPosHowFlag");
    }
}



LONG CvtPosHowToFurnFlag(BYTE hPos, BYTE hHow)
{
    LONG nFurnFlag;
    
    switch(hPos)
    {
        case kPosStanding:
            switch(hHow)
            {
                case kUseAt: nFurnFlag = kStandAt; break;
                case kUseOn: nFurnFlag = kStandOn; break;
                case kUseIn: nFurnFlag = kStandIn; break;
                default: ASSERT(!"Unhandled use"); break;
            }
            break;
        case kPosSitting:
            switch(hHow)
            {
                case kUseAt: nFurnFlag = kSitAt; break;
                case kUseOn: nFurnFlag = kSitOn; break;
                case kUseIn: nFurnFlag = kSitIn; break;
                default: ASSERT(!"Unhandled use"); break;
            }
            break;
        case kPosResting:
            switch(hHow)
            {
                case kUseAt: nFurnFlag = kRestAt; break;
                case kUseOn: nFurnFlag = kRestOn; break;
                case kUseIn: nFurnFlag = kRestIn; break;
                default: ASSERT(!"Unhandled use"); break;
            }
            break;
        case kPosSleeping:
            switch(hHow)
            {
                case kUseAt: nFurnFlag = kSleepAt; break;
                case kUseOn: nFurnFlag = kSleepOn; break;
                case kUseIn: nFurnFlag = kSleepIn; break;
                default: ASSERT(!"Unhandled use"); break;
            }
            break;
        default:
            ASSERT(!"Unhandled position in CvtPosHowToFurnFlag");
            break;
    }
    
    return nFurnFlag;
}


