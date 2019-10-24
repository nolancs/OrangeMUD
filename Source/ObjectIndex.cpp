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
#include "MUD.h"
#include "ObjectIndex.h"
#include "Person.h"
#include "StringMagic.h"
#include "Tables.h"


ObjectIndex::ObjectIndex()
    : mContCapacity     (0)
    , mContMultiplier   (0)
    , mContKey          (0)
    , mContFlags        (0)
    , mCost             (0)
    , mDurability       (0)
    , mItemType         (0)     
    , mLevel            (0)
    , mMaterials        (0) 
    , mAntiFlags        (0)
    , mObjectFlags      (0)
    , mWearFlags        (0)
    , mInPlay           (0)
{
    mWeight = 0;
    for(SHORT i = 0; i < 5; ++i)
        mValue[i] = 0;
}


ObjectIndex::~ObjectIndex()
{
}



ObjectIndex* ObjectIndex::Get(VNUM hVnum)
{
    return MUD::Get()->GetObjectIndex(hVnum);
}

ObjectIndex* ObjectIndex::Get(VREF hVRef)
{
    return MUD::Get()->GetObjectIndex(hVRef);
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




ObjectData* ObjectIndex::GetFlyweight()
{
    if(!mObjectFlyweight)
        mObjectFlyweight = new ObjectData(this);

    return mObjectFlyweight;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectIndex::StatTo
//
//   Desc:: Sends all the statistics about an ObjectIndex to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectIndex::StatTo(Person* hTo)
{
    STRINGCW nBuf, nTitleBuf;
    
    nTitleBuf.SPrintF("(( ^T%s^x :: ^T%s^x ))", *GetIdentTo(hTo), *mName);
    nTitleBuf = CenterLine(nTitleBuf.Capitalize(true)); 
    nBuf.CSPrintF(
    //////////////////////////////////////////////////////////////////////////////////////////
    kGfxSep"\n"
    "%s\n"
    kGfxSep"\n" 
    "    Name: %-57s[InPlay:%ld]\n"
    "   PName: %s\n"
    "   Short: ^&%s^x\n"
    "  PShort: ^&%s^x\n"
    "Keywords: %s^x\n"
    "^UDescription...                                                                  ^x\n"
    "%s\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *nTitleBuf      ,
    *mName          , mInPlay,
    *mPName         ,
    *mShortDesc     ,
    *mPShortDesc    ,
    *mKeywords      ,
    *mDescription);
    hTo->Send(nBuf);
    
    
    nBuf = gItemTable[mItemType].mName;
    nTitleBuf.SPrintF("[_^!%s^x_]", *nBuf.Capitalize(true));
    hTo->Send(OverlayCenterLine(nTitleBuf, kGfxSep));
    
    hTo->SendF("\n Level: %-3hd   Cost: %-7ld Weight: %-5.3f    Durability: %-5s  VRef: %s\n\n",
    mLevel, mCost, mWeight, "xxxx", *GetIdentTo(hTo));
    
    StatSpecificsTo(hTo);

    hTo->SendF(
    //////////////////////////////////////////////////////////////////////////////////////////
    "     Extra: %s\n"
    " Materials: %s\n"
    " Wear Locs: %s\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *gObjectFlags.ListBitNames(mObjectFlags)        ,
    *gMaterialTable.ListBitNames(mMaterials, hTo)   ,
    *gWearTable.ListBitNames(mWearFlags, true));

    //Spots
    SpottedThing::StatListTo(hTo);
    
    //Scripts
    ScriptedThing::StatListTo(hTo);
    
    hTo->Send(kGfxLine"\n");
}






void ObjectIndex::StatSpecificsTo(Person* hTo, LONG* hInPlayValues)
{
    LONG* nValue = hInPlayValues ? hInPlayValues : &mValue[0];
    bool nInPlay = hInPlayValues != NULL;
    ObjectIndex* nKey = NULL;
    
    switch(mItemType)
    {
        case kItemLight:
            hTo->SendF("Light Life: %ld minutes.\n", nValue[kLightLife]);
            break;
            
        case kItemWeapon:
            hTo->SendF("     Class: %-10s  Damage Mod: %ld%%  [%ld-%ld]\n",  
            gWeaponTable[nValue[kWeapClass]].mName, 
            nValue[kWeapMod], nValue[kWeapMin], nValue[kWeapMax]);
            break;

        case kItemScroll:
        case kItemWand:
        case kItemStaff:
        case kItemPotion:
        case kItemPill:
            break;
            
        case kItemArmor:
            hTo->SendF("    AC Mod: %ld%%  [%.1f]\n", nValue[kArmMod], Tenth(nValue[kArmAC]));
            break;
            
        case kItemFurniture:
            hTo->SendF(
            "    People: %d\n"
            "     Flags: %s\n"
            "  Hp/En/Mv: %ld/%ld/%ld%%\n",
            nValue[kFurnPeople], *gFurnFlags.ListBitNames(nValue[kFurnFlags], hTo), nValue[kFurnHit], 
            nValue[kFurnEnergy], nValue[kFurnMove]);
            break;
            
        case kItemContainer:
            hTo->SendF(
            "  Capacity: %.1f lbs\n"
            "   Opening: %.1f lbs\n"
            "Multiplier: %ld%%\n"
            "     Flags: %s\n", Tenth(nValue[kContCap]), Tenth(nValue[kContOpening]), 
            nValue[kContMulti], *gContFlags.ListBitNames(nValue[kContFlags], hTo));
        
            nKey = nValue[kContKey] ? ObjectIndex::Get(nValue[kContKey]) : NULL;
            if(nKey)
                hTo->SendF("       Key: %hd/%hd, \"%s\"\n", GetArea(nValue[kContKey]),
                GetVnum(nValue[kContKey]), nKey ? *nKey->mName : "none");
            break;
            
        case kItemDrinkCon:
        case kItemFountain:
            hTo->SendF("  Liquid: %s\nCapacity: %ld/%ld mL (Milliliters)\n",
            gLiqTable[nValue[kDContLiq]].mName, nValue[kDContCurCap], nValue[kDContMaxCap]);
            break;
            
        case kItemFood:
            hTo->SendF(
            "Nourishing: %ld\n"
            "   Filling: %ld\n"
            "  Highness: %ld\n", 
            nValue[kFoodHunger], nValue[kFoodFull], nValue[kFoodHigh]);
            break;
            
        case kItemCurrency:
        case kItemBoat:
        case kItemCorpseNPC:
        case kItemCorpsePC:
            break;
            
        case kItemReagent:
            hTo->SendF("   Charges: %ld to %ld    (Random for every repop)\n", nValue[kReagMin], nValue[kReagMax]);
            break;
            
        case kItemStones:
        case kItemLuxury:
        case kItemSkin:
        case kItemJunk:
        case kItemOther:
        case kItemInstrument:
            break;              
    }
    
    

/*
    else if(itemType == ITEM_INSTRUMENT)
    {
        sprintf(buf, "\n\rClass: %-10s\n\r", InstrumentTable[value[INST_CLASS]].name);
        to->Send(buf);
    }
    */
}


