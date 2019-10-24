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
#include "ANSI.h"
#include "Area.h"
#include "Channel.h"
#include "MUD.h"
#include "Player.h"
#include "RoomIndex.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"
#include "XMLFile.h"


Player::Player()
    //Runtime Data
    : isImm(NULL)
    , mDamRoll(0)
    , mHitRoll(0)
    , mFlagsStatus(0)
    //Persistant Data
    , mUID(0)
    , mLastLogin(0)
    , mTimePlayed(0)
    , mTrust(0)
    , mScroll(20)
    , mInvisLevel(0)
    , mFlagsOptions(0)
    , mFlagsInfo(0)
{
    isPlayer = this;
    isMobile = NULL;

    for(LONG i = 0; i < kCondMax; ++i)
        mCondition[i] = 0;

    for(LONG i = 0; i < kColorPrefMax; ++i)
        mColorPref[i] = 0;
}


Player::~Player()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Player::PlaceInGame(PersonHolder* hTo, bool hLoggingIn)
{
    ASSERT(hTo != NULL);
    ASSERT(mSession != NULL);
    ASSERT(mUID != 0);

    mSession->mColorPrefStrs.clear();
    for(LONG i = 0; i < kColorPrefMax; ++i)
        mSession->mColorPrefStrs.AddBack(gANSIPrefTable.BuildANSICode(mColorPref[i]));

    if(mKeywords.Empty())
        mKeywords = mName;

    MUD::Get()->PlaceInGame(this);
    Person::PlaceInGame(hTo, hLoggingIn);
    
    if(hLoggingIn)
    {
        Channel::Broadcast(-1, NULL, 
            kInfoConnect, "^]Info || ", "$n has connected.^x", 
            this, NULL, NULL, kActWorld|kActNoChar|kActOnlySeeChar);
    }
}



void Player::RemoveFromGame(bool hLoggingOut)
{
    Person::RemoveFromGame(hLoggingOut);
    if(hLoggingOut)
    {
        //Show To World
        Channel::Broadcast(-1, NULL, 
            kInfoQuit, "^]Info || ", "$n has left our reality.^x", 
            this, NULL, NULL, kActWorld|kActNoChar);
    }
    MUD::Get()->RemoveFromGame(this);   
}


void Player::PlaceIn(PersonHolder* hTo)
{
    Person::PlaceIn(hTo);
    if(mInRoom)
    {
        ++mInRoom->mNumPlayers;
        ++mInRoom->mInArea->mNumPlayers;
    }
}


void Player::RemoveFrom()
{
    if(mInRoom)
    {
        --mInRoom->mNumPlayers;
        --mInRoom->mInArea->mNumPlayers;
    }
    Person::RemoveFrom();
}



bool Player::GetRoundAttacks(AttackArray& hAttacks)
{
    return false;
}




void Player::StatTo(Person* hTo)
{
    SHORT nBStr, nBDex, nBInt, nBWis, nBCon;
    SHORT nStr, nDex, nInt, nWis, nCon;
    LONG nHits, nHitsMax, nEnergy, nEnergyMax, nMoves, nMovesMax;
    CHAR nResCold, nResFire, nResAcid, nResElec, nResPois, nResMind;
    float nHR, nDR, nHitRegen, nEnergyRegen, nMoveRegen;
    LONG nCopperBank, nCopperHere, nExpTNL;
    STRINGCW nCondBuf;

    nStr        = GetStr();
    nDex        = GetDex();
    nInt        = GetInt();
    nWis        = GetWis();
    nCon        = GetCon();
    nBStr       = GetBaseStr();
    nBDex       = GetBaseDex();
    nBInt       = GetBaseInt();
    nBWis       = GetBaseWis();
    nBCon       = GetBaseCon();
    nHits       = GetHits();
    nHitsMax    = GetMaxHits();
    nEnergy     = GetEnergy();
    nEnergyMax  = GetMaxEnergy();
    nMoves      = GetMoves();
    nMovesMax   = GetMaxMoves();
    nResCold    = GetColdRes();
    nResFire    = GetFireRes();
    nResAcid    = GetAcidRes();
    nResElec    = GetElecRes();
    nResPois    = GetPoisRes();
    nResMind    = GetMindRes();
    nHR         = 0;
    nDR         = 0;
    nHitRegen   = 0;
    nEnergyRegen = 0;
    nMoveRegen  = 0;
    nCopperBank = 0;
    nCopperHere = 0;
    nExpTNL     = 0;
    
    
    //Conditions    
    if(mCondition[kCondThirst] >= kNowThirsty)
    {
        nCondBuf += ", thirsty";
        if(hTo->IsImm()) nCondBuf += QBuf("(%d)", mCondition[kCondThirst]);
    }
    if(mCondition[kCondHunger] >= kNowHungry)
    {
        nCondBuf += ", hungry";
        if(hTo->IsImm()) nCondBuf += QBuf("(%d)", mCondition[kCondHunger]);
    }
    if(mCondition[kCondDrunk] > 0)
    {
        nCondBuf += ", drunk";
        if(hTo->IsImm()) nCondBuf += QBuf("(%d)", mCondition[kCondDrunk]);
    }
    if(mCondition[kCondHigh] > 0)
    {
        nCondBuf += ", high";
        if(hTo->IsImm()) nCondBuf += QBuf("(%d)", mCondition[kCondHigh]);
    }
    
    
    //Setup Some Work Buffers
    STRINGCW nTitleBuf, nHBuf, nEBuf, nMBuf;
    nTitleBuf.SPrintF("%s the %s %s", *GetNameTo(hTo), *GetRaceNameTo(hTo), *GetClassNameTo(hTo));
    nHBuf.SPrintF("%ld/%ld", nHits  , nHitsMax);
    nEBuf.SPrintF("%ld/%ld", nEnergy, nEnergyMax);
    nMBuf.SPrintF("%ld/%ld", nMoves , nMovesMax);
    
    STRINGCW nBuf;
    nBuf.SPrintF(
    ////////////////////////////////// Score Template //////////////////////////////////////
    "-==============================================================================-\n"
    "^!%s^x\n"
    "\n"
    "              Hits: %-11s Energy: %-11s Moves: %-11s\n"
    "\n",
    ///////////////////////////////////////////////////////////////////////////////////////
    CenterLine(nTitleBuf, 80),
    *nHBuf, *nEBuf, *nMBuf);
    hTo->Send(nBuf);

    if(nCondBuf.Empty() == false)
    {
        hTo->Send("[ ");
        hTo->Send(&nCondBuf[2]);
        hTo->Send(" ]\n\n");
    }
    

    nBuf.SPrintF(
    ////////////////////////////////// Score Template //////////////////////////////////////
    "     Str:%2d(%2d)     Dex:%2d(%2d)     Int:%2d(%2d)     Wis:%2d(%2d)     Con:%2d(%2d)\n"
    "\n"
    "             ^bCold^x:%4d%%            ^cElec^x:%4d%%         ^yPoison^x:%4d%%\n"
    "             ^rFire^x:%4d%%            ^gAcid^x:%4d%%         ^mMental^x:%4d%%\n"
    "\n"
    "            Level: %-4d           Focus: %-7s          Sex: %s\n"
    "        Hit Regen: %+-7.1f      Hitroll: %+-7.1f        Align: %s\n"
    "     Energy Regen: %+-7.1f      Damroll: %+-7.1f      In Bank: %ldcp\n"
    "   Movement Regen: %+-7.1f   Experience: %-12ld Carried: %ldcp\n"
    "-==============================================================================-\n",
    ///////////////////////////////////////////////////////////////////////////////////////
    nBStr, nStr, nBDex, nDex, nBInt, nInt, nBWis, nWis, nBCon, nCon,
    nResCold        , nResElec          , nResPois              ,
    nResFire        , nResAcid          , nResMind              ,
    mLevel          , ""                , gSexTable[mSex].mName ,
    nHitRegen       , nHR               , "gone"                ,
    nEnergyRegen    , nDR               , nCopperBank           ,
    nMoveRegen      , nExpTNL           , nCopperHere);
    hTo->Send(nBuf);
}
