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
#ifndef AREA_H
#define AREA_H 1
#include "LoadOld.h"
#include "UIDHolder.h"

class AreaInfo;

class Area
    : public LoadOld
    , public SaveLoadable
{
    //** De/constructors **//
    public: Area();
    public: virtual ~Area();
    
    //** SaveLoadable Functions **//
    bool                Save();
    virtual void        SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void        SaveValuesTo(XMLWriter& hFile);
    virtual bool        MatchValues(XMLReader& hXML, XMLTag& hTag); 
    
    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    virtual void    OnLoad();
    void            LoadAreaRoomsOld(MUDFile& hFp);
    void            LoadAreaObjectsOld(MUDFile& hFp);
    void            LoadAreaMobilesOld(MUDFile& hFp);


    //** Public Member Functions **//
    public:
    void            StatTo(Person* hTo);
    void            StatListTo(Person* hTo);
    void            GetInfo(AreaInfo& hInfo);
    IRoomMap&       GetRooms() {return mRoomIndexes;}
    IObjectMap&     GetObjects() {return mObjectIndexes;}
    IMobileMap&     GetMobiles() {return mMobileIndexes;}
    void            AddRoomIndex(RoomIndex* hIndex);
    void            AddObjectIndex(ObjectIndex* hIndex);
    void            AddMobileIndex(MobileIndex* hIndex);
    void            RePopulate();
    void            UpdateAmbience();
    /// Search ///
    RoomIndex*      FindRoomIndex(STRINGCW& hName);
    ObjectIndex*    FindObjectIndex(STRINGCW& hName);
    MobileIndex*    FindMobileIndex(STRINGCW& hName);
    RoomIndex*      GetRoomIndex(VNUM hVNum);
    ObjectIndex*    GetObjectIndex(VNUM hVNum);
    MobileIndex*    GetMobileIndex(VNUM hVNum);



    //** Runtime Data Members **//
    public:
    SHORT               mNumPlayers;
    TIMET               mLastRespawn;
    Vector<Person*>     mEditors;
    bool                mChanged;
    
    //** Persistant Index Members **//
    public:
    VNUM                mANum;
    STRINGCW            mName;
    STRINGCW            mFileName;
    SHORT               mLowRange;
    SHORT               mHighRange;
    UIDHolder           mBuilders;
    SHORT               mRespawnRate;
    SHORT               mVersion;
    IRoomMap            mRoomIndexes;
    IObjectMap          mObjectIndexes;
    IMobileMap          mMobileIndexes;
    AmbienceArray       mRoomAmbience;
    AmbienceArray       mMobAmbience;
    PopulationArray     mPopulations;
    
    //** Outdated **//
    STRINGCW        mCredits;       //Outdated
    VNUM            mMinOldVnum;    //Outdated
    VNUM            mMaxOldVnum;    //Outdated

};


class AreaInfo
{
    public: AreaInfo();
    LONG        mTotalLines;

    LONG        mRoomLines;
    LONG        mObjectLines;
    LONG        mMobileLines;
    LONG        mRoomSpotLines;
    LONG        mObjectSpotLines;
    LONG        mMobileSpotLines;
    LONG        mRoomScriptLines;   
    LONG        mObjectScriptLines; 
    LONG        mMobileScriptLines; 
    LONG        mRoomAmbienceLines;
    LONG        mMobileAmbienceLines;
    LONG        mRepopLines;
    LONG        mShopLines;
    LONG        mRooms;
    LONG        mRoomSpots;
    LONG        mRoomScripts;
    LONG        mObjects;
    LONG        mObjectSpots;
    LONG        mObjectScripts;
    LONG        mMobiles;
    LONG        mMobileSpots;
    LONG        mMobileScripts;
    LONG        mRoomAmbience;
    LONG        mMobileAmbience;
    LONG        mRepops;
    LONG        mShops;
};




#endif //ifndef AREA_H
