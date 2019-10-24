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
#include "MobilePopulation.h"
#include "ObjectPopulation.h"
#include "MobileIndex.h"
#include "Mobile.h"
#include "Conversion.h"
#include "MUD.h"
#include "RoomIndex.h"




MobilePopulation::MobilePopulation()
    : mMobileVRef(0)
    , mMobileOldVnum(0)
    , mInPosition(kPosDead)
    , mChance(100)
    , mInPlay(NULL)
{
}


MobilePopulation::~MobilePopulation()
{
}



void MobilePopulation::Convert()
{
    Population::Convert();

    mMobileVRef = CvtToVRef(mMobileOldVnum);
    
    if(mInPosition == kPosHunted || mInPosition < kPosResting)
        mInPosition = kPosStanding;
    
    for(ULONG i = 0; i < mObjectsInside.size(); ++i)
    {
        SHORT nShouldBeWearLoc = mObjectsInside[i]->mObjectRoomValue1Vnum;
        mObjectsInside[i]->mToWearLoc = nShouldBeWearLoc;
        mObjectsInside[i]->Convert();
    }
    
    for(ULONG i = 0; i < mShopObjects.size(); ++i)
    {
        SHORT nShouldBeWearLoc = mShopObjects[i]->mObjectRoomValue1Vnum;
        mShopObjects[i]->mToWearLoc = nShouldBeWearLoc;
        mShopObjects[i]->Convert();
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MobilePopulation::Dirty
//
//   Desc:: Makes the population "Dirty" meaning that it needs to be cleaned up
//          or repopulated on the next call to RePopulate. If a Population is
//          not dirty calling RePopulate does nothing.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MobilePopulation::Dirty()
{
    if(mInPlay)
    {
        mInPlay->mPopulation = NULL;
        mInPlay = NULL;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MobilePopulation::RePopulate
//
//   Desc:: Repopulates the Population.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MobilePopulation::RePopulate()
{
    if(mInPlay)
        return;

    RoomIndex*      nRoom       = MUD::Get()->GetRoomIndex(mRoomVRef);
    MobileIndex*    nMobIndex   = MUD::Get()->GetMobileIndex(mMobileVRef);
    
    if(!nRoom || !nMobIndex)
        return;
    
    mInPlay = new Mobile(nMobIndex);
    
    mInPlay->mPosition = mInPosition;
    for(ULONG i = 0; i < mObjectsInside.size(); ++i)
        mObjectsInside[i]->RePopulate(mInPlay);
        
    mInPlay->PlaceInGame(nRoom, false);
}




