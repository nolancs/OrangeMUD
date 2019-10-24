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
#include "Ambience.h"
#include "Area.h"
#include "Attack.h"
#include "ExitPopulation.h"
#include "Help.h"
#include "HelpFile.h"
#include "Immortal.h"
#include "MobileIndex.h"
#include "MobilePopulation.h"
#include "MUD.h"
#include "Object.h"
#include "ObjectHolder.h"
#include "ObjectIndex.h"
#include "ObjectPopulation.h"
#include "OrangeScript.h"
#include "OrangeScriptDataType.h"
#include "OrangeScriptVariable.h"
#include "Part.h"
#include "RoomIndex.h"
#include "SaveLoadable.h"
#include "Skin.h"
#include "Social.h"
#include "SocialFile.h"
#include "Spot.h"
#include "StringUtils.h"
#include "Tables.h"
#include "UIDHolder.h"
#include "XMLFile.h"

//## Matching Macros ##//
#define MatchStr(hN, hX)        if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsString(); return true; }
#define MatchULong(hN, hX)      if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsULong(); return true; }
#define MatchLong(hN, hX)       if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsLong(); return true; }
#define MatchUShort(hN, hX)     if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsUShort(); return true; }
#define MatchShort(hN, hX)      if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsShort(); return true; }
#define MatchByte(hN, hX)       if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsByte(); return true; }
#define MatchChar(hN, hX)       if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsChar(); return true; }
#define MatchFloat(hN, hX)      if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsFloat(); return true; }
#define MatchDouble(hN, hX)     if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsDouble(); return true; }
#define MatchBool(hN, hX)       if(hTag.GetName() == hN){ hXML.ReadTagContents(); hXML.CloseTag(); hX = hTag.AsBool(); return true; }
#define MatchEquals(hN, hX, hY) if(hTag.GetName() == hN){ hX = hY; return true; }
#define MatchCall(hN, hCall)    if(hTag.GetName() == hN){ hCall; return true; }
#define MatchIf(hN)             if(hTag.GetName() == hN)


template<typename AsT>
void LoadValuesFromAs(XMLTag& hTag, AsT* nNothing = NULL);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: SaveLoadable::LoadValuesFromAs<>
//
//   Desc:: Same as LoadValuesFrom but it won't go all the way down the
//          chain of inheiritance to the sub-most subclass to check with its
//          MatchValues call. You put in the class you want it to use when
//          calling MatchValues and uses that one and only that one.
//
//  Input:: Same as LoadValuesFrom, the nNothing is just there
//          because templates need the typename somewhere in the function
//          signature.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

template<typename AsT>
void LoadValuesFromAs(AsT* hThis, XMLReader& hXML, XMLTag& hTag)
{
    AsT* nAsPtr = static_cast<AsT*>(hThis);
    ASSERT(nAsPtr);

    XMLTag* nNext;
    while(hXML.OpenTag(nNext))
    {
        if(!nAsPtr->AsT::MatchValues(hXML, *nNext))
        {
            hXML.ReadTagContents();
            hXML.CloseTag();
        }
    }
    hXML.CloseTag();
}




SaveLoadable::SaveLoadable()
{
}


SaveLoadable::~SaveLoadable()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: SaveLoadable::LoadValuesFrom
//
//   Desc:: Called from any object that derives from SaveLoadable, this loads
//          the object based on the values in the XML tag. Reads in everything
//          that SaveValues wrote out basically, it calls the object's
//          MatchValues function.
//
//  Input:: The super tag full of values.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void SaveLoadable::LoadValuesFrom(XMLReader& hXML, XMLTag& hTag)
{
    XMLTag* nNext;
    while(hXML.OpenTag(nNext))
    {
        if(!MatchValues(hXML, *nNext))
        {
            hXML.SkipTagContents();
            hXML.CloseTag();
        }
    }
    hXML.CloseTag();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::LoadPlayer
//
//   Desc:: You specify the name of the player and this function will attempt
//          to load the player with the name you specified. It will allocate
//          the player object for you only if it returns a successful load.
//
//  Input:: The name of the player and a reference to a pointer that will point
//          to the player object if successful.
//
// Output:: Three possible outputs.  kLoadPlayer_NonExistant if the player does
//          not exist. kLoadPlayer_ErrWithFile if something really bad went
//          wrong but the player does exist. kLoadPlayer_Success if the player
//          did exist and was successfully loaded.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR MUD::LoadPlayer(STRINGCW& hName, Player*& hPlayer)
{
    FilePath    nPlayerPath(kPath_Player); 
    STRING      nCharDir, nNameDir;
    FileLink*   nFileLink;


    nCharDir    += UPPER(hName[0]);
    nNameDir    = *(STRINGCW(hName).Capitalize());

    nPlayerPath.Add(nCharDir);                  //Players/V/
    nPlayerPath.Add(nNameDir);                  //Players/V/Volkaran
    if(!FileManager::Get()->Exists(nPlayerPath))
        return kLoadPlayer_NonExistant;

    nPlayerPath.Add(kFile_PFile);               //Players/V/Volkaran/PFile.xml
    if(!FileManager::Get()->Exists(nPlayerPath))
        return kLoadPlayer_ErrWithFile;
    
    if(!(nFileLink = FileManager::Get()->OpenRead(nPlayerPath)))
        return kLoadPlayer_ErrWithFile;
    

    
    //** Load The XML Player File **//
    try
    {
        XMLReader nXMLReader(nFileLink);
        APtr<XMLTag> nRootTag;
        nXMLReader.OpenBaseTag(nRootTag);
        if(nRootTag->GetAttributes()[kAttr_ObjectClass] == "Immortal")
        {
            hPlayer = new Immortal();
        }else{
            ASSERT(nRootTag->GetAttributes()[kAttr_ObjectClass] == "Player");
            hPlayer = new Player();
        }       
        hPlayer->LoadValuesFrom(nXMLReader, *nRootTag);
    }
    catch(eXMLError hExceptionErr)
    {
        printf("%d exception XML thrown on read.\n", hExceptionErr);
        return kLoadPlayer_ErrWithFile;
    }


    return kLoadPlayer_Success;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: MUD::PlayerExists
//
//   Desc:: Checks if a player with hName exists in the MUDs player files.
//
//  Input:: The player name.
//
// Output:: True if the player exists, false if not.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool MUD::PlayerExists(STRINGCW& hName)
{
    FilePath    nPlayerPath(kPath_Player);      //Players/
    STRING      nCharDir, nNameDir;

    nCharDir    += UPPER(hName[0]);
    nNameDir    = *(STRINGCW(hName).Capitalize());

    nPlayerPath.Add(nCharDir);                  //Players/V/
    nPlayerPath.Add(nNameDir);                  //Players/V/Volkaran
    if(!FileManager::Get()->Exists(nPlayerPath))
        return false;

    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Person::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString   ("Name"         , mName                 );
    hFile.WriteString   ("PName"        , mPName                );
    hFile.WriteString   ("ShortDesc"    , mShortDesc            );
    hFile.WriteString   ("PShortDesc"   , mPShortDesc           );
    hFile.WriteString   ("Description"  , mDescription          );
    hFile.WriteString   ("Sex"          , gSexTable[mSex].mName );
    hFile.WriteShort    ("Level"        , mLevel                );
    hFile.WriteByte     ("Race"         , mRace                 );
    hFile.WriteByte     ("Class"        , mClass                );
    hFile.WriteByte     ("Alignment"    , mAlignment            );
    hFile.WriteByte     ("Size"         , mSize                 );
    hFile.WriteByte     ("Position"     , mPosition             );

    //Wimpys
    hFile.WriteLong     ("WimpyFlee"    , mWimpy[kWimpyFlee]    );
    hFile.WriteLong     ("WimpyBlink"   , mWimpy[kWimpyBlink]   );
    hFile.WriteLong     ("WimpyRecall"  , mWimpy[kWimpyRecall]  );
    hFile.WriteLong     ("WimpyPray"    , mWimpy[kWimpyPray]    );

    //Resistances
    hFile.WriteChar     ("ResCold"      , mResist[kResCold]     );
    hFile.WriteChar     ("ResFire"      , mResist[kResFire]     );
    hFile.WriteChar     ("ResAcid"      , mResist[kResAcid]     );
    hFile.WriteChar     ("ResElec"      , mResist[kResElec]     );
    hFile.WriteChar     ("ResPois"      , mResist[kResPois]     );
    hFile.WriteChar     ("ResMind"      , mResist[kResMind]     );
    hFile.WriteChar     ("ResSleep"     , mResist[kResSlee]     );
    hFile.WriteChar     ("ResCharm"     , mResist[kResChar]     );

    //Stats
    hFile.WriteShort    ("Strength"     , mStat[kStatStr]       );
    hFile.WriteShort    ("Dexterity"    , mStat[kStatDex]       );
    hFile.WriteShort    ("Intelligence" , mStat[kStatInt]       );
    hFile.WriteShort    ("Wisdom"       , mStat[kStatWis]       );
    hFile.WriteShort    ("Constitution" , mStat[kStatCon]       );

    //Hits/Energy/Moves
    hFile.WriteLong     ("HitPoints"    , mHits                 );
    hFile.WriteLong     ("HitPointsMax" , mHitsMax              );
    hFile.WriteLong     ("Energy"       , mEnergy               );
    hFile.WriteLong     ("EnergyMax"    , mEnergyMax            );
    hFile.WriteLong     ("Moves"        , mMoves                );
    hFile.WriteLong     ("MovesMax"     , mMovesMax             );
    
    //Leveling Values
    hFile.WriteLong     ("ExpTNL"       , mExpTNL               );
    hFile.WriteShort    ("LevelLast"    , mLevelLast            );
    hFile.WriteShort    ("LevelHitT"    , mLevelHitT            );
    hFile.WriteShort    ("LevelEnergyT" , mLevelEnergyT         );
    hFile.WriteShort    ("LevelMoveT"   , mLevelMoveT           );
    
    
    //Write The Room They Save In
    ASSERT(mInRoom);
    hFile.WriteLong     ("InRoom"       , mInRoom->VRef()       );
    
    
    //Write Carrying
    if(!ObjectHolder::Empty())
        ObjectHolder::SaveTo(hFile, "Carrying");

    //Write Wearing
    if(!mWearing.Empty())
        mWearing.SaveTo(hFile, "Wearing");
}


bool Person::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchStr    ("Name"         , mName);
    MatchStr    ("PName"        , mPName);
    MatchStr    ("ShortDesc"    , mShortDesc);
    MatchStr    ("PShortDesc"   , mPShortDesc);
    MatchStr    ("Description"  , mDescription);
    MatchIf("Sex")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mSex = TableLookup(gSexTable, hTag.AsCStr());
        return true;
    }
    MatchShort  ("Level"        , mLevel);
    MatchByte   ("Race"         , mRace);
    MatchIf("Class")
    {
        mClass = 0;
        BYTE nTmp;
        MatchByte   ("Class"        , nTmp);
    }
    MatchByte   ("Alignment"    , mAlignment);
    MatchByte   ("Size"         , mSize);
    MatchByte   ("Position"     , mPosition);
    
    //Wimpys
    MatchLong   ("WimpyFlee"    , mWimpy[kWimpyFlee]);
    MatchLong   ("WimpyBlink"   , mWimpy[kWimpyBlink]);
    MatchLong   ("WimpyRecall"  , mWimpy[kWimpyRecall]);
    MatchLong   ("WimpyPray"    , mWimpy[kWimpyPray]);

    //Resistances
    MatchChar   ("ResCold"      , mResist[kResCold]);
    MatchChar   ("ResFire"      , mResist[kResFire]);
    MatchChar   ("ResAcid"      , mResist[kResAcid]);
    MatchChar   ("ResElec"      , mResist[kResElec]);
    MatchChar   ("ResPois"      , mResist[kResPois]);
    MatchChar   ("ResMind"      , mResist[kResMind]);
    MatchChar   ("ResSleep"     , mResist[kResSlee]);
    MatchChar   ("ResCharm"     , mResist[kResChar]);

    //Stats
    MatchShort  ("Strength"     , mStat[kStatStr]);
    MatchShort  ("Dexterity"    , mStat[kStatDex]);
    MatchShort  ("Intelligence" , mStat[kStatInt]);
    MatchShort  ("Wisdom"       , mStat[kStatWis]);
    MatchShort  ("Constitution" , mStat[kStatCon]);

    //Hits/Energy/Moves
    MatchLong   ("HitPoints"    , mHits);
    MatchLong   ("HitPointsMax" , mHitsMax);
    MatchLong   ("Energy"       , mEnergy);
    MatchLong   ("EnergyMax"    , mEnergyMax);
    MatchLong   ("Moves"        , mMoves);
    MatchLong   ("MovesMax"     , mMovesMax);
    
    //Leveling Values
    MatchLong   ("ExpTNL"       , mExpTNL);
    MatchShort  ("LevelLast"    , mLevelLast);
    MatchShort  ("LevelHitT"    , mLevelHitT);
    MatchShort  ("LevelEnergyT" , mLevelEnergyT);
    MatchShort  ("LevelMoveT"   , mLevelMoveT);


    //Load The Room They Saved In
    MatchIf("InRoom")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mWasInRoom = RoomIndex::Get(hTag.AsLong());
        return true;
    }


    //Carrying
    MatchCall("Carrying", LoadValuesFromAs<ObjectHolder>(this, hXML, hTag));
    
    
    //Match Wearing, Special Loading
    MatchIf("Wearing")
    {
        ObjectHolder nWearing;
        nWearing.LoadValuesFrom(hXML, hTag);
        while(!nWearing.Empty())
        {
            Object* nObject = *nWearing.Begin();
            EquipObject(nObject, nObject->mWearLoc);
        }
        return true;
    }

    return false;
}




bool Player::Save()
{
    FilePath        nPlayerPath(kPath_Player); 
    APtr<FileLink>  nFileLink;
    STRING          nCharDir;


    //Check that "Players/" directory is here
    if(!FileManager::Get()->Exists(nPlayerPath))
        FileManager::Get()->MakeDir(nPlayerPath);

    //Check "Players/V/" first letter path
    nCharDir += UPPER(mName[0]);
    nPlayerPath.Add(nCharDir);
    if(!FileManager::Get()->Exists(nPlayerPath))
        FileManager::Get()->MakeDir(nPlayerPath);
    
    //Check "Players/V/Volkaran" name path
    nPlayerPath.Add(*(STRINGCW(mName).Capitalize()));
    if(!FileManager::Get()->Exists(nPlayerPath))
        FileManager::Get()->MakeDir(nPlayerPath);

    //Make PFile path "Players/V/Volkaran/PFile.xml"
    nPlayerPath.Add(kFile_PFile);
    if(!(nFileLink = FileManager::Get()->OpenWrite(nPlayerPath)))
        return false;
    
    XMLWriter nXMLFile(nFileLink);
    
    nXMLFile.BeginFile();   
    SaveTo(nXMLFile, "Player");
    nXMLFile.EndFile();
    nFileLink->Close();
    return true;
}




void Player::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Player");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Person::SaveValuesTo(hFile);
    Player::SaveValuesTo(hFile);
    hFile.EndTag();
}



void Player::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteULong    ("UID"          , mUID              );
    hFile.WriteString   ("Passwd"       , mPassword         );
    hFile.WriteString   ("Prompt"       , mPrompt           );
    hFile.WriteString   ("Title"        , mTitle            );
    hFile.WriteString   ("TitleAFK"     , mTitleAFK         );
    hFile.WriteString   ("Email"        , mEmail            );
    hFile.WriteString   ("Homepage"     , mHomepage         );
    hFile.WriteLong     ("LastLogin"    , mLastLogin        );
    hFile.WriteLong     ("TimePlayed"   , mTimePlayed       );
    hFile.WriteShort    ("Trust"        , mTrust            );
    hFile.WriteShort    ("Scroll"       , mScroll           );
    hFile.WriteShort    ("InvisLevel"   , mInvisLevel       );
    hFile.WriteLong     ("FlagsOptions" , mFlagsOptions     );
    hFile.WriteLong     ("FlagsInfo"    , mFlagsInfo        );
    
    //Write Conditions
    hFile.WriteShort    ("CondDrunk"    , mCondition[kCondDrunk]    );
    hFile.WriteShort    ("CondFull"     , mCondition[kCondFull]     );
    hFile.WriteShort    ("CondThirst"   , mCondition[kCondThirst]   );
    hFile.WriteShort    ("CondHunger"   , mCondition[kCondHunger]   );
    hFile.WriteShort    ("CondHigh"     , mCondition[kCondHigh]     );
    hFile.WriteShort    ("CondWithdrawl", mCondition[kCondWithdrawl]);
    
    
    //Write Bank
    if(!mBank.Empty())
        mBank.SaveTo(hFile, "Bank");


    //Write Color Prefs
    for(LONG i = 0; i < kColorPrefMax; ++i)
    {
        XMLAttributes nCPrefAttrs;
        nCPrefAttrs.AddLong("Index", i);
        hFile.WriteLong("ColorPref", mColorPref[i], &nCPrefAttrs);
    }   
}



bool Player::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchULong  ("UID"          , mUID          );
    MatchStr    ("Passwd"       , mPassword     );
    MatchStr    ("Prompt"       , mPrompt       );
    MatchStr    ("Title"        , mTitle        );
    MatchStr    ("TitleAFK"     , mTitleAFK     );
    MatchStr    ("Email"        , mEmail        );
    MatchStr    ("Homepage"     , mHomepage     );
    MatchLong   ("LastLogin"    , mLastLogin    );
    MatchLong   ("TimePlayed"   , mTimePlayed   );
    MatchShort  ("Trust"        , mTrust        );
    MatchShort  ("Scroll"       , mScroll       );
    MatchShort  ("InvisLevel"   , mInvisLevel   );
    MatchLong   ("FlagsOptions" , mFlagsOptions );
    MatchLong   ("FlagsInfo"    , mFlagsInfo    );

    MatchShort  ("CondDrunk"    , mCondition[kCondDrunk]    );
    MatchShort  ("CondFull"     , mCondition[kCondFull]     );
    MatchShort  ("CondThirst"   , mCondition[kCondThirst]   );
    MatchShort  ("CondHunger"   , mCondition[kCondHunger]   );
    MatchShort  ("CondHigh"     , mCondition[kCondHigh]     );
    MatchShort  ("CondWithdrawl", mCondition[kCondWithdrawl]);

    MatchCall   ("Bank"         , mBank.LoadValuesFrom(hXML, hTag));
    
    MatchIf("Password")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        printf("Converting password.\n");
        STRING n64Pass, nPass = hTag.AsCStr();
        Base64Encode(nPass.data(), nPass.length(), n64Pass);
        mPassword = n64Pass.c_str();
        return true;
    }
    
    MatchIf("ColorPref")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        SHORT nIndex = atoi(hTag.GetAttributes()["Index"].c_str());
        mColorPref[nIndex] = hTag.AsLong();
        return true;
    }
    
    
    return Person::MatchValues(hXML, hTag);
}



void Immortal::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Immortal");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Person::SaveValuesTo(hFile);
    Player::SaveValuesTo(hFile);
    Immortal::SaveValuesTo(hFile);
    hFile.EndTag();
}



void Immortal::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteByte     ("InvisLevel"       , mInvisLevel       );
    hFile.WriteString   ("PoofIn"           , mPoofIn           );
    hFile.WriteString   ("PoofOut"          , mPoofOut          );
    hFile.WriteString   ("GoInvisSelf"      , mGoInvisSelf      );
    hFile.WriteString   ("GoInvisRoom"      , mGoInvisRoom      );
    hFile.WriteString   ("GoVisSelf"        , mGoVisSelf        );
    hFile.WriteString   ("GoVisRoom"        , mGoVisRoom        );
    hFile.WriteString   ("MakeObjOneSelf"   , mMakeObjOneSelf   );
    hFile.WriteString   ("MakeObjOneRoom"   , mMakeObjOneRoom   );
    hFile.WriteString   ("MakeObjMultiSelf" , mMakeObjMultiSelf );
    hFile.WriteString   ("MakeObjMultiRoom" , mMakeObjMultiRoom );
    hFile.WriteString   ("MakeMobOneSelf"   , mMakeMobOneSelf   );
    hFile.WriteString   ("MakeMobOneRoom"   , mMakeMobOneRoom   );
    hFile.WriteString   ("MakeMobMultiSelf" , mMakeMobMultiSelf );
    hFile.WriteString   ("MakeMobMultiRoom" , mMakeMobMultiRoom );
}



bool Immortal::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchByte   ("InvisLevel"       , mInvisLevel       );
    MatchStr    ("PoofIn"           , mPoofIn           );
    MatchStr    ("PoofOut"          , mPoofOut          );
    MatchStr    ("GoInvisSelf"      , mGoInvisSelf      );
    MatchStr    ("GoInvisRoom"      , mGoInvisRoom      );
    MatchStr    ("GoVisSelf"        , mGoVisSelf        );
    MatchStr    ("GoVisRoom"        , mGoVisRoom        );
    MatchStr    ("MakeObjOneSelf"   , mMakeObjOneSelf   );
    MatchStr    ("MakeObjOneRoom"   , mMakeObjOneRoom   );
    MatchStr    ("MakeObjMultiSelf" , mMakeObjMultiSelf );
    MatchStr    ("MakeObjMultiRoom" , mMakeObjMultiRoom );
    MatchStr    ("MakeMobOneSelf"   , mMakeMobOneSelf   );
    MatchStr    ("MakeMobOneRoom"   , mMakeMobOneRoom   );
    MatchStr    ("MakeMobMultiSelf" , mMakeMobMultiSelf );
    MatchStr    ("MakeMobMultiRoom" , mMakeMobMultiRoom );

    return Player::MatchValues(hXML, hTag);
}






#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Object::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes; 
    nXMLAttributes.AddString(kAttr_ObjectClass, "Object");
    nXMLAttributes.AddLong(kAttr_VRef, mIndex->VRef());

    hFile.BeginSuperTag(hTagName, &nXMLAttributes); 
    Object::SaveValuesTo(hFile);    
    hFile.EndTag();
}


void Object::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort("WearLoc"  , mWearLoc);

    if(!Empty())
        ObjectHolder::SaveTo(hFile, "Contents");
}


bool Object::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchShort  ("WearLoc"      , mWearLoc);
    MatchCall   ("Contents"     , LoadValuesFromAs<ObjectHolder>(this, hXML, hTag));

    return false;
}


void ObjectHolder::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    
    nXMLAttributes.AddString(kAttr_ObjectClass, "ObjectHolder");    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    
    ObjectHolder::SaveValuesTo(hFile);
    
    hFile.EndTag();
}


void ObjectHolder::SaveValuesTo(XMLWriter& hFile)
{
    for(ObjectIter nScan = Begin(); nScan != End(); ++nScan)
        (*nScan)->SaveTo(hFile, "Object");
}


bool ObjectHolder::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchIf("Object")
    {
        VREF            nLoadRef    = atol(hTag.GetAttributes()[kAttr_VRef].c_str());
        ObjectIndex*    nIndex      = ObjectIndex::Get(nLoadRef);
        APtr<Object>    nLoadObj;
        
        if(!nIndex)
            nIndex = ObjectIndex::Get(kVRef_Object_Null);

        nLoadObj = new Object(nIndex);
        nLoadObj->LoadValuesFrom(hXML, hTag);
        
        AddObject(nLoadObj.Detach(), true);
        return true;
    }
    
    ASSERT(!"Unknown ObjectHolder tag");
    return false;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void UIDHolder::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "UIDHolder");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    UIDHolder::SaveValuesTo(hFile); 
    hFile.EndTag();
}



void UIDHolder::SaveValuesTo(XMLWriter& hFile)
{
    for(UIDList::iterator i = mUIDs.begin(); i != mUIDs.end(); ++i)
        hFile.WriteULong("UID", *i);
}



bool UIDHolder::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchIf("UID")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mUIDs.AddBack((UID) hTag.AsULong());
        return true;
    }
    
    ASSERT(!"Unknown UIDHolder tag");
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool Area::Save()
{
    FilePath nPath(kPath_AreaFiles); 
    APtr<FileLink> nLink;

    //Check that "Resources/AreaFiles/" directory is here
    if(!FileManager::Get()->Exists(nPath))
        FileManager::Get()->MakeDir(nPath);
    nPath.Add(STRING(*mFileName));
    
    //Backup Old File
    if(FileManager::Get()->Exists(nPath))
    {
        FilePath nBackupPath = nPath;
        nBackupPath.mPathStrs.back() += ".bak";
        FileManager::Get()->Move(nPath, nBackupPath);
    }
    
    //Save New File
    if(!(nLink = FileManager::Get()->OpenWrite(nPath)))
        return false;
    
    XMLWriter nXMLWriter(nLink);
    nXMLWriter.BeginFile(); 
    SaveTo(nXMLWriter, "Area");
    nXMLWriter.EndFile();
    nLink->Close(); 
    return true;

}



void Area::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Area");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Area::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Area::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort    ("Anum"         , mANum         );
    hFile.WriteString   ("Name"         , mName         );
    hFile.WriteString   ("FileName"     , mFileName     );
    hFile.WriteShort    ("LowRange"     , mLowRange     );
    hFile.WriteShort    ("HighRange"    , mHighRange    );
    hFile.WriteShort    ("RespawnRate"  , mRespawnRate  );
    hFile.WriteShort    ("Version"      , mVersion      );
    mBuilders.SaveTo(hFile, "Builders");

    {
    hFile.BeginSuperTag("RoomIndexes");
    for(IRoomIter i = GetRooms().begin(); i != GetRooms().end(); ++i)
        i->second->SaveTo(hFile, "RIndex");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("ObjectIndexes");
    for(IObjectIter i = GetObjects().begin(); i != GetObjects().end(); ++i)
        i->second->SaveTo(hFile, "OIndex");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("MobileIndexes");
    for(IMobileIter i = GetMobiles().begin(); i != GetMobiles().end(); ++i)
        i->second->SaveTo(hFile, "MIndex");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("RoomAmbience");
    for(ULONG i = 0; i < mRoomAmbience.size(); ++i)
        mRoomAmbience[i]->SaveTo(hFile, "RAmbience");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("MobAmbience");
    for(ULONG i = 0; i < mMobAmbience.size(); ++i)
        mMobAmbience[i]->SaveTo(hFile, "MAmbience");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("Populations");
    for(ULONG i = 0; i < mPopulations.size(); ++i)
        mPopulations[i]->SaveTo(hFile, "Population");
    hFile.EndTag();
    }
}



bool Area::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchShort  ("Anum"         , mANum         );
    MatchStr    ("Name"         , mName         );
    MatchStr    ("FileName"     , mFileName     );
    MatchShort  ("LowRange"     , mLowRange     );
    MatchShort  ("HighRange"    , mHighRange    );
    MatchShort  ("RespawnRate"  , mRespawnRate  );
    MatchShort  ("Version"      , mVersion      );
    MatchCall   ("Builders"     , mBuilders.LoadValuesFrom(hXML, hTag));
    
    MatchIf("RoomIndexes")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "RIndex");
            APtr<RoomIndex> nIndex(new RoomIndex());
            nIndex->SpottedThing::LoadValuesFrom(hXML, *nScanTag);
            AddRoomIndex(nIndex.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    MatchIf("ObjectIndexes")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "OIndex");
            APtr<ObjectIndex> nIndex(new ObjectIndex());
            nIndex->LoadValuesFrom(hXML, *nScanTag);
            AddObjectIndex(nIndex.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    MatchIf("MobileIndexes")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "MIndex");
            APtr<MobileIndex> nIndex(new MobileIndex());
            nIndex->LoadValuesFrom(hXML, *nScanTag);
            AddMobileIndex(nIndex.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    MatchIf("RoomAmbience")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "RAmbience");
            APtr<Ambience> nAmbience(new Ambience());
            nAmbience->LoadValuesFrom(hXML, *nScanTag);         
            mRoomAmbience.AddBack(nAmbience.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    MatchIf("MobAmbience")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "MAmbience");
            APtr<Ambience> nAmbience(new Ambience());
            nAmbience->LoadValuesFrom(hXML, *nScanTag);         
            mMobAmbience.AddBack(nAmbience.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    MatchIf("Populations")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Population");
            APtr<Population> nPopulation;
            if(nScanTag->GetAttributes()[kAttr_ObjectClass] == "ObjectPopulation")
                nPopulation = new ObjectPopulation(false);
            else if(nScanTag->GetAttributes()[kAttr_ObjectClass] == "MobilePopulation")
                nPopulation = new MobilePopulation();
            else if(nScanTag->GetAttributes()[kAttr_ObjectClass] == "ExitPopulation")
                nPopulation = new ExitPopulation();
            else
                ASSERT(!"Unknown Population Subclass!");

            nPopulation->LoadValuesFrom(hXML, *nScanTag);
            mPopulations.AddBack(nPopulation.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void RoomIndex::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "RoomIndex");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    RoomIndex::SaveValuesTo(hFile); 
    SpottedThing::SaveValuesTo(hFile);
    ScriptedThing::SaveValuesTo(hFile);
    hFile.EndTag();
}



void RoomIndex::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong     ("VRef"         , mVRef         );
    hFile.WriteString   ("Name"         , mName         );
    hFile.WriteString   ("Description"  , mDescription  );
    hFile.WriteULong    ("Owner"        , mOwner        );
    hFile.WriteShort    ("Clan"         , mClan         );
    hFile.WriteLong     ("Flags"        , mFlags        );
    hFile.WriteShort    ("Terrain"      , mTerrain      );
    hFile.WriteShort    ("Capacity"     , mCapacity     );
    hFile.WriteShort    ("AmbFreq"      , mAmbFreq      );
    hFile.WriteShort    ("HitBonus"     , mHitBonus     );
    hFile.WriteShort    ("EnergyBonus"  , mEnergyBonus  );
    hFile.WriteShort    ("MoveBonus"    , mMoveBonus    );
    for(ULONG i = 0; i < kDirMax; ++i)
    if(mExits[i])
        mExits[i]->SaveTo(hFile, "Exit");
}



bool RoomIndex::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchLong   ("VRef"         , mVRef         );
    MatchStr    ("Name"         , mName         );
    MatchStr    ("Description"  , mDescription  );
    MatchULong  ("Owner"        , mOwner        );
    MatchShort  ("Clan"         , mClan         );
    MatchLong   ("Flags"        , mFlags        );
    MatchShort  ("Terrain"      , mTerrain      );
    MatchShort  ("Capacity"     , mCapacity     );
    MatchShort  ("AmbFreq"      , mAmbFreq      );
    MatchShort  ("HitBonus"     , mHitBonus     );
    MatchShort  ("EnergyBonus"  , mEnergyBonus  );
    MatchShort  ("MoveBonus"    , mMoveBonus    );
    
    MatchIf("Exit")
    {
        APtr<Exit> nExit(new Exit(this));
        nExit->LoadValuesFrom(hXML, hTag);
        ASSERT(nExit->mDirection >= 0 && nExit->mDirection < kDirMax);
        mExits[nExit->mDirection] = nExit;
        nExit.Detach();
        return true;
    }
    
    //Check other classes
    if(SpottedThing::MatchValues(hXML, hTag))
        return true;
    if(ScriptedThing::MatchValues(hXML, hTag))
        return true;
    return false;
}



void Exit::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Exit");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Exit::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Exit::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong     ("ToVRef"       , mToVRef       );
    hFile.WriteLong     ("Flags"        , mFlags        );
    hFile.WriteLong     ("Key"          , mKey          );
    hFile.WriteByte     ("Direction"    , mDirection    );
    hFile.WriteString   ("Name"         , mName         );
    hFile.WriteString   ("Keywords"     , mKeywords     );
    hFile.WriteString   ("Description"  , mDescription  );
}



bool Exit::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchLong   ("ToVRef"       , mToVRef       );
    MatchLong   ("Flags"        , mFlags        );
    MatchLong   ("Key"          , mKey          );
    MatchByte   ("Direction"    , mDirection    );
    MatchStr    ("Name"         , mName         );
    MatchStr    ("Keywords"     , mKeywords     );
    MatchStr    ("Description"  , mDescription  );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void ObjectIndex::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "ObjectIndex");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    ObjectIndex::SaveValuesTo(hFile);   
    SpottedThing::SaveValuesTo(hFile);
    ScriptedThing::SaveValuesTo(hFile);
    hFile.EndTag();
}



void ObjectIndex::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong     ("VRef"             , mVRef         );
    hFile.WriteString   ("Name"             , mName             );
    hFile.WriteString   ("PName"            , mPName            );
    hFile.WriteString   ("Keywords"         , mKeywords         );
    hFile.WriteString   ("ShortDesc"        , mShortDesc        );
    hFile.WriteString   ("PShortDesc"       , mPShortDesc       );
    hFile.WriteString   ("Description"      , mDescription      );
    hFile.WriteFloat    ("ContCapacity"     , mContCapacity     );
    hFile.WriteLong     ("ContMultiplier"   , mContMultiplier   );
    hFile.WriteLong     ("ContKey"          , mContKey          );
    hFile.WriteLong     ("ContFlags"        , mContFlags        );
    hFile.WriteLong     ("Cost"             , mCost             );
    hFile.WriteShort    ("Durability"       , mDurability       );
    hFile.WriteShort    ("ItemType"         , mItemType         );
    hFile.WriteShort    ("Level"            , mLevel            );
    hFile.WriteFloat    ("Weight"           , mWeight           );
    hFile.WriteLong     ("Materials"        , mMaterials        );
    hFile.WriteLong     ("AntiFlags"        , mAntiFlags        );
    hFile.WriteLong     ("ObjectFlags"      , mObjectFlags      );
    hFile.WriteLong     ("WearFlags"        , mWearFlags        );
    hFile.WriteLong     ("Value0"           , mValue[0]         );
    hFile.WriteLong     ("Value1"           , mValue[1]         );
    hFile.WriteLong     ("Value2"           , mValue[2]         );
    hFile.WriteLong     ("Value3"           , mValue[3]         );
    hFile.WriteLong     ("Value4"           , mValue[4]         );
}



bool ObjectIndex::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchLong   ("VRef"             , mVRef             );
    MatchStr    ("Name"             , mName             );
    MatchStr    ("PName"            , mPName            );
    MatchStr    ("Keywords"         , mKeywords         );
    MatchStr    ("ShortDesc"        , mShortDesc        );
    MatchStr    ("PShortDesc"       , mPShortDesc       );
    MatchStr    ("Description"      , mDescription      );
    MatchFloat  ("ContCapacity"     , mContCapacity     );
    MatchLong   ("ContMultiplier"   , mContMultiplier   );
    MatchLong   ("ContKey"          , mContKey          );
    MatchLong   ("ContFlags"        , mContFlags        );
    MatchLong   ("Cost"             , mCost             );
    MatchShort  ("Durability"       , mDurability       );
    MatchShort  ("ItemType"         , mItemType         );
    MatchShort  ("Level"            , mLevel            );
    MatchFloat  ("Weight"           , mWeight           );
    MatchLong   ("Materials"        , mMaterials        );
    MatchLong   ("AntiFlags"        , mAntiFlags        );
    MatchLong   ("ObjectFlags"      , mObjectFlags      );
    MatchLong   ("WearFlags"        , mWearFlags        );
    MatchLong   ("Value0"           , mValue[0]         );
    MatchLong   ("Value1"           , mValue[1]         );
    MatchLong   ("Value2"           , mValue[2]         );
    MatchLong   ("Value3"           , mValue[3]         );
    MatchLong   ("Value4"           , mValue[4]         );  
    
    //Check other classes
    if(SpottedThing::MatchValues(hXML, hTag))
        return true;
    if(ScriptedThing::MatchValues(hXML, hTag))
        return true;
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void MobileIndex::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "MobileIndex");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    MobileIndex::SaveValuesTo(hFile);   
    SpottedThing::SaveValuesTo(hFile);
    ScriptedThing::SaveValuesTo(hFile);
    hFile.EndTag();
}



void MobileIndex::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong     ("VRef"             , mVRef                 );
    hFile.WriteString   ("Name"             , mName                 );
    hFile.WriteString   ("PName"            , mPName                );
    hFile.WriteString   ("Keywords"         , mKeywords             );
    hFile.WriteString   ("ShortDesc"        , mShortDesc            );
    hFile.WriteString   ("PShortDesc"       , mPShortDesc           );
    hFile.WriteString   ("Description"      , mDescription          );
    hFile.WriteString   ("Movement"         , mMovement             );
    hFile.WriteByte     ("Sex"              , mSex                  );
    hFile.WriteShort    ("Level"            , mLevel                );
    hFile.WriteByte     ("Alignment"        , mAlignment            );
    hFile.WriteString   ("Size"             , gSizeTable[mSize].mName);
    hFile.WriteString   ("DefaultPos"       , gPosTable[mDefaultPos].mName);
    hFile.WriteShort    ("Ridable"          , mRidable              );
    hFile.WriteChar     ("ResColdMod"       , mResistMod[kResCold]  );
    hFile.WriteChar     ("ResFireMod"       , mResistMod[kResFire]  );
    hFile.WriteChar     ("ResAcidMod"       , mResistMod[kResAcid]  );
    hFile.WriteChar     ("ResElecMod"       , mResistMod[kResElec]  );
    hFile.WriteChar     ("ResPoisMod"       , mResistMod[kResPois]  );
    hFile.WriteChar     ("ResMindMod"       , mResistMod[kResMind]  );
    hFile.WriteChar     ("ResSleeMod"       , mResistMod[kResSlee]  );
    hFile.WriteChar     ("ResCharMod"       , mResistMod[kResChar]  );
    hFile.WriteShort    ("StatStrMod"       , mStatMod[kStatStr]    );
    hFile.WriteShort    ("StatDexMod"       , mStatMod[kStatDex]    );
    hFile.WriteShort    ("StatIntMod"       , mStatMod[kStatInt]    );
    hFile.WriteShort    ("StatWisMod"       , mStatMod[kStatWis]    );
    hFile.WriteShort    ("StatConMod"       , mStatMod[kStatCon]    );
    hFile.WriteShort    ("ExpMod"           , mExpMod               );
    hFile.WriteShort    ("HitsMod"          , mHitsMod              );
    hFile.WriteShort    ("EnergyMod"        , mEnergyMod            );
    hFile.WriteShort    ("MovesMod"         , mMovesMod             );
    hFile.WriteLong     ("HitsMin"          , mHits[0]              );
    hFile.WriteLong     ("HitsMax"          , mHits[1]              );
    hFile.WriteLong     ("EnergyMin"        , mEnergy[0]            );
    hFile.WriteLong     ("EnergyMax"        , mEnergy[1]            );
    hFile.WriteLong     ("MovesMin"         , mMoves[0]             );
    hFile.WriteLong     ("MovesMax"         , mMoves[1]             );
    hFile.WriteLong     ("VulnerableFlags"  , mVunerableFlags       );
    hFile.WriteLong     ("DefensiveFlags"   , mDefensiveFlags       );
    hFile.WriteLong     ("ActFlags"         , mActFlags             );
    hFile.WriteShort    ("AttacksPerRound"  , mAPR                  );
    
    {
    hFile.BeginSuperTag("Attacks");
    for(ULONG i = 0; i < mAttacks.size(); ++i)
        mAttacks[i]->SaveTo(hFile, "Attack");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("Parts");
    for(ULONG i = 0; i < mParts.size(); ++i)
        mParts[i]->SaveTo(hFile, "Part");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("Skins");
    for(ULONG i = 0; i < mSkins.size(); ++i)
        mSkins[i]->SaveTo(hFile, "Skin");
    hFile.EndTag();
    }
}   



bool MobileIndex::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchLong   ("VRef"             , mVRef             );
    MatchStr    ("Name"             , mName             );
    MatchStr    ("PName"            , mPName            );
    MatchStr    ("Keywords"         , mKeywords         );
    MatchStr    ("ShortDesc"        , mShortDesc        );
    MatchStr    ("PShortDesc"       , mPShortDesc       );
    MatchStr    ("Description"      , mDescription      );
    MatchStr    ("Movement"         , mMovement         );
    MatchByte   ("Sex"              , mSex              );
    MatchShort  ("Level"            , mLevel            );
    MatchByte   ("Alignment"        , mAlignment        );
    MatchIf("Size")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mSize = TableLookup(gSizeTable, hTag.AsCStr(), true);
        return true;
    }
    MatchIf("DefaultPos")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mDefaultPos = TableLookup(gPosTable, hTag.AsCStr(), true);
        return true;
    }
    MatchShort  ("Ridable"          , mRidable              );
    MatchChar   ("ResColdMod"       , mResistMod[kResCold]  );
    MatchChar   ("ResFireMod"       , mResistMod[kResFire]  );
    MatchChar   ("ResAcidMod"       , mResistMod[kResAcid]  );
    MatchChar   ("ResElecMod"       , mResistMod[kResElec]  );
    MatchChar   ("ResPoisMod"       , mResistMod[kResPois]  );
    MatchChar   ("ResMindMod"       , mResistMod[kResMind]  );
    MatchChar   ("ResSleeMod"       , mResistMod[kResSlee]  );
    MatchChar   ("ResCharMod"       , mResistMod[kResChar]  );
    MatchShort  ("StatStrMod"       , mStatMod[kStatStr]    );
    MatchShort  ("StatDexMod"       , mStatMod[kStatDex]    );
    MatchShort  ("StatIntMod"       , mStatMod[kStatInt]    );
    MatchShort  ("StatWisMod"       , mStatMod[kStatWis]    );
    MatchShort  ("StatConMod"       , mStatMod[kStatCon]    );
    MatchShort  ("ExpMod"           , mExpMod               );
    MatchShort  ("HitsMod"          , mHitsMod              );
    MatchShort  ("EnergyMod"        , mEnergyMod            );
    MatchShort  ("MovesMod"         , mMovesMod             );
    MatchLong   ("HitsMin"          , mHits[0]              );
    MatchLong   ("HitsMax"          , mHits[1]              );
    MatchLong   ("EnergyMin"        , mEnergy[0]            );
    MatchLong   ("EnergyMax"        , mEnergy[1]            );
    MatchLong   ("MovesMin"         , mMoves[0]             );
    MatchLong   ("MovesMax"         , mMoves[1]             );
    MatchLong   ("VulnerableFlags"  , mVunerableFlags       );
    MatchLong   ("DefensiveFlags"   , mDefensiveFlags       );
    MatchLong   ("ActFlags"         , mActFlags             );
    MatchShort  ("AttacksPerRound"  , mAPR                  );
    
    MatchIf("Attacks")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Attack");
            APtr<Attack> nAttack(new Attack());
            nAttack->LoadValuesFrom(hXML, *nScanTag);
            mAttacks.AddPtrAt(nAttack, nAttack->mNumber-1);
            nAttack.Detach();
        }
        hXML.CloseTag();
        return true;
    }

    MatchIf("Parts")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Part");
            APtr<Part> nPart(new Part());
            nPart->LoadValuesFrom(hXML, *nScanTag);
            mParts.AddBack(nPart.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    MatchIf("Skins")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Skin");
            APtr<Skin> nSkin(new Skin());
            nSkin->LoadValuesFrom(hXML, *nScanTag);
            mSkins.AddBack(nSkin.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    
    //Check other classes
    if(SpottedThing::MatchValues(hXML, hTag))
        return true;
    if(ScriptedThing::MatchValues(hXML, hTag))
        return true;
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Attack::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Attack");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Attack::SaveValuesTo(hFile);    
    hFile.EndTag();
}



void Attack::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort    ("Number"       , mNumber       );
    hFile.WriteString   ("Message"      , mMessage      );
    hFile.WriteByte     ("Chance"       , mChance       );
    hFile.WriteByte     ("Type"         , mType         );
    hFile.WriteLong     ("Flags"        , mFlags        );
    hFile.WriteShort    ("HitMod"       , mHitMod       );
    hFile.WriteShort    ("DamMod"       , mDamMod       );
    hFile.WriteByte     ("DamType"      , mDamageType   );
}



bool Attack::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchShort  ("Number"       , mNumber       );
    MatchStr    ("Message"      , mMessage      );
    MatchByte   ("Chance"       , mChance       );
    MatchByte   ("Type"         , mType         );
    MatchLong   ("Flags"        , mFlags        );
    MatchShort  ("HitMod"       , mHitMod       );
    MatchShort  ("DamMod"       , mDamMod       );
    MatchByte   ("DamType"      , mDamageType   );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Part::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Part");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Part::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Part::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString   ("Name"     , mName     );
    hFile.WriteShort    ("ACMod"    , mACMod    );
    hFile.WriteShort    ("Size"     , mSize     );
    hFile.WriteLong     ("WearBits" , mWearBits );
}



bool Part::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchStr    ("Name"     , mName     );
    MatchShort  ("ACMod"    , mACMod    );
    MatchShort  ("Size"     , mSize     );
    MatchLong   ("WearBits" , mWearBits );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Skin::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Skin");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Skin::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Skin::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteByte     ("Chance"   , mChance   );
    hFile.WriteShort    ("Number"   , mNumber   );
    hFile.WriteLong     ("VRef"     , mVRef     );
}



bool Skin::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchByte   ("Chance"   , mChance   );
    MatchShort  ("Number"   , mNumber   );
    MatchLong   ("VRef"     , mVRef     );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void SpottedThing::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    ASSERT(!"Shouldnt get called");
}



void SpottedThing::SaveValuesTo(XMLWriter& hFile)
{
    hFile.BeginSuperTag("SpotList");
    for(SpotIter i = mSpots.begin(); i != mSpots.end(); ++i)
        (*i)->SaveTo(hFile, "Spot");
    hFile.EndTag();
}



bool SpottedThing::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchIf("SpotList")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Spot");
            APtr<Spot> nSpot(new Spot());
            nSpot->LoadValuesFrom(hXML, *nScanTag);
            mSpots.AddBack(nSpot.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    return false;
}



void Spot::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Spot");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Spot::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Spot::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString   ("Name"         , mName         );
    hFile.WriteString   ("Keywords"     , mKeywords     );
    hFile.WriteString   ("Description"  , mDescription  );
    hFile.WriteLong     ("Flags"        , mFlags        );
}



bool Spot::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchStr    ("Name"         , mName         );
    MatchStr    ("Keywords"     , mKeywords     );
    MatchStr    ("Description"  , mDescription  );
    MatchLong   ("Flags"        , mFlags        );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void ScriptedThing::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    ASSERT(!"Shouldnt get called");
}



void ScriptedThing::SaveValuesTo(XMLWriter& hFile)
{
    hFile.BeginSuperTag("ScriptList");
    for(OScriptIter i = mScriptMap.begin(); i != mScriptMap.end(); ++i)
        i->second->SaveTo(hFile, "Script");
    hFile.EndTag();
}



bool ScriptedThing::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchIf("ScriptList")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Script");
            APtr<OrangeScript> nOrangeScript(new OrangeScript());
            nOrangeScript->LoadValuesFrom(hXML, *nScanTag);
            ASSERT(mScriptMap.find(nOrangeScript->mID) == mScriptMap.end());
            mScriptMap[nOrangeScript->mID] = nOrangeScript;
            nOrangeScript->mOnThis = this;
            nOrangeScript.Detach();
        }
        hXML.CloseTag();
        return true;
    }
    return false;
}



void OrangeScript::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "OrangeScript");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    OrangeScript::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void OrangeScript::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString   ("Source"       , mSource.c_str()   );
    hFile.WriteString   ("Commands"     , mCommands         );
    hFile.WriteString   ("Keywords"     , mKeywords         );
    hFile.WriteShort    ("Type"         , mType             );
    hFile.WriteShort    ("Trigger"      , mTrigger          );
    hFile.WriteLong     ("Flags"        , mFlags            );
    hFile.WriteUShort   ("ID"           , mID               );
    for(OScriptVariableIter i = mTextVariables.begin(); i != mTextVariables.end(); ++i)
        (*i)->SaveTo(hFile, "TextVar");
}



bool OrangeScript::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchStr    ("Source"       , mSource       );
    MatchStr    ("Commands"     , mCommands     );
    MatchStr    ("Keywords"     , mKeywords     );
    MatchShort  ("Type"         , mType         );
    MatchShort  ("Trigger"      , mTrigger      );
    MatchLong   ("Flags"        , mFlags        );
    MatchUShort ("ID"           , mID           );
    MatchIf("TextVar")
    {
        APtr<OrangeScriptVariable> nSVar(new OrangeScriptVariable(gDTDef_STRING));
        nSVar->LoadValuesFrom(hXML, hTag);
        mTextVariables.AddBack(nSVar.Detach());
        return true;
    }
    return false;
}



void OrangeScriptVariable::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "OrangeScriptVariable");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    OrangeScriptVariable::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void OrangeScriptVariable::SaveValuesTo(XMLWriter& hFile)
{
    STRING& nStringVal = *((STRING*)mValue->mValuePtr);
    hFile.WriteString("Name"    , mName.c_str()     );
    hFile.WriteString("Value"   , nStringVal.c_str());
}



bool OrangeScriptVariable::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    STRING& nStringVal = static_cast<DataTypeSTRING*>(mValue.Ptr())->mString;
    MatchStr("Name"     , mName         );
    MatchStr("Value"    , nStringVal    );
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Ambience::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Ambience");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Ambience::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Ambience::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort    ("Number"   , mNumber   );
    hFile.WriteString   ("Message"  , mMessage  );
    for(ULONG i = 0; i < mVnums.size(); ++i)
        hFile.WriteUShort("VNum", mVnums[i]);
}



bool Ambience::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchShort  ("Number"   , mNumber                           );
    MatchStr    ("Message"  , mMessage                          );
    MatchIf("VNum")
    {
        hXML.ReadTagContents();
        hXML.CloseTag();
        mVnums.AddBack(hTag.AsUShort());
        return true;
    }
    return false;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Population::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort    ("Number"   , mNumber       );
    hFile.WriteLong     ("RoomVRef" , mRoomVRef     );
}



bool Population::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchShort  ("Number"   , mNumber       );
    MatchLong   ("RoomVRef" , mRoomVRef     );
    return false;
}



void ExitPopulation::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "ExitPopulation");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Population::SaveValuesTo(hFile);    
    ExitPopulation::SaveValuesTo(hFile);    
    hFile.EndTag();
}



void ExitPopulation::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort("Direction"    , mDirection    );
    hFile.WriteLong ("Flags"        , mFlags        );
}



bool ExitPopulation::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchShort  ("Direction"    , mDirection    );
    MatchLong   ("Flags"        , mFlags        );

    //Check superclass
    return Population::MatchValues(hXML, hTag);
}



void MobilePopulation::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "MobilePopulation");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Population::SaveValuesTo(hFile);    
    MobilePopulation::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void MobilePopulation::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong ("MobileVRef"   , mMobileVRef   );
    hFile.WriteShort("InPosition"   , mInPosition   );
    hFile.WriteShort("Chance"       , mChance       );

    {
    hFile.BeginSuperTag("InvObjs");
    for(ULONG i = 0; i < mObjectsInside.size(); ++i)
        mObjectsInside[i]->SaveTo(hFile, "InvPop");
    hFile.EndTag();
    }

    {
    hFile.BeginSuperTag("ShopObjs");
    for(ULONG i = 0; i < mShopObjects.size(); ++i)
        mShopObjects[i]->SaveTo(hFile, "ShopPop");
    hFile.EndTag();
    }
}



bool MobilePopulation::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchLong   ("MobileVRef"   , mMobileVRef   );
    MatchShort  ("InPosition"   , mInPosition   );
    MatchShort  ("Chance"       , mChance       );

    MatchIf("InvObjs")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "InvPop");
            APtr<ObjectPopulation> nOPop(new ObjectPopulation(false));
            nOPop->LoadValuesFrom(hXML, *nScanTag);
            mObjectsInside.AddBack(nOPop.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    MatchIf("ShopObjs")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "ShopPop");
            APtr<ObjectPopulation> nOPop(new ObjectPopulation(false));
            nOPop->LoadValuesFrom(hXML, *nScanTag);
            mShopObjects.AddBack(nOPop.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    //Check superclass
    return Population::MatchValues(hXML, hTag);
}


void ObjectPopulation::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "ObjectPopulation");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Population::SaveValuesTo(hFile);    
    ObjectPopulation::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void ObjectPopulation::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteLong ("ObjectVRef"   , mObjectVRef   );
    hFile.WriteLong ("ObjectFlags"  , mObjectFlags  );
    hFile.WriteByte ("ContFlags"    , mContFlags    );
    hFile.WriteLong ("LoadMin"      , mLoadMin      );
    hFile.WriteLong ("LoadMax"      , mLoadMax      );
    hFile.WriteByte ("Chance"       , mChance       );
    hFile.WriteShort("Liquid"       , mLiquid       );
    hFile.WriteShort("ToWearLoc"    , mToWearLoc    );

    hFile.BeginSuperTag("ObjectsInside");
    for(ULONG i = 0; i < mPopulationsInside.size(); ++i)
        mPopulationsInside[i]->SaveTo(hFile, "ObjPop");
    hFile.EndTag();
}



bool ObjectPopulation::MatchValues(XMLReader& hXML, XMLTag& hTag)
{   
    MatchLong   ("ObjectVRef"   , mObjectVRef   );
    MatchLong   ("ObjectFlags"  , mObjectFlags  );
    MatchByte   ("ContFlags"    , mContFlags    );
    MatchLong   ("LoadMin"      , mLoadMin      );
    MatchLong   ("LoadMax"      , mLoadMax      );
    MatchByte   ("Chance"       , mChance       );
    MatchShort  ("Liquid"       , mLiquid       );
    MatchShort  ("ToWearLoc"    , mToWearLoc    );
    
    MatchIf("ObjectsInside")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "ObjPop");
            APtr<ObjectPopulation> nOPop(new ObjectPopulation(false));
            nOPop->LoadValuesFrom(hXML, *nScanTag);
            mPopulationsInside.AddBack(nOPop.Detach());
        }
        hXML.CloseTag();
        return true;
    }
    
    //Check superclass
    return Population::MatchValues(hXML, hTag);
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool HelpFile::Save()
{
    FilePath nPath(kPath_HelpFiles); 
    APtr<FileLink> nLink;

    //Check that "Resources/HelpFiles/" directory is here
    if(!FileManager::Get()->Exists(nPath))
        FileManager::Get()->MakeDir(nPath);
    nPath.Add(STRING(*mFileName));
    
    //Backup Old File
    if(FileManager::Get()->Exists(nPath))
    {
        FilePath nBackupPath = nPath;
        nBackupPath.mPathStrs.back() += ".bak";
        FileManager::Get()->Move(nPath, nBackupPath);
    }
    
    //Save New File
    if(!(nLink = FileManager::Get()->OpenWrite(nPath)))
        return false;
    
    XMLWriter nXMLWriter(nLink);
    nXMLWriter.BeginFile(); 
    SaveTo(nXMLWriter, "HelpFile");
    nXMLWriter.EndFile();
    nLink->Close();
    return true;
}


void HelpFile::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "HelpFile");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    HelpFile::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void HelpFile::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString("Name"    , mName     );
    hFile.WriteString("FileName", mFileName );
    
    hFile.BeginSuperTag("Helps");
    for(HelpList::iterator i = mHelps.begin(); i != mHelps.end(); ++i)
        (*i)->SaveTo(hFile, "Help");
    hFile.EndTag();
}



bool HelpFile::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchStr("Name"     , mName     );
    MatchStr("FileName" , mFileName );
    
    MatchIf("Helps")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Help");
            APtr<Help> nHelp(new Help());
            nHelp->LoadValuesFrom(hXML, *nScanTag);
            nHelp->mInFile = this;
            
            if(!StrCmp(nHelp->mKeywords, "greeting"))
                MUD::Get()->mHelpGreeting = nHelp;
            if(!StrCmp(nHelp->mKeywords, "menu"))
                MUD::Get()->mHelpMenu = nHelp;
            if(!StrCmp(nHelp->mKeywords, "motd"))
                MUD::Get()->mHelpMOTD = nHelp;
                
            mHelps.Add(nHelp.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    return false;
}



void Help::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Help");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Help::SaveValuesTo(hFile);  
    hFile.EndTag();
}



void Help::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteShort    ("Level"    , mLevel    );
    hFile.WriteShort    ("Category" , mCategory );
    hFile.WriteString   ("Title"    , mTitle    );
    hFile.WriteString   ("Keywords" , mKeywords );
    hFile.WriteString   ("Summary"  , mSummary  );
    hFile.WriteString   ("Syntax"   , mSyntax   );
    hFile.WriteString   ("Text"     , mText     );
    hFile.WriteString   ("SeeAlso"  , mSeeAlso  );
}



bool Help::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchShort  ("Level"    , mLevel    );
    MatchShort  ("Category" , mCategory );
    MatchStr    ("Title"    , mTitle    );
    MatchStr    ("Keywords" , mKeywords );
    MatchStr    ("Summary"  , mSummary  );
    MatchStr    ("Syntax"   , mSyntax   );
    MatchStr    ("Text"     , mText     );
    MatchStr    ("SeeAlso"  , mSeeAlso  );
    return false;
}






#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool SocialFile::Save()
{
    FilePath nPath(kPath_SocialFiles); 
    APtr<FileLink> nLink;

    //Check that "Resources/SocialFiles/" directory is here
    if(!FileManager::Get()->Exists(nPath))
        FileManager::Get()->MakeDir(nPath);
    nPath.Add(STRING(*mFileName));
    
    //Backup Old File
    if(FileManager::Get()->Exists(nPath))
    {
        FilePath nBackupPath = nPath;
        nBackupPath.mPathStrs.back() += ".bak";
        FileManager::Get()->Move(nPath, nBackupPath);
    }
    
    //Save New File
    if(!(nLink = FileManager::Get()->OpenWrite(nPath)))
        return false;
    
    XMLWriter nXMLWriter(nLink);
    nXMLWriter.BeginFile(); 
    SaveTo(nXMLWriter, "SocialFile");
    nXMLWriter.EndFile();
    nLink->Close();
    return true;
}


void SocialFile::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "SocialFile");
        
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    SocialFile::SaveValuesTo(hFile);    
    hFile.EndTag();
}



void SocialFile::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString("Name"    , mName     );
    hFile.WriteString("FileName", mFileName );
    
    hFile.BeginSuperTag("Socials");
    for(SocialList::iterator i = mSocials.begin(); i != mSocials.end(); ++i)
        (*i)->SaveTo(hFile, "Social");
    hFile.EndTag();
}



bool SocialFile::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchStr("Name"     , mName     );
    MatchStr("FileName" , mFileName );
    
    MatchIf("Socials")
    {
        XMLTag* nScanTag;
        while(hXML.OpenTag(nScanTag))
        {
            ASSERT(nScanTag->GetName() == "Social");
            APtr<Social> nSocial(new Social());
            nSocial->LoadValuesFrom(hXML, *nScanTag);
            nSocial->mInFile = this;                
            mSocials.Add(nSocial.Detach());
        }
        hXML.CloseTag();
        return true;
    }

    return false;
}



void Social::SaveTo(XMLWriter& hFile, const CHAR* hTagName)
{
    XMLAttributes   nXMLAttributes;
    nXMLAttributes.AddString(kAttr_ObjectClass, "Social");
    
    hFile.BeginSuperTag(hTagName, &nXMLAttributes);
    Social::SaveValuesTo(hFile);    
    hFile.EndTag();
}



void Social::SaveValuesTo(XMLWriter& hFile)
{
    hFile.WriteString("Name"        , mName         );
    hFile.WriteString("CharNoArg"   , mCharNoArg    );
    hFile.WriteString("OthersNoArg" , mOthersNoArg  );
    hFile.WriteString("CharFound"   , mCharFound    );
    hFile.WriteString("OthersFound" , mOthersFound  );
    hFile.WriteString("VictFound"   , mVictFound    );
    hFile.WriteString("CharAuto"    , mCharAuto     );
    hFile.WriteString("OthersAuto"  , mOthersAuto   );
}



bool Social::MatchValues(XMLReader& hXML, XMLTag& hTag)
{
    MatchStr("Name"         , mName         );
    MatchStr("CharNoArg"    , mCharNoArg    );
    MatchStr("OthersNoArg"  , mOthersNoArg  );
    MatchStr("CharFound"    , mCharFound    );
    MatchStr("OthersFound"  , mOthersFound  );
    MatchStr("VictFound"    , mVictFound    );
    MatchStr("CharAuto"     , mCharAuto     );
    MatchStr("OthersAuto"   , mOthersAuto   );
    return false;
}



