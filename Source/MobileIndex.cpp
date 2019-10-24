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
#include "Attack.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "ObjectIndex.h"
#include "Part.h"
#include "Skin.h"
#include "StringMagic.h"
#include "Tables.h"



MobileIndex::MobileIndex()
    //Runtime
    : mExperience       (0)
    
    //Persistant
    , mSex              (0)
    , mLevel            (0)
    , mAlignment        (0)
    , mSize             (0)
    , mDefaultPos       (0)
    , mRidable          (0)
    , mExpMod           (100)
    , mHitsMod          (100)
    , mEnergyMod        (100)
    , mMovesMod         (100)
    , mVunerableFlags   (0)
    , mDefensiveFlags   (0)
    , mActFlags         (0)
    , mAPR              (0)
{
    mHits[0]    = 0;
    mHits[1]    = 0;
    mEnergy[0]  = 0;
    mEnergy[1]  = 0;
    mMoves[0]   = 0;
    mMoves[1]   = 0;
    
    for(SHORT i = 0; i < kResMax; ++i)
    {
        mResist[i] = 0;
        mResistMod[i] = 0;
    }
    
    for(SHORT i = 0; i < kStatMax; ++i)
    {
        mStat[i] = 0;
        mStatMod[i] = 0;
    }
}


MobileIndex::~MobileIndex()
{
    for(ULONG i = 0; i < mParts.size(); ++i)
        delete mParts[i];
    for(ULONG i = 0; i < mSkins.size(); ++i)
        delete mSkins[i];
    for(ULONG i = 0; i < mAttacks.size(); ++i)
        delete mAttacks[i];
}



MobileIndex* MobileIndex::Get(VNUM hVnum)
{
    return MUD::Get()->GetMobileIndex(hVnum);
}


MobileIndex* MobileIndex::Get(VREF hVRef)
{
    return MUD::Get()->GetMobileIndex(hVRef);
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MobileIndex::StatTo
//
//   Desc:: Sends all the statistics about a MobileIndex to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MobileIndex::StatTo(Person* hTo)
{
    STRINGCW nBuf, nTitleBuf;
    
    nTitleBuf.SPrintF("(( ^T%s^x :: ^T%s^x ))", *GetIdentTo(hTo), *mName);
    nTitleBuf = CenterLine(nTitleBuf.Capitalize(true)); 

    nBuf.CSPrintF(
    //////////////////////////////////////////////////////////////////////////////////////////
    kGfxSep"\n"
    "%s\n"
    kGfxSep"\n"
    "    Name: %-27s Position: %s\n"
    "   PName: %-27s Movement: %s\n"
    "   Short: ^*%s^x\n"
    "  PShort: ^*%s^x\n"
    "Keywords: %s\n"
    "^UDescription...                                                                  ^x\n"
    "%s\n"
    kGfxSep"\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *nTitleBuf      ,
    *mName          , gPosTable[mDefaultPos].mName  ,
    *mPName         , *mMovement                    ,
    *mShortDesc     ,
    *mPShortDesc    ,
    *mKeywords      ,
    *mDescription);
    hTo->Send(nBuf);
    
    
    STRINGCW hitRoll, eneRoll, movRoll;
    hitRoll.SPrintF("%3hd%% [%ldhp]", mHitsMod  , Avg(mHits[0]  , mHits[1]));
    eneRoll.SPrintF("%3hd%% [%lden]", mEnergyMod, Avg(mEnergy[0], mEnergy[1]));
    movRoll.SPrintF("%3hd%% [%ldmv]", mMovesMod , Avg(mMoves[0] , mMoves[1]));
    nBuf.CSPrintF(
    //////////////////////////////////////////////////////////////////////////////////////////
    "Str: %-2d     Level: %-3d                                ^bCold^x:%4d%%      ^mMind^x:%4d%%\n"
    "Dex: %-2d       Exp: %3hd%%               Sex: %-7s    ^rFire^x:%4d%%     Sleep:%4d%%\n"
    "Int: %-2d        hp: %-15s   Size: %-10s ^gAcid^x:%4d%%     Charm:%4d%%\n"
    "Wis: %-2d        en: %-15s   Race: %-10s ^cElec^x:%4d%%\n"
    "Con: %-2d        mv: %-15s    APR: %-2d         ^yPois^x:%4d%%   Ridable: %d\n"
    "Flags: %s\n"
    "\n"
    "^U AC^x ^USz^x ^UPart...        ^x  ^U AC^x ^USz^x ^UPart...        ^x   ^U     Skins                     ^x\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    mStat[kStatStr] , mLevel    ,                             mResist[kResCold] , mResist[kResMind] ,
    mStat[kStatDex] , mExpMod   , gSexTable[mSex].mName     , mResist[kResFire] , mResist[kResSlee] ,
    mStat[kStatInt] , *hitRoll  , gSizeTable[mSize].mName   , mResist[kResAcid] , mResist[kResChar] ,
    mStat[kStatWis] , *eneRoll  , "<none>"                  , mResist[kResElec] ,
    mStat[kStatCon] , *movRoll  , mAPR                      , mResist[kResPois] , mRidable          ,
    *gMobileFlags.ListBitNames(mActFlags, hTo)
    );
    hTo->Send(nBuf);


    //list parts and skins
    LONG numParts = mParts.size();
    LONG numSkins = mSkins.size();
    LONG totalLines = UMAX(((numParts/2)+(numParts%2)), numSkins);
    LONG i, currentPart = 0, currentSkin = 0;
    Part* nPart;
    Skin* nSkin;
    ObjectIndex* nSkinObj;
    
    for(i = 0; i < totalLines; ++i)
    {
        nPart = currentPart < numParts ? mParts[currentPart++] : NULL;
        if(nPart)
        {   nBuf.CSPrintF("%3d %2d %-15s  ", nPart->mAC, nPart->mSize, CTrunc(nPart->mName, 15));           
        }
        else
        {   nBuf.SPrintF("%24s", "");
        
        }
        hTo->Send(nBuf);
    
        nPart = currentPart < numParts ? mParts[currentPart++] : NULL;
        if(nPart)
        {   nBuf.CSPrintF("%3d %2d %-15s   ", nPart->mAC, nPart->mSize, CTrunc(nPart->mName, 15));          
        }
        else
        {   nBuf.SPrintF("%26s", "");
        }
        hTo->Send(nBuf);
        
        nSkin = currentSkin < numSkins ? mSkins[currentSkin++] : NULL;
        if(nSkin)
        {
            nSkinObj = MUD::Get()->GetObjectIndex(nSkin->mVRef);
            nBuf.SPrintF("%3d%% [%d/%d] %s", nSkin->mChance, GetArea(nSkin->mVRef), 
            GetVnum(nSkin->mVRef), nSkinObj ? *nSkinObj->mName : "<non-existant>");
            nBuf = CTrunc(nBuf, 31);
            nBuf += "^x\n";
        }else
        {   nBuf = "\n";
        }
        hTo->Send(nBuf);
    }
    
    
    //list attacks
    if(mAttacks.size())
    {
        hTo->Send(kGfxSep"\n");
        for(ULONG i = 0; i < mAttacks.size(); ++i)
            mAttacks[i]->StatListTo(hTo);
    }
    
    //Spots
    SpottedThing::StatListTo(hTo);  
    
    //Scripts
    ScriptedThing::StatListTo(hTo);

    hTo->Send(kGfxLine"\n");
}




