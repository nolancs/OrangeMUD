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
#include "VRefThing.h"
#include "MUD.h"


VRefThing::VRefThing()
    : mVRef(0)
    , mOldVnum(0)
{
}


VRefThing::~VRefThing()
{
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




VREF VRefThing::VRef() const
{
    return mVRef;
}


VNUM VRefThing::ANum() const
{
    return GetArea(mVRef);
}



VNUM VRefThing::VNum() const
{
    return GetVnum(mVRef);
}



Area* VRefThing::InArea() const
{
    return MUD::Get()->GetArea(ANum());
}



STRINGCW VRefThing::GetIdentTo(const Person* hCh) const
{
    STRINGCW nIdentStr;
    return nIdentStr.SPrintF("%hd/%hd", ANum(), VNum());    
}
