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
#ifndef ATTACK_H
#define ATTACK_H 1
#include "LoadOld.h"
#include "SaveLoadable.h"


class Attack
    : public LoadOld
    , public SaveLoadable
{
    public: Attack();
    public: virtual ~Attack();

    
    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag); 


    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    static void     LoadListInto(MUDFile& hFp, AttackArray& hList);


    //** Public Member Functions **//
    public:
    void            StatListTo(Person* hTo);



    //** Runtime Data Members **//
    public:
    LONG        mHitroll[2];    //X to Y hitroll
    LONG        mDamroll[2];    //X to Y damroll


    //** Persistant Data Members **//
    public:
    SHORT       mNumber;        //Keeping track of in OLC
    STRINGCW    mMessage;       //Mob's 'vicious goring horn' MUTILATES you!
    BYTE        mChance;        //1-100% chance it goes off
    BYTE        mType;          //Spell, Singular, Area , some skill? etc...
    LONG        mFlags;         //Plural, No Name etc...
    SHORT       mHitMod;
    SHORT       mDamMod;
    BYTE        mDamageType;    //cold,fire,physical... Stored script ID if script attack
};




//mFlags 
#define kAttPlural      (A)
#define kAttJustMsg     (B)
#define kAttNoPart      (C)
#define kAttReaction    (D)

//mTypes
#define kAttTank        0
#define kAttArea        1
#define kAttRandom      2
#define kAttScript      3
extern const CHAR* gAttackTypeStrs[];



//** Free Functions **//
Attack* FindAttack(AttackArray& hAttacks, SHORT nNum);




#endif //ifndef ATTACK_H