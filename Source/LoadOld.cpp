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
#include "StringUtils.h"
#include "Tables.h"
#include "LoadOld.h"
#include "MUDFile.h"
#include "MUD.h"
#include "Player.h"
#include "Area.h"
#include "Mobile.h"
#include "MobileIndex.h"
#include "Attack.h"
#include "Part.h"
#include "Skin.h"
#include "Spot.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "RoomIndex.h"
#include "Ambience.h"
#include "ObjectPopulation.h"
#include "MobilePopulation.h"
#include "ExitPopulation.h"
#include "HelpFile.h"
#include "Help.h"
#include "SocialFile.h"
#include "Social.h"
#include "Conversion.h"
#include "OrangeSession.h"
#include "Immortal.h"
#include "OrangeScript.h"
#include "OrangeScriptVariable.h"
#include "ScriptLibrary.h"
#include "XMLFile.h"

//## Defines ##//
#define MATCH(str, x, y)            if(!strcmp(str, varName)){(x)=(y);return true;}
#define MATCHCALL(str, callfunc)    if(!strcmp(str, varName)){callfunc;return true;}






template <class Wants>
Wants* LoadObject(MUDFile& hFp, Wants *&wNew)
{
    if(!hFp.OK())
        return (wNew = NULL);

    return LoadObject(hFp, wNew, hFp.ReadWord());
}

template <class Wants>
Wants* LoadObject(MUDFile& hFp, Wants *&wNew, CHAR* word)
{
    LoadOld *loadOn;
    
    if(!hFp.OK())
        return (wNew = NULL);
    
    if(!LoadOld::LoadGenericObject(hFp, loadOn, word))
    {
        wNew = NULL;
    }else{
        if(!(wNew = dynamic_cast<Wants*>(loadOn)))
            delete loadOn;
    }
    
    //If the file isn't ok, the object can't be either
    if(!hFp.OK())
    {
        delete wNew;
        wNew = NULL;
    }
    
    return wNew;
}


template <class Wants>
bool LinkedListLoop(MUDFile& hFp, Wants *&wNew)
{
    CHAR* word = hFp.ReadWord();
    
    wNew = NULL;
    if(!hFp.OK())
        return false;
        
    if(!word || word[0] == EOF || !strcmp(word, SAVE_EOF))
    {
        hFp.SetNotOK();
        MFBug(hFp, "LinkListLoop: Reached premature EOF");
        return false;
    }
    
    if(!strcmp(word, END_LLIST))
    {
        return false;
    }
    
    if(!LoadObject(hFp, wNew, word))
    {
        hFp.SetNotOK();
        MFBug(hFp, "LinkListLoop: Failed to load an object");
        return false;
    }
    
    return true;
}


bool LoadOld::LoadAnyFile(MUDFile& hFp)
{
    LoadOld *anyObject;
    bool fileDone = false;
    
    do
    {
        fileDone = LoadGenericObject(hFp, anyObject);
    }
    while(anyObject && hFp.OK() && !fileDone);
    
    return hFp.OK();
}




bool LoadOld::LoadGenericObject(MUDFile& hFp, LoadOld *&loadOn)
{   
    return LoadGenericObject(hFp, loadOn, hFp.ReadWord());
}




bool LoadOld::LoadGenericObject(MUDFile& hFp, LoadOld *&loadOn, CHAR* word)
{
    CHAR type[kMaxInputLen];
    LONG vnum = 0;
    
    if(!hFp.OK())
        return false;
    
    loadOn = NULL;
    
    if(!strcmp(word, SAVE_EOF))
    {
        //Not an error, just done with the file
        return false;
    }
    
    if(!strcmp(word, BEGIN_OBJECT))
    {
        strcpy(type, hFp.ReadWord());
    }
    else
    if(!strcmp(word, BEGIN_VNUM_OBJECT))
    {
        strcpy(type, hFp.ReadWord());
        vnum = atoi(hFp.ReadWord());
    }
    else{
        hFp.SetNotOK();
        MFBug(hFp, "LoadOld::LoadGenericObject: No BEGIN_OBJECT found, %s", word);
        return false;
    }
    

    //** Create Correct Type of SaveLoad Object **//
         if(!strcmp(type, IO_PLAYER  )) loadOn = new Player();
    else if(!strcmp(type, IO_AREAFILE)) loadOn = new Area();
    else if(!strcmp(type, IO_ROOMIDX )) loadOn = new RoomIndex();
    else if(!strcmp(type, IO_OBJIDX  )) loadOn = new ObjectIndex();
    else if(!strcmp(type, IO_OBJECT  )) loadOn = new Object(ObjectIndex::Get(CvtToVRef(vnum)));
    else if(!strcmp(type, IO_EDESC   )) loadOn = new Spot();
    else if(!strcmp(type, IO_SCRIPT  )) loadOn = new OrangeScript();
    else if(!strcmp(type, IO_VAR     )) loadOn = new OrangeScriptVariable(gDTDef_STRING);
    else if(!strcmp(type, IO_EXIT    )) loadOn = new Exit(NULL);
    else if(!strcmp(type, IO_AMBIENCE)) loadOn = new Ambience();
    else if(!strcmp(type, IO_MOBIDX  )) loadOn = new MobileIndex();
    else if(!strcmp(type, IO_ATTACK  )) loadOn = new Attack();
    else if(!strcmp(type, IO_PART    )) loadOn = new Part();
    else if(!strcmp(type, IO_SKIN    )) loadOn = new Skin();
    else if(!strcmp(type, IO_OREPOP  )) loadOn = new ObjectPopulation(true);
    else if(!strcmp(type, IO_OLR     )) loadOn = new ObjectPopulation(false);
    else if(!strcmp(type, IO_MREPOP  )) loadOn = new MobilePopulation();
    else if(!strcmp(type, IO_EREPOP  )) loadOn = new ExitPopulation();
    else if(!strcmp(type, IO_SOCFILE )) loadOn = new SocialFile();
    else if(!strcmp(type, IO_SOCIAL  )) loadOn = new Social();
    else if(!strcmp(type, IO_HELPFILE)) loadOn = new HelpFile();
    else if(!strcmp(type, IO_HELP    )) loadOn = new Help();
/*
    else if(!strcmp(type, IO_MOBILE  )) loadOn = new Mobile(MobIndex::Create(vnum));
    else if(!strcmp(type, IO_CURRENCY)) loadOn = Object::Create(ObjIndex::Get(vnum));
    else if(!strcmp(type, IO_SHOP    )) loadOn = new Shop();
*/
    else
    {
        hFp.SetNotOK();
        MFBug(hFp, "LoadOld::LoadGenericObject: Unknown Object Type, %s", type);
        return false;
    }
    
    loadOn->BeforeLoad();
    
    //** Generic Loading **//
    while(true)
    {       
        if(!hFp.OK())
            return false;
        
        word = hFp.ReadWord();
        
        if(!word || word[0] == EOF || !strcmp(word, SAVE_EOF))
        {
            hFp.SetNotOK();
            MFBug(hFp, "LoadOld::LoadGenericObject: Reached premature EOF");
            return false;
        }
        
        if(!strcmp(word, END_OBJECT))
            break;
        
        
        //** Start Of A Section, Read It **//
        if(!strcmp(word, BEGIN_SECTION))
        {
            loadOn->LoadSection(hFp);
        }else
        //** Start Of A Linked List, Read It **//
        if(!strcmp(word, BEGIN_LLIST))
        {
            loadOn->LoadLinkedList(hFp);
        }
        else{
            delete loadOn;
            loadOn = NULL;
            hFp.SetNotOK();
            MFBug(hFp, "LoadOld::LoadGenericObject: Unknown file marker, %s", word);
            return false;
        }
    }

    loadOn->OnLoad();
    
    return true;
}



bool LoadOld::LoadSection(MUDFile& hFp)
{
    CHAR* word;
    
    if(!hFp.OK())
        return false;
    
    word = hFp.ReadWord();

    if(!strcmp(word, SECT_MAIN))
    {
        return LoadSectMain(hFp);
    }
    
    hFp.SetNotOK();
    MFBug(hFp, "LoadOld::LoadSection: Unknown File Section, %s", word);
    return false;
}

bool LoadOld::LoadLinkedList(MUDFile& hFp)
{
    CHAR* word;
    
    if(!hFp.OK())
        return false;
        
    word = hFp.ReadWord();

    if(MatchLists(hFp, word))
        return true;
    
    //** Didn't Match A List, Read Till Out **//
    word = hFp.ReadWord();
        
    while(strcmp(word, END_LLIST))
    {
        hFp.ReadEOL();
        word = hFp.ReadWord();
    }
    
    return false;
}


bool LoadOld::LoadSectMain(MUDFile& hFp)
{
    CHAR* word;
    
    while(true)
    {
        if(!hFp.OK())
            return false;
        
        word = hFp.ReadWord();
    
        if(!word || word[0] == EOF || !strcmp(word, SAVE_EOF))
        {
            hFp.SetNotOK();
            MFBug(hFp, "LoadOld::LoadSectMain: Reached premature EOF");
            return false;
        }
        
        if(!strcmp(word, END_SECTION))
            break;
        
        if(!MatchValues(hFp, word))
            hFp.ReadEOL();
    }
    
    return true;
}

void LoadOld::BeforeLoad()
{
    //Place holder, so you dont have to define this for objects
    //if you dont want to.
}

void LoadOld::OnLoad()
{
    //Place holder, so you dont have to define this for objects
    //if you dont want to.
}



#if 0
//#######################################################################################
#pragma mark -
#endif





void MUD::LoadAllAreasOld()
{
    FilePath    nAreaListPath(kPath_Old_Area, kFile_Old_AreaList);
    FileLink*   nAreaListLink;

    cout << "Loading Areas." << endl;
    if(!FileManager::Get()->Exists(nAreaListPath))
        return;
    
    if(!(nAreaListLink = FileManager::Get()->OpenRead(nAreaListPath)))
        return;
    
    FilePath nLoadListPath(kPath_Resources);    //Make a new Resources/LoadList.xml
    FileManager::Get()->MakeDir(nLoadListPath);
    nLoadListPath.Add(kFile_LoadList);
    List<STRING> nAreaFileNames;
    List<STRING> nHelpFileNames;
    List<STRING> nSocialFileNames;

    
    MUD::Get()->StartLoadingProgress();
    MUDFile nAreaFile(nAreaListLink);

    while(true)
    {
        STRINGCW nSomeArea = nAreaFile.ReadStringEOL();
        
        if(nSomeArea.Empty())
            MUDFatalBug("Area list reached premature EOF");
        
        if(nSomeArea[0] == '$')
            break;
        
        if(nSomeArea[0] == '/')
            continue;
        
        if(nSomeArea[0] == '\0')
            MUDFatalBug("Area list not correctly terminated with $ line");
        
        LoadAreaOld(nSomeArea);
        if(StrSuffix(nSomeArea, ".are"))
        {
            nAreaFileNames.push_back(STRING(nSomeArea));
        }
        else if(StrSuffix(nSomeArea, ".hlp"))
        {
            nHelpFileNames.push_back(STRING(nSomeArea));
        }
        else if(StrSuffix(nSomeArea, ".scl"))
        {
            nSocialFileNames.push_back(STRING(nSomeArea));
        }
        else
            ASSERT(false);
    
        MUD::Get()->UpdateLoadingProgress();
    }

    MUD::Get()->FinishLoadingProgress();

    /////////////////////////////////////////////////////////////////////
    FileLink* nLoadListLink(FileManager::Get()->OpenWrite(nLoadListPath));
    XMLWriter nLoadListXML(nLoadListLink);

    nLoadListXML.BeginFile();
    nLoadListXML.BeginSuperTag("LoadList");
    {
        nLoadListXML.BeginSuperTag("AreaFiles");
        {
            for(List<STRING>::iterator i = nAreaFileNames.begin(); i != nAreaFileNames.end(); ++i)
                nLoadListXML.WriteString("File", (*i).c_str());
        }
        nLoadListXML.EndTag();
        
        nLoadListXML.BeginSuperTag("HelpFiles");
        {
            for(List<STRING>::iterator i = nHelpFileNames.begin(); i != nHelpFileNames.end(); ++i)
                nLoadListXML.WriteString("File", (*i).c_str());
        }
        nLoadListXML.EndTag();
        
        nLoadListXML.BeginSuperTag("SocialFiles");
        {
            for(List<STRING>::iterator i = nSocialFileNames.begin(); i != nSocialFileNames.end(); ++i)
                nLoadListXML.WriteString("File", (*i).c_str());
        }
        nLoadListXML.EndTag();
    }
    nLoadListXML.EndTag();
    nLoadListXML.EndFile();
    nLoadListLink->Close();
    delete nLoadListLink;
    /////////////////////////////////////////////////////////////////////
    
    nAreaFile.mLink->Close();
    cout << "Area Database Loaded." << endl;
}



CHAR MUD::LoadPlayerOld(STRINGCW& hName, Player*& hPlayer)
{
    FilePath    nPlayerPath(kPath_Old_Player); 
    FileLink*   nFileLink;
    STRING      nCharDir;

    
    //** Check First Letter Path **//
    nCharDir += UPPER(hName[0]);
    nPlayerPath.Add(nCharDir);
    if(!FileManager::Get()->Exists(nPlayerPath))
        return kLoadPlayer_NonExistant;
    
    //** Check Character Name Path **//
    nPlayerPath.Add(*(STRINGCW(hName).Capitalize()));
    if(!FileManager::Get()->Exists(nPlayerPath))
        return kLoadPlayer_NonExistant;

    //** Check Character Name Path PFile **//
    nPlayerPath.Add("pfile");
    if(!FileManager::Get()->Exists(nPlayerPath))
        return kLoadPlayer_ErrWithFile;
    
    if(!(nFileLink = FileManager::Get()->OpenRead(nPlayerPath)))
        return kLoadPlayer_ErrWithFile;
    
    
    MUDFile nMudFile(nFileLink);
    
    if(!LoadObject(nMudFile, hPlayer))  //Read a Player* Object
        return kLoadPlayer_ErrWithFile;
    
    //Did we already give them a UID? If not make them one
    hPlayer->mUID = MUD::Get()->ResolveNameToUID(hPlayer->mName);
    if(hPlayer->mUID == 0)
    {
        hPlayer->mUID = MUD::Get()->GenerateUniqueID();
        MUD::Get()->AddUniqueID(hPlayer->mUID, hPlayer->mName);
    }
    hPlayer->mTrust = UMAX(hPlayer->mTrust, hPlayer->mLevel);
    if(hPlayer->mTrust >= kLevelImmortal)
        hPlayer = MUD::Get()->TransformToImmortal(hPlayer);
    
    
    //** Close Player File **//
    nMudFile.mLink->Close();    
    return kLoadPlayer_Success;
}




CHAR MUD::LoadAreaOld(STRINGCW& hPath)
{
    FilePath    nFilePath(kPath_Old_Area); 
    FileLink*   nFileLink;
    
    nFilePath.Add(*hPath);
    if(!FileManager::Get()->Exists(nFilePath))
        return 0;
    
    if(!(nFileLink = FileManager::Get()->OpenRead(nFilePath)))
        return 0;
    
    
    MUDFile nMudFile(nFileLink);

    if(!LoadOld::LoadAnyFile(nMudFile))
    {
        nMudFile.mLink->Close();    
        cout << "[    ] " << hPath << " failed." << endl;
        return -1;
    }
    
    nMudFile.mLink->Close();
    cout << "[$$$$] Loading of " << hPath << " successful." << endl;
    return 1;
}



#if 0
//#######################################################################################
#pragma mark -
#endif



bool Person::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NAME"        , mName                 , hFp.ReadString());
    MATCH("KEYWORDS"    , mKeywords             , hFp.ReadString());
    MATCH("SDESC"       , mShortDesc            , hFp.ReadString());
    MATCH("PSDESC"      , mPShortDesc           , hFp.ReadString());
    MATCH("DESC"        , mDescription          , hFp.ReadString());
    MATCH("LEVEL"       , mLevel                , hFp.ReadNumber());

    MATCH("ALIGN"       , mAlignment            , hFp.ReadNumber());
    MATCH("HIT"         , mHits                 , hFp.ReadNumber());
    MATCH("HITM"        , mHitsMax              , hFp.ReadNumber());
    MATCH("ENERGY"      , mEnergy               , hFp.ReadNumber());
    MATCH("ENERGYM"     , mEnergyMax            , hFp.ReadNumber());
    MATCH("MOVE"        , mMoves                , hFp.ReadNumber());
    MATCH("MOVEM"       , mMovesMax             , hFp.ReadNumber());
    MATCH("LHITT"       , mLevelHitT            , hFp.ReadNumber());
    MATCH("LENERGYT"    , mLevelEnergyT         , hFp.ReadNumber());
    MATCH("LMOVET"      , mLevelMoveT           , hFp.ReadNumber());
    MATCH("EXPTNL"      , mExpTNL               , hFp.ReadNumber());
    MATCH("POS"         , mPosition             , hFp.ReadNumber());
    MATCH("PSTR"        , mStat[kStatStr]       , hFp.ReadNumber());
    MATCH("PINT"        , mStat[kStatInt]       , hFp.ReadNumber());
    MATCH("PWIS"        , mStat[kStatWis]       , hFp.ReadNumber());
    MATCH("PDEX"        , mStat[kStatDex]       , hFp.ReadNumber());
    MATCH("PCON"        , mStat[kStatCon]       , hFp.ReadNumber());
    MATCH("WFLEE"       , mWimpy[kWimpyFlee]    , hFp.ReadNumber());
    MATCH("WBLINK"      , mWimpy[kWimpyBlink]   , hFp.ReadNumber());
    MATCH("WRECALL"     , mWimpy[kWimpyRecall]  , hFp.ReadNumber());
    MATCH("WPRAY"       , mWimpy[kWimpyPray]    , hFp.ReadNumber());
    
    if(!strcmp(varName, "ROOM"))
    {
        VNUM roomVnum = hFp.ReadNumber();
        mWasInRoom = RoomIndex::Get(roomVnum);
        return true;
    }
                
    return false;
}




bool Person::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_OBJ_INVENTORY  , Object::LoadInto(hFp, this));
    return false;
}





#if 0
//#######################################################################################
#pragma mark -
#endif




void Player::OnLoad()
{
    /*
    LONG i, colorBits;
    
    //Build ANSI Codes From Preferences
    for(i = 0; i < MAX_COLOR_PREF; ++i)
    {
        if(!(colorBits = cPref[i]))
            continue;
        
        free_string(cPrefCode[i]);
        cPrefCode[i] = str_dup(BuildAnsiCode(colorBits));       
    }
    */
}


bool Player::MatchValues(MUDFile& hFp, CHAR* varName)
{
    //Person
    MATCH("SEX"         , mSex              , hFp.ReadNumber());
    MATCH("CLASS"       , mClass            , hFp.ReadNumber());
    MATCH("RACE"        , mRace             , hFp.ReadNumber());
//  MATCH("SCROLL"      , mLines            , hFp.ReadNumber());
    //Player
    MATCH("HR"          , mHitRoll          , hFp.ReadNumber());
    MATCH("DR"          , mDamRoll          , hFp.ReadNumber());
//  MATCH("PASS"        , mPassword         , hFp.ReadString());
    MATCH("PROMPT"      , mPrompt           , hFp.ReadString());
    MATCH("TITLE"       , mTitle            , hFp.ReadString());
    MATCH("TITLEAFK"    , mTitleAFK         , hFp.ReadString());
    MATCH("EMAIL"       , mEmail            , hFp.ReadString());
    MATCH("HOMEPAGE"    , mHomepage         , hFp.ReadString());
    MATCH("TLLOGIN"     , mLastLogin        , hFp.ReadULong());
    MATCH("TPLAYED"     , mTimePlayed       , hFp.ReadULong());
    MATCH("TRUST"       , mTrust            , hFp.ReadNumber());
//  MATCH("?????"       , mInvisLevel       , hFp.ReadNumber());
    MATCH("INFO"        , mFlagsInfo        , hFp.ReadFlags());
    MATCH("OPTIONS"     , mFlagsOptions     , hFp.ReadFlags());
    MATCH("CDRUNK"      , mCondition[kCondDrunk]    , hFp.ReadNumber());
    MATCH("CFULL"       , mCondition[kCondFull]     , hFp.ReadNumber());
    MATCH("CHUNGER"     , mCondition[kCondHunger]   , hFp.ReadNumber());
    MATCH("CTHIRST"     , mCondition[kCondThirst]   , hFp.ReadNumber());
    MATCH("CHIGH"       , mCondition[kCondHigh]     , hFp.ReadNumber());

    if(!strcmp(varName, "PASS"))
    {
        STRING n64Pass, nPass = hFp.ReadString();
        
        Base64Encode(nPass.data(), nPass.length(), n64Pass);
        mPassword = n64Pass.c_str();
        return true;
    }   
    if(!strcmp(varName, "CP"))
    {
        SHORT hIndex        = hFp.ReadNumber();
        mColorPref[hIndex]  = hFp.ReadNumber();
        return true;
    }

    
    //Check SuperClass
    return this->Person::MatchValues(hFp, varName);
}


bool Player::MatchLists(MUDFile& hFp, CHAR* varName)
{
//  MATCH(IO_OBJ_BANK   , oBank , Object::LoadIntoObjs(fp, oBank, this, true));
    MATCHCALL(IO_OBJ_BANK   , Object::LoadInto(hFp, &mBank));
    
    //Check SuperClass
    return this->Person::MatchLists(hFp, varName);
}

/*

bool Mobile::MatchValues(MUDFile& hFp, CHAR* varName)
{
    //Check SuperClass
    return this->Person::MatchValues(hFp, varName);
}

bool Mobile::MatchLists(MUDFile& hFp, CHAR* varName)
{
    //Check SuperClass
    return this->Person::MatchLists(hFp, varName);
}


void Mobile::OnLoad()
{
}


*/

#if 0
//#######################################################################################
#pragma mark -
#endif


bool Object::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("WR"      , mWearLoc      , hFp.ReadNumber());
//  MATCH("TM"      , Timer()       , hFp.ReadNumber());
    MATCH("NAME"    , xName()       , hFp.ReadString());
    MATCH("PNAME"   , xPName()      , hFp.ReadString());
    MATCH("KEY"     , xKeywords()   , hFp.ReadString());
    MATCH("SDESC"   , xShortDesc()  , hFp.ReadString());
    MATCH("PSDESC"  , xPShortDesc() , hFp.ReadString());
//  MATCH("COND"    , xCond()       , hFp.ReadNumber());
//  MATCH("V0"      , xValue(0)     , hFp.ReadNumber());
//  MATCH("V1"      , xValue(1)     , hFp.ReadNumber());
//  MATCH("V2"      , xValue(2)     , hFp.ReadNumber());
//  MATCH("V3"      , xValue(3)     , hFp.ReadNumber());
//  MATCH("V4"      , xValue(4)     , hFp.ReadNumber());

    return false;
}



bool Object::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_OBJ_CONTENTS, Object::LoadInto(hFp, this));
    
    return false;
}


void Object::LoadInto(MUDFile& hFp, ObjectHolder* hInto)
{
    Object *nNewObject;

    while(LinkedListLoop(hFp, nNewObject))
    {   
        hInto->AddObject(nNewObject, true);

        
        //** Into Person's Worn Items **//
        if(nNewObject->mWearLoc != kLocNone)
        {
            Person* nToPerson = dynamic_cast<Person*>(hInto);
            if(nToPerson == NULL)
                nNewObject->mWearLoc = kLocNone;
            else
            {
                Object* nAlreadyEqd = nToPerson->GetEquipAt(nNewObject->mWearLoc);
                if(nAlreadyEqd)
                    nToPerson->EquipObject(nAlreadyEqd, kLocNone);
                nToPerson->EquipObject(nNewObject, nNewObject->mWearLoc);
            }
        }
    }

}


#if 0
//#######################################################################################
#pragma mark -
#endif


bool Area::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("FNAME"   , mFileName     , hFp.ReadString());
    MATCH("NAME"    , mName         , hFp.ReadString());
    MATCH("CREDITS" , mCredits      , hFp.ReadString());
    MATCH("LOW"     , mLowRange     , hFp.ReadNumber());
    MATCH("HIGH"    , mHighRange    , hFp.ReadNumber());
    MATCH("MINVNUM" , mMinOldVnum   , hFp.ReadNumber());
    MATCH("MAXVNUM" , mMaxOldVnum   , hFp.ReadNumber());
    MATCH("VERSION" , mVersion      , hFp.ReadNumber());
    MATCH("RESPAWN" , mRespawnRate  , hFp.ReadNumber());
                
    return false;
}


bool Area::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_ROOM_IDXS      , LoadAreaRoomsOld(hFp));
    MATCHCALL(IO_OBJ_IDXS       , LoadAreaObjectsOld(hFp));
    MATCHCALL(IO_MOB_IDXS       , LoadAreaMobilesOld(hFp));
    MATCHCALL(IO_AREA_MOB_AMB   , Ambience::LoadListInto(hFp, mMobAmbience));
    MATCHCALL(IO_AREA_ROOM_AMB  , Ambience::LoadListInto(hFp, mRoomAmbience));
    MATCHCALL(IO_AREA_REPOPS    , Population::LoadListInto(hFp, mPopulations));
    
    return false;
}

void Area::LoadAreaRoomsOld(MUDFile& hFp)
{
    RoomIndex*  nNewRoom;
    
    while(LinkedListLoop(hFp, nNewRoom))
    {
        mRoomIndexes[nNewRoom->mOldVnum-mMinOldVnum] = nNewRoom;
        MUD::Get()->mOldRoomIndexes[nNewRoom->mOldVnum] = nNewRoom;
        nNewRoom->mInArea = this;
    }
}


void Area::LoadAreaObjectsOld(MUDFile& hFp)
{
    ObjectIndex*    nNewObjectIndex;
    
    while(LinkedListLoop(hFp, nNewObjectIndex))
    {
        mObjectIndexes[nNewObjectIndex->mOldVnum-mMinOldVnum] = nNewObjectIndex;
        MUD::Get()->mOldObjectIndexes[nNewObjectIndex->mOldVnum] = nNewObjectIndex;
    }
}


void Area::LoadAreaMobilesOld(MUDFile& hFp)
{
    MobileIndex*    nNewMobileIndex;
    
    while(LinkedListLoop(hFp, nNewMobileIndex))
    {
        mMobileIndexes[nNewMobileIndex->mOldVnum-mMinOldVnum] = nNewMobileIndex;
        MUD::Get()->mOldMobileIndexes[nNewMobileIndex->mOldVnum] = nNewMobileIndex;
    }
}




void Area::OnLoad()
{
    MUD::Get()->mOldAreas[mMinOldVnum] = this;
}






#if 0
//#######################################################################################
#pragma mark -
#endif



bool RoomIndex::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("VNUM"        , mOldVnum      , hFp.ReadNumber());
    MATCH("NAME"        , mName         , hFp.ReadString());
    MATCH("DESC"        , mDescription  , hFp.ReadString());
    MATCH("RFLAGS"      , mFlags        , hFp.ReadFlags());
    MATCH("SECTOR"      , mTerrain      , hFp.ReadNumber());
    MATCH("CAPACITY"    , mCapacity     , hFp.ReadNumber());
    MATCH("FREQ"        , mAmbFreq      , hFp.ReadNumber());
    MATCH("BHP"         , mHitBonus     , hFp.ReadNumber());
    MATCH("BEN"         , mEnergyBonus  , hFp.ReadNumber());
    MATCH("BMV"         , mMoveBonus    , hFp.ReadNumber());
                
    return false;
}


bool RoomIndex::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_ROOM_EXITS     , Exit::LoadRoomExits(hFp, this));
    MATCHCALL(IO_ROOM_EDESCS    , Spot::LoadListInto(hFp, this));
    MATCHCALL(IO_ROOM_SPOTS     , Spot::LoadListInto(hFp, this));
    MATCHCALL(IO_ROOM_SCRIPTS   , OrangeScript::LoadListInto(hFp, this));

    return false;
}



#if 0
//#######################################################################################
#pragma mark -
#endif


bool Exit::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("DIR"         , mDirection        , hFp.ReadNumber());    
    MATCH("DESC"        , mDescription      , hFp.ReadString());
    MATCH("NAME"        , mName             , hFp.ReadString());
    MATCH("KEYWORDS"    , mKeywords         , hFp.ReadString());
    MATCH("FLAGS"       , mFlags            , hFp.ReadFlags());
    MATCH("KEY"         , mOldKey           , hFp.ReadNumber());
    MATCH("TOROOM"      , mToOldVnum        , hFp.ReadNumber());
    return false;
}


bool Exit::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void Exit::LoadRoomExits(MUDFile& hFp, RoomIndex* hIntoRoom)
{
    Exit *nNewExit;
    
    while(LinkedListLoop(hFp, nNewExit))
    {   
        nNewExit->mInRoom = hIntoRoom;
        if(nNewExit->mDirection < 0 || nNewExit->mDirection > kDirMax)
            ASSERT(!"Bad exit dir");
        
        hIntoRoom->mExits[nNewExit->mDirection] = nNewExit;
    }
}



#if 0
//#######################################################################################
#pragma mark -
#endif



bool Spot::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NAME"        , mName         , hFp.ReadString());
    MATCH("KEYWORDS"    , mKeywords     , hFp.ReadString());
    MATCH("DESC"        , mDescription  , hFp.ReadString());
    MATCH("FLAGS"       , mFlags        , hFp.ReadFlags());
                
    return false;
}

bool Spot::MatchLists(MUDFile& hFp, CHAR* varName)
{
    cout << "Ah:" << varName << endl;
    return false;
}



void Spot::LoadListInto(MUDFile& hFp, SpottedThing* hInto)
{
    Spot* nNewSpot;
    
    while(LinkedListLoop(hFp, nNewSpot))
        hInto->mSpots.Add(nNewSpot);
}




#if 0
//#######################################################################################
#pragma mark -
#endif




bool OrangeScript::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("COMMANDS"    , mCommands , hFp.ReadString());
    MATCH("KEYWORDS"    , mKeywords , hFp.ReadString());
    MATCH("TYPE"        , mType     , hFp.ReadNumber());
    MATCH("TRIGGER"     , mTrigger  , hFp.ReadNumber());
    MATCH("ID"          , mID       , hFp.ReadNumber());
    MATCH("FLAGS"       , mFlags    , hFp.ReadFlags());
    MATCH("SOURCE"      , mSource   , hFp.ReadString());
                
    return false;
}


bool OrangeScript::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_SCRIPT_VARS, OrangeScriptVariable::LoadListInto(hFp, mTextVariables));
    
    return false;
}


void OrangeScript::LoadListInto(MUDFile& hFp, ScriptedThing* hInto)
{
    OrangeScript* nNewScript;
    
    while(LinkedListLoop(hFp, nNewScript))
    {   
        ASSERT(hInto->GetScript(nNewScript->mID) == NULL);
        hInto->mScriptMap[nNewScript->mID] = nNewScript;
        
        //** Set Variables **//
        nNewScript->mOnThis = hInto;
    }
}



#if 0
//#######################################################################################
#pragma mark -
#endif



bool OrangeScriptVariable::MatchValues(MUDFile& hFp, CHAR* varName)
{
    STRING& nStringVal = static_cast<DataTypeSTRING*>(mValue.Ptr())->mString;
    
    MATCH("NAME"    , mName         , hFp.ReadString());
    MATCH("TEXT"    , nStringVal    , hFp.ReadString());
    MATCH("TYPE"    , mType         , hFp.ReadNumber());
                
    return false;
}


bool OrangeScriptVariable::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void OrangeScriptVariable::LoadListInto(MUDFile& hFp, OScriptVariableList& hList)
{
    OrangeScriptVariable* nNewVariable;
    
    while(LinkedListLoop(hFp, nNewVariable))
    {   
        hList.Add(nNewVariable);
        
        //** Set Variables **//
        //sNew->onThis = ontoThis;
    }
}



#if 0
//#######################################################################################
#pragma mark -
#endif


bool Ambience::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NUM"     , mNumber   , hFp.ReadNumber());
    MATCH("MESSAGE" , mMessage  , hFp.ReadString());
    MATCH("VNUMS"   , mOldVnums , hFp.ReadString());
                
    return false;
}


bool Ambience::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void Ambience::LoadListInto(MUDFile& hFp, AmbienceArray& hList)
{
    Ambience* nNewAmbience;
    
    while(LinkedListLoop(hFp, nNewAmbience))
    {   
        hList.Add(nNewAmbience);
    }
}


#if 0
//#######################################################################################
#pragma mark -
#endif




bool ObjectIndex::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("VNUM"        , mOldVnum          , hFp.ReadNumber());
    MATCH("UNKNOWN"     , mName             , hFp.ReadString());//Outdated
    MATCH("PUNKNOWN"    , mPName            , hFp.ReadString());//Outdated
    MATCH("NAME"        , mName             , hFp.ReadString());
    MATCH("PNAME"       , mPName            , hFp.ReadString());
    MATCH("KEYWORDS"    , mKeywords         , hFp.ReadString());
    MATCH("SHORT"       , mShortDesc        , hFp.ReadString());
    MATCH("PSHORT"      , mPShortDesc       , hFp.ReadString());
    MATCH("DESC"        , mDescription      , hFp.ReadString());
    
    MATCH("TYPE"        , mItemType         , TableLookup(gItemTable, hFp.ReadWord()));
    MATCH("MATERIALS"   , mMaterials        , hFp.ReadFlags());
    MATCH("ANTI"        , mAntiFlags        , hFp.ReadFlags());
    MATCH("EXTRA"       , mObjectFlags      , hFp.ReadFlags());
    MATCH("WEAR"        , mWearFlags        , hFp.ReadFlags());
    
    MATCH("LEVEL"       , mLevel            , hFp.ReadNumber());
    MATCH("WEIGHT"      , mWeight           , float(hFp.ReadNumber())/10.0f);
    MATCH("WEIGHTF"     , mWeight           , hFp.ReadFloat());
    MATCH("COST"        , mCost             , hFp.ReadNumber());
    MATCH("DUR"         , mDurability       , hFp.ReadNumber());
    
    MATCH("V0"          , mValue[0]         , hFp.ReadNumber());
    MATCH("V1"          , mValue[1]         , hFp.ReadNumber());
    MATCH("V2"          , mValue[2]         , hFp.ReadNumber());
    MATCH("V3"          , mValue[3]         , hFp.ReadNumber());
    MATCH("V4"          , mValue[4]         , hFp.ReadNumber());
    MATCH("V0F"         , mValue[0]         , hFp.ReadFlags());
    MATCH("V1F"         , mValue[1]         , hFp.ReadFlags());
    MATCH("V2F"         , mValue[2]         , hFp.ReadFlags());
    MATCH("V3F"         , mValue[3]         , hFp.ReadFlags());
    MATCH("V4F"         , mValue[4]         , hFp.ReadFlags());
    MATCH("LIQ"         , mValue[kDContLiq] , TableLookup(gLiqTable, hFp.ReadWord()));
    
    return false;
}


bool ObjectIndex::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_OBJ_SPOTS  , Spot::LoadListInto(hFp, this));
    MATCHCALL(IO_OBJ_SCRIPTS, OrangeScript::LoadListInto(hFp, this));

    return false;
}



#if 0
//#######################################################################################
#pragma mark -
#endif


bool MobileIndex::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("VNUM"        , mOldVnum          , hFp.ReadNumber());
    MATCH("KEYWORDS"    , mKeywords         , hFp.ReadString());
    MATCH("NAME"        , mName             , hFp.ReadString());
    MATCH("PNAME"       , mPName            , hFp.ReadString());
    MATCH("SDESC"       , mShortDesc        , hFp.ReadString());
    MATCH("PSDESC"      , mPShortDesc       , hFp.ReadString());
    MATCH("DESC"        , mDescription      , hFp.ReadString());
    MATCH("FLAGS"       , mActFlags         , hFp.ReadFlags());
    
    MATCH("ALIGN"       , mAlignment        , hFp.ReadNumber());
    MATCH("RIDABLE"     , mRidable          , hFp.ReadNumber());
    MATCH("LEVEL"       , mLevel            , hFp.ReadNumber());
//  MATCH("EXPPER"      , mExpPercent       , hFp.ReadFloat());
    MATCH("EXPMOD"      , mExpMod           , hFp.ReadFloat());
    MATCH("SIZE"        , mSize             , hFp.ReadNumber());
    
    MATCH("HIT_LOW"     , mHits[0]          , hFp.ReadNumber());
    MATCH("HIT_HIGH"    , mHits[1]          , hFp.ReadNumber());
    MATCH("ENERGY_LOW"  , mEnergy[0]        , hFp.ReadNumber());
    MATCH("ENERGY_HIGH" , mEnergy[1]        , hFp.ReadNumber());
    MATCH("MOVE_LOW"    , mMoves[0]         , hFp.ReadNumber());
    MATCH("MOVE_HIGH"   , mMoves[1]         , hFp.ReadNumber());
    
    MATCH("STATSTR"     , mStat[kStatStr]   , hFp.ReadNumber());
    MATCH("STATDEX"     , mStat[kStatDex]   , hFp.ReadNumber());
    MATCH("STATINT"     , mStat[kStatInt]   , hFp.ReadNumber());
    MATCH("STATWIS"     , mStat[kStatWis]   , hFp.ReadNumber());
    MATCH("STATCON"     , mStat[kStatCon]   , hFp.ReadNumber());
    
    MATCH("RESCOLD"     , mResist[kResCold] , hFp.ReadNumber());
    MATCH("RESFIRE"     , mResist[kResFire] , hFp.ReadNumber());
    MATCH("RESACID"     , mResist[kResAcid] , hFp.ReadNumber());
    MATCH("RESELEC"     , mResist[kResElec] , hFp.ReadNumber());
    MATCH("RESPOIS"     , mResist[kResPois] , hFp.ReadNumber());
    MATCH("RESMIND"     , mResist[kResMind] , hFp.ReadNumber());
    MATCH("RESSLEE"     , mResist[kResSlee] , hFp.ReadNumber());
    MATCH("RESCHAR"     , mResist[kResChar] , hFp.ReadNumber());
    
    MATCH("APR"         , mAPR              , hFp.ReadNumber());
    MATCH("SEX"         , mSex              , TableLookup(gSexTable, hFp.ReadWord()));
    MATCH("POS"         , mDefaultPos       , TableLookup(gPosTable, hFp.ReadWord(), false));                           
    MATCH("MOVE"        , mMovement         , hFp.ReadString());
    
    return false;
}


bool MobileIndex::MatchLists(MUDFile& hFp, CHAR* varName)
{   
    MATCHCALL(IO_MOB_SCRIPTS    , OrangeScript::LoadListInto(hFp, this));
    MATCHCALL(IO_MOB_SPOTS      , Spot::LoadListInto        (hFp, this));
    MATCHCALL(IO_MOB_ATTACKS    , Attack::LoadListInto      (hFp, mAttacks));
    MATCHCALL(IO_MOB_PARTS      , Part::LoadListInto        (hFp, mParts));
    MATCHCALL(IO_MOB_SKINS      , Skin::LoadListInto        (hFp, mSkins));
//  MATCH(IO_MOB_SHOP       , shop      , Shop::LoadSingle(fp));

    return false;
}

void MobileIndex::OnLoad()
{
    //ReasonExp(NULL);
}







#if 0
//#######################################################################################
#pragma mark -
#endif


bool Attack::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NUMBER"      , mNumber           , hFp.ReadNumber());
    MATCH("MESSAGE"     , mMessage          , hFp.ReadString());
    MATCH("CHANCE"      , mChance           , hFp.ReadNumber());
    MATCH("TYPE"        , mType             , hFp.ReadNumber());
    MATCH("DAMAGE"      , mDamageType       , hFp.ReadNumber());
    MATCH("FLAGS"       , mFlags            , hFp.ReadFlags());

    MATCH("HITLOW"      , mHitroll[0]       , hFp.ReadNumber());
    MATCH("HITHIGH"     , mHitroll[1]       , hFp.ReadNumber());
    MATCH("DAMLOW"      , mDamroll[0]       , hFp.ReadNumber());
    MATCH("DAMHIGH"     , mDamroll[1]       , hFp.ReadNumber());

    return false;
}



bool Attack::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}



void Attack::LoadListInto(MUDFile& hFp, AttackArray& hList)
{
    Attack* nNewAttack;
    
    while(LinkedListLoop(hFp, nNewAttack))
    {
        hList.AddFront(nNewAttack);
    }
}


#if 0
//#######################################################################################
#pragma mark -
#endif



bool Part::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NAME"    , mName     , hFp.ReadString());
    MATCH("AC"      , mAC       , hFp.ReadNumber());
    MATCH("SIZE"    , mSize     , hFp.ReadNumber());

    return false;
}


bool Part::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void Part::LoadListInto(MUDFile& hFp, PartArray& hList)
{
    Part* nNewPart;
    
    while(LinkedListLoop(hFp, nNewPart))
        hList.Add(nNewPart);
}




#if 0
//#######################################################################################
#pragma mark -
#endif


bool Skin::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NUMBER"  , mNumber   , hFp.ReadNumber());
    MATCH("CHANCE"  , mChance   , hFp.ReadNumber());
    MATCH("VNUM"    , mOldVnum  , hFp.ReadNumber());

    return false;
}


bool Skin::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void Skin::LoadListInto(MUDFile& hFp, SkinArray& hList)
{
    Skin* nNewSkin;
    
    while(LinkedListLoop(hFp, nNewSkin))
        hList.AddFront(nNewSkin);
}




#if 0
//#######################################################################################
#pragma mark -
#endif


bool Population::MatchValues(MUDFile& hFp, CHAR* varName)
{

    MATCH("NUMBER"  , mNumber       , hFp.ReadNumber());

    return false;
}



bool Population::MatchLists(MUDFile& hFp, CHAR* varName)
{
//  MATCHCALL(IO_REPOP_OLIST    , oList     , RepopObjList::LoadAsList(fp, NULL));
    
    return false;
}


void Population::LoadListInto(MUDFile& hFp, PopulationArray& hList)
{
    Population* nNewPop;
    
    while(LinkedListLoop(hFp, nNewPop))
    {
        ObjectPopulation* nObjPop = dynamic_cast<ObjectPopulation*>(nNewPop);
        
        if(nObjPop && nObjPop->mIsOldObjRepop)
        {
            ASSERT(nObjPop->mPopulationsInside.size() == 1);
            
            nObjPop->mPopulationsInside[0]->mRoomOldVnum = nObjPop->mPopulationsInside[0]->mObjectRoomValue1Vnum;
            nNewPop = nObjPop->mPopulationsInside[0];           
        }
        
        hList.Add(nNewPop);
    }
}



bool ObjectPopulation::MatchValues(MUDFile& hFp, CHAR* varName)
{
    //Old List 'RepopObjList' Class
    MATCH("VNUM"    , mObjectOldVnum        , hFp.ReadNumber());
    MATCH("MIN"     , mLoadMin              , hFp.ReadNumber());
    MATCH("MAX"     , mLoadMax              , hFp.ReadNumber());
    MATCH("CHANCE"  , mChance               , hFp.ReadNumber());
    MATCH("V0"      , mChance               , hFp.ReadNumber());
    MATCH("V1"      , mObjectRoomValue1Vnum , hFp.ReadNumber());
    MATCH("VLIQ"    , mLiquid               , TableLookup(gLiqTable, hFp.ReadWord()));
    MATCH("USE"     , mObjectFlags          , hFp.ReadFlags());

    //Check SuperClass
    return this->Population::MatchValues(hFp, varName);
}

bool ObjectPopulation::MatchLists(MUDFile& hFp, CHAR* varName)
{
    //Old Empty 'RepopObj' Class
    MATCHCALL(IO_REPOP_OLIST, ObjectPopulation::LoadListInto(hFp, mPopulationsInside));

    //Old List 'RepopObjList' Class
    MATCHCALL(IO_OLR_CONTENTS, ObjectPopulation::LoadListInto(hFp, mPopulationsInside));


    //Check SuperClass
    return this->Population::MatchLists(hFp, varName);
}



void ObjectPopulation::LoadListInto(MUDFile& hFp, OPopulationArray& hList)
{
    ObjectPopulation* nNewObjectPop;
    
    while(LinkedListLoop(hFp, nNewObjectPop))
        hList.Add(nNewObjectPop);
}



bool MobilePopulation::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("MOBILE"  , mMobileOldVnum, hFp.ReadNumber());
    MATCH("ROOM"    , mRoomOldVnum  , hFp.ReadNumber());
    MATCH("CHANCE"  , mChance       , hFp.ReadNumber());
    MATCH("POS"     , mInPosition   , hFp.ReadNumber());
    MATCH("POSITION", mInPosition   , TableLookup(gPosTable, hFp.ReadWord(), false));
    
    //Check SuperClass
    return this->Population::MatchValues(hFp, varName);
}


bool MobilePopulation::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_REPOP_MOB_SHOPLIST , ObjectPopulation::LoadListInto(hFp, mShopObjects));
    MATCHCALL(IO_REPOP_OLIST        , ObjectPopulation::LoadListInto(hFp, mObjectsInside));

    //Check SuperClass
    return this->Population::MatchLists(hFp, varName);
}






bool ExitPopulation::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("ROOM"    , mRoomOldVnum  , hFp.ReadNumber());
    MATCH("DIR"     , mDirection    , hFp.ReadNumber());
    MATCH("FLAGS"   , mFlags        , hFp.ReadNumber());
    
    //Check SuperClass
    return this->Population::MatchValues(hFp, varName);
}

bool ExitPopulation::MatchLists(MUDFile& hFp, CHAR* varName)
{
    //Check SuperClass
    return this->Population::MatchLists(hFp, varName);
}






#if 0
//#######################################################################################
#pragma mark -
#endif


bool HelpFile::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("FNAME"   , mFileName     , hFp.ReadString());
    MATCH("NAME"    , mName         , hFp.ReadString());

    return false;
}


bool HelpFile::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_HFILE_HELPS, Help::LoadHelpFileHelps(hFp, this));
    
    return false;
}


void HelpFile::OnLoad()
{
    MUD::Get()->mHelpFiles.Add(this);
}



bool Help::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("CAT"         , mCategory     , LookupHelp(hFp.ReadWord()));
    MATCH("KEYWORDS"    , mKeywords     , hFp.ReadString());
    MATCH("LEVEL"       , mLevel        , hFp.ReadNumber());
    MATCH("SYNTAX"      , mSyntax       , hFp.ReadString());
    MATCH("SUMMARY"     , mSummary      , hFp.ReadString());
    MATCH("TEXT"        , mText         , hFp.ReadString());
    MATCH("TITLE"       , mTitle        , hFp.ReadString());
    MATCH("SEE"         , mSeeAlso      , hFp.ReadString());

    return false;
}


bool Help::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}


void Help::LoadHelpFileHelps(MUDFile& hFp, HelpFile* hFile)
{
    Help* nNewHelp;
    
    while(LinkedListLoop(hFp, nNewHelp))
    {
        nNewHelp->mInFile = hFile;
        hFile->mHelps.Add(nNewHelp);

        if(!StrCmp(nNewHelp->mKeywords, "greeting"))
            MUD::Get()->mHelpGreeting = nNewHelp;
        
        if(!StrCmp(nNewHelp->mKeywords, "menu"))
            MUD::Get()->mHelpMenu = nNewHelp;
    
        if(!StrCmp(nNewHelp->mKeywords, "motd"))
            MUD::Get()->mHelpMOTD = nNewHelp;
    }


}




#if 0
//#######################################################################################
#pragma mark -
#endif



bool SocialFile::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("FNAME"   , mFileName     , hFp.ReadString());
    MATCH("NAME"    , mName         , hFp.ReadString());

    return false;
}


bool SocialFile::MatchLists(MUDFile& hFp, CHAR* varName)
{
    MATCHCALL(IO_SFILE_SOCIALS, Social::LoadSocialFileSocials(hFp, this));
    
    return false;
}


void SocialFile::OnLoad()
{
    MUD::Get()->mSocialFiles.Add(this);
}



bool Social::MatchValues(MUDFile& hFp, CHAR* varName)
{
    MATCH("NAME"        , mName             , hFp.ReadString());
    MATCH("CHNOARG"     , mCharNoArg        , hFp.ReadString());
    MATCH("OTNOARG"     , mOthersNoArg      , hFp.ReadString());
    MATCH("CHFOUND"     , mCharFound        , hFp.ReadString());
    MATCH("OTFOUND"     , mOthersFound      , hFp.ReadString());
    MATCH("VIFOUND"     , mVictFound        , hFp.ReadString());
    MATCH("CHAUTO"      , mCharAuto         , hFp.ReadString());
    MATCH("OTAUTO"      , mOthersAuto       , hFp.ReadString());

    return false;
}



bool Social::MatchLists(MUDFile& hFp, CHAR* varName)
{
    return false;
}



void Social::LoadSocialFileSocials(MUDFile& hFp, SocialFile* hFile)
{
    Social* nNewSocial;
    
    while(LinkedListLoop(hFp, nNewSocial))
    {
        nNewSocial->mInFile = hFile;
        hFile->mSocials.Add(nNewSocial);

        //** Add To Main Social Hash **//
        //short iHash = UPPER((sNew->name)[0])-65;

    }
}



#if 0
//#########################################################################//
//#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#//
//*************************************************************************//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
#pragma mark -
#pragma mark ее-- No Further Compilation --ее
#pragma mark -





#if 0
//#######################################################################################
#pragma mark -
#endif




bool Shop::MatchValues(MUDFile& hFp, CHAR* varName)
{
    int i;
    
    if(!strcmp("SELL", varName))
    {
        if((i = TableLookup(ItemTable, hFp.ReadWord())) == -1)
            return false;
        sell[i] = hFp.ReadNumber();
        return true;
    }
    if(!strcmp("BUY", varName))
    {
        if((i = LookupItem(hFp.ReadWord())) == -1)
            return false;
        buy[i] = hFp.ReadNumber();
        return true;
    }
    if(!strcmp("REP", varName))
    {
        if((i = LookupItem(hFp.ReadWord())) == -1)
            return false;
        repair[i] = hFp.ReadNumber();
        return true;
    }   

    return false;
}

Shop* Shop::LoadSingle(MUDFile& hFp)
{
    Shop *aNew, *aShop = NULL;
    
    while(LinkedListLoop(fp, aNew))
    {   
        if(aShop)
            delete aShop;
        aShop = aNew;
    }
    
    return aShop;
}

#endif //KILL
