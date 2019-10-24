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
#include "Conversion.h"
#include "ExitPopulation.h"
#include "HelpFile.h"
#include "LoadOld.h"
#include "Mobile.h"
#include "MobileIndex.h"
#include "MobilePopulation.h"
#include "MUD.h"
#include "MUDFile.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "ObjectPopulation.h"
#include "OrangeScript.h"
#include "Part.h"
#include "Player.h"
#include "RoomIndex.h"
#include "Script.h"
#include "Skin.h"
#include "SocialFile.h"
#include "Spot.h"
#include "StringUtils.h"
#include "Tables.h"



//$$$ Old Form $$$--------------------------
//Area XYZ from vnum 200-500
//Object in XYZ is Vnum 205
//------------------------------------------
//
//$$$ In New Form $$$-----------------------
//Area XYZ has a unique area number
//VRef to Object is AreaNumber|VirtualNumber
//Object in XYZ is now SubVnum 5
//------------------------------------------




VREF CvtToVRef(VNUM hOldVnum)
{
    Area*   nArea       = CvtFindArea(hOldVnum);
    if(!nArea)
        return MakeVRef(0,0);

    VNUM    nAnum       = nArea->mANum;
    VNUM    nSubVnum    = hOldVnum - nArea->mMinOldVnum;

    return MakeVRef(nAnum, nSubVnum);
}


Area* CvtFindArea(VNUM hOldVnum)
{
    AreaMap&    nOldAreas = MUD::Get()->mOldAreas;
    AreaIter    nScan;

    
    for(nScan = nOldAreas.begin(); nScan != nOldAreas.end(); ++nScan)
    if(hOldVnum >= (*nScan).second->mMinOldVnum && hOldVnum <= (*nScan).second->mMaxOldVnum)
        return (*nScan).second;
        
    //ASSERT(!"Couldn't CvtFindVnum find area.");
    return NULL;
}




void MUD::ConvertToNewForm()
{
    //////////////////////////////////////////////////////////////////////////////
    //Areas
    AreaIter    nOldAreaScan;
    VNUM        nAnum = 0;
    //Organize Areas Into Super-Vnums Based On Original Order, and convert every
    //object/room/mobile (thing) into the new vref structure.
    for(nOldAreaScan = mOldAreas.begin();
    nOldAreaScan != mOldAreas.end(); ++nOldAreaScan, nAnum++)
    {
        Area* nArea = (*nOldAreaScan).second;
        
        nArea->mANum        = nAnum;
        GetAreas()[nAnum]   = nArea;
        ConvertToNewFormThings(nArea);
    }
    //Secondly, convert anything that used to reference the old vnum structure.
    for(nOldAreaScan = mOldAreas.begin();
    nOldAreaScan != mOldAreas.end(); ++nOldAreaScan, nAnum++)
        ConvertToNewFormReferencesToThings((*nOldAreaScan).second);
    //////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////
    //Old Players File
    Vector<STRINGCW> nConvertPlayers;
    nConvertPlayers.Add("Volkaran");
    nConvertPlayers.Add("Morb");
    nConvertPlayers.Add("Aerdie");
    nConvertPlayers.Add("Drolen");
    nConvertPlayers.Add("Hades");
    nConvertPlayers.Add("Liriel");
    nConvertPlayers.Add("Luminaca");
    nConvertPlayers.Add("Matrix");
    nConvertPlayers.Add("Medina");
    nConvertPlayers.Add("Nadia");
    nConvertPlayers.Add("Riklas");
    nConvertPlayers.Add("Selestia");
    nConvertPlayers.Add("Shiki");
    nConvertPlayers.Add("Shinigami");
    nConvertPlayers.Add("Terek");
    nConvertPlayers.Add("Zanzer");
    for(ULONG i = 0; i < nConvertPlayers.size(); ++i)
    {
        Player* nOldPlayer = NULL;
        if(MUD::Get()->LoadPlayerOld(nConvertPlayers[i], nOldPlayer) != kLoadPlayer_Success)
        {
            printf("Couldn't convert %s\n", *nConvertPlayers[i]);
            continue;
        }
        if(!nOldPlayer->mWasInRoom)
            nOldPlayer->mWasInRoom = RoomIndex::Get(kVRef_Room_Start);
        nOldPlayer->mInRoom = nOldPlayer->mWasInRoom;
        nOldPlayer->Save();
        delete nOldPlayer;
    }
    //////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////
    for(AreaIter i = GetAreas().begin(); i != GetAreas().end(); ++i)
    {
        Area* nArea = i->second;
        //Convert Area Credits String into Builders UID List now that the old
        //players have been saved and granted the new UIDs.
        Vector<STRINGCW> nBuilderNames;
        nArea->mCredits.Explode(nBuilderNames);
        for(ULONG i = 0; i < nBuilderNames.size(); ++i)
        {
            UID nUID = MUD::Get()->ResolveNameToUID(nBuilderNames[i]);
            if(nUID)
                nArea->mBuilders.Add(nUID);
        }

        ASSERT(nArea->Save());
        printf("ReSaved %s\n", *nArea->mFileName);
    }
    for(HelpFileList::iterator i = mHelpFiles.begin(); i != mHelpFiles.end(); ++i)
    {
        (*i)->Save();
        printf("ReSaved %s\n", *(*i)->mFileName);
    }
    for(SocialFileList::iterator i = mSocialFiles.begin(); i != mSocialFiles.end(); ++i)
    {
        (*i)->Save();
        printf("ReSaved %s\n", *(*i)->mFileName);
    }
    //////////////////////////////////////////////////////////////////////////////
}



    
void MUD::ConvertToNewFormThings(Area* hArea)
{   
    LONG            nConvObject = 0, nConvMobile = 0, nConvRoom = 0;
    IRoomIter       nRoomScan;
    IObjectIter     nObjectScan;
    IMobileIter     nMobileScan;
    
    //Convert Room Indexes & Room Exits to VREF
    for(nRoomScan = hArea->GetRooms().begin(); nRoomScan != hArea->GetRooms().end(); ++nRoomScan)
    {
        RoomIndex* nRoom = (*nRoomScan).second;
        
        nRoom->mVRef = CvtToVRef(nRoom->mOldVnum);
        nConvRoom++;
        
        ASSERT(nRoom == GetRoomIndex(nRoom->mVRef));
    }
    
    
    //Convert Object Indexes to VREF
    for(nObjectScan = hArea->GetObjects().begin(); nObjectScan != hArea->GetObjects().end(); ++nObjectScan)
    {
        ObjectIndex* nObject = (*nObjectScan).second;
        
        nObject->mVRef = CvtToVRef(nObject->mOldVnum);
        nConvObject++;
        
        nObject->mObjectFlags = 0;      
        ASSERT(nObject == GetObjectIndex(nObject->mVRef));
    }
    
    
    //Convert Mobile Indexes & Skins to VREF
    for(nMobileScan = hArea->GetMobiles().begin(); nMobileScan != hArea->GetMobiles().end(); ++nMobileScan)
    {
        MobileIndex* nMobile = (*nMobileScan).second;
        
        nMobile->mVRef = CvtToVRef(nMobile->mOldVnum);
        nConvMobile++;
        
        ASSERT(nMobile == GetMobileIndex(nMobile->mVRef));
    }
    
    
    
    
    //** Show Conversion Stats **//
    cout << "Converted: R: " << nConvRoom << " O:" << nConvObject << " M:" << nConvMobile << "  " << hArea->mFileName << endl;
}



void MUD::ConvertToNewFormReferencesToThings(Area *hArea)
{
    IRoomIter       nRoomScan;
    IMobileIter     nMobileScan;
    IObjectIter     nObjectScan;
    

    //Convert All Exits & Exit Keys & Scripts;
    for(nRoomScan = hArea->GetRooms().begin(); nRoomScan != hArea->GetRooms().end(); ++nRoomScan)
    {
        RoomIndex* nRoom = (*nRoomScan).second;

        for(SHORT i = 0; i < kDirMax; ++i)
        if(nRoom->mExits[i])
        {
            nRoom->mExits[i]->mToVRef = CvtToVRef(nRoom->mExits[i]->mToOldVnum);
            if(nRoom->mExits[i]->mOldKey > 0)
                nRoom->mExits[i]->mKey = CvtToVRef(nRoom->mExits[i]->mOldKey);
        }
        
        for(OScriptIter i = nRoom->mScriptMap.begin(); i != nRoom->mScriptMap.end(); ++i)
            i->second->Convert();
    }


    //Convert All Skins & Scripts
    for(nMobileScan = hArea->GetMobiles().begin();
        nMobileScan != hArea->GetMobiles().end(); ++nMobileScan)
    {
        MobileIndex* nMobile = (*nMobileScan).second;

        for(ULONG i = 0; i < nMobile->mSkins.size(); ++i)
            nMobile->mSkins[i]->mVRef = CvtToVRef(nMobile->mSkins[i]->mOldVnum);

        for(OScriptIter i = nMobile->mScriptMap.begin(); i != nMobile->mScriptMap.end(); ++i)
            i->second->Convert();
    }

    //Convert All Containers & Keys & Scripts
    for(nObjectScan = hArea->GetObjects().begin(); nObjectScan != hArea->GetObjects().end(); ++nObjectScan)
    {
        ObjectIndex* nObject = (*nObjectScan).second;

        if(nObject->mItemType == kItemContainer)
        {
            SetBit(nObject->mObjectFlags, kObjectIsContainer);
            nObject->mContCapacity      = nObject->mValue[kContCap];
            nObject->mContMultiplier    = nObject->mValue[kContMulti];
            nObject->mContKey           = CvtToVRef(nObject->mValue[kContKey]);
            
            if(IsSet(nObject->mValue[kContFlags], kOldContCloseable))
                SetBit(nObject->mContFlags, kContCloseable);
    
            if(IsSet(nObject->mValue[kContFlags], kOldContLockable))
                SetBit(nObject->mContFlags, kContLockable);
    
            if(IsSet(nObject->mValue[kContFlags], kOldContPickable))
                SetBit(nObject->mContFlags, kContPickable);
            
            nObject->mValue[0] = 0;
            nObject->mValue[1] = 0;
            nObject->mValue[2] = 0;
            nObject->mValue[3] = 0;
            nObject->mValue[4] = 0;
        }

        for(OScriptIter i = nObject->mScriptMap.begin(); i != nObject->mScriptMap.end(); ++i)
            i->second->Convert();
    }

    //Convert All Populations to VREF Rooms
    //Convert All ObjectPopulations to VREF Objects
    //Convert All MobilePopulations to VREF Mobiles
    for(ULONG i = 0; i < hArea->mPopulations.size(); ++i)
        hArea->mPopulations[i]->Convert();

    
    //Convert Room Ambience
    for(ULONG i = 0; i < hArea->mRoomAmbience.size(); ++i)
    {
        Ambience*           nAmb = hArea->mRoomAmbience[i];
        Vector<STRINGCW>    nVnumStrs;

        nAmb->mOldVnums.Explode(nVnumStrs);
        for(ULONG j = 0; j < nVnumStrs.size(); ++j)
        {
            VREF nAmbRef = CvtToVRef(atol(nVnumStrs[j]));
            nAmb->AddOntoRoom(nAmbRef);
        }
    }
    
    //Convert Mobile Ambience
    for(ULONG i = 0; i < hArea->mMobAmbience.size(); ++i)
    {
        Ambience*           nAmb = hArea->mMobAmbience[i];
        Vector<STRINGCW>    nVnumStrs;

        nAmb->mOldVnums.Explode(nVnumStrs);
        for(ULONG j = 0; j < nVnumStrs.size(); ++j)
        {
            VREF nAmbRef = CvtToVRef(atol(nVnumStrs[j]));
            nAmb->AddOntoMobile(nAmbRef);
        }
    }
    
    //Convert All Ambience to Vnum ONLY-OUR-AREA wise
    
    
}



