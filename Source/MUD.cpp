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
#include "Ambience.h"
#include "Area.h"
#include "Exit.h"
#include "Fight.h"
#include "Help.h"
#include "HelpFile.h"
#include "Immortal.h"
#include "Interpret.h"
#include "MemFileLink.h"
#include "Mobile.h"
#include "MobilePopulation.h"
#include "MUD.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "ObjectPopulation.h"
#include "OrangeSession.h"
#include "OrangeScript.h"
#include "OrangeScriptBinary.h"
#include "OSUtils.h"
#include "Part.h"
#include "Player.h"
#include "RoomIndex.h"
#include "ScriptDataType.h"
#include "ScriptVariable.h"
#include "Social.h"
#include "SocialFile.h"
#include "StringUtils.h"
#include "XMLFile.h"

#define MEMFILE_RESOURCES 1
#define SHOW_LOADING 0

//## Global Variable ###//
TimeV   gExactTime = ExactTime();
TIMET   gTime = CurrentTime();


MUD::MUD()
    : mMUDAlive(true)
    , mLoading(false)
    , mHelpGreeting(NULL)
    , mHelpMenu(NULL)
    , mHelpMOTD(NULL)
    , mPlayerDeaths(0)
    , mMobileDeaths(0)
    , mLoadingTotalFiles(0)
    , mLoadingLoadedFiles(0)
    , mRunningNow(NULL)
{
    for(INT i = 0; i < kTickMax; ++i)
        mTicks[i] = 0;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::Get
//
//   Desc:: Access to the singleton class MUD.
//
// Output:: The MUD object
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MUD* MUD::Get()
{
    static MUD* nThisMud = NULL;
    
    if(!nThisMud)
        nThisMud =  new MUD();
        
    return nThisMud;
}


void MUD::StartLoadingProgress(SHORT hNumAreas)
{
    mLoadingTotalFiles = hNumAreas;
}


void MUD::UpdateLoadingProgress()
{
    if(mLoadingTotalFiles)
    ASSERT(mLoadingLoadedFiles < mLoadingTotalFiles);
    
    //** Calculate What Text To Send **//
    ++mLoadingLoadedFiles;
    STRINGCW nShowProgress;
    if(mLoadingTotalFiles)
    {
        SHORT nAmt = (SHORT)(79.0*(mLoadingLoadedFiles/((float)mLoadingTotalFiles)));
        CHAR nBuf[80];
        memset(nBuf, '#', nAmt);
        nBuf[nAmt]='\0';
        nShowProgress = nBuf;
        ASSERT(nShowProgress.Length() >= mLoadingProgress.Length());
        nShowProgress.Erase(0, mLoadingProgress.Length());
    }else{
        nShowProgress = "#";
    }
    
    //** Update Every Session **//
    mSessionsTemp.clear();
    while(!mSessions.empty())
    {
        OrangeSession* nSession = mSessions.front();
        mSessionsTemp.push_back(nSession);
        mSessions.pop_front();
        if(nSession->mTCP.Ptr())
        {
            nSession->mTCP->Write(nShowProgress);
        }
    }
    mSessions = mSessionsTemp;

    mLoadingProgress += nShowProgress;
    ProcessInput();
    ProcessOutput();
}


void MUD::FinishLoadingProgress()
{
    //** Move Every Session To Splash Screen **//
    mSessionsTemp.clear();
    while(!mSessions.empty())
    {
        OrangeSession* nSession = mSessions.front();

        mSessionsTemp.push_back(nSession);
        mSessions.pop_front();
        
        if(nSession->mTCP.Ptr())
        {
            nSession->mTCP->Write("\n");
        }
        nSession->EnterSplash();
    }
    mSessions = mSessionsTemp;

    mLoading = false;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::Startup
//
//   Desc:: Startup the network. Read the area files. Get the game into its
//          ready state so people can connect.
//
// Output:: True of the startup was successful.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MUD::Startup()
{
    SHORT nPortNum = 8888;
    
    cout << "-------------------------------" << endl;
    cout << "OrangeMUD v3.0 Starting up." << endl;
    mLoading = true;

    
    //Initialize Random Numbers
    srand((unsigned int) time(NULL));
    
    
    FileManager::Get()->Startup();
    TCPManager::Get()->Initialize();
    TCPManager::Get()->StartListening(nPortNum);
    cout << ":: Listening on port " << nPortNum << " ::" << endl;
    cout << ":: Connect using: telnet localhost " << nPortNum << " ::" << endl;

    //** Load UniqueID-Name Map **//
    FilePath nPathToIDMap(kPath_IDMap, kFile_IDMap);
    FileLink* nIDMapFile = FileManager::Get()->OpenRead(nPathToIDMap);
    if(nIDMapFile)
    {
        XMLFile nIDMapXMLFile;
        XMLReader nXMLReader(nIDMapFile);
        if(eXML_NoErr != nXMLReader.ReadFile(&nIDMapXMLFile))
            ASSERT(!"Unable to load IDMapFile");
        nIDMapFile->Close();
        delete nIDMapFile;
        
        ASSERT(nIDMapXMLFile.mTags.size() == 1);
        XMLTagArray& entryTags = nIDMapXMLFile.mTags[0]->GetSubTags();
        for(ULONG i = 0; i < entryTags.size(); ++i)
        {
            XMLTag* nThisEntry = entryTags[i];
            UID nEntryUID = (UID) atol(nThisEntry->GetAttributes()["UID"].c_str());
            STRINGCW nEntryName = nThisEntry->AsCStr();
            ASSERT(mUniqueIDMap.find(nEntryUID) == mUniqueIDMap.end());
            mUniqueIDMap[nEntryUID] = nEntryName;
        }
    }

    //** Load Areas **//    //Outdated
    FilePath nPathToResources(kPath_Resources);
    if(FileManager::Get()->Exists(nPathToResources))
    {
        LoadResources();
    }else{
        LoadAllAreasOld();
        ConvertToNewForm();
    }

    //** Link All Room Exits/Ambience **//
    for(AreaIter nAScan = GetAreas().begin(); nAScan != GetAreas().end(); ++nAScan)
    {
        Area*       nArea   = (*nAScan).second;
        IRoomMap&   nRooms  = nArea->GetRooms();
        
        for(IRoomIter nRScan = nRooms.begin(); nRScan != nRooms.end(); ++nRScan)
        {
            RoomIndex*  nRoom = (*nRScan).second;
            Exit*       nExit;
            
            for(SHORT i = 0; i < kDirMax; ++i)
            if((nExit = nRoom->mExits[i]) != NULL)
                nExit->mToRoom = RoomIndex::Get(nExit->mToVRef);
        }
        
        //Link Room Ambiences To Their Rooms
        for(ULONG i = 0; i < nArea->mRoomAmbience.size(); ++i)
        {
            Vector<VNUM> nVnums = nArea->mRoomAmbience[i]->mVnums;
            nArea->mRoomAmbience[i]->mVnums.clear();
            for(ULONG j = 0; j < nVnums.size(); ++j)
                nArea->mRoomAmbience[i]->AddOntoRoom(MakeVRef(nArea->mANum, nVnums[j]));
        }

        //Link Mobile Ambiences To Their Mobiles
        for(ULONG i = 0; i < nArea->mMobAmbience.size(); ++i)
        {
            Vector<VNUM> nVnums = nArea->mMobAmbience[i]->mVnums;
            nArea->mMobAmbience[i]->mVnums.clear();
            for(ULONG j = 0; j < nVnums.size(); ++j)
                nArea->mMobAmbience[i]->AddOntoMobile(MakeVRef(nArea->mANum, nVnums[j]));
        }
    }
    
    
    //** Optimize Interpet's CommandTable **//
    InitCommandHash();
    
    
    cout << "OrangeMUD v3.0 Online." << endl;
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::LoadResources
//
//   Desc:: Loads every file in the Resources/LoadList.xml resource list file.
//          This includes all area files, social files, help files.
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::LoadResources()
{
    TimeV nTimeStartLoad = ExactTime();
    
    FilePath nPathToList(kPath_Resources, kFile_LoadList);
    APtr<FileLink> nLinkToList = FileManager::Get()->OpenRead(nPathToList);
    if(nLinkToList == NULL)
        ASSERT(!"No kFile_LoadList");


    XMLFile nXMLLoadList;
    XMLReader nXMLReader(nLinkToList);
    if(eXML_NoErr != nXMLReader.ReadFile(&nXMLLoadList))
        ASSERT(!"Couldn't read the xml LoadList");
    nLinkToList->Close();
    ASSERT(nXMLLoadList.mTags.size() == 1);

    
    //Locate our specific tags of interest
    XMLTag* nBaseTag = nXMLLoadList.mTags[0];
    XMLTag* nAreaTag = NULL;
    XMLTag* nHelpTag = NULL;
    XMLTag* nSocialTag = NULL;
    for(ULONG i = 0; i < nBaseTag->GetSubTags().size(); ++i)
    {
        if(nBaseTag->GetSubTags()[i]->GetName() == "AreaFiles")
            nAreaTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "HelpFiles")
            nHelpTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "SocialFiles")
            nSocialTag = nBaseTag->GetSubTags()[i];
    }
    ASSERT(nAreaTag != NULL);
    ASSERT(nHelpTag != NULL);
    ASSERT(nSocialTag != NULL);

    
    //Count the files to load
    SHORT nTotalFilesToLoad = 0;
    for(ULONG i = 0; i < nAreaTag->GetSubTags().size(); ++i)
    if(nAreaTag->GetSubTags()[i]->GetName() == "File")
        ++nTotalFilesToLoad;
    for(ULONG i = 0; i < nHelpTag->GetSubTags().size(); ++i)
    if(nHelpTag->GetSubTags()[i]->GetName() == "File")
        ++nTotalFilesToLoad;
    for(ULONG i = 0; i < nSocialTag->GetSubTags().size(); ++i)
    if(nSocialTag->GetSubTags()[i]->GetName() == "File")
        ++nTotalFilesToLoad;
    
    
    //Load the files
    StartLoadingProgress(nTotalFilesToLoad);
    for(ULONG i = 0; i < nAreaTag->GetSubTags().size(); ++i)
    if(nAreaTag->GetSubTags()[i]->GetName() == "File")
    {
        STRINGCW nFileName = nAreaTag->GetSubTags()[i]->AsCStr();
        LoadAreaFile(nFileName);
        UpdateLoadingProgress();
    }
    for(ULONG i = 0; i < nHelpTag->GetSubTags().size(); ++i)
    if(nHelpTag->GetSubTags()[i]->GetName() == "File")
    {
        STRINGCW nFileName = nHelpTag->GetSubTags()[i]->AsCStr();
        LoadHelpFile(nFileName);
        UpdateLoadingProgress();
    }
    for(ULONG i = 0; i < nSocialTag->GetSubTags().size(); ++i)
    if(nSocialTag->GetSubTags()[i]->GetName() == "File")
    {
        STRINGCW nFileName = nSocialTag->GetSubTags()[i]->AsCStr();
        LoadSocialFile(nFileName);
        UpdateLoadingProgress();
    }
    FinishLoadingProgress();

    TimeV nSecsToLoad = ExactTime()-nTimeStartLoad;
    printf(":: %f seconds to load :: \n", nSecsToLoad.InFloatSecs());
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::LoadAreaFile
//
//   Desc:: Loads a help file into the mud. The hName is assumed to just be 
//          the name of the file, which would be located in the 
//          Resources/AreaFiles/ directory.
//  
//  Input:: The file name.
//
// Output:: True on success or false if failure.
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MUD::LoadAreaFile(STRINGCW& hName)
{
    FilePath nPathToFile(kPath_AreaFiles, hName);
    APtr<FileLink> nLinkToFile = FileManager::Get()->OpenRead(nPathToFile);
    if(nLinkToFile == NULL)
        return false;
    #if(MEMFILE_RESOURCES)
    APtr<MemFileLink> nMemLink = new MemFileLink(nLinkToFile);
    #endif

    APtr<Area> nArea(new Area());
    try
    {
        ///////////////////////////////////////////////
        #if(SHOW_LOADING)
        printf("Loading XML Area File: %s\n", *hName);
        #endif
        #if(MEMFILE_RESOURCES)
        XMLReader nXMLReader(nMemLink);
        #else
        XMLReader nXMLReader(nLinkToFile);
        #endif
        APtr<XMLTag> nRootTag;
        nXMLReader.OpenBaseTag(nRootTag);
        ASSERT(nRootTag->GetName() == "Area");
        nArea->LoadValuesFrom(nXMLReader, *nRootTag);
        ///////////////////////////////////////////////
    }
    catch(eXMLError hExceptionErr)
    {
        printf("%d exception XML thrown on read.\n", hExceptionErr);
        return false;
    }

    ASSERT(GetAreas().find(nArea->mANum) == GetAreas().end());
    GetAreas()[nArea->mANum] = nArea;
    nArea.Detach();
    return true;
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::LoadHelpFile
//
//   Desc:: Loads a help file into the mud. The hName is assumed to just be 
//          the name of the file, which would be located in the 
//          Resources/HelpFiles/ directory.
//  
//  Input:: The file name.
//
// Output:: True on success or false if failure.
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MUD::LoadHelpFile(STRINGCW& hName)
{
    FilePath nPathToFile(kPath_HelpFiles, hName);
    APtr<FileLink> nLinkToFile = FileManager::Get()->OpenRead(nPathToFile);
    if(!nLinkToFile)
        return false;
    #if(MEMFILE_RESOURCES)
    APtr<MemFileLink> nMemLink = new MemFileLink(nLinkToFile);
    #endif


    APtr<HelpFile> nHelpFile(new HelpFile());
    try
    {
        ///////////////////////////////////////////////
        #if(SHOW_LOADING)
        printf("Loading XML Help File: %s\n", *hName);
        #endif
        #if(MEMFILE_RESOURCES)
        XMLReader nXMLReader(nMemLink);
        #else
        XMLReader nXMLReader(nLinkToFile);
        #endif
        APtr<XMLTag> nRootTag;
        nXMLReader.OpenBaseTag(nRootTag);
        ASSERT(nRootTag->GetName() == "HelpFile");
        nHelpFile->LoadValuesFrom(nXMLReader, *nRootTag);
        ///////////////////////////////////////////////
    }
    catch(eXMLError hExceptionErr)
    {
        printf("%d exception XML thrown on read.\n", hExceptionErr);
        return false;
    }


    MUD::Get()->mHelpFiles.Add(nHelpFile.Detach());
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::LoadSocialFile
//
//   Desc:: Loads a social file into the mud. The hName is assumed to just be 
//          the name of the file, which would be located in the 
//          Resources/SocialFiles/ directory.
//  
//  Input:: The file name.
//
// Output:: True on success or false if failure.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MUD::LoadSocialFile(STRINGCW& hName)
{
    FilePath nPathToFile(kPath_SocialFiles, hName);
    APtr<FileLink> nLinkToFile = FileManager::Get()->OpenRead(nPathToFile);
    if(!nLinkToFile)
        return false;
    #if(MEMFILE_RESOURCES)
    APtr<MemFileLink> nMemLink = new MemFileLink(nLinkToFile);
    #endif


    APtr<SocialFile> nSocialFile(new SocialFile());
    try
    {
        ///////////////////////////////////////////////
        #if(SHOW_LOADING)
        printf("Loading XML Socl File: %s\n", *hName);
        #endif
        #if(MEMFILE_RESOURCES)
        XMLReader nXMLReader(nMemLink);
        #else
        XMLReader nXMLReader(nLinkToFile);
        #endif
        APtr<XMLTag> nRootTag;
        nXMLReader.OpenBaseTag(nRootTag);
        ASSERT(nRootTag->GetName() == "SocialFile");
        nSocialFile->LoadValuesFrom(nXMLReader, *nRootTag);
        ///////////////////////////////////////////////
    }
    catch(eXMLError hExceptionErr)
    {
        printf("%d exception XML thrown on read.\n", hExceptionErr);
        return false;
    }


    for(SocialList::iterator i = nSocialFile->mSocials.begin(); i != nSocialFile->mSocials.end(); ++i)
        AddSocial(*i);
    mSocialFiles.Add(nSocialFile.Detach());
    return true;
}




bool MUD::AddResourceFile(STRINGCW hName)
{
    TimeV nTimeStartLoad = ExactTime();
    
    FilePath nPathToList(kPath_Resources, kFile_LoadList);
    APtr<FileLink> nLinkToList = FileManager::Get()->OpenRead(nPathToList);
    if(nLinkToList == NULL)
        ASSERT(!"No kFile_LoadList");


    XMLFile nXMLLoadList;
    XMLReader nXMLReader(nLinkToList);
    if(eXML_NoErr != nXMLReader.ReadFile(&nXMLLoadList))
        ASSERT(!"Couldn't read the xml LoadList");
    nLinkToList->Close();
    nLinkToList.Dispose();
    ASSERT(nXMLLoadList.mTags.size() == 1);

    
    //Locate our specific tags of interest
    XMLTag* nBaseTag = nXMLLoadList.mTags[0];
    XMLTag* nAreaTag = NULL;
    XMLTag* nHelpTag = NULL;
    XMLTag* nSocialTag = NULL;
    for(ULONG i = 0; i < nBaseTag->GetSubTags().size(); ++i)
    {
        if(nBaseTag->GetSubTags()[i]->GetName() == "AreaFiles")
            nAreaTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "HelpFiles")
            nHelpTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "SocialFiles")
            nSocialTag = nBaseTag->GetSubTags()[i];
    }
    ASSERT(nAreaTag != NULL);
    ASSERT(nHelpTag != NULL);
    ASSERT(nSocialTag != NULL);
    
    
    //** Add FIle Tag **//
    XMLTag* nInTag;
         if(StrSuffix(hName, ".are")) nInTag = nAreaTag;
    else if(StrSuffix(hName, ".hlp")) nInTag = nHelpTag;
    else if(StrSuffix(hName, ".scl")) nInTag = nSocialTag;
    else ASSERT(!"Bad resource suffix!");
    APtr<XMLTag> nNewTag(new XMLTag(nInTag));
    nNewTag->GetName() = "File";
    nNewTag->GetValue() = hName;
    nInTag->GetSubTags().push_back(nNewTag.Detach());   
    

    //** Resave Resources File **//
     nLinkToList = FileManager::Get()->OpenWrite(nPathToList);
    if(nLinkToList == NULL)
        ASSERT(!"No kFile_LoadList");
    XMLWriter nXMLWriter(nLinkToList);
    nXMLWriter.WriteXMLFile(&nXMLLoadList);
    nLinkToList->Close();
    nLinkToList.Dispose();
    return true;
}




bool MUD::RemoveResourceFile(STRINGCW hName)
{
    TimeV nTimeStartLoad = ExactTime();
    
    FilePath nPathToList(kPath_Resources, kFile_LoadList);
    APtr<FileLink> nLinkToList = FileManager::Get()->OpenRead(nPathToList);
    if(nLinkToList == NULL)
        ASSERT(!"No kFile_LoadList");


    XMLFile nXMLLoadList;
    XMLReader nXMLReader(nLinkToList);
    if(eXML_NoErr != nXMLReader.ReadFile(&nXMLLoadList))
        ASSERT(!"Couldn't read the xml LoadList");
    nLinkToList->Close();
    nLinkToList.Dispose();
    ASSERT(nXMLLoadList.mTags.size() == 1);

    
    //Locate our specific tags of interest
    XMLTag* nBaseTag = nXMLLoadList.mTags[0];
    XMLTag* nAreaTag = NULL;
    XMLTag* nHelpTag = NULL;
    XMLTag* nSocialTag = NULL;
    for(ULONG i = 0; i < nBaseTag->GetSubTags().size(); ++i)
    {
        if(nBaseTag->GetSubTags()[i]->GetName() == "AreaFiles")
            nAreaTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "HelpFiles")
            nHelpTag = nBaseTag->GetSubTags()[i];
        else if(nBaseTag->GetSubTags()[i]->GetName() == "SocialFiles")
            nSocialTag = nBaseTag->GetSubTags()[i];
    }
    ASSERT(nAreaTag != NULL);
    ASSERT(nHelpTag != NULL);
    ASSERT(nSocialTag != NULL);

    
    //** Remove FIle Tag **//
    ASSERT(!"Not done");
    


    //** Resave Resources File **//
     nLinkToList = FileManager::Get()->OpenWrite(nPathToList);
    if(nLinkToList == NULL)
        ASSERT(!"No kFile_LoadList");
    XMLWriter nXMLWriter(nLinkToList);
    nXMLWriter.WriteXMLFile(&nXMLLoadList);
    nLinkToList->Close();
    nLinkToList.Dispose();
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::Process
//
//   Desc:: Heartbeat of the game. Everything is called on a Per-Pulse-Basis
//          from this loop which calculates, executes, and shows you anything
//          you see in the game. The network reads and the writes. Agressive
//          mobiles look around for anything to kill. Everything.
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::Process()
{
    //** Read From Links **//
    ProcessInput();
    
    
    //** Updates **//
    if(--mTicks[kTickTime] <= 0)
    {
        UpdateTime();
        mTicks[kTickTime] = kPulseTime;
    }   
    if(--mTicks[kTickArea] <= 0)
    {
        UpdateAreas();
        mTicks[kTickArea] = kPulseArea;
    }
    if(--mTicks[kTickAggro] <= 0)
    {
        UpdateAgression();
        mTicks[kTickAggro] = kPulseAggro;
    }
    if(--mTicks[kTickBattle] <= 0)
    {
        UpdateBattles();
        mTicks[kTickBattle] = kPulseBattle;
    }   
    if(--mTicks[kTickRegen] <= 0)
    {
        UpdateRegen();
        mTicks[kTickRegen] = kPulseRegen;
    }
    if(--mTicks[kTickCondition] <= 0)
    {
        UpdateConditions();
        mTicks[kTickCondition] = kPulseCondition;
    }
    if(--mTicks[kTickScript] <= 0)
    {
        UpdateScripts();
        mTicks[kTickScript] = kPulseScript;
    }
    if(--mTicks[kTickSave] <= 0)
    {
        UpdateAutoSave();
        mTicks[kTickSave] = kPulseAutoSave;
    }
    if(--mTicks[kTickMobile] <= 0)
    {
        UpdateMobiles();
        mTicks[kTickMobile] = kPulseMobile;
    }
    

    //** Write To Links **//
    ProcessOutput();


    //** Be Nice **//
    ProcessSleep();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ProcessSleep
//
//   Desc:: If there is any time left since the last "Pulse" then relinquish it
//          to the computer to be nice. By default there are 2 "Pulses" per
//          seconds, it's defined in MUD.h
///@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::ProcessSleep()
{
    TimeV nFullTick(0, 1000000/kPulsesPerSecond);
    
    
    TimeV nDeltaTime = ExactTime() - gExactTime;
    TimeV nSleepTime = nFullTick - nDeltaTime;
    
    if(nSleepTime.mSeconds > 0 || nSleepTime.mMicroSeconds > 0)
    {
        Sleep(nSleepTime);
    }
    
    gExactTime  = ExactTime();
    gTime       = CurrentTime();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ProcessInput
//
//   Desc:: 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::ProcessInput()
{
    //** Accept New Connections **//
    while(TCPManager::Get()->IncomingConnections())
    {
        OrangeSession*      nSession;
        OrangeTCP*          nTCP;
        APtr<TCPLink>       nLink;
        
        try
        {
            nLink = TCPManager::Get()->RecieveConnection();
        }
        catch(...)
        {
            nLink = NULL;
            MUDBug("Exception thrown on RecieveConnection()");
            continue;
        }
        
        if(!nLink)
            continue;
        
        nTCP        = new OrangeTCP(nLink.Detach());
        nSession    = new OrangeSession(nTCP);

        AddSession(nSession);
        if(mLoading)
            nSession->EnterMUDLoading();
        else
            nSession->EnterSplash();
    }
    
    
    //** Process Input/Process Every Session **//
    for(SessionIter i = mSessions.begin(); i != mSessions.end(); ++i)
    {
        OrangeSession* nSession = *i;

        if(nSession->mTCP.Ptr())
            nSession->mTCP->ProcessInput();
        
        nSession->ProcessCommands();
    }
    
    
    //** Put Dying Sessions Out Of Their Misery **//
    while(!mDyingSessions.empty())
    {
        OrangeSession* nSession = mDyingSessions.front();

        mDyingSessions.pop_front();
        mSessions.Remove(nSession);
        delete nSession;
        cout << "Done deleting" << endl;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ProcessOutput
//
//   Desc:: 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::ProcessOutput()
{
    for(SessionIter i = mSessions.begin(); i != mSessions.end(); ++i)
    {
        OrangeSession* nSession = *i;

        if(nSession->mTCP.Ptr())
            nSession->mTCP->ProcessOutput();
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::Shutdown
//
//   Desc:: Shutdown the entire game. Unload networking, files, clean up.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::Shutdown()
{
    cout << "OrangeMUD v3.0 Shutting down." << endl;
    
    TCPManager::Get()->Shutdown();
    FileManager::Get()->Shutdown();

    cout << "OrangeMUD v3.0 Offline." << endl;
    cout << "-------------------------------" << endl;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateTime
//
//   Desc:: Update the MUD times.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateTime()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateAreas
//
//   Desc:: Checks for Areas needing to be repopulated and repopulates them
//          accordingly.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateAreas()
{
    AreaIter    nAreaScan;
    Area*       nArea;
    
    for(nAreaScan = GetAreas().begin(); nAreaScan != GetAreas().end(); ++nAreaScan)
    {
        nArea = (*nAreaScan).second;
        
        //Repopulation
        if((gTime-nArea->mLastRespawn)/60 >= nArea->mRespawnRate)
        {
            nArea->mLastRespawn = gTime;
            nArea->RePopulate();
        }
        
        //Ambience
        nArea->UpdateAmbience();
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateAgression
//
//   Desc:: Allows agressive mobiles to scan the room and attack anyone they
//          feel like killing.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateAgression()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateBattles
//
//   Desc:: Process the next round in the battles that are going.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateBattles()
{
    FightList& nFights = MUD::Get()->mFights;
    for(FightIter i = nFights.begin(); i != nFights.end(); ++i)
        (*i)->DoRound();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateRegen
//
//   Desc:: Heal and regenerate both players and mobiles.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateRegen()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateConditions
//
//   Desc:: Updates hunger, thirst, drunkeness, highness, etc.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateConditions()
{
    for(PersonIter i = mPeople.Begin(); i != mPeople.End(); ++i)
    {
        Person* nPerson = *i;
        //Update Hunger
        
        //Update Thirst
        
        //Update Drunkeness/High
        
        //Update Lights
        for(ObjectIter o = nPerson->mWearing.Begin(); o != nPerson->mWearing.End(); ++o)
        {
            Object* nObject = *o;
            if(nObject->IsLighting() 
            && nObject->ItemType() == kItemLight && nObject->Value(kLightLife) > 0)
            if((--nObject->xValue(kLightLife)) == 0) //Burned Out
            {
                Act("Your $o burns out.\n", nPerson, nObject, NULL, ToChar);
                Act("$n's $o burns out.\n", nPerson, nObject, NULL, ToRoom);
                if(nPerson->mInRoom)
                    --nPerson->mInRoom->mLights;
                //dont junk it, kLightLife of zero means its done anyways
            }
        }
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateScripts
//
//   Desc:: Continue processing scripts that Wait()ed somewhere in their code
//          and are still waiting to finish executing.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateScripts()
{
    for(List<RunningScript*>::iterator i = mScripts.begin(); i != mScripts.end(); )
    {
        List<RunningScript*>::iterator nProcessMe = i++;
        (*nProcessMe)->Process();
        if((*nProcessMe)->Done())
        {
            delete *nProcessMe;
            mScripts.erase(nProcessMe);
        }
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateAutoSave
//
//   Desc:: Saves all the players and relevant things in the world. Basically
//          takes a snapshot of life in the MUD and writes it all out to disk
//          should the unthinkable happen and the MUD crash ;) Also saves the
//          web-whois information and web-wholist.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateAutoSave()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::UpdateMobiles
//
//   Desc:: Allows mobiles to wander, scavenge, and do whatever else they do
//          to appear to be more than the stupid code routines that they are.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::UpdateMobiles()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetRoomIndex                                        [Outdated]
//
//   Desc:: Retrieves a RoomIndex by the outdated VNUM protocol. Only used
//          during conversion.
//
//  Input:: The old virtual number of the index.
//
// Output:: A Non-NULL pointer to the RoomIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* MUD::GetRoomIndex(VNUM hID)
{
    IRoomIter nRoom = mOldRoomIndexes.find(hID);
    return nRoom == mOldRoomIndexes.end() ? NULL : (*nRoom).second; 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetObjectIndex                                      [Outdated]
//
//   Desc:: Retrieves an ObjectIndex by the outdated VNUM protocol. Only used
//          during conversion.
//
//  Input:: The old virtual number of the index.
//
// Output:: A Non-NULL pointer to the ObjectIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ObjectIndex* MUD::GetObjectIndex(VNUM hID)
{
    IObjectIter nObject = mOldObjectIndexes.find(hID);
    return nObject == mOldObjectIndexes.end() ? NULL : (*nObject).second;   
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetMobileIndex                                      [Outdated]
//
//   Desc:: Retrieves an MobileIndex by the outdated VNUM protocol. Only used
//          during conversion.
//
//  Input:: The old virtual number of the index.
//
// Output:: A Non-NULL pointer to the MobileIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MobileIndex* MUD::GetMobileIndex(VNUM hID)
{
    IMobileIter nMobile = mOldMobileIndexes.find(hID);
    return nMobile == mOldMobileIndexes.end() ? NULL : (*nMobile).second;   
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ResolveRoomVRef
//
//   Desc:: Attempts to resolve any valid VReference from text form such as a
//          "1/sword", "del.are/12", or "newbie/backpack" into what room it
//          actually points to. Returning the RoomIndex if one was found,
//          otherwise if hTo is not NULL it tells hTo why it failed.
//
//  Input:: hTo: Alert Person if resolution fails, can be NULL.
//          hVRefStr: The virtual reference string.
//
// Output:: A Non-NULL pointer to the RoomIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* MUD::ResolveRoomVRef(Person* hTo, const STRINGCW& hVRefStr)
{
    STRINGCW    nAreaStr, nThingStr;
    RoomIndex*  nThing;
    Area*       nArea;
    
    if(!hVRefStr.IsVRef(&nAreaStr, &nThingStr))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid virtual reference.\n", *hVRefStr);
        return NULL;
    }
    
        
    if(!(nArea = FindArea(nAreaStr)))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid area name or number.\n", *nAreaStr);
        return NULL;
    }
    
    if(!(nThing = nArea->FindRoomIndex(nThingStr)))
    {
        if(hTo)
            hTo->SendF("No room \"%s\" exists in %s.\n", *nThingStr, *nArea->mFileName);
        return NULL;
    }

    return nThing;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ResolveObjectVRef
//
//   Desc:: Attempts to resolve any valid VReference from text form such as a
//          "1/sword", "del.are/12", or "newbie/backpack" into what object it
//          actually points to. Returning the ObjectIndex if one was found,
//          otherwise if hTo is not NULL it tells hTo why it failed.
//
//  Input:: hTo: Alert Person if resolution fails, can be NULL.
//          hVRefStr: The virtual reference string.
//
// Output:: A Non-NULL pointer to the ObjectIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ObjectIndex* MUD::ResolveObjectVRef(Person* hTo, const STRINGCW& hVRefStr)
{
    STRINGCW        nAreaStr, nThingStr;
    ObjectIndex*    nThing;
    Area*           nArea;
    
    if(!hVRefStr.IsVRef(&nAreaStr, &nThingStr))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid virtual reference.\n", *hVRefStr);
        return NULL;
    }
    
        
    if(!(nArea = FindArea(nAreaStr)))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid area name or number.\n", *nAreaStr);
        return NULL;
    }
    
    if(!(nThing = nArea->FindObjectIndex(nThingStr)))
    {
        if(hTo)
            hTo->SendF("No object \"%s\" exists in %s.\n", *nThingStr, *nArea->mFileName);
        return NULL;
    }

    return nThing;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ResolveMobileVRef
//
//   Desc:: Attempts to resolve any valid VReference from text form such as a
//          "1/sword", "del.are/12", or "newbie/backpack" into what mobile it
//          actually points to. Returning the MobileIndex if one was found,
//          otherwise if hTo is not NULL it tells hTo why it failed.
//
//  Input:: hTo: Alert Person if resolution fails, can be NULL.
//          hVRefStr: The virtual reference string.
//
// Output:: A Non-NULL pointer to the MobileIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MobileIndex* MUD::ResolveMobileVRef(Person* hTo, const STRINGCW& hVRefStr)
{
    STRINGCW        nAreaStr, nThingStr;
    MobileIndex*    nThing;
    Area*           nArea;
    
    if(!hVRefStr.IsVRef(&nAreaStr, &nThingStr))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid virtual reference.\n", *hVRefStr);
        return NULL;
    }
    
        
    if(!(nArea = FindArea(nAreaStr)))
    {
        if(hTo)
            hTo->SendF("\"%s\" is not a valid area name or number.\n", *nAreaStr);
        return NULL;
    }
    
    if(!(nThing = nArea->FindMobileIndex(nThingStr)))
    {
        if(hTo)
            hTo->SendF("No mobile \"%s\" exists in %s.\n", *nThingStr, *nArea->mFileName);
        return NULL;
    }

    return nThing;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::FindArea
//
//   Desc:: Retrieves an Area by its file name.
//
//  Input:: The name or partial name of the area you want.
//
// Output:: A Non-NULL pointer to the Area if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Area* MUD::FindArea(STRINGCW& hName)
{
    //By Area Number?
    if(hName.IsNumber())
    {
        VNUM nAreaNum = (VNUM) hName.AsNumber();
        return GetArea(nAreaNum);
    }
    
    
    //By Area Name?
    AreaIter    nAreaScan;
    Area*       nArea;
    
    for(nAreaScan = GetAreas().begin(); nAreaScan != GetAreas().end(); ++nAreaScan)
    {
        nArea = (*nAreaScan).second;
    
        if(nArea->mFileName.IsNamed(hName))
            return nArea;
    }
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetArea
//
//   Desc:: Retrieves an Area by its area number.
//
//  Input:: The area number of the area you want.
//
// Output:: A Non-NULL pointer to the Area if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Area* MUD::GetArea(VNUM hANum)
{
    static AreaIter nFind;
    
    nFind = GetAreas().find(hANum);
    if(nFind == GetAreas().end())
        return NULL;
    
    return nFind->second;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetRoomIndex
//
//   Desc:: Retrieves a RoomIndex in the MUD by its virtual reference.
//
//  Input:: The VREF of the index you want.
//
// Output:: A Non-NULL pointer to the RoomIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* MUD::GetRoomIndex(VREF hVRef)
{
    VNUM        nAnum       = ::GetArea(hVRef);
    VNUM        nVnum       = ::GetVnum(hVRef);
    AreaIter    nAreaScan   = GetAreas().find(nAnum);
    
    if(nAreaScan == GetAreas().end())
        return NULL;
    
    Area*       nArea   = (*nAreaScan).second;
    IRoomIter   nRoom   = nArea->GetRooms().find(nVnum);
    
    return nRoom == nArea->GetRooms().end() ? NULL : (*nRoom).second;   
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetObjectIndex
//
//   Desc:: Retrieves an ObjectIndex in the MUD by its virtual reference.
//
//  Input:: The VREF of the index you want.
//
// Output:: A Non-NULL pointer to the ObjectIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ObjectIndex* MUD::GetObjectIndex(VREF hVRef)
{
    VNUM        nAnum       = ::GetArea(hVRef);
    VNUM        nVnum       = ::GetVnum(hVRef);
    AreaIter    nAreaScan   = GetAreas().find(nAnum);
    
    if(nAreaScan == GetAreas().end())
        return NULL;
    
    Area*       nArea   = (*nAreaScan).second;
    IObjectIter nObject = nArea->GetObjects().find(nVnum);
    
    return nObject == nArea->GetObjects().end() ? NULL : (*nObject).second; 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GetMobileIndex
//
//   Desc:: Retrieves a MobileIndex in the MUD by its virtual reference.
//
//  Input:: The VREF of the index you want.
//
// Output:: A Non-NULL pointer to the MobileIndex if it was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MobileIndex* MUD::GetMobileIndex(VREF hVRef)
{
    VNUM        nAnum       = ::GetArea(hVRef);
    VNUM        nVnum       = ::GetVnum(hVRef);
    AreaIter    nAreaScan   = GetAreas().find(nAnum);
    
    if(nAreaScan == GetAreas().end())
        return NULL;
    
    Area*       nArea   = (*nAreaScan).second;
    IMobileIter nMobile = nArea->GetMobiles().find(nVnum);
    
    return nMobile == nArea->GetMobiles().end() ? NULL : (*nMobile).second; 
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::Bug
//
//   Desc:: Reports a file-related bug in the code to the MUD and any immortals
//          that may be online at the time.
//
//  Input:: hBugStr: A short description of what went wrong.
//          hFile: The name of the file that is bugged.
//          hLine: The line where things went wrong.
//          hIsFatal: If true if quits the game.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::Bug(STRINGCW hBugStr, STRINGCW hFile, LONG hLine, bool hIsFatal)
{
    cout << hFile << "(" << hLine << ") " << hBugStr << endl;
    
    if(!hIsFatal)
        return;
        
    ExitApp(1);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::AddSession
//
//   Desc:: Adds a new OrangeSession to the game and processes it accordingly
//          with all the other sessions.
//
//  Input:: The session.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::AddSession(OrangeSession* hSession)
{
    ASSERT(hSession);
    ASSERT(!mSessions.Exists(hSession));

    mSessions.Add(hSession);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::KillSession
//
//   Desc:: Removes a session from the game. Doesn't remove it right away but
//          queues it up for removal before the next loop. This is what things
//          like Do_Quit use to end a players connection to the game.
//
//  Input:: The session.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::KillSession(OrangeSession* hSession)
{
    ASSERT(hSession);
    ASSERT(!mDyingSessions.Exists(hSession));
    
    mDyingSessions.Add(hSession);
    
    //Clear all their input
    while(!hSession->mInputLines.empty())
        hSession->mInputLines.pop();
}



void MUD::PlaceInGame(Player* hPlayer)
{
    ASSERT(hPlayer);

    mPeople.AddPerson(hPlayer);
    mPlayers.AddPerson(hPlayer);
}


void MUD::RemoveFromGame(Player* hPlayer)
{
    ASSERT(hPlayer);
    ASSERT(mPeople.Exists(hPlayer));
    ASSERT(mPlayers.Exists(hPlayer));

    mPeople.RemovePerson(hPlayer);
    mPlayers.RemovePerson(hPlayer);
}



void MUD::PlaceInGame(Mobile* hMobile)
{
    ASSERT(hMobile);

    mPeople.AddPerson(hMobile);
    mMobiles.AddPerson(hMobile);
}


void MUD::RemoveFromGame(Mobile* hMobile)
{
    ASSERT(hMobile);
    ASSERT(mPeople.Exists(hMobile));
    ASSERT(mMobiles.Exists(hMobile));

    if(hMobile->mPopulation)
        hMobile->mPopulation->Dirty();

    mPeople.RemovePerson(hMobile);
    mMobiles.RemovePerson(hMobile);
}




void MUD::PlaceInGame(Object* hObject)
{
    ASSERT(hObject);
    ++hObject->mIndex->mInPlay;

    mObjects.AddObject(hObject);    
}



void MUD::RemoveFromGame(Object* hObject)
{
    ASSERT(hObject);
    --hObject->mIndex->mInPlay;

    if(hObject->mPopulation)
        hObject->mPopulation->Dirty();

    mObjects.RemoveObject(hObject);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::TransformToImmortal
//
//   Desc:: Will transform this Player into an Immortal. This takes care of
//          handling special things. Immortal class is derived from Player so
//          we need a specialized function to turn the Player into an Immortal.
//          Their session remains in tact and they are reinserted into the game
//          if they were in it to begin with, same with the room.
//
//  Input:: The Player.
//
// Output:: The Immortalized Player.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Immortal* MUD::TransformToImmortal(Player* hPlayer)
{
    ASSERT(hPlayer != NULL);
    ASSERT(dynamic_cast<Immortal*>(hPlayer) == NULL);
    Immortal*       nImm            = new Immortal();
    bool            nWasInGame      = false;
    PersonHolder*   nWasInHolder    = NULL;


    //** Remove Player **//
    if(mPlayers.Exists(hPlayer))
    {
        nWasInGame      = true;
        nWasInHolder    = hPlayer->mInRoom;
        hPlayer->RemoveFromGame(false);
    }


    //** Implicit Copy Everything From Player Up **//
    *static_cast<Player*>(nImm) = *hPlayer;
    
    
    //** Transfer Objects **//
    nImm->ClearObjects();
    nImm->mBank.ClearObjects();
    nImm->mWearing.ClearObjects();
    nImm->AddObjects(hPlayer, true);
    nImm->mBank.AddObjects(&hPlayer->mBank, true);
    nImm->mWearing.AddObjects(&hPlayer->mWearing, true);
    
    
    //** Handle Special Copies **//
    nImm->isImm     = nImm;
    nImm->isPlayer  = nImm;
    hPlayer->mSession = NULL;
    if(nImm->mSession)
        nImm->mSession->mPerson = nImm;
    
    nImm->mParts.resize(hPlayer->mParts.size());
    for(ULONG i = 0; i < hPlayer->mParts.size(); ++i)
        nImm->mParts[i] = new Part(*hPlayer->mParts[i]);

    
    //** Cleanup And Restore **//   
    delete hPlayer;
    
    if(nWasInGame)
        nImm->PlaceInGame(nWasInHolder, false);
    
    return nImm;
}




Player* MUD::TransformToPlayer(Immortal* hPlayer)
{   
    ASSERT(!"Not-Written Yet");
    
    hPlayer->RemoveFromGame(false);
    
    
    
    return NULL;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ResolveUIDToName
//
//   Desc:: Returns the name of the player associated with the unique ID.
//
//  Input:: The unique ID.
//
// Output:: The player's name.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

const CHAR* MUD::ResolveUIDToName(UID hID)
{
    map<UID,STRINGCW>::iterator i = mUniqueIDMap.find(hID);
    
    if(i == mUniqueIDMap.end())
        return NULL;
    
    return i->second;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::ResolveNameToUID
//
//   Desc:: Resolves a player's name into the associated unique id.
//
//  Input:: The player's name.
//
// Output:: The unique ID.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UID MUD::ResolveNameToUID(const CHAR* hName)
{
    for(map<UID,STRINGCW>::iterator i = mUniqueIDMap.begin(); i != mUniqueIDMap.end(); ++i)
    if(i->second == hName)
        return i->first;

    return 0;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::AddUniqueID
//
//   Desc:: Adds an entry into the MUDs UID/Name map. The map is saved to disk
//          after every call to this function. You get an unused UID using
//          the GenerateUniqueID() call.
//
//  Input:: hID: The unused unique id.
//          hName: The name of the player to associate with the unique id.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::AddUniqueID(UID hID, const CHAR* hName)
{
    ASSERT(mUniqueIDMap.find(hID) == mUniqueIDMap.end());
    mUniqueIDMap[hID] = hName;
    SaveUniqueIDMap();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::RemoveUniqueID
//
//   Desc:: Removes an entry in the MUDs UID/Name map. The map is saved to disk
//          after every call to this function.
//
//  Input:: hID: The unique id.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::RemoveUniqueID(UID hID)
{
    map<UID,STRINGCW>::iterator i = mUniqueIDMap.find(hID);
    if(i == mUniqueIDMap.end())
    {
        ASSERT(!"Removing unique ID that doesn't even exist in the map!");
        return;
    }
    mUniqueIDMap.erase(i);
    SaveUniqueIDMap();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::GenerateUniqueID
//
//   Desc:: Generates a unique that is unused as of yet and can be used in a
//          call to AddUniqueID(). This is really only ever used at character
//          generation once the character is saved to disk for the first time.
//
// Output:: hID: The unique id.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UID MUD::GenerateUniqueID()
{
    UID nCounter = 1;
    while(mUniqueIDMap.find(nCounter) != mUniqueIDMap.end())
    {
        nCounter++;
        if(nCounter == ULONG_MAX)
            ASSERT(!"WTF? Out of Unique IDs!");
    }
    return nCounter;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::SaveUniqueIDMap
//
//   Desc:: Writes the UniqueID/PlayerName map out to disk.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::SaveUniqueIDMap()
{
    FilePath nPathToIDMap(kPath_IDMap);
    if(!FileManager::Get()->Exists(nPathToIDMap))
        FileManager::Get()->MakeDir(nPathToIDMap);
    nPathToIDMap.Add(kFile_IDMap);
    
    //Backup Old One
    if(FileManager::Get()->Exists(nPathToIDMap))
    {
        FilePath nPathToIDMapBackup(kPath_IDMap);
        STRING nBackupFileName = kFile_IDMap;
        nBackupFileName += ".bak";
        nPathToIDMapBackup.Add(nBackupFileName);
        FileManager::Get()->Move(nPathToIDMap, nPathToIDMapBackup);
    }
    
    //Save New One
    APtr<FileLink> nIDMapFile = FileManager::Get()->OpenWrite(nPathToIDMap);
    if(nIDMapFile == NULL)
    {
        ASSERT(!"Couldnt save UniqueIDMap");
        return;
    }
    
    XMLWriter nIDMapXMLFile(nIDMapFile);
    nIDMapXMLFile.BeginFile();
    nIDMapXMLFile.BeginSuperTag("IDMap");
    for(map<UID,STRINGCW>::iterator i = mUniqueIDMap.begin(); i != mUniqueIDMap.end(); ++i)
    {
        XMLAttributes nEntryAttrs;
        nEntryAttrs.AddULong("UID", i->first);
        nIDMapXMLFile.WriteString("Entry", i->second, i->second.Length(), &nEntryAttrs);
    }
    nIDMapXMLFile.EndTag();
    nIDMapXMLFile.EndFile();
    nIDMapFile->Close();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::AddSocial
//
//   Desc:: Adds a Social into the MUD. The social will be available in-play.
//
//  Input:: The social.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::AddSocial(Social* hSocial)
{
    ASSERT(!hSocial->mName.Empty());
    CHAR nIndex = LOWER(hSocial->mName[0])-'a';
    mSocials[nIndex].Add(hSocial);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::RemoveSocial
//
//   Desc:: Removes a Social from the MUD.
//
//  Input:: The social.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::RemoveSocial(Social* hSocial)
{
    USHORT nHashSize = 'z'-'a';
    for(USHORT i = 0; i < nHashSize; ++i)
    {
        SocialList::iterator nFound = std::find(mSocials[i].begin(), mSocials[i].end(), hSocial);
        if(nFound != mSocials[i].end())
        {
            mSocials[i].erase(nFound);
            return;
        }
    }
    
    ASSERT(!"Social Not Found");
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::FindSocial
//
//   Desc:: Searches for a Social inside the mud. This method uses a hash table
//          with an entry for each letter from A to Z; a faster implementation
//          is probably possible.
//
//  Input:: Partial/full name of a possible Social
//
// Output:: Non-NULL Social if a match was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Social* MUD::FindSocial(const STRINGCW& hName)
{
    CHAR nIndex = LOWER(hName[0]);
    if(nIndex < 'a' || nIndex > 'z')
        return NULL;
    
    nIndex -= 'a';
    SocialList& nSearch = mSocials[nIndex];
    for(SocialList::iterator i = nSearch.begin(); i != nSearch.end(); ++i)
    if(!StrNCmp((*i)->mName, hName, hName.Length()))
        return *i;
        
    return NULL;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::FindHelp
//
//   Desc:: Searches for a Help inside the mud.
//
//  Input:: Partial/full keyword of a possible Help
//
// Output:: Non-NULL Help if a match was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Help* MUD::FindHelp(const STRINGCW& hName)
{
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::RunScript
//
//   Desc:: Runs an OrangeScript in the game given the Room, Character, Mobile,
//          Object, and Victim - which all differ depending on how it was
//          triggered. A MUD::RunningScript object is added to the list, which
//          gets processed through UpdateScripts().
//
//  Input:: hScript: the OrangeScript to run.
//          hRoom: running in this RoomIndex.
//          hCh: trigger by this Person.
//          hMobile: is a mobile script running on this Mobile.
//          hObject: is an object script running on this Object.
//          hVictim: victim Person in a weapon attack script.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::RunScript
            (OrangeScript*  hScript,
             RoomIndex*     hRoom,
             Person*        hCh,
             Mobile*        hMobile,
             Object*        hObject,
             Person*        hVictim)
{
    if(hScript->mBinary == NULL)
    {
        if(!ScriptedThing::CompileTo(hScript, hCh->IsImm()?hCh:NULL))
            return;
    }
    
    MUD::RunningScript* nRun = new MUD::RunningScript(hScript);
    
    nRun->mRoom = hRoom;
    nRun->mCh = hCh;
    nRun->mMobile = hMobile;
    nRun->mObject = hObject;
    nRun->mVictim = hVictim;
    
    mScripts.AddBack(nRun);
    nRun->Process();
}



MUD::RunningScript::RunningScript(OrangeScript* hScript)
    : mExecutor(hScript)
    , mScript(hScript)
{
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::RunningScript::Process
//
//   Desc:: Called every script tick to continue execution of the script.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void MUD::RunningScript::Process()
{
    if(!mExecutor.Done())
    {
        MUD::Get()->mRunningNow = this;
    
        //Swap Room/Ch/Mob/Obj/Vict
        OrangeScriptBinary* nBinary = static_cast<OrangeScriptBinary*>(mScript->mBinary.Ptr());
        nBinary->mRoom->mValue->OperatorEqual(mRoom);
        nBinary->mCh->mValue->OperatorEqual(mCh);
        nBinary->mMob->mValue->OperatorEqual(mMobile);
        nBinary->mObj->mValue->OperatorEqual(mObject);
        nBinary->mVictim->mValue->OperatorEqual(mVictim);
        
        //Execute
        mExecutor.DecrementWait();
        mExecutor.Execute();

        //UnSwap Room/Ch/Mob/Obj/Vict
        nBinary->mRoom->mValue->OperatorEqual((void*)NULL);
        nBinary->mCh->mValue->OperatorEqual((void*)NULL);
        nBinary->mMob->mValue->OperatorEqual((void*)NULL);
        nBinary->mObj->mValue->OperatorEqual((void*)NULL);
        nBinary->mVictim->mValue->OperatorEqual((void*)NULL);

        MUD::Get()->mRunningNow = NULL;
    }
}


bool MUD::RunningScript::Done()
{
    return mExecutor.Done();
}


