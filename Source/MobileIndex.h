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
#ifndef MOBILEINDEX_H
#define MOBILEINDEX_H 1
#include "VRefThing.h"
#include "SpottedThing.h"
#include "ScriptedThing.h"
#include "LoadOld.h"
#include "Mobile.h"


class MobileIndex
    : public VRefThing
    , public SpottedThing
    , public ScriptedThing
    , public LoadOld
//  , public SaveLoadable //Comes from ScriptedThing/SpottedThing
{
    //** De/constructors **//
    public: MobileIndex();
    public: virtual ~MobileIndex();

    //** Class Functions **//
    public: static MobileIndex* Get(VNUM hVnum);
    public: static MobileIndex* Get(VREF hVRef);

    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag);

    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    virtual void    OnLoad();

    //** Member Functions **//
    public:
    void            StatTo(Person* hTo);
    
    
    
    
    //** Runtime Data Members **//
    public:
    AmbienceArray   mAmbiences;
    LONG            mHits[2];           //Main dicerolls
    LONG            mEnergy[2];
    LONG            mMoves[2];
    CHAR            mResist[kResMax];
    SHORT           mStat[kStatMax];
    LONG            mExperience;
        
    //** Persistant Data Members **//
    public:
    STRINGCW        mName;
    STRINGCW        mPName;
    STRINGCW        mKeywords;
    STRINGCW        mShortDesc;
    STRINGCW        mPShortDesc;
    STRINGCW        mDescription;
    STRINGCW        mMovement;          //Mob slithers east
    
    BYTE            mSex;
    SHORT           mLevel;
    BYTE            mAlignment;
    BYTE            mSize;
    BYTE            mDefaultPos;
    SHORT           mRidable;           //How many can ride it 
    CHAR            mResistMod[kResMax];//BaseForLevel + Mod
    SHORT           mStatMod[kStatMax]; //BaseForLevel + Mod
    SHORT           mExpMod;            //100% modifier of experience
    SHORT           mHitsMod;           //100% modifier of hit points
    SHORT           mEnergyMod;         //100% modifier of energy points
    SHORT           mMovesMod;          //100% modifier of move points  
    LONG            mVunerableFlags;    //May be charm, zombied, ...
    LONG            mDefensiveFlags;    //May dodge, parry, ...
    LONG            mActFlags;          //How it ACTs flags (hmm)
    SHORT           mAPR;               //Attacks per round

    AttackArray     mAttacks;
    PartArray       mParts;
    SkinArray       mSkins;
//  APtr<Shop>      mShop;
};



#endif //ifndef MOBILEINDEX_H

