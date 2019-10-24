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
#include "Act.h"
#include "Attack.h"
#include "Fight.h"
#include "MUD.h"
#include "Person.h"



Fight::Fight(Person* hTank, Person* hVict)
    : mKiller(hTank)
    , mVict(hVict)
{
    Add(hTank);
    MUD::Get()->mFights.AddBack(this);
}


Fight::~Fight()
{
    MUD::Get()->mFights.Remove(this);
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool Person::CanFight(Person* hVict)
{
    return true;
}



bool Person::TryFight(Person* hVict)
{
    ASSERT(hVict);
    if(mImFighting)
    {
        Act("You stop attacking $N.\n" , this, NULL, mImFighting->mVict, ToChar);
        Act("$n stops attacking you.\n", this, NULL, mImFighting->mVict, ToVict);
        Act("$n stops attacking $N.\n" , this, NULL, mImFighting->mVict, ToNotVict);
        mImFighting->Remove(this);
        mImFighting = NULL;
    }
    
    if(hVict->mFightingMe)
    {
        mImFighting = hVict->mFightingMe;
        mImFighting->Add(this);
    }
    else
    {
        hVict->mFightingMe = new Fight(this, hVict);
        mImFighting = hVict->mFightingMe;
    }
    
    Act("^wYou attack $N!^x\n"   , this, NULL, mImFighting->mVict, ToChar);
    Act("^w^!$n attacks you!^x\n", this, NULL, mImFighting->mVict, ToVict);
    Act("^w$n attacks $N!^x\n"   , this, NULL, mImFighting->mVict, ToGChar);
    Act("^w$n attacks $N!^x\n"   , this, NULL, mImFighting->mVict, ToGVict);
    Act("^!$n attacks $N!^x\n"   , this, NULL, mImFighting->mVict, ToGNotVict);
    return true;
}


void Person::DoRoundOfAttacks(Fight* hInFight)
{
    Person* nVictim = hInFight->mVict;
    UCHAR nDidAttacks = 0;
    AttackArray nAttacks;
    
    nAttacks.reserve(10);
    if(!GetRoundAttacks(nAttacks))
    {
        Act("$n cowers in fear of you.\n", this, NULL, nVictim, ToVict);
        Act("$n cowers in fear of $N.\n" , this, NULL, nVictim, ToNotVict);
        Act("You cower in fear of $N.\n" , this, NULL, nVictim, ToChar);
        return;
    }

    //Act("$n cautiously stalks $N.\n", nKiller, NULL, mVict, ToAll);
    for(ULONG i = 0; i < nAttacks.size(); ++i)
    {
        Attack* nAttack = nAttacks[i];
        if(Random(0,100) > nAttack->mChance)
            continue;

        nDidAttacks++;
        Blow nBlow(hInFight, this, nAttack);
        nBlow.DoCalculation();
        nBlow.DoDisplay();
        nBlow.DoDamage();
        if(nBlow.Killed())
            break;
    }
    
    if(!nDidAttacks)
    {
        Act("$n finds no opening to attack you.\n", this, NULL, nVictim, ToVict);
        Act("$n finds no opening to attack $N.\n" , this, NULL, nVictim, ToNotVict);
        Act("You finds no opening to attack $N.\n", this, NULL, nVictim, ToChar);
    }
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Fight::End()
{
    while(mKillers.size())
        Remove(mKillers.front());
    
    ASSERT(mVict->mFightingMe == this);
    mVict->mFightingMe = NULL;
    mVict = NULL;
    delete this;
}

void Fight::Add(Person* hPerson)
{
    mKillers.AddBack(hPerson);  
}




void Fight::Remove(Person* hPerson)
{
    PersonIter nFind = std::find(mKillers.begin(), mKillers.end(), hPerson);
    ASSERT(nFind != mKillers.end());
    ASSERT((*nFind)->mImFighting == this);
    (*nFind)->mImFighting = NULL;
    mKillers.erase(nFind);
}



void Fight::DoRound()
{
    for(PersonIter nPScan = mKillers.begin(); nPScan != mKillers.end(); ++nPScan)
    {
        Person* nKiller = *nPScan;
        nKiller->DoRoundOfAttacks(this);
    }
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


Blow::Blow()
    : mVict(NULL)
    , mKiller(NULL)
    , mWeapon(NULL)
    , mPart(NULL)
    , mMessage(NULL)
    , mFlags(0)
    , mDistance(0)
    , mDamage(0)
    , mResultFlags(0)
{
}


Blow::Blow(Fight* hFight, Person* hKiller, Attack* hAttack)
    : mVict(NULL)
    , mKiller(hKiller)
    , mWeapon(NULL)
    , mPart(NULL)
    , mMessage(NULL)
    , mFlags(0)
    , mDistance(0)
    , mDamage(0)
    , mResultFlags(0)
{
    if(hAttack)
    {
        mType       = hAttack->mType;
        mFlags      = hAttack->mFlags;
        mMessage    = hAttack->mMessage;
        mHitroll[0] = hAttack->mHitroll[0];
        mHitroll[1] = hAttack->mHitroll[1];
        mDamroll[0] = hAttack->mDamroll[0];
        mDamroll[1] = hAttack->mDamroll[1];
        mDamageType = hAttack->mDamageType;
    }
}


Blow::~Blow()
{
}


void Blow::DoCalculation()
{
    if(Random(0,100) < 30)
    {
        SetBit(mResultFlags, Blow::kMissed);
        return;
    }
    
    mDamage = Random(mDamroll[0], mDamroll[1]);
}


void Blow::DoDisplay()
{
    if(Missed())
    {
        Act("You miss $N with your $t.\n", mKiller, ActStr(mMessage), mVict, ToChar);
        Act("$n misses you with $s $t.\n", mKiller, ActStr(mMessage), mVict, ToVict);
        Act("$n misses $N with $s $t.\n" , mKiller, ActStr(mMessage), mVict, ToNotVict);
        return;
    }

    Act("You *HIT* $N with your $t.\n", mKiller, ActStr(mMessage), mVict, ToChar);
    Act("$n *HITS* you with $s $t.\n" , mKiller, ActStr(mMessage), mVict, ToVict);
    Act("$n *HITS* $N with $s $t.\n"  , mKiller, ActStr(mMessage), mVict, ToNotVict);
}


void Blow::DoDamage()
{
}
    

bool Blow::Killed()
{
    return IsSet(mResultFlags, Blow::kKilled);
}


bool Blow::Missed()
{
    return IsSet(mResultFlags, Blow::kMissed);
}


