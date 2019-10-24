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
#include "Skin.h"





Skin::Skin()
{
}


Skin::~Skin()
{
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Skin::StatListTo
//
//   Desc:: Shows a Person a single line listing the Skin, its name, and some
//          other information about it. Primarily used with OLC skin cmds.
//
//  Input:: The Person to send the line of info to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Skin::StatListTo(Person* hTo)
{
    ObjectIndex* nSkinObject = MUD::Get()->GetObjectIndex(mVRef);
    STRINGCW nName = nSkinObject ? nSkinObject->mName : STRINGCW("A NON-EXISTANT OBJECT");

    hTo->SendF("Skin: #%-2hd   Chance: %3hd%%   Produces: [%hd/%hd] %s\n", 
    mNumber, (SHORT)mChance, GetArea(mVRef), GetVnum(mVRef), *nName);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




Skin* FindSkin(SkinArray& hSkins, SHORT nNum)
{
    nNum--;
    if(nNum < 0)
        return NULL;
    if(nNum >= hSkins.size())
        return NULL;    
    
    Skin* nSkin = hSkins[nNum];
    ASSERT(nSkin->mNumber == nNum+1);
    return nSkin;
}


