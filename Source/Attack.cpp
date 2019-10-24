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
#include "Person.h"
#include "Tables.h"




const CHAR* gAttackTypeStrs[] = {"Tank", "Area", "Random", "Script", "\n"};




Attack::Attack()
    : mHitMod(100)
    , mDamMod(100)
{
}


Attack::~Attack()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Attack::StatListTo
//
//   Desc:: Shows a Person a single line listing the Attack, its name, and
//          some other information about it. Primarily used with OLC attack
//          cmds.
//
//  Input:: The Person to send the line of info to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Attack::StatListTo(Person* hTo)
{
    //====================================[Attacks]===================================
    //#1  slash                     40%   Phys/Tank    Hit:100% [80]    Dam:100% [500]
    //#2  His double-handed swing   20%   Phys/Tank    Hit:100% [80]    Dam:100% [500]
    //#3  heavy kick                40%   Phys/Tank    Hit:100% [80]    Dam:100% [500]
    
    STRINGCW nHitChanceBuf, nDamAmountBuf;
    nHitChanceBuf.SPrintF("[%ld]", Avg(mHitroll[0], mHitroll[1]));
    nDamAmountBuf.SPrintF("[%ld]", Avg(mDamroll[0], mDamroll[1]));
    
    if(mType != kAttScript)
    {
        hTo->SendF("#%-2d %-22.22s %3d%%   %4s/%-6s   Hit:%3hd%% %-5s  Dam:%3hd%% %s\n", mNumber,
        *mMessage, mChance, gDamageTable[mDamageType].mSName, gAttackTypeStrs[mType], mHitMod,
        *nHitChanceBuf, mDamMod, *nDamAmountBuf);       
    }
    else
    {
        hTo->SendF("#%-2d %-24.24s %3d%%   Script #%-3hd   Hit:%3hd%% %-5s  Dam:%3hd%% %s\n", mNumber,
        *mMessage, mChance, (SHORT) mDamageType, mHitMod, *nHitChanceBuf, mDamMod, *nDamAmountBuf);
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




Attack* FindAttack(AttackArray& hAttacks, SHORT nNum)
{
    nNum--;
    if(nNum < 0)
        return NULL;
    if(nNum >= hAttacks.size())
        return NULL;    
    
    Attack* nAttack = hAttacks[nNum];
    ASSERT(nAttack->mNumber == nNum+1);
    return nAttack;
}



