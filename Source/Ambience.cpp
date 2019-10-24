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
#include "Ambience.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "RoomIndex.h"


Ambience::Ambience()
{
}


Ambience::~Ambience()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Ambience::AddOntoRoom(VREF hRef)
{
    VNUM nVnum = GetVnum(hRef);
    ASSERT(std::find(mVnums.begin(), mVnums.end(), nVnum) == mVnums.end());

    RoomIndex* nRoom = MUD::Get()->GetRoomIndex(hRef);
    if(nRoom)
        nRoom->mAmbiences.Add(this);
    
    mVnums.Add(nVnum);
}



void Ambience::AddOntoMobile(VREF hRef)
{
    VNUM nVnum = GetVnum(hRef);
    ASSERT(std::find(mVnums.begin(), mVnums.end(), nVnum) == mVnums.end());
    
    MobileIndex* nMobile = MUD::Get()->GetMobileIndex(hRef);
    if(nMobile)
        nMobile->mAmbiences.Add(this);  
    
    mVnums.Add(nVnum);
}



void Ambience::RemoveFromRoom(VREF hRef)
{
    VNUM nVnum = GetVnum(hRef);
    Vector<VNUM>::iterator nFind = std::find(mVnums.begin(), mVnums.end(), nVnum);
    ASSERT(nFind != mVnums.end());
    
    RoomIndex* nRoom = MUD::Get()->GetRoomIndex(hRef);
    if(nRoom)
        nRoom->mAmbiences.Remove(this);
    
    mVnums.erase(nFind);
}



void Ambience::RemoveFromMobile(VREF hRef)
{
    VNUM nVnum = GetVnum(hRef);
    Vector<VNUM>::iterator nFind = std::find(mVnums.begin(), mVnums.end(), nVnum);
    ASSERT(nFind != mVnums.end());  
    
    MobileIndex* nMobile = MUD::Get()->GetMobileIndex(hRef);
    if(nMobile)
        nMobile->mAmbiences.Remove(this);       
    
    mVnums.erase(nFind);
}

