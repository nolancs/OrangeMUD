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
#include "ExitPopulation.h"
#include "Exit.h"
#include "RoomIndex.h"
#include "MUD.h"




ExitPopulation::ExitPopulation()
    : mDirection(0)
    , mFlags(0)
{
}


ExitPopulation::~ExitPopulation()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ExitPopulation::Dirty
//
//   Desc:: Makes the population "Dirty" meaning that it needs to be cleaned up
//          or repopulated on the next call to RePopulate. If a Population is
//          not dirty calling RePopulate does nothing.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void ExitPopulation::Dirty()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ExitPopulation::RePopulate
//
//   Desc:: Repopulates the Population.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ExitPopulation::RePopulate()
{
    RoomIndex* nRoom = MUD::Get()->GetRoomIndex(mRoomVRef);
    
    if(!nRoom)
        return;
    
    ASSERT(mDirection >= 0 && mDirection < kDirMax);
    if(!nRoom->mExits[mDirection])
        return;

    if(IsSet(mFlags, kExClosed))
        SetBit(nRoom->mExits[mDirection]->mFlags, kExClosed);
    else
        RemoveBit(nRoom->mExits[mDirection]->mFlags, kExClosed);
    
    if(IsSet(mFlags, kExLocked))
        SetBit(nRoom->mExits[mDirection]->mFlags, kExLocked);
    else
        RemoveBit(nRoom->mExits[mDirection]->mFlags, kExLocked);
    
    if(IsSet(mFlags, kExHidden))
        SetBit(nRoom->mExits[mDirection]->mFlags, kExHidden);
    else
        RemoveBit(nRoom->mExits[mDirection]->mFlags, kExHidden);
}




