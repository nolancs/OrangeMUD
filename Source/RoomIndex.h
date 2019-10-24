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
#ifndef ROOMINDEX_H
#define ROOMINDEX_H 1
#include "VRefThing.h"
#include "SpottedThing.h"
#include "ScriptedThing.h"
#include "PersonHolder.h"
#include "ObjectHolder.h"
#include "LoadOld.h"
#include "Exit.h"
#include "SaveLoadable.h"


class RoomIndex 
    : public VRefThing
    , public ObjectHolder
    , public PersonHolder
    , public SpottedThing
    , public ScriptedThing
    , public LoadOld
//  , public SaveLoadable //Comes from ScriptedThing/SpottedThing
{
    //** De/constructors **//
    public: RoomIndex();
    public: virtual ~RoomIndex();
    
    //** Class Functions **//
    public: static RoomIndex* Get(VNUM hVnum);
    public: static RoomIndex* Get(VREF hVRef);

    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag); 

    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);

    //** ActThing Overrides **//
    virtual STRINGCW GetTextForCodeTo(Person* hTo, CHAR hCode);

    //** Public Member Functions **//
    public:
    bool            IsDark();
    bool            IsIndoors();
    bool            IsPrivate();
    bool            IsWater();
    void            ShowTo(Person* hTo, bool hEnteringRoom = false);
    void            ShowPeopleTo(Person* hTo);
    void            ShowObjectsTo(Person* hTo);
    void            ShowExitsTo(Person* hTo);
    void            ShowScanTo(Person* hTo, bool hAutoScan);
    void            StatTo(Person* hTo);
    STRINGCW&       GetExitsCharsTo(Person* hTo);
    
    //** Runtime Data Members **//
    public:
    Area*           mInArea;
    SHORT           mLights;
    USHORT          mNumPlayers;
    AmbienceArray   mAmbiences;
    TIMET           mLastAmbience;
    
    //** Persistant Data Members **//
    public:
    STRINGCW    mName;
    STRINGCW    mDescription;
    UID         mOwner;
    SHORT       mClan;
    LONG        mFlags;
    SHORT       mTerrain;
    SHORT       mCapacity;
    SHORT       mAmbFreq;
    SHORT       mHitBonus;
    SHORT       mEnergyBonus;
    SHORT       mMoveBonus;
    Exit*       mExits[kDirMax];    
};




//## Room Flags ##//
#define kRoomDark           (A) //Forced Darkness
#define kRoomLight          (B) //Forced Light
#define kRoomIndoors        (C) //No Weather Message
#define kRoomSafe           (D) //No Fighting, Magic
#define kRoomLaw            (E) //Adhear to laws or be captured
#define kRoomNoMob          (F) //No Mobiles
#define kRoomNoRecall       (G) //No Recalling
#define kRoomNoSpell        (H) //No Spell Casting
#define kRoomNoSound        (I) //No Shouts/Yells In/Out
#define kRoomBank           (J) //Banking May Take Place
#define kRoomCurrency       (K) //Currency exchanging may take place
#define kRoomUnderwater     (L) //Must Be Able To Breathe Water
#define kRoomSaveItems      (Y) //Jail room for criminals
#define kRoomPunish         (Z) //Timeout room for bad mortals


//## Room Land/Sector Types ##//
enum
{
    kTerInside = 0      ,
    kTerCity            ,
    kTerField           ,
    kTerForest          ,
    kTerHills           ,
    kTerMountain        ,
    kTerWaterSwim       ,
    kTerWaterNoSwim     ,
    kTerMidAir          , 
    kTerDesert          ,
    kTerUnused          ,
    kTerMax
};




#endif //ifndef ROOMINDEX_H
