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
#include "SpottedThing.h"
#include "Person.h"
#include "Spot.h"
#include "StringMagic.h"
#include "Tables.h"


SpottedThing::SpottedThing()
{
}


SpottedThing::~SpottedThing()
{
    for(SpotIter i = mSpots.begin(); i != mSpots.end(); ++i)
        delete *i;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




Spot* SpottedThing::FindSpot(const Person* hCh, const CHAR* hArg)
{
    LONG        nStartAt, nWantAmount, nFound = 0;
    STRINGCW    nMatchArg;
    bool        nAll;
    
    ParseMatching(hArg, nStartAt, nWantAmount, nMatchArg, nAll);
    
    for(SpotIter i = mSpots.begin(); i != mSpots.end(); ++i)
    if((*i)->mKeywords.IsNamed(nMatchArg))
    if(++nFound == nStartAt)
        return *i;
        
    return NULL;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: SpottedThing::StatListTo
//
//   Desc:: Shows a Person a listing of the Spots, their names, descs, and
//          flags. Primarily used with stat functions.
//
//  Input:: The Person to send the lines of info to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void SpottedThing::StatListTo(Person* hTo)
{
    if(!mSpots.empty())
    {
        hTo->Send("\n", 1);

        Spot* nSpot;
        LONG nBufLen;
        CHAR nBuf[kMaxStringLen];
        for(SpotIter i = mSpots.begin(); i != mSpots.end(); ++i)
        {
            nSpot = *i;
            STRINGCW spotFlags = gSpotFlags.ListBitNames(nSpot->mFlags);
            if(spotFlags == "<none>")
                spotFlags = ""; else spotFlags.SPrintF("(%s) ", *spotFlags);
        
            nBufLen = csnprintf(nBuf, 78, "Spot %s'%s': %s", *spotFlags, *nSpot->mName, *nSpot->mDescription);
            nBuf[nBufLen] = '\n'; //add a newline
            nBuf[++nBufLen] = '\0';
            hTo->Send(nBuf, nBufLen);
        }
    }
}




