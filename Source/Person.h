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
#ifndef PERSON_H
#define PERSON_H 1
#include <iostream>
#include <iomanip>
#include <streambuf>
#include "ActThing.h"
#include "ObjectHolder.h"
#include "LoadOld.h"
#include "OrangeTCP.h"
#include "SaveLoadable.h"



class Person 
    : public ObjectHolder
    , public LoadOld
//  , public ActThing       (Comes From ObjectHolder)
//  , public SaveLoadable   (Comes From ObjectHolder)
{
    public: Person();
    public: virtual ~Person();


    //** Overriden Virtual Functions **//
    public:
    virtual void        SaveTo(XMLWriter& hFile, const CHAR* hTagName) = 0;
    virtual void        SaveValuesTo(XMLWriter& hFile);
    virtual bool        MatchValues(XMLReader& hXML, XMLTag& hTag);
    virtual bool        MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool        MatchLists(MUDFile& hFp, CHAR* hName);
    virtual STRINGCW    GetTextForCodeTo(Person* hTo, CHAR hCode);
    virtual void        PlaceIn(PersonHolder* hTo);
    virtual void        PlaceInGame(PersonHolder* hTo, bool hLoggingIn);
    virtual void        RemoveFrom();
    virtual void        RemoveFromGame(bool hLoggingOut);
    virtual bool        GetRoundAttacks(AttackArray& hAttacks) = 0;
    virtual void        DoRoundOfAttacks(Fight* hInFight);
    

    //** Public Member Functions **//
    public:
    void            Interpret(STRINGCW& hInput, bool hCheck = true);
    void            Interpret(STRINGCW& hCmd, STRINGCW& hArgs, bool hCheck = true);
    void            Send(const CHAR* hData, LONG hLen = -1);
    void            Send(const STRINGCW& hData, LONG hLen = -1);
    void            SendF(const CHAR* hData, ...);
    void            SendPaged(const CHAR* hData, LONG hLen = -1);
    void            SendPaged(const STRINGCW& hData, LONG hLen = -1);
    void            SendPagedF(const CHAR* hData, ...);
    void            SendLined(const CHAR* hText, bool hShowColorCodes);


    //** Informational **//
    STRINGCW        GetNameTo(Person* hTo);
    STRINGCW        GetPNameTo(Person* hTo, LONG hAmount);
    STRINGCW        GetKeywordsTo(Person* hTo);
    STRINGCW        GetShortDescTo(Person* hTo);
    STRINGCW        GetPShortDescTo(Person* hTo, LONG hAmount);
    STRINGCW        GetDescriptionTo(Person* hTo);
    STRINGCW        GetConditionTo(Person *to, bool hInColor, bool hShort);
    STRINGCW&       GetParsedPrompt();
    STRINGCW        GetRaceAbrvTo(Person* hTo);
    STRINGCW        GetClassAbrvTo(Person* hTo);
    STRINGCW        GetRaceNameTo(Person* hTo);
    STRINGCW        GetClassNameTo(Person* hTo);


    //** Searching MUD **//
    RoomIndex*      FindLocation(const STRINGCW& hArg);
    Exit*           FindExit(const STRINGCW& hArg);
    Object*         RetrieveObjectHere(VREF hVRef, BYTE hMode);
    bool            RetrieveObjectsHere(const CHAR* hArg, ObjectHolder& hHolder, BYTE hMode);
    Object*         ResolveObject(const CHAR* hArg, BYTE hMode);
    bool            ResolveObjects(const CHAR* hArg, BYTE hMode, ObjectHolder& hHolder);
    Person*         ResolvePerson(const CHAR* hArg);
    bool            ResolvePeople(const CHAR* hArg, PersonHolder& hHolder);


    //** Anything **//
    bool            CheckNumber(STRINGCW& hArg);
    SHORT           CountUsers();
    bool            DoEditor(CHAR* hArg, STRINGCW& hText, CHAR* hClearMsg);
    void            ShowTo(Person* hTo);
    virtual void    StatTo(Person* hTo) = 0;
    bool            IsImm();
    bool            IsTrueImm();
    bool            IsOpt(LONG hOptBit);
    bool            IsInfo(LONG hInfoBit);
    bool            IsStatus(LONG hStatBit);
    bool            IsAwake() { return mPosition > kPosSleeping; }
    bool            IsSameGroupAs(Person* hTo);
    void            ChangePosition(BYTE hToPos, Object* hUse = NULL, BYTE hHow = 0);
    void            EquipMessages(Object* hItem, CHAR hToLoc);
    void            EquipObject(Object* hItem, CHAR hToLoc);
    Object*         GetEquipAt(CHAR hAtLoc);
    void            ShowEquipTo(Person* hTo);
    void            UseFurniture(SHORT hPosition, STRINGCW& hArg);


    //** Action **//
    bool CanHandle(Object* hItem);
    bool CanSee(Person* hCh);
    bool CanSee(Object* hCh);
    bool CanSee(RoomIndex* hRoom);
    bool CanSee(Exit* hExit);
    bool CanMove(SHORT hInDir, bool hAsPassenger);
    bool CanEnter(RoomIndex* hRoom);
    bool TryMove(SHORT hInDir, bool hFollow);
    bool CanStand(Object* hUse = NULL, BYTE hHow = 0);
    bool TryStand(Object* hUse = NULL, BYTE hHow = 0);
    bool CanSit(Object* hUse = NULL, BYTE hHow = 0);
    bool TrySit(Object* hUse = NULL, BYTE hHow = 0);
    bool CanRest(Object* hUse = NULL, BYTE hHow = 0);
    bool TryRest(Object* hUse = NULL, BYTE hHow = 0);
    bool CanSleep(Object* hUse = NULL, BYTE hHow = 0);
    bool TrySleep(Object* hUse = NULL, BYTE hHow = 0);
    bool CanWear(Object* hItem, CHAR hInPos);
    bool TryWear(Object* hItem, CHAR hInPos);
    bool CanRemove(Object* hItem);
    bool TryRemove(Object* hItem);
    bool CanFight(Person* hVict);
    bool TryFight(Person* hVict);




    //** Runtime Data Members **//
    public:
    OrangeSession*  mSession;
    Player*         isPlayer;
    Mobile*         isMobile;
    RoomIndex*      mInRoom;
    RoomIndex*      mWasInRoom;
    Spot*           mInSpot;
    BYTE            mOnHow;
    Object*         mOnObject;
    Person*         mRiding;
    SHORT           mTimer;
    PartArray       mParts;
    deque<SHORT>    mCmds[kWaitMax];            //Index in command table
    deque<STRINGCW> mArgs[kWaitMax];            //argument for command
    SHORT           mLags[kWaitMax];            //input channel lags
    Person*         mFighting;
    Person*         mFollowing;
    Fight*          mImFighting;
    Fight*          mFightingMe;
    float           mHitRegen, mEnergyRegen, mMoveRegen;
    float           mFocus;
    BYTE            mHitT, mEnergyT, mMoveT;    //Store Tenth Values For Regeneration
    BYTE            mAttacksLeft;               //Attacks left this round   


    //** Persistant Data Members **//
    public:
    STRINGCW        mName;
    STRINGCW        mPName;
    STRINGCW        mKeywords;
    STRINGCW        mShortDesc;
    STRINGCW        mPShortDesc;
    STRINGCW        mDescription;
    //Score Values
    BYTE            mSex;
    SHORT           mLevel;
    BYTE            mRace;
    BYTE            mClass;
    BYTE            mAlignment;
    BYTE            mSize;
    BYTE            mPosition;
    LONG            mWimpy[kWimpyMax];
    CHAR            mResist[kResMax];
    SHORT           mStat[kStatMax];
    LONG            mHits   , mHitsMax;
    LONG            mEnergy , mEnergyMax;
    LONG            mMoves  , mMovesMax;
    //Leveling-Misc Values
    LONG            mExpTNL;    
    SHORT           mLevelLast;                 //Past Leveling Info
    SHORT           mLevelHitT;
    SHORT           mLevelEnergyT;
    SHORT           mLevelMoveT;
    //Other
    ObjectHolder    mWearing;       




    /////////////////////////////////////////////////////
    // Get Value Methods
    /////////////////////////////////////////////////////
    UID             GetUID();
    SHORT           GetTrust();
    SHORT           GetStr()        { return mStat[kStatStr]; }
    SHORT           GetDex()        { return mStat[kStatDex]; }
    SHORT           GetInt()        { return mStat[kStatInt]; }
    SHORT           GetWis()        { return mStat[kStatWis]; }
    SHORT           GetCon()        { return mStat[kStatCon]; }
    SHORT           GetBaseStr()    { return mStat[kStatStr] - GetStrMods(); }
    SHORT           GetBaseDex()    { return mStat[kStatDex] - GetDexMods(); }
    SHORT           GetBaseInt()    { return mStat[kStatInt] - GetIntMods(); }
    SHORT           GetBaseWis()    { return mStat[kStatWis] - GetWisMods(); }
    SHORT           GetBaseCon()    { return mStat[kStatCon] - GetConMods(); }
    SHORT           GetStrMods()    { return 0; }
    SHORT           GetDexMods()    { return 0; }
    SHORT           GetIntMods()    { return 0; }
    SHORT           GetWisMods()    { return 0; }
    SHORT           GetConMods()    { return 0; }
    CHAR            GetColdRes()    { return mResist[kResCold]; }
    CHAR            GetFireRes()    { return mResist[kResFire]; }
    CHAR            GetAcidRes()    { return mResist[kResAcid]; }
    CHAR            GetElecRes()    { return mResist[kResElec]; }
    CHAR            GetPoisRes()    { return mResist[kResPois]; }
    CHAR            GetMindRes()    { return mResist[kResMind]; }
    CHAR            GetSleepRes()   { return mResist[kResSlee]; }
    CHAR            GetCharmRes()   { return mResist[kResChar]; }
    LONG            GetHits()       { return mHits; }
    LONG            GetEnergy()     { return mEnergy; }
    LONG            GetMoves()      { return mMoves; }
    LONG            GetMaxHits()    { return mHitsMax; }
    LONG            GetMaxEnergy()  { return mEnergyMax; }
    LONG            GetMaxMoves()   { return mMovesMax; }
};





//## RetrieveObjectHere Modes ##//
enum
{
    kFromInv            = 0 ,
    kFromInvWear            ,
    kFromInvWearRoom        ,
    kFromWear               ,
    kFromRoom
};


//## Option Preference Bits ##//
#define kOptAutoAssist      (A)
#define kOptAutoLoot        (B)
#define kOptAutoGold        (C)
#define kOptAutoSplit       (D)
#define kOptAutoSkin        (E)
#define kOptColor           (F)
#define kOptAutoExits       (G)
#define kOptNoSummon        (H)
#define kOptBrief           (I)
#define kOptPrompt          (J)
#define kOptMovement        (K)
#define kOptDamBySelf       (L)
#define kOptDamByGroup      (M)
#define kOptDamToSelf       (N)
#define kOptDamToGroup      (O)
#define kOptIncognito       (P)
#define kOptMisses          (Q)
#define kOptAutoScan        (R)
#define kOptRepeat          (S)
#define kOptImmPowers       (dd)
#define kOptInfo            (ee)

//## Info Preference Bits ##//
#define kInfoConnect        (A)
#define kInfoQuit           (B)
#define kInfoLinks          (C)
#define kInfoLevels         (D)
#define kInfoDeaths         (E)
#define kInfoNotes          (F)
//Info Channels
#define kInfoChannelImm     (G)
#define kInfoChannelChat    (H)
#define kInfoChannelGossip  (I)
#define kInfoChannelOOC     (J)
#define kInfoChannelTell    (K)
#define kInfoChannelClan    (L)
#define kInfoChannelAuction (M)
//Imm Info
#define kInfoImm            (Y)
#define kInfoRepops         (Z)
#define kInfoRewards        (aa)
#define kInfoCreate         (bb)
#define kInfoPunish         (cc)
#define kInfoDebug          (dd)
#define kInfoOLC            (ee)

//## Status Bits ##//
#define kStatusAFK          (A)



#endif //ifndef PERSON_H



