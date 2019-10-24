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
#include "Mobile.h"
#include "MobileIndex.h"
#include "MUD.h"


Mobile::Mobile(MobileIndex* hFromIndex)
    : mPopulation(NULL)
{
    ASSERT(hFromIndex != NULL);
    mIndex              = hFromIndex;
    mVRef               = hFromIndex->mVRef;
    
    isMobile            = this;
    mName               = hFromIndex->mName;
    mPName              = hFromIndex->mPName;
    mKeywords           = hFromIndex->mKeywords;
    mShortDesc          = hFromIndex->mShortDesc;
    mPShortDesc         = hFromIndex->mPShortDesc;
    mDescription        = hFromIndex->mDescription;
    //Score Values
    mSex                = hFromIndex->mSex;
    mLevel              = hFromIndex->mLevel;
    mRace               = 0;
    mClass              = 0;
    mAlignment          = hFromIndex->mAlignment;
    mSize               = hFromIndex->mSize;
    mPosition           = hFromIndex->mDefaultPos;

    for(ULONG i = 0; i < kResMax; ++i)
        mResist[i] = hFromIndex->mResist[i];
        
    for(ULONG i = 0; i < kStatMax; ++i)
        mStat[i] = hFromIndex->mStat[i];

    mHitsMax            = Random(hFromIndex->mHits[0], hFromIndex->mHits[1]);
    mEnergyMax          = Random(hFromIndex->mEnergy[0], hFromIndex->mEnergy[1]);
    mMovesMax           = Random(hFromIndex->mMoves[0], hFromIndex->mMoves[1]);
    mHits               = mHitsMax;
    mEnergy             = mEnergyMax;
    mMoves              = mMovesMax;
}



Mobile::~Mobile()
{
    ASSERT(mPopulation = NULL);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Mobile::PlaceInGame(PersonHolder* hTo, bool hLoggingIn)
{
    MUD::Get()->PlaceInGame(this);
    Person::PlaceInGame(hTo, hLoggingIn);
}


void Mobile::RemoveFromGame(bool hLoggingOut)
{
    Person::RemoveFromGame(hLoggingOut);
    MUD::Get()->RemoveFromGame(this);   
}



void Mobile::PlaceIn(PersonHolder* hTo)
{
    Person::PlaceIn(hTo);
}


void Mobile::RemoveFrom()
{
    Person::RemoveFrom();
}


bool Mobile::GetRoundAttacks(AttackArray& hAttacks)
{
    hAttacks = mIndex->mAttacks;
    return true;
}



void Mobile::StatTo(Person* hTo)
{
    hTo->Send("mobile\n");
}

