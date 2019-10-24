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
#include "StringMagic.h"
#include "StringUtils.h"
#include "Interpret.h"
#include "RoomIndex.h"
#include "ObjectIndex.h"
#include "MobileIndex.h"
#include "Immortal.h"
#include "Object.h"
#include "Spot.h"
#include "Act.h"
#include "Tables.h"
#include "MUD.h"






void Do_Color(Player* hCh, STRINGCW& hArg)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static const CHAR* sColorTemplate =
    "\n"    
    "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-[ ^TColor Settings^x ]-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
    "\n"
    "   ^t{^U Communication ^x^t}^x              ^t{^U Rooms ^x^t}^x                   ^t{^U Combat ^x^t}^x\n"
    "  (^)Looks Like^x) Say           (^@Looks Like^x) Room.Title     (^;Looks Like^x) By.Self\n"
    "  (^_Looks Like^x) SayTo         (^#Looks Like^x) Exits          (^<Looks Like^x) By.Group\n"
    "  (^-Looks Like^x) Whisper       (^%Looks Like^x) Room.Desc      (^|Looks Like^x) To.Self\n"
    "  (^}Looks Like^x) OOC           (^.Looks Like^x) Scan           (^:Looks Like^x) To.Group\n"
    "  (^=Looks Like^x) Chat          (^(Looks Like^x) Players\n"
    "  (^+Looks Like^x) Gossip        (^*Looks Like^x) Mobiles          ^t{^U Highlighting ^x^t}^x\n"
    "  (^`Looks Like^x) Tell          (^&Looks Like^x) Items          (^TLooks Like^x) Titles\n"
    "  (^{Looks Like^x) Gtell                                     (^tLooks Like^x) Topics\n"
    "  (^[Looks Like^x) Auction           ^t{^U Immortal ^x^t}^x            (^HLooks Like^x) Highlight\n"
    "  (^]Looks Like^x) Info          (^>Looks Like^x) Immtalk\n"
    "  (^,Looks Like^x) Clantalk      (^/Looks Like^x) OLC\n"
    "\n"
    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ASSERT(hCh->mSession != NULL);
    STRINGCW nColorArg;


    if(!hCh->IsOpt(kOptColor))
    {
        hCh->Send("You don't have color turned on. Use 'opt color'.\n");
        return;
    }
    
    
    hArg = hArg.OneArgument(nColorArg); 
    if(nColorArg.Empty())
    {
        hCh->Send(sColorTemplate);
        return;
    }   
    
    
    //** Find Which Type Of Text **//
    ANSITable::Iter nKind;
    for(nKind = gANSITable.begin(); nKind != gANSITable.end(); ++nKind)
    if(!(*nKind).second.mIsStatic)
    if(STRINGCW((*nKind).second.mName).IsNamed(nColorArg))
        break;
    
    if(nKind == gANSITable.end())
    {
        hCh->Send("Invalid text kind. Choose one from 'color'.\n");
        return;
    }
    
    
    //** Find All Requested Color Bits **//
    STRINGCW    nColorDescript;
    LONG        nColorBits = 0;
    
    hArg = hArg.OneArgument(nColorArg); 
    while(!nColorArg.Empty())
    {
        SHORT nIndex = gANSIPrefTable.FindNamed(nColorArg);
        
        if(nIndex == -1)
        {
            hCh->SendF("Invalid color type \"%s\". Read 'help color'.\n", *nColorArg);
            return;
        }
        
        SetBit(nColorBits, gANSIPrefTable[nIndex].mBit);
        
        if(!nColorDescript.Empty())
            nColorDescript += ", ";
        nColorDescript += gANSIPrefTable[nIndex].mLName;
        hArg = hArg.OneArgument(nColorArg); 
    }
    
    hCh->mColorPref[(*nKind).second.mPos] = nColorBits;
    hCh->mSession->mColorPrefStrs[(*nKind).second.mPos] = gANSIPrefTable.BuildANSICode(nColorBits); 
    hCh->SendF("All %s text will now be ^%c%s^x.\n", (*nKind).second.mName, (*nKind).second.mMarker, *nColorDescript);
}




void Do_Commands(Person* hCh, STRINGCW& hArg)
{
    CHAR**  nTable = NULL;
    CHAR**  nImmTable = NULL;
    ULONG   nMaxCommandNameLen = 0, nNumCommands = 0, nNumImmCommands = 0;
    SHORT   nOurTrust = hCh->GetTrust();
    
    //Locate And Store Commands
    for(LONG i = 0; i < kCommandsMax; ++i)
    if(CommandTable[i].mShow)
    if(CommandTable[i].mLevel <= nOurTrust)
    if(CommandTable[i].mWho == ALL
    || (CommandTable[i].mWho == PC  && hCh->isPlayer)
    || (CommandTable[i].mWho == NPC && hCh->isMobile)
    || (CommandTable[i].mWho == IMM && hCh->isPlayer && hCh->isPlayer->isImm))
    {
        if(CommandTable[i].mWho == IMM)
            ++nNumImmCommands;
        else
            ++nNumCommands;
    }
    
    if(nNumCommands)
        nTable = new CHAR*[nNumCommands];
    if(nNumImmCommands)
        nImmTable = new CHAR*[nNumImmCommands];
    
    for(LONG i = 0, mrt = 0, imm = 0; i < kCommandsMax; ++i)
    if(CommandTable[i].mShow)
    if(CommandTable[i].mLevel <= nOurTrust)
    if(CommandTable[i].mWho == ALL
    || (CommandTable[i].mWho == PC  && hCh->isPlayer)
    || (CommandTable[i].mWho == NPC && hCh->isMobile)
    || (CommandTable[i].mWho == IMM && hCh->isPlayer && hCh->isPlayer->isImm))
    {
        nMaxCommandNameLen = UMAX(nMaxCommandNameLen, strlen(CommandTable[i].mName));
        if(CommandTable[i].mWho == IMM)
            nImmTable[imm++] = (CHAR*) CommandTable[i].mName;
        else
            nTable[mrt++] = (CHAR*) CommandTable[i].mName;
    }
    
    //Alphabetize
    qsort(nTable, nNumCommands, sizeof(CHAR*), StrAlphaSort);
    qsort(nImmTable, nNumImmCommands, sizeof(CHAR*), StrAlphaSort);
    
    
    //Determine Columns/Rows/Formating & Send
    ULONG       nColumns    = 80/(nMaxCommandNameLen+1);
    ULONG       nRows       = nNumCommands/nColumns + (nNumCommands%nColumns?1:0);
    ULONG       nImmRows    = nNumImmCommands/nColumns + (nNumImmCommands%nColumns?1:0);
    STRINGCW    nColumnFormatStr, nTotalStr, nLineStr;
    
    nColumnFormatStr = QBuf("%%-%lds", nMaxCommandNameLen).c_str();
    
    ULONG r, c;
    for(r = 0; r < nRows; ++r) //Show mortal commands
    {
        for(c = 0; c < nColumns; ++c)
        {
            if(r*nColumns+c >= nNumCommands)
                continue;
            
            if(!nLineStr.Empty())
                nLineStr += " ";
            
            nLineStr += QBuf(nColumnFormatStr, nTable[r*nColumns+c]).c_str();   
        }
        nTotalStr += nLineStr + "\n";
        nLineStr = "";
    }
    
    if(nNumImmCommands) //Show immortal commands
    {
        nTotalStr.Append("^H", 2);
        for(r = 0; r < nImmRows; ++r)
        {
            for(c = 0; c < nColumns; ++c)
            {
                if(r*nColumns+c >= nNumImmCommands)
                    continue;
                
                if(!nLineStr.Empty())
                    nLineStr += " ";
                
                nLineStr += QBuf(nColumnFormatStr, nImmTable[r*nColumns+c]).c_str();
                nLineStr.Append("^H", 2);
            }
            nTotalStr += nLineStr + "\n";
            nLineStr = "";
        }
        nTotalStr.Append("^x", 2);
    }
    
    if(nTable)
        delete [] nTable;
    if(nImmTable)
        delete [] nImmTable;
    hCh->Send(nTotalStr);
    hCh->SendF("\nTotal Commands: %ld\n", nNumCommands+nNumImmCommands);
}




void Do_Description(Player* hCh, STRINGCW& hArg)
{
    hCh->DoEditor(hArg, hCh->mDescription, "[ Description Cleared ]");
}




void Do_Equipment(Person* hCh, STRINGCW& hArg)
{
    hCh->Send("Your are using:\n");
    hCh->ShowEquipTo(hCh);
}




void Do_Exits(Person* hCh, STRINGCW& hArg)
{
    Exit* nExit;
    bool nFoundAnyExits = false;
    STRINGCW nBuf;
        
    for(USHORT i = 0; i < kDirMax; ++i)
    {
        nExit = hCh->mInRoom->mExits[i];
        if(nExit == NULL)
            continue;
        
        if(!hCh->CanSee(nExit))
            continue;
        
        nFoundAnyExits = true;
        
        if(hCh->IsImm())
        {
            nExit->StatTo(hCh);
            continue;
        }
        
        STRINGCW nLeadsTo = "closed door";
        if(!nExit->IsClosed())
            nLeadsTo = nExit->mToRoom->mName;
        nBuf.SPrintF("%8s leads to: %s\n", gDirNameStrs[i], *nLeadsTo);
        hCh->Send(nBuf);
    }
    
    if(!nFoundAnyExits)
        hCh->Send("You don't see any exits out of here.\n");
}




void Do_Info(Player* hCh, STRINGCW& hArg)
{
    STRINGCW nBuf;
    
    //** Display Info Settings **//
    if(hArg.Empty())
    {
        #define IDOT(xBit) (hCh->IsInfo(xBit) ? "^b^!*^x" : " ")
        
        nBuf.SPrintF(
        //////////////////////////////////////////////////////////////////////////////////////////
        "\n"
        "%s\n"
        "\n"
        "  (%s) Chat          (%s) Tell          (%s) Connect       (%s) Levels\n"
        "  (%s) Gossip        (%s) Clan          (%s) Quit          (%s) Deaths \n"
        "  (%s) OOC           (%s) Auction       (%s) Links         (%s) Notes\n",
        //////////////////////////////////////////////////////////////////////////////////////////
        OverlayCenterLine("[ ^TInfo Channel Settings^x ]", kGfxSep),
        IDOT(kInfoChannelChat  ), IDOT(kInfoChannelTell   ), IDOT(kInfoConnect), IDOT(kInfoLevels),
        IDOT(kInfoChannelGossip), IDOT(kInfoChannelClan   ), IDOT(kInfoQuit   ), IDOT(kInfoDeaths),
        IDOT(kInfoChannelOOC   ), IDOT(kInfoChannelAuction), IDOT(kInfoLinks  ), IDOT(kInfoNotes ));
        hCh->Send(nBuf);

        if(hCh->IsTrueImm())
        {
            nBuf.SPrintF(
            //////////////////////////////////////////////////////////////////////////////////////////
            "\n"    
            "%s\n"
            "\n"
            "  (%s) Immtalk       (%s) Rewards       (%s) Creates       (%s) OLC\n"
            "  (%s) Repops        (%s) Punishments   (%s) Debug         (%s) Immortal\n",
            //////////////////////////////////////////////////////////////////////////////////////////
            OverlayCenterLine("[ ^TImmortal Info Channel Settings^x ]", kGfxSep),
            IDOT(kInfoChannelImm), IDOT(kInfoRewards), IDOT(kInfoCreate), IDOT(kInfoOLC),
            IDOT(kInfoRepops    ), IDOT(kInfoPunish ), IDOT(kInfoDebug ), IDOT(kInfoImm));
            hCh->Send(nBuf);
        }
        
        hCh->SendF("\n%s\n", kGfxThinSep);
        return;
    }
    


    //** Set An Information Option **//
    STRINGCW    nArg1;
    SHORT       nIndex;
    
    hArg.OneArgument(nArg1);
    nIndex = gInfoFlags.FindNamed(nArg1, hCh);
    
    
    //Toggle A Bit Preference
    if(nIndex != -1)
    {
        ToggleBit(hCh->mFlagsInfo, gInfoFlags[nIndex].mBit);
        if(hCh->IsInfo(gInfoFlags[nIndex].mBit))
            hCh->SendF("Info '^H%s^x' is now ^HON^x.\n", gInfoFlags[nIndex].mName);
        else
            hCh->SendF("Info '%s' is now OFF.\n", gInfoFlags[nIndex].mName);
        return;     
    }
    
    
    hCh->SendF("There's no such info setting as \"%s\".\n", *nArg1);
}




void Do_Inventory(Person* hCh, STRINGCW& hArg)
{
    ObjectHolder nInventory;
    if(!hCh->RetrieveObjectsAs(hCh, (hArg.Empty() ? "all" : *hArg), nInventory))
    {
        hCh->Send("\n -- You're Carrying Nothing --\n");
        return;
    }
    
    nInventory.ShowInventoryTo(hCh);
}




void Do_Look(Person* hCh, STRINGCW& hArg)
{
    ASSERT(hCh->mInRoom);
    STRINGCW        nArg1, nArg2;
    ObjectHolder    nObjects;
    PersonHolder    nPeople;
    Exit*           nExit;
    Spot*           nLookSpot;
    bool            nCanSeeRoom = hCh->CanSee(hCh->mInRoom);
    
    hArg = hArg.OneArgument(nArg1);
    hArg = hArg.OneArgument(nArg2); 
    if(!nArg1.Cmp("n") || !nArg1.Cmp("e") || !nArg1.Cmp("s")
    || !nArg1.Cmp("w") || !nArg1.Cmp("u") || !nArg1.Cmp("d"))
        goto Look_Exit;

    if(!nArg1.Cmp("in"))
        goto Look_In;
    
    
    //** Look At Room **//
    if(nArg1.Empty())
    {
        hCh->mInRoom->ShowTo(hCh, false);
        return;
    }
    
    
    //** Look At Person **//
    if(nCanSeeRoom)
    if(hCh->mInRoom->RetrievePeopleAs(hCh, nArg1, nPeople))
    {
        if(nPeople.Size() != 1)
        {
            hCh->Send("You can only look at one person at a time!\n");
            return;
        }
        
        Person* nPerson = nPeople.Front();
        if(nArg2.Empty())
        {
            nPerson->ShowTo(hCh);
            //Pio looks at you
            //You look at guy
            //Pio looks at guy
            return;
        
        }
        
        //Look At Person Spot
        nLookSpot = NULL;
        if(nPerson->isMobile)
            nLookSpot = nPerson->isMobile->mIndex->FindSpot(hCh, nArg2);
        
        if(!nLookSpot)
        {
            Act("$N has no \"$t\" spot on $M.\n", hCh, ActStr(nArg2), nPerson, ToChar);
            return;
        }
        nLookSpot->ShowTo(hCh);
        return;
    }
    
    
    //** Look At Object **//
    if(nCanSeeRoom)
    if(hCh->RetrieveObjectsHere(nArg1, nObjects, kFromInvWearRoom))
    {
        if(nObjects.Size() != 1)
        {
            hCh->Send("You can only look at one object at a time!\n");
            return;
        }
        
        Object* nObject = nObjects.Front();
        
        if(nArg2.Empty()) 
        {
            nObject->ShowTo(hCh);
            return;
        }
        
        //Look At Object Spot
        nLookSpot = nObject->mIndex->FindSpot(hCh, nArg2);
        if(!nLookSpot)
        {
            Act("$p has no \"$T\" spot on it.\n", hCh, nObject, ActStr(nArg2), ToChar);
            return;
        }
        nLookSpot->ShowTo(hCh);
        return;
    }
    
    
    //** Look At Spot In Room **//
    nLookSpot = nCanSeeRoom ? hCh->mInRoom->FindSpot(hCh, nArg1) : NULL;
    if(nLookSpot)
    {
        nLookSpot->ShowTo(hCh);
        return;
    }
    

    //** Look Exit/Direction **//
    Look_Exit:;
    if((nExit = hCh->FindExit(nArg1)) != NULL)
    {
        nExit->ShowTo(hCh);
        return;
    }

    if(nCanSeeRoom)
        hCh->Send("You don't see that here.\n");
    else
        hCh->Send("You don't make out anything like that in the darkness.\n");
    return;

    ///////////////////////////////////////////////////////////////////////////
    Look_In:;
    if(nArg2.Empty())
    {
        hCh->Send("Look in what?\n");
        return;
    }
    
    ObjectHolder nLookInObjects;
    if(hCh->RetrieveObjectsHere(nArg2, nLookInObjects, kFromInvWearRoom))
    {
        if(nLookInObjects.Size() != 1)
        {
            hCh->Send("You can only look in one object at a time!\n");
            return;
        }
        
        Object* nObject = nLookInObjects.Front();
        if(!nObject->IsContainer())
            Act("$p is not a container.\n", hCh, nObject, NULL, ToChar);
        else
            nObject->ShowContentsTo(hCh);
        return;
    }
        
    hCh->Send("You don't see that here.\n");
    return;
}




void Do_Option(Player* hCh, STRINGCW& hArg)
{
    #define ODOT(xBit) (hCh->IsOpt(xBit) ? "^b^!*^x" : " ")

    STRINGCW nBuf;


    //** Report Options Window **//
    if(hArg.Empty())    
    {
        nBuf.SPrintF(
        //////////////////////////////////////////////////////////////////////////////////////////
        "\n"
        "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-[ ^TOption Settings^x ]=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
        "\n"
        "    (%s) AutoAssist     (%s) AutoSkin      (%s) Color          (%s) Misses\n"
        "    (%s) AutoLoot       (%s) AutoExits     (%s) Info Channel   (%s) Movement\n"        
        "    (%s) AutoGold       (%s) AutoScan      (%s) Incognito      (%s) DamBySelf\n"
        "    (%s) AutoSplit      (%s) Prompt        (%s) NoSummon       (%s) DamByGroup\n"
        "                       (%s) Brief                            (%s) DamToSelf\n"
        "   (%2d) Scroll         (%s) Repeat                           (%s) DamToGroup\n",
        //////////////////////////////////////////////////////////////////////////////////////////
        ODOT(kOptAutoAssist), ODOT(kOptAutoSkin ), ODOT(kOptColor    ), ODOT(kOptMisses    ),
        ODOT(kOptAutoLoot  ), ODOT(kOptAutoExits), ODOT(kOptInfo     ), ODOT(kOptMovement  ),
        ODOT(kOptAutoGold  ), ODOT(kOptAutoScan ), ODOT(kOptIncognito), ODOT(kOptDamBySelf ),
        ODOT(kOptAutoSplit ), ODOT(kOptPrompt   ), ODOT(kOptNoSummon ), ODOT(kOptDamByGroup),
                              ODOT(kOptBrief    ),                      ODOT(kOptDamToSelf ),
        hCh->mScroll,         ODOT(kOptRepeat   ),                      ODOT(kOptDamToGroup));
        hCh->Send(nBuf);
        if(hCh->IsTrueImm())
        {
            nBuf.SPrintF(
            //////////////////////////////////////////////////////////////////////////////////////////
            "\n"
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=[ ^TImmortal Settings^x ]-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
            "\n"
            "    (%s) ImmPowers\n",
            //////////////////////////////////////////////////////////////////////////////////////////
            ODOT(kOptImmPowers));
            hCh->Send(nBuf);
        }
        
        nBuf.SPrintF(
        "\n%s\n"
        "Use 'opt <option>' to toggle and 'opt <option> <value>' to set a value.\n", kGfxThinSep);
        hCh->Send(nBuf);
        return;
    }


    //** Set An Option **//
    STRINGCW    nArg1, nArg2;
    SHORT       nIndex;
    
    hArg = hArg.OneArgument(nArg1);
    hArg = hArg.OneArgument(nArg2);
    nIndex = gOptionFlags.FindNamed(nArg1, hCh);
    
    
    //Toggle A Bit Preference
    if(nIndex != -1)
    {
        ToggleBit(hCh->mFlagsOptions, gOptionFlags[nIndex].mBit);
        if(hCh->IsOpt(gOptionFlags[nIndex].mBit))
            hCh->SendF("The '^H%s^x' option is now ^HON^x.\n", gOptionFlags[nIndex].mName);
        else
            hCh->SendF("The '%s' option is now OFF.\n", gOptionFlags[nIndex].mName);
        return;     
    }
    
    
    //Some Other Non-Bit Preference
    static STRINGCW sOptScroll = "scroll";
    
    if(sOptScroll.IsNamed(nArg1))
    {
        if(!hCh->CheckNumber(nArg2))
            return;
        
        SHORT nLines = nArg2.AsNumber();
        
        if(nLines < 0)
        {
            hCh->Send("Invalid scroll number.\n");
            return;
        }
        
        hCh->mScroll = nLines;
        if(nLines == 0)
            hCh->Send("Scroll OFF. Pages will no longer be paused.\n");
        else
            hCh->SendF("Scroll ON. Pages will be paused every %d lines.\n", nLines);
        return;
    }
    
    
    //No Dice
    hCh->SendF("There is no such option as \"%s\".\n", *nArg1);
}




void Do_Prompt(Player* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->SendF("Current prompt: ^%c%s^%c\n", kColorPause, *hCh->mPrompt, kColorPause);
        return;
    }   
    
    
    hCh->mPrompt = hArg;
    hCh->SendF("Your prompt is now: %s\n", *hCh->mPrompt);
}




void Do_Save(Player* hCh, STRINGCW& hArgs)
{
    if(hCh->Save())
        hCh->Send("Saved your pfile.\n");
    else
        hCh->Send("Failed to save your pfile!\n");
}




void Do_Scan(Person* hCh, STRINGCW& hArgs)
{
    ASSERT(hCh->mInRoom != NULL);
    if(hCh->CanSee(hCh->mInRoom))
    {
        hCh->mInRoom->ShowScanTo(hCh, false);
    }else{
        hCh->Send("You can't even make out your own hands in the darkness.\n");
    }
}




void Do_Score(Person* hCh, STRINGCW& hArgs)
{
    hCh->StatTo(hCh);
}




void Do_Who(Person* hCh, STRINGCW& hArgs)
{
    //////////////////////////////////////////////////////////////////////////////////////////
    static const CHAR* sImmBar = "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=[_^!IMMORTALS^x_]-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    static const CHAR* sMrtBar = "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-[_^!MORTALS^x_]=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    static const CHAR* sEndBar = "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    //////////////////////////////////////////////////////////////////////////////////////////
    PersonHolder    nOnlinePeople, nMortals, nImmortals;
    PersonList      nMortList, nImmList;
    CHAR            nBuf[kMaxStringLen];
    ULONG           nOnline = 0;
    STRINGCW        nOutput;
    PersonIter      i;
    
    
    MUD::Get()->mPlayers.RetrievePeopleAs(hCh, "all", nOnlinePeople);
    for(i = nOnlinePeople.Begin(); i != nOnlinePeople.End(); ++i)
    if((*i)->IsImm())
        nImmortals.AddPerson(*i); else nMortals.AddPerson(*i);
        
    
    if(!nImmortals.Empty())
    {
        Immortal* nPerson;
        nOutput += sImmBar;
        nImmortals.SortPeopleTo(hCh, nImmList, PersonLevelSort);
        for(i = nImmList.begin(); i != nImmList.end(); ++i)
        {
            nPerson = (*i)->isPlayer->isImm;
            nOnline++;
            
            STRINGCW& nTitle = nPerson->IsStatus(kStatusAFK) ? nPerson->mTitleAFK : nPerson->mTitle;
            
            csnprintf(nBuf, 79, "[%3d %3s %-3s] ^c^!%s^x%s%s%s%s", 
            nPerson->mLevel                     ,
            *nPerson->GetRaceAbrvTo(hCh)        , 
            *nPerson->GetClassAbrvTo(hCh)       ,
            *nPerson->GetNameTo(hCh)            , 
            (nPerson->mSession->IsLinkless()    ? " (linkless)" : ""), 
            (nPerson->IsStatus(kStatusAFK)      ? " (AFK)"      : ""),
            (nTitle[0] == ',' ? "" : " ")       , 
            *nTitle);

            nOutput += nBuf;
            nOutput += "^x\n";
        }
    }
    
    if(!nMortals.Empty())
    {
        Player* nPerson;
        nOutput += sMrtBar;
        nMortals.SortPeopleTo(hCh, nMortList, PersonNameSort);
        for(i = nMortList.begin(); i != nMortList.end(); ++i)
        {
            nPerson = (*i)->isPlayer;
            nOnline++;
            
            STRINGCW& nTitle = nPerson->IsStatus(kStatusAFK) ? nPerson->mTitleAFK : nPerson->mTitle;
            
            csnprintf(nBuf, 79, "[%3d %3s %-3s] ^c^!%s^x%s%s%s%s", 
            nPerson->mLevel                     ,
            *nPerson->GetRaceAbrvTo(hCh)        , 
            *nPerson->GetClassAbrvTo(hCh)       ,
            *nPerson->GetNameTo(hCh)            , 
            (nPerson->mSession->IsLinkless()    ? " (linkless)" : ""), 
            (nPerson->IsStatus(kStatusAFK)      ? " (AFK)"      : ""),
            (nTitle[0] == ',' ? "" : " ")       , 
            *nTitle);

            nOutput += nBuf;
            nOutput += "^x\n";
        }
    }
    
    sprintf(nBuf, "\nTotal Players Online: %lu\n%s", nOnline, sEndBar);
    nOutput += nBuf;
    hCh->Send(nOutput);
}   




