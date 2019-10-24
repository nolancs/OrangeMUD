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
*********************************,*********************************************/
#ifndef MUD_H
#define MUD_H 1
#include "TCPManager.h"
#include "FileManager.h"
#include "PersonHolder.h"
#include "ObjectHolder.h"
#include "ScriptExecutor.h"

//## Defines ##//
//
//About kPulsesPerSecond:
//Lower values make the mud less responsive, as it only samples and processes
//input this many times per second. 2 is the minimum and is kind of laggy,
//and 20 is the maximum which is really snappy/instantly responsive.
//
#define kPulsesPerSecond    20
#define kPulsesPerMinute    ( 60 * kPulsesPerSecond)
#define kPulseTime          ( 10 * kPulsesPerSecond)
#define kPulseArea          ( 20 * kPulsesPerSecond)
#define kPulseAggro         (  2 * kPulsesPerSecond)
#define kPulseBattle        (  3 * kPulsesPerSecond)
#define kPulseRegen         ( 10 * kPulsesPerSecond)
#define kPulseCondition     ( 60 * kPulsesPerSecond)
#define kPulseScript        (  3 * kPulsesPerSecond)
#define kPulseAutoSave      (240 * kPulsesPerSecond)
#define kPulseMobile        ( 30 * kPulsesPerSecond)
//Un-impl
#define kPulseAuction       (  6 * kPulsesPerSecond)
#define kPulseTick          ( 60 * kPulsesPerSecond)


enum {
    kTickTime   = 0 , kTickArea     , kTickAggro        , kTickBattle       ,
    kTickRegen      , kTickCondition, kTickScript       , kTickSave         ,
    kTickMobile     , kTickMax      
    };
    


//## Classes ##//
class MUD
{
    //** Singleton Access & De/Constructors **//
    public: MUD();
    public: ~MUD() {}
    public: static MUD* Get();
    class RunningScript;
    typedef List<RunningScript*> RunningScriptList;


    //** Public Member Functions **//
    public:
    bool            Startup();
    void            Process();
    void            ProcessSleep();
    void            ProcessInput();
    void            ProcessOutput();
    void            Shutdown();
    void            UpdateTime();
    void            UpdateAreas();
    void            UpdateAgression();
    void            UpdateBattles();
    void            UpdateRegen();
    void            UpdateConditions();
    void            UpdateScripts();
    void            UpdateAutoSave();
    void            UpdateMobiles();
    /// Accessor Functions ///
    Area*           GetArea(VNUM hANum);
    RoomIndex*      GetRoomIndex(VREF hVRef);
    ObjectIndex*    GetObjectIndex(VREF hVRef);
    MobileIndex*    GetMobileIndex(VREF hVRef);
    AreaMap&        GetAreas() {return mAreas;}
    RoomIndex*      GetRoomIndex(VNUM hID);     //Outdated
    ObjectIndex*    GetObjectIndex(VNUM hID);   //Outdated
    MobileIndex*    GetMobileIndex(VNUM hID);   //Outdated
    /// Searching ///
    bool            ResolveVRef(STRINGCW& hFromStr, VREF& hToVRef);
    RoomIndex*      ResolveRoomVRef(Person* hTo, const STRINGCW& hVRefStr);
    ObjectIndex*    ResolveObjectVRef(Person* hTo, const STRINGCW& hVRefStr);
    MobileIndex*    ResolveMobileVRef(Person* hTo, const STRINGCW& hVRefStr);
    Area*           FindArea(STRINGCW& hName);
    bool            PlayerExists(STRINGCW& hName);
    const CHAR*     ResolveUIDToName(UID hID);
    UID             ResolveNameToUID(const CHAR* hName);
    /// Loading/Saving ///
    void            StartLoadingProgress(SHORT hNumAreas = 0);
    void            UpdateLoadingProgress();
    void            FinishLoadingProgress();
    void            LoadResources();
    bool            LoadAreaFile(STRINGCW& hName);
    bool            LoadHelpFile(STRINGCW& hName);
    bool            LoadSocialFile(STRINGCW& hName);
    CHAR            LoadPlayer(STRINGCW& hName, Player*& hPlayer);
    bool            AddResourceFile(STRINGCW hName);
    bool            RemoveResourceFile(STRINGCW hName);
    void            LoadAllAreasOld();                                  //Outdated
    CHAR            LoadAreaOld(STRINGCW& hPath);                       //Outdated
    CHAR            LoadPlayerOld(STRINGCW& hName, Player*& hPlayer);   //Outdated
    void            SaveUniqueIDMap();
    /// Other ///
    void            Bug(STRINGCW hBugStr, STRINGCW hFile, LONG hLine, bool hIsFatal);
    /// MUD-In/Out ///
    void            AddSession(OrangeSession* hSession);
    void            KillSession(OrangeSession* hSession);
    void            PlaceInGame(Player* hPlayer);
    void            PlaceInGame(Mobile* hMobile);
    void            PlaceInGame(Object* hObject);
    void            RemoveFromGame(Player* hPlayer);
    void            RemoveFromGame(Mobile* hMobile);
    void            RemoveFromGame(Object* hObject);
    Immortal*       TransformToImmortal(Player* hPlayer);
    Player*         TransformToPlayer(Immortal* hPlayer);
    UID             GenerateUniqueID();
    void            AddUniqueID(UID hID, const CHAR* hName);
    void            RemoveUniqueID(UID hID);
    void            AddSocial(Social* hSocial);
    void            RemoveSocial(Social* hSocial);
    Social*         FindSocial(const STRINGCW& hName);
    Help*           FindHelp(const STRINGCW& hName);
    void            RunScript(OrangeScript* hScript, RoomIndex* hRoom, Person* hCh, 
                                Mobile* hMobile, Object* hObject, Person* hVictim);

    //** Public Data Members **//
    public:
    bool                mMUDAlive;
    bool                mLoading;
    STRINGCW            mLoadingProgress;
    SHORT               mLoadingTotalFiles;
    SHORT               mLoadingLoadedFiles;
    PersonHolder        mPeople;
    PersonHolder        mMobiles;
    PersonHolder        mPlayers;
    ObjectHolder        mObjects;
    SessionList         mSessions;          //Everyone Connected
    SessionList         mSessionsTemp;      //Everyone Connected
    SessionList         mDyingSessions;     //To-Be-Destroyed Sessions
    PersonList          mPendingCmds;       //Queued Commands Waiting
    FightList           mFights;            //Current fights
    RunningScriptList   mScripts;           //Scripts runnings
    ULONG               mPlayerDeaths;
    ULONG               mMobileDeaths;
    /// World Data ///
    AreaMap             mAreas;
    HelpFileList        mHelpFiles;
    SocialFileList      mSocialFiles;
    Help*               mHelpGreeting;
    Help*               mHelpMenu;
    Help*               mHelpMOTD;
    RunningScript*      mRunningNow;

    /// Old Indexes ///
    ////////////////////////////////////////////////////////////////////
    void                ConvertToNewForm();
    void                ConvertToNewFormThings(Area *hArea);
    void                ConvertToNewFormReferencesToThings(Area *hArea);
    AreaMap             mOldAreas;
    IRoomMap            mOldRoomIndexes;
    IObjectMap          mOldObjectIndexes;
    IMobileMap          mOldMobileIndexes;
    ////////////////////////////////////////////////////////////////////
    
    
    //** Protected Data Members **//
    protected:
    map<UID,STRINGCW>   mUniqueIDMap;
    SHORT               mTicks[kTickMax];
    SocialList          mSocials['z'-'a'];
    
    //** Public Sub-Class **//
    public:
    class RunningScript
    {
        public: RunningScript(OrangeScript* hScript);
        void    Process();
        bool    Done();
        
        public:
        ScriptExecutor  mExecutor;
        OrangeScript*   mScript;
        RoomIndex*      mRoom;
        Person*         mCh;
        Mobile*         mMobile;
        Object*         mObject;
        Person*         mVictim;
    };
};


//## External Variables ##//
extern TimeV    gExactTime;
extern TIMET    gTime;
void    MFBug(MUDFile& hFile, CHAR* hStr, ...);
#define MUDBug(x)       MUD::Get()->Bug(x, __FILE__, __LINE__, false)
#define MUDFatalBug(x)  MUD::Get()->Bug(x, __FILE__, __LINE__, true)


//## More Defines ##//

//** Game Loading **//
#define kPath_Player        "Resources","PlayerFiles"
#define kFile_PFile         "PFile.xml"
#define kPath_IDMap         "Resources","PlayerFiles"
#define kFile_IDMap         "IDMap.xml"
#define kPath_Resources     "Resources"
#define kFile_LoadList      "LoadList.xml"
#define kPath_AreaFiles     "Resources","AreaFiles"
#define kPath_HelpFiles     "Resources","HelpFiles"
#define kPath_SocialFiles   "Resources","SocialFiles"

#define kPath_Old_Player    "player"
#define kPath_Old_Area      "area"
#define kFile_Old_AreaList  "area.lst"          //List of areas, socials, helps
#define kFile_Old_WebWho    "online.who"        //Web-based who/is info

/*
#define NOTE_DIR        "../note"
#define AREA_DIR        "../area"
#define BACKUP_DIR      "../area/backup"
#define PLAYER_DIR      "../player" 
#define WEB_DIR         "../web"
#define NULL_FILE       "/dev/null" 
#define SEP             "/"
#define OUT             ".."
*/

#endif //ifndef MUD_H
