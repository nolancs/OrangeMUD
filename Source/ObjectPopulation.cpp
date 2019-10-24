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
#include "ObjectPopulation.h"
#include "RoomIndex.h"
#include "ObjectIndex.h"
#include "Object.h"
#include "Conversion.h"
#include "Person.h"



ObjectPopulation::ObjectPopulation(bool hIsOldObjRepop)
    : mObjectVRef(0)
    , mLoadMin(1)
    , mLoadMax(1)
    , mToWearLoc(kLocNone)
    , mObjectFlags(0)
    , mContFlags(0)
    , mInPlay(NULL)
    //Outdated
    , mObjectOldVnum(0)
    , mObjectRoomValue1Vnum(0)
    , mIsOldObjRepop(hIsOldObjRepop)
{
}


ObjectPopulation::~ObjectPopulation()
{
}



void ObjectPopulation::Convert()
{
    Population::Convert();
    
    mObjectVRef = CvtToVRef(mObjectOldVnum);
    
    for(ULONG i = 0; i < mPopulationsInside.size(); ++i)
        mPopulationsInside[i]->Convert();
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectPopulation::Dirty
//
//   Desc:: Makes the population "Dirty" meaning that it needs to be cleaned up
//          or repopulated on the next call to RePopulate. If a Population is
//          not dirty calling RePopulate does nothing.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectPopulation::Dirty()
{
    if(mInPlay)
    {
        mInPlay->mPopulation = NULL;
        mInPlay = NULL;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectPopulation::RePopulate
//
//   Desc:: Repopulates the Population.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectPopulation::RePopulate()
{
    if(mInPlay)
        return;

    RePopulate(RoomIndex::Get(mRoomVRef));
}



void ObjectPopulation::RePopulate(ObjectHolder* hOH)
{
    LONG nLoadAmount = Random(mLoadMin, mLoadMax);
    
    for(LONG x = 0; x < nLoadAmount; ++x)
    {
        mInPlay = new Object(ObjectIndex::Get(mObjectVRef));
        

        //** Into Object Holder Directly **//   
        mInPlay->mWearLoc = mToWearLoc;
        if(mToWearLoc == kLocNone)
        {
            hOH->AddObject(mInPlay, true);      
        }
        
        
        //** Into Person's Worn Items **//
        if(mToWearLoc != kLocNone)
        {
            Person* nToPerson = dynamic_cast<Person*>(hOH);
            ASSERT(nToPerson != NULL);
            
            nToPerson->EquipObject(mInPlay, mToWearLoc);
        }

        
        //** Repopulate Innards **//
        for(ULONG i = 0; i < mPopulationsInside.size(); ++i)
            mPopulationsInside[i]->RePopulate(mInPlay);
    }
}




