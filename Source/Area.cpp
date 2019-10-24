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
#include "Attack.h"
#include "Balance.h"
#include "MobileIndex.h"
#include "MobilePopulation.h"
#include "MUD.h"
#include "ObjectIndex.h"
#include "OrangeScript.h"
#include "Population.h"
#include "RoomIndex.h"
#include "Spot.h"
#include "StringMagic.h"
#include "StringUtils.h"


AreaInfo::AreaInfo()
    : mTotalLines(0)
    , mRoomLines(0)
    , mObjectLines(0)
    , mMobileLines(0)
    , mRoomSpotLines(0)
    , mObjectSpotLines(0)
    , mMobileSpotLines(0)
    , mRoomScriptLines(0)
    , mObjectScriptLines(0)
    , mMobileScriptLines(0)
    , mRoomAmbienceLines(0)
    , mMobileAmbienceLines(0)
    , mRepopLines(0)
    , mShopLines(0)
    , mRooms(0)
    , mRoomSpots(0)
    , mRoomScripts(0)
    , mObjects(0)
    , mObjectSpots(0)
    , mObjectScripts(0)
    , mMobiles(0)
    , mMobileSpots(0)
    , mMobileScripts(0)
    , mRoomAmbience(0)
    , mMobileAmbience(0)
    , mRepops(0)
    , mShops(0)
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




Area::Area()
    : mMinOldVnum(0)
    , mMaxOldVnum(0)
    , mNumPlayers(0)
    , mLastRespawn(0)
    , mRespawnRate(1)
    , mChanged(false)
{
}



Area::~Area()
{
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::StatTo
//
//   Desc:: Sends all the statistics about an Area to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::StatTo(Person* hTo)
{
    AreaInfo    nInfo;
    STRINGCW    nBuf, nTotalBuf, nPopBuf, nCreditBuf, nTitleBuf;

    GetInfo(nInfo);
    LONG nTotalAmbience = nInfo.mRoomAmbience + nInfo.mMobileAmbience;
    LONG nTotalAmbienceLines = nInfo.mRoomAmbienceLines + nInfo.mMobileAmbienceLines; 

    nTitleBuf.SPrintF("^T%s - Area Info^x", *mName);
    nTitleBuf = CenterLine(nTitleBuf);
    nTotalBuf.SPrintF("%d lines written", nInfo.mTotalLines);
    nPopBuf.SPrintF("Every %d minutes", mRespawnRate);
    
    Vector<STRINGCW> nBuildersNames;
    mBuilders.ResolveNames(nBuildersNames);
    nCreditBuf.Implode(nBuildersNames, ", ");

    nBuf.SPrintF(
    kGfxSep"\n"
    "%s\n"
    kGfxSep"\n"
    //////////////////////////////////////////////////////////////////////////////////////////
    "  File Name: %-40s   ^HRooms^x: %-4d [%d lines]\n"
    "  Area Name: %-40s Scripts: %-4d [%d lines]\n"
    "   Area Num: %-40d   Spots: %-4d [%d lines]\n" 
    "             %-40s ^HObjects^x: %-4d [%d lines]\n"
    "Total Lines: %-40s Scripts: %-4d [%d lines]\n"
    "             %-40s   Spots: %-4d [%d lines]\n"
    "  Min Level: %-40d ^HMobiles^x: %-4d [%d lines]\n"
    "  Max Level: %-40d Scripts: %-4d [%d lines]\n"
    " Population: %-40s   Spots: %-4d [%d lines]\n"
    "             %-40sAmbience: %-4d [%d lines]\n"
    "^UCredits...^x   %-40s  Repops: %-4d [%d lines]\n"
    "%-55s Shops: %-4d [%d lines]\n"
    kGfxThinSep"\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *nTitleBuf,
    *mFileName          , nInfo.mRooms          , nInfo.mRoomLines          ,
    *mName              , nInfo.mRoomScripts    , nInfo.mRoomScriptLines    ,
    mANum               , nInfo.mRoomSpots      , nInfo.mRoomSpotLines      ,
    ""                  , nInfo.mObjects        , nInfo.mObjectLines        ,
    *nTotalBuf          , nInfo.mObjectScripts  , nInfo.mObjectScriptLines  ,
    ""                  , nInfo.mObjectSpots    , nInfo.mObjectSpotLines    ,
    mLowRange           , nInfo.mMobiles        , nInfo.mMobileLines        ,
    mHighRange          , nInfo.mMobileScripts  , nInfo.mMobileScriptLines  ,
    *nPopBuf            , nInfo.mRoomAmbience   , nInfo.mMobileSpotLines    ,
    ""                  , nTotalAmbience        , nTotalAmbienceLines       ,
    ""                  , nInfo.mRepops         , nInfo.mRepops             ,
    *nCreditBuf         , nInfo.mShops          , nInfo.mShopLines);
    
    hTo->Send(nBuf);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::StatListTo
//
//   Desc:: Shows a Person a single line listing the Area, its name, and some
//          other information about it. Primarily used with Do_AList.
//
//  Input:: The Person to send the line of info to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::StatListTo(Person* hTo)
{
    Vector<STRINGCW> nEditors;
    
    //** Build List Of Builders Editing The Area **//
    if(!mEditors.empty())
    {
        for(ULONG i = 0; i < mEditors.size(); ++i)
        {
            STRINGCW nEditorName;
            if(hTo->CanSee(mEditors[i]))
                nEditorName = mEditors[i]->mName;
            else
                nEditorName = "Someone";
            nEditors.Add(nEditorName);
        }
    }else{
        nEditors.Add(STRINGCW("<none>"));
    }
    
    
    //** Send Info **//
    hTo->SendPagedF(
    " %3d  %-15s  %-32s  %-13s %s\n",
    mANum, *mFileName, *mName, *nEditors[0], mChanged?"^!Yes^x":"No");
    
    for(ULONG i = 1; i < nEditors.size(); ++i)
        hTo->SendPagedF("%56s  %s\n", "", *nEditors[i]);
}



void Area::GetInfo(AreaInfo& hInfo)
{
    //Gather RoomIndex Info
    {
    for(IRoomCIter i = GetRooms().begin(); i != GetRooms().end(); ++i)
    {
        RoomIndex* nRoom = i->second;
        ++hInfo.mRooms;
        hInfo.mRoomLines += 1;
        hInfo.mRoomLines += CountLines(nRoom->mDescription);
        
        {
        for(ULONG j = 0; j < kDirMax; ++j)
        if(nRoom->mExits[j])
        {
            hInfo.mRoomLines += CountLines(nRoom->mExits[j]->mName);
            hInfo.mRoomLines += CountLines(nRoom->mExits[j]->mKeywords);
            hInfo.mRoomLines += CountLines(nRoom->mExits[j]->mDescription);
        }
        }
        {
        for(SpotCIter j = nRoom->mSpots.begin(); j != nRoom->mSpots.end(); ++j)
        {
            ++hInfo.mRoomSpots;
            hInfo.mRoomSpotLines += 2;
            hInfo.mRoomSpotLines += CountLines((*j)->mDescription);
        }
        }
        {
        for(OScriptCIter j = nRoom->mScriptMap.begin(); j != nRoom->mScriptMap.end(); ++j)
        {
            ++hInfo.mRoomScripts;
            hInfo.mRoomScriptLines += 2;
            hInfo.mRoomScriptLines += CountLines(j->second->mSource.c_str());
        }
        }
    }
    }
    
    //Gather ObjectIndex Info
    {
    for(IObjectCIter i = GetObjects().begin(); i != GetObjects().end(); ++i)
    {
        ObjectIndex* nObject = i->second;
        ++hInfo.mObjects;
        hInfo.mObjectLines += 5;
        hInfo.mObjectLines += CountLines(nObject->mDescription);
        
        {
        for(SpotCIter j = nObject->mSpots.begin(); j != nObject->mSpots.end(); ++j)
        {
            ++hInfo.mObjectSpots;
            hInfo.mObjectSpotLines += 2;
            hInfo.mObjectSpotLines += CountLines((*j)->mDescription);
        }
        }
        {
        for(OScriptCIter j = nObject->mScriptMap.begin(); j != nObject->mScriptMap.end(); ++j)
        {
            ++hInfo.mObjectScripts;
            hInfo.mObjectScriptLines += 2;
            hInfo.mObjectScriptLines += CountLines(j->second->mSource.c_str());
        }
        }
    }
    }
    
    //Gather MobileIndex Info
    {
    for(IMobileCIter i = GetMobiles().begin(); i != GetMobiles().end(); ++i)
    {
        MobileIndex* nMobile = i->second;
        ++hInfo.mMobiles;
        hInfo.mMobileLines += 6;
        hInfo.mMobileLines += CountLines(nMobile->mDescription);
        
        {
        for(ULONG j = 0; j < nMobile->mAttacks.size(); ++j)
            hInfo.mMobileLines += 2;

        }
        {
        for(ULONG j = 0; j < nMobile->mParts.size(); ++j)
            hInfo.mMobileLines += 1;

        }
        {
        for(ULONG j = 0; j < nMobile->mSkins.size(); ++j)
            hInfo.mMobileLines += 1;
        }
        {
        for(SpotCIter j = nMobile->mSpots.begin(); j != nMobile->mSpots.end(); ++j)
        {
            ++hInfo.mMobileSpots;
            hInfo.mMobileSpotLines += 2;
            hInfo.mMobileSpotLines += CountLines((*j)->mDescription);
        }
        }
        {
        for(OScriptCIter j = nMobile->mScriptMap.begin(); j != nMobile->mScriptMap.end(); ++j)
        {
            ++hInfo.mMobileScripts;
            hInfo.mMobileScriptLines += 2;
            hInfo.mMobileScriptLines += CountLines(j->second->mSource.c_str());
        }
        }
    }
    }

    //Gather Ambience Info
    {
    for(ULONG i = 0; i < mRoomAmbience.size(); ++i)
    {
        ++hInfo.mRoomAmbience;
        hInfo.mRoomAmbienceLines += CountLines(mRoomAmbience[i]->mMessage);
    }
    }
    {
    for(ULONG i = 0; i < mMobAmbience.size(); ++i)
    {
        ++hInfo.mMobileAmbience;
        hInfo.mMobileAmbienceLines += CountLines(mMobAmbience[i]->mMessage);
    }
    }
    
    //Gather Population Info
    hInfo.mRepops = mPopulations.size();
    hInfo.mRepopLines = mPopulations.size();

    //Gather Shop Info
    {
    for(ULONG i = 0; i < mPopulations.size(); i++)
    {
        MobilePopulation* nMPop = dynamic_cast<MobilePopulation*>(mPopulations[i]);
        if(nMPop && nMPop->mShopObjects.size())
        {
            ++hInfo.mShops;
            hInfo.mShopLines += nMPop->mShopObjects.size();
        }
    }
    }



    //Any SubTotaling?
    hInfo.mTotalLines += hInfo.mRoomLines;
    hInfo.mTotalLines += hInfo.mObjectLines;
    hInfo.mTotalLines += hInfo.mMobileLines;
    hInfo.mTotalLines += hInfo.mRoomSpotLines;
    hInfo.mTotalLines += hInfo.mObjectSpotLines;
    hInfo.mTotalLines += hInfo.mMobileSpotLines;
    hInfo.mTotalLines += hInfo.mRoomScriptLines;    
    hInfo.mTotalLines += hInfo.mObjectScriptLines;  
    hInfo.mTotalLines += hInfo.mMobileScriptLines;  
    hInfo.mTotalLines += hInfo.mRoomAmbienceLines;
    hInfo.mTotalLines += hInfo.mMobileAmbienceLines;
    hInfo.mTotalLines += hInfo.mRepopLines;
    hInfo.mTotalLines += hInfo.mShopLines;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::AddRoomIndex
//
//   Desc:: Adds a RoomIndex to this Area.
//
//  Input:: The RoomIndex to be added.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::AddRoomIndex(RoomIndex* hIndex)
{
    hIndex->mVRef = MakeVRef(mANum, hIndex->VNum());
    GetRooms()[hIndex->VNum()] = hIndex;
    hIndex->mInArea = this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::AddObjectIndex
//
//   Desc:: Adds an ObjectIndex to this Area.
//
//  Input:: The ObjectIndex to be added.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::AddObjectIndex(ObjectIndex* hIndex)
{
    hIndex->mVRef = MakeVRef(mANum, hIndex->VNum());
    GetObjects()[hIndex->VNum()] = hIndex;
    BalanceObject(hIndex);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::AddMobileIndex
//
//   Desc:: Adds a MobileIndex to this Area.
//
//  Input:: The MobileIndex to be added.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::AddMobileIndex(MobileIndex* hIndex)
{
    hIndex->mVRef = MakeVRef(mANum, hIndex->VNum());
    GetMobiles()[hIndex->VNum()] = hIndex;
    BalanceMobile(hIndex, kBalanceAttacksOnLoad);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::RePopulate
//
//   Desc:: Repopulates the entire area. Scans every Population object in the
//          area and Repopulates them if they need to be.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::RePopulate()
{
    for(ULONG i = 0; i < mPopulations.size(); ++i)
        mPopulations[i]->RePopulate();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::UpdateAmbience
//
//   Desc:: Sends out those lovely ambience messages to rooms that need it.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Area::UpdateAmbience()
{
    IRoomIter           nRoomScan;
    RoomIndex*          nRoom;
    AmbienceArray       nChoices;
    Vector<Person*>     nChoicesActs;
    STRINGCW            nSend(128);
    
    nChoices.reserve(64);
    nChoicesActs.reserve(64);
    for(nRoomScan = GetRooms().begin(); nRoomScan != GetRooms().end(); ++nRoomScan)
    {
        nRoom = (*nRoomScan).second;
        if(nRoom->mNumPlayers == 0)
            continue;
        if((gTime-nRoom->mLastAmbience) < nRoom->mAmbFreq)
            continue; //Randomize this at some point
    
        nChoices.clear();
        nChoicesActs.clear();
        nChoices.insert(nChoices.end(), nRoom->mAmbiences.begin(), nRoom->mAmbiences.end());
        nChoicesActs.insert(nChoicesActs.end(), nRoom->mAmbiences.size(), NULL);
        if(nRoom->mNumPlayers != nRoom->PersonHolder::Size())
        for(PersonIter i = nRoom->PersonHolder::Begin(); i != nRoom->PersonHolder::End(); ++i)
        {
            Mobile* nMobile = (*i)->isMobile;
            if(!nMobile)
                continue;
            nChoices.insert(nChoices.end(), nMobile->mIndex->mAmbiences.begin(), nMobile->mIndex->mAmbiences.end());
            nChoicesActs.insert(nChoicesActs.end(), nMobile->mIndex->mAmbiences.size(), nMobile);
        }

        if(!nChoices.empty())
        {
            ULONG nChoice = Random(0, nChoices.size()-1);
            Ambience*   nAmbience = nChoices[nChoice];
            
            nSend = nAmbience->mMessage;
            nSend += '\n';
            if(nChoicesActs[nChoice] == NULL)
                Act(nSend, nRoom, NULL, NULL, ToAll);
            else
                Act(nSend, nChoicesActs[nChoice], NULL, NULL, ToAll);
            nRoom->mLastAmbience = gTime + (Randomize(nRoom->mAmbFreq, 25.0)-nRoom->mAmbFreq);
        }
    }
    
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::FindRoomIndex
//
//   Desc:: Attempts to search for any valid RoomIndex inside this Area. It
//          searches using a textual argument that is something like "shop" or
//          "3.shop". The argument can also be a vnum for the room inside this
//          area.
//
//  Input:: Textual argument describing the RoomIndex you're looking for.
//
// Output:: A Non-NULL pointer to the RoomIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* Area::FindRoomIndex(STRINGCW& hName)
{
    //** By Room Number **//
    if(hName.IsNumber())
    {
        VNUM nVNum = (VNUM) hName.AsNumber();
        return GetRoomIndex(nVNum);
    }



    //** By Room Keywords **//
    LONG            nStartWhere, nWantAmount, nFound = 0;
    STRINGCW        nMatchArg;
    IRoomIter       nRoomScan;
    RoomIndex*      nRoom;
    bool            nAll;
    
    ParseMatching(hName, nStartWhere, nWantAmount, nMatchArg, nAll);
    for(nRoomScan = GetRooms().begin(); nRoomScan != GetRooms().end(); ++nRoomScan)
    {
        nRoom = (*nRoomScan).second;
    
        if(nRoom->mName.IsNamed(nMatchArg))
        if(++nFound == nStartWhere)
            return nRoom;
    }

    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::FindObjectIndex
//
//   Desc:: Attempts to search for any valid ObjectIndex inside this Area. It
//          searches using a textual argument that is something like "sword" or
//          "3.sword". The argument can also be a vnum for the object inside
//          this area.
//
//  Input:: Textual argument describing the ObjectIndex you're looking for.
//
// Output:: A Non-NULL pointer to the ObjectIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ObjectIndex* Area::FindObjectIndex(STRINGCW& hName)
{
    //** By Object Number **//
    if(hName.IsNumber())
    {
        VNUM nVNum = (VNUM) hName.AsNumber();
        return GetObjectIndex(nVNum);
    }
    
    
    //** By Object Keywords **//
    LONG            nStartWhere, nWantAmount, nFound = 0;
    STRINGCW        nMatchArg;
    IObjectIter     nObjectScan;
    ObjectIndex*    nObject;
    bool            nAll;
    
    ParseMatching(hName, nStartWhere, nWantAmount, nMatchArg, nAll);
    for(nObjectScan = GetObjects().begin(); nObjectScan != GetObjects().end(); ++nObjectScan)
    {
        nObject = (*nObjectScan).second;
    
        if(nObject->mKeywords.IsNamed(nMatchArg))
        if(++nFound == nStartWhere)
            return nObject;
    }

    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Area::FindMobileIndex
//
//   Desc:: Attempts to search for any valid MobileIndex inside this Area. It
//          searches using a textual argument that is something like "guard" or
//          "3.guard". The argument can also be a vnum for the mobile inside
//          this area.
//
//  Input:: Textual argument describing the MobileIndex you're looking for.
//
// Output:: A Non-NULL pointer to the MobileIndex if found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MobileIndex* Area::FindMobileIndex(STRINGCW& hName)
{
    //** By Mobile Number **//
    if(hName.IsNumber())
    {
        VNUM nVNum = (VNUM) hName.AsNumber();
        return GetMobileIndex(nVNum);
    }
    
    
    //** By Mobile Keywords **//
    LONG            nStartWhere, nWantAmount, nFound = 0;
    STRINGCW        nMatchArg;
    IMobileIter     nMobileScan;
    MobileIndex*    nMobile;
    bool            nAll;
    
    ParseMatching(hName, nStartWhere, nWantAmount, nMatchArg, nAll);
    for(nMobileScan = GetMobiles().begin(); nMobileScan != GetMobiles().end(); ++nMobileScan)
    {
        nMobile = (*nMobileScan).second;
    
        if(nMobile->mKeywords.IsNamed(nMatchArg))
        if(++nFound == nStartWhere)
            return nMobile;
    }

    return NULL;
}




RoomIndex* Area::GetRoomIndex(VNUM hVNum)
{
    return MUD::Get()->GetRoomIndex(MakeVRef(mANum, hVNum));
}




ObjectIndex* Area::GetObjectIndex(VNUM hVNum)
{
    return MUD::Get()->GetObjectIndex(MakeVRef(mANum, hVNum));
}




MobileIndex* Area::GetMobileIndex(VNUM hVNum)
{
    return MUD::Get()->GetMobileIndex(MakeVRef(mANum, hVNum));
}








