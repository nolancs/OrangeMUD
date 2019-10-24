/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: October 2001                                          [Crossplatform]
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
#include "Area.h"
#include "Attack.h"
#include "Balance.h"
#include "Immortal.h"
#include "MUD.h"
#include "MobileIndex.h"
#include "ObjectIndex.h"
#include "OLC.h"
#include "OrangeScript.h"
#include "Part.h"
#include "Skin.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"
//
//ToDo: MEditDelete
//      MEditCopy
//      MSetExperience
//      MSetPartMap
//      Shop
//      MSpot
//      





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_MList
//
//   Desc:: Mobile listing function.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_MList(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid mlist options are...\n"
        "mlist <area>         : List mobiles from an area file.\n"
        "mlist <keyword>      : List mobiles partially matching the keyword(s).\n"
        "mlist <area/keyword> : List mobiles in area(s) matching the keyword(s).\n"
        "mlist all            : List ALL mobiles.\n";
        hCh->Send(nShortHelp);
        return;
    }


    //Fill nCheckAreas and nMatchArg for checking
    STRINGCW    nAreaArg, nMatchArg, nTitle;
    bool        nMatchAll = false;
    MUD*        nMUD = MUD::Get();
    Area*       nArea = NULL;
    List<Area*> nCheckAreas;

    if(StrSuffix(hArg, ".are"))
    {
        nMatchAll = true;
        if(!(nArea = MUD::Get()->FindArea(hArg)))
        {
            hCh->SendF("No areas found matching \"%s\".\n", *hArg);
            return;
        }
        nCheckAreas.push_back(nArea);
        nTitle.SPrintF("[ ^TMobiles In The %s Area^x ]", *nArea->mName);
    }
    else if(hArg.IsVRef(&nAreaArg, &nMatchArg))
    {
        if(!(nArea = MUD::Get()->FindArea(nAreaArg)))
        {
            hCh->SendF("No areas found matching \"%s\".\n", *nAreaArg);
            return;
        }
        nCheckAreas.push_back(nArea);
        if(nMatchArg == "all" || nMatchArg == "*" || nMatchArg.Empty())
        {
            nMatchAll = true;
            nTitle.SPrintF("[ ^TMobiles In The %s Area^x ]", *nArea->mName);
        }else{
            nTitle.SPrintF("[ ^TMobiles Matching \"%s\" In The %s Area^x ]", *nMatchArg, *nArea->mName);
        }           
    }
    else
    {
        nMatchArg = hArg;
        if(nMatchArg == "all" || nMatchArg == "*")
        {
            nMatchAll = true;
            nTitle.SPrintF("[ ^TAll Mobiles^x ]", *nMatchArg);
        }else{
            nTitle.SPrintF("[ ^TMobiles Matching \"%s\"^x ]", *nMatchArg);
        }
        for(AreaIter i = nMUD->GetAreas().begin(); i != nMUD->GetAreas().end(); ++i)
            nCheckAreas.push_back(i->second);

    }

    
    //Match every ObjectIndex in nCheckAreas against nMatchArg
    ULONG nMatched = 0;
    bool nLeft = true;
    STRINGCW nBuf;
    
    hCh->SendPaged(OverlayCenterLine(nTitle, kGfxSep));
    hCh->SendPaged("\n\n^UVRef   ^x  ^UMobile Name...               ^x    ^UVRef   ^x  ^UMobile Name...               ^x");
    for(List<Area*>::iterator i = nCheckAreas.begin(); i != nCheckAreas.end(); ++i)
    for(IMobileIter j = (*i)->GetMobiles().begin(); j != (*i)->GetMobiles().end(); ++j)
    {
        MobileIndex* nIndex = j->second;
        if(nMatchAll || nIndex->mKeywords.IsNamed(nMatchArg))
        {
            ++nMatched;
            if(nLeft)
                nBuf.CSPrintF("\n#%-6s  %-29s^x", *nIndex->GetIdentTo(hCh), CTrunc(nIndex->mName, 29));
            else
                nBuf.CSPrintF("    #%-6s  %s^x"   , *nIndex->GetIdentTo(hCh), CTrunc(nIndex->mName, 29));
            hCh->SendPaged(nBuf);
            nLeft = !nLeft;
        }
    }
    hCh->SendPagedF("\n\nFound: %lu.\n%s\n", nMatched, kGfxLine);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_MStat
//
//   Desc:: Mobile information command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_MStat(Immortal* hCh, STRINGCW& hArg)
{
    MobileIndex* nIndex;
    STRINGCW nArg1, nArg2, nArg3;
    
    hArg = hArg.OneArgument(nArg1);
    hArg = hArg.OneArgument(nArg2);
    hArg = hArg.OneArgument(nArg3);
    if(nArg1 == "script" || nArg1 == "spot")
    {
        nArg3 = nArg2;
        nArg2 = nArg1;
        nArg1.Clear();
    }

    if(nArg1.Empty() && hCh->mOLC != NULL && hCh->mOLC->GetOpenMobile() != NULL)
        nIndex = hCh->mOLC->GetOpenMobile();
    else
    if(!(nIndex = MUD::Get()->ResolveMobileVRef(hCh, nArg1)))
        return;
    
    if(nArg2.Empty())
    {
        nIndex->StatTo(hCh);
        return;
    }
    else if(nArg2 == "script")
    {
        OrangeScript* nScript;
        if(!nIndex->ResolveScript(hCh, nArg3, nScript))
            return;
        nScript->StatTo(hCh);
    }
    else if(nArg2 == "spot")
    {
    }
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_MEdit
//
//   Desc:: Mobile editing command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_MEdit(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid medit options are...\n"
        "medit ^!open^x <vref>      : Edit a mobile.\n"
        "medit ^!close^x            : Stop editing a mobile.\n"
        "medit ^!new^x [vnum]       : Create a mobile. [vnum] is optional.\n"
        "medit ^!delete^x <vref>    : Delete a mobile.\n"
        "medit ^!copy^x <from> [to] : Copy a mobile. [to] is optional.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = { "open", "close", "new", "delete", "copy", "\n" };
    LONG nChoice = hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg, true);
    if(nChoice == -1)
        return;

    if(nChoice != 0 /*MEditOpen*/)
    {
        if(!hCh->OLCCheck(kOLC_Check_Area))
            return;
    }
    
    switch(nChoice)
    {
        case -1: return;
        case 0: hCh->mOLC->MEditOpen(hArg);     break;
        case 1: hCh->mOLC->MEditClose(hArg);    break;
        case 2: hCh->mOLC->MEditNew(hArg);      break;
        case 3: hCh->mOLC->MEditDelete(hArg);   break;
        case 4: hCh->mOLC->MEditCopy(hArg);     break;
    }
}




void OLC::MEditOpen(STRINGCW& hArg)
{   
    MobileIndex* nMobile;
    if(mArea == NULL)
    {
        nMobile = MUD::Get()->ResolveMobileVRef(&mBuilder, hArg);
        if(nMobile == NULL)
            return;
        VNUM nAreaNum = GetArea(nMobile->VRef());
        Area* nArea = MUD::Get()->GetArea(nAreaNum);
        STRINGCW nArg = nArea->mFileName;
        AEditOpen(nArg);
        if(!mArea)
            return;
    }
    else
    {
        if(NULL == (nMobile = mArea->FindMobileIndex(hArg))
        && NULL == (nMobile = MUD::Get()->ResolveMobileVRef(NULL, hArg)))
        {       
            mBuilder.SendF("No mobiles found matching \"%s\".\n", *hArg);
            return;
        }
    }
    
    if(nMobile->ANum() != mArea->mANum)
    {
        mBuilder.SendF("Mobile %s is not from %s.\n", *nMobile->mName, *mArea->mName);
        return;
    }
        
    if(GetOpenMobile())
    {
        STRINGCW nNoArg;
        MEditClose(nNoArg);
    }

    Msg("You begin editing the \"%s\" mobile.", *nMobile->mName);
    mMobile = nMobile;
}


void OLC::MEditClose(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    //Close mobile scripts
    STRINGCW nNoArg;
    WorkingWithMobileScripts();
    if(GetOpenScript())
        ScriptClose(nNoArg);

    Msg("You close the \"%s\" mobile.", *GetOpenMobile()->mName);
    mMobile = NULL;
}



void OLC::MEditNew(STRINGCW& hArg)
{
    VNUM nUseVNum = -1;
    
    
    if(hArg.IsNumber())
    {
        if(!hArg.ResolveShort(&mBuilder, nUseVNum, 0))
            return;
    }
    else if(!hArg.Empty())
    {
        mBuilder.Send("You may only specify a vnum for the mobile.\n");
        return;
    }


    //** Pick An Unused Vnum **//
    if(nUseVNum == -1)
    {
        for(nUseVNum = 0; nUseVNum < SHRT_MAX; ++nUseVNum)
        if(!GetOpenArea()->GetMobileIndex(nUseVNum))
            break;
    
        if(nUseVNum == SHRT_MAX)
        {
            mBuilder.SendF("The area is full and has no more mobile vnums left.\n");
            return;
        }
    }
    

    //** Mob Used? **//
    if(GetOpenArea()->GetMobileIndex(nUseVNum))
    {
        mBuilder.SendF("The %hd mob vnum is already taken.\n", nUseVNum);
        return;
    }



    //** Finally Get Around To Making The Mob **//
    const CHAR* nBaseDesc = "Under Construction.";
    //const CHAR* nBaseDesc =
    //"You see a spherical little protomob floating in front of you. Its pinkish "
    //"fleshy body waiting to be shaped by the divine powers of Age of Wonder.";

    MobileIndex* nMobile = new MobileIndex();
    nMobile->mKeywords          = "pink protomob mobile";
    nMobile->mName              = "a protomob";
    nMobile->mPName             = "protomobs";
    nMobile->mShortDesc         = "A soft pink protomob hovers here.";
    nMobile->mPShortDesc        = "$t soft pink protomobs hover here.";
    nMobile->mDescription       = nBaseDesc;
    nMobile->mMovement          = "walks";
    nMobile->mActFlags          = kMobNPC;
    nMobile->mVRef              = MakeVRef(GetOpenArea()->mANum, nUseVNum);
    nMobile->mDefaultPos        = kPosStanding;

    GetOpenArea()->AddMobileIndex(nMobile);
    Msg("New mobile #%hd created in %s.", nUseVNum, *GetOpenArea()->mName);
    AreaChanged();


    //** Finish Up **//
    if(GetOpenMobile())
        MEditClose(hArg = "");
    hArg.SPrintF("%hd", nUseVNum);
    MEditOpen(hArg);

    MSetPartNew(hArg = "body");
    MSetAttackNew(hArg = "attack");
    BalanceMobile(nMobile, false);
    nMobile->mResist[kResChar] = 100;
}



void OLC::MEditDelete(STRINGCW& hArg)
{
    Msg("Not implemented");
}



void OLC::MEditCopy(STRINGCW& hArg)
{
    Msg("Not implemented");
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_MSet
//
//   Desc:: Mobile setting command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_MSet(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "You may set the following strings...\n"
        "    ^!name^x, ^!pname^x, ^!keywords^x, ^!short^x, ^!pshort^x, ^!movestr^x.\n"
        "mset ^!desc^x <editor> : Edit mobile description.\n"
        "mset ^!size^x <size>   : Set the size.\n"
        "mset ^!sex^x <sex>     : Set the sex.\n"
        "mset ^!apr^x <amount>  : Set the attacks per round.\n"
        "mset ^!hp^x  <%mod>    : Set the hit points modifier.\n"
        "mset ^!en^x  <%mod>    : Set the energy points modifier.\n"
        "mset ^!mv^x  <%mod>    : Set the move points modifier.\n"
        "mset ^!exp^x <%mod>    : Set the experience points modifier.\n"
        "mset ^!attack^x ...    : Edit the lists of attacks.\n"
        "mset ^!part^x   ...    : Edit the lists of parts.\n"
        "mset ^!skin^x   ...    : Edit the lists of skins.\n"
        "mset ^!posit^x <pos>   : Set the position to use short/pshort.\n"
        "mset ^!level^x <amount>    : Set the level of the mobile.\n"
        "mset ^!flags^x <flags>     : Set the behavior flags of the mobile.\n"
        "mset ^!ridable^x <amount>  : Set how many people can ride the mobile.\n"
        "mset ^!resist^x <type> <%>            : Set resistances, normal is 0%.\n"
        "mset ^!str/dex/int/wis/con^x <amount> : Set the main statistics.\n";

        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = {"name", "pname", "keywords", "short", "pshort", "movestr",
    "description", "position", "level", "experience", "size", "sex", "apr", "flags",
    "hitpoints hp", "energy", "mv", "strength", "dexterity", "intelligence", "wisdom",
    "constitution", "resist", "attack", "part", "skin", "ridable", "\n" };
    SHORT nOption = hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg);
    switch(nOption)
    {
        case -1: return;
        case 0: 
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6: hCh->mOLC->MSetStrings(hArg, nOption);  break;
        case 7: hCh->mOLC->MSetPosition(hArg);          break;
        case 8: hCh->mOLC->MSetLevel(hArg);             break;
        case 9: hCh->mOLC->MSetExperience(hArg);        break;
        case 10: hCh->mOLC->MSetSize(hArg);             break;
        case 11: hCh->mOLC->MSetSex(hArg);              break;
        case 12: hCh->mOLC->MSetAPR(hArg);              break;
        case 13: hCh->mOLC->MSetFlags(hArg);            break;
        case 14:
        case 15:
        case 16: hCh->mOLC->MSetHpEnMv(hArg, nOption);  break;
        case 17:
        case 18:
        case 19:
        case 20:
        case 21: hCh->mOLC->MSetStats(hArg, nOption);   break;
        case 22: hCh->mOLC->MSetResist(hArg);           break;
        case 23: hCh->mOLC->MSetAttack(hArg);           break;
        case 24: hCh->mOLC->MSetPart(hArg);             break;
        case 25: hCh->mOLC->MSetSkin(hArg);             break;
        case 26: hCh->mOLC->MSetRidable(hArg);          break;
    }
}





#define kMSetName           0
#define kMSetPName          1
#define kMSetKeywords       2
#define kMSetShort          3
#define kMSetPShort         4
#define kMSetMoveStr        5
#define kMSetDescription    6
void OLC::MSetStrings(STRINGCW& hArg, SHORT hOpt)
{
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    if(hOpt != kMSetMoveStr && hOpt != kMSetDescription && hArg.Empty())
    {
        mBuilder.Send("Set it to what?\n");
        return;
    }
    
    switch(hOpt)
    {
        case kMSetName:
            Msg("Name set to \"%s\".", *hArg);
            GetOpenMobile()->mName = hArg;
            AreaChanged();
            break;
        case kMSetPName:
            Msg("Plural name set to \"%s\".", *hArg);
            GetOpenMobile()->mPName = hArg;
            AreaChanged();
            break;
        case kMSetKeywords:
            Msg("Set mobile keywords to \"%s\".", *hArg);
            GetOpenMobile()->mKeywords = hArg;
            AreaChanged();
            break;
        case kMSetShort:
            hArg.Replace("^x", "^x^*");
            Msg("SHORT: ^*%s^x", *hArg);
            GetOpenMobile()->mShortDesc = hArg;
            AreaChanged();
            break;
        case kMSetPShort:
            hArg.Replace("^x", "^x^*");
            Msg("SHORT: ^*%s^x", *hArg);
            GetOpenMobile()->mPShortDesc = hArg;
            AreaChanged();
            if(hArg.Find("$t") == cwsNotFound)
                Msg("Warning: Plural short description doesn't contain a $t for the amount.");          
            break;
        case kMSetMoveStr:
            if(!hArg.Empty())
                Msg("MV: %s %s north.", *GetOpenMobile()->mName, *hArg);
            else
                Msg("Movement message set to default \"walks\".");
            GetOpenMobile()->mMovement = hArg;
            AreaChanged();
            break;
        case kMSetDescription:
            if(mBuilder.DoEditor(hArg, GetOpenMobile()->mDescription, "[ Mobile Description Cleared ]"))
                AreaChanged();
            break;
    }
}



void OLC::MSetPosition(STRINGCW& hArg)
{
    SHORT nPos = TableLookup(gPosTable, hArg, false);
    if(nPos == -1)
    {
        const CHAR* nShortHelp = 
        "Invalid position. Valid positions are...\n"
        "   sleeping, resting, sitting, and standing\n";
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mDefaultPos = nPos;
    Msg("Default position set to %s.", gPosTable[nPos].mName);
    AreaChanged();
}



void OLC::MSetLevel(STRINGCW& hArg)
{
    SHORT nLevel;
    if(!hArg.ResolveShort(&mBuilder, nLevel, 0, mBuilder.GetTrust()))
        return;
    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mLevel = nLevel;
    BalanceMobile(GetOpenMobile(), false);
    Msg("%s is now a level %hd mobile.", *GetOpenMobile()->mName, nLevel);
    AreaChanged();
}



void OLC::MSetExperience(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    STRINGCW nModStr;
    hArg = hArg.OneArgument(nModStr);

    SHORT nMod;
    if(!nModStr.ResolveShort(&mBuilder, nMod, 0))
        return;
        
    GetOpenMobile()->mExpMod = nMod;
    BalanceMobile(GetOpenMobile(), false);
    Msg("%s now has a %hd%% experience points modifier.", *GetOpenMobile()->mName, nMod);
}



void OLC::MSetSize(STRINGCW& hArg)
{
    SHORT nSize = TableLookup(gSizeTable, hArg, false);
    if(nSize == -1)
    {
        const CHAR* nShortHelp = 
        "Valid sizes are...\n"
        "Insect  -  fly.\n"
        "Rodent  -  rabbit, gopher.\n"
        "Animal  -  fox, lynx, boar.\n"
        "Small   -  goblin, hobbits, stags.\n"
        "Normal  -  human, elf, dwarves, cows.\n"
        "Large   -  troll, ogre, bears.\n"
        "Giant   -  hill giants.\n"
        "Titan   -  cyclops, biggest.\n";
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mSize = nSize;
    Msg("%s is now %s-sized.", *GetOpenMobile()->mName, gSizeTable[nSize].mName);
    AreaChanged();
}



void OLC::MSetSex(STRINGCW& hArg)
{
    SHORT nSex = TableLookup(gSexTable, hArg, false);
    if(nSex == -1)
    {
        const CHAR* nShortHelp = 
        "Invalid sex. Valid sexes are...\n"
        "   none, male, female, and either (random).\n";
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mSex = nSex;
    Msg("%s is now %s.", *GetOpenMobile()->mName, gSexTable[nSex].mName);
    AreaChanged();
}



void OLC::MSetAPR(STRINGCW& hArg)
{
    SHORT nAPR;
    if(!hArg.ResolveShort(&mBuilder, nAPR, 0, 25))
        return;
    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mAPR = nAPR;
    BalanceMobile(GetOpenMobile(), false);
    Msg("%s now gets %hd attacks per round.", *GetOpenMobile()->mName, nAPR);
    AreaChanged();
}



void OLC::MSetFlags(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    MobileIndex* nMobile = GetOpenMobile();
    if(gMobileFlags.FlagSet(&mBuilder, hArg, nMobile->mActFlags))
    {
        Msg("%s's flags are now: %s.", *nMobile->mName, *gMobileFlags.ListBitNames(nMobile->mActFlags, &mBuilder));     
        AreaChanged();
    }
}



void OLC::MSetHpEnMv(STRINGCW& hArg, SHORT hOpt)
{
    #define kMSetHp 14
    #define kMSetEn 15
    #define kMSetMv 16

    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;
    
    STRINGCW nModStr;
    hArg = hArg.OneArgument(nModStr);

    SHORT nMod;
    if(!nModStr.ResolveShort(&mBuilder, nMod, 0))
        return;
    

    MobileIndex* nMobile = GetOpenMobile();

    AreaChanged();
    switch(hOpt)
    {
        case kMSetHp:
            nMobile->mHitsMod = nMod;
            BalanceMobile(nMobile, false);
            Msg("%s now has %hd%% hit points for its level (roughly %ld-%ldhp).", 
            *nMobile->mName, nMod, nMobile->mHits[0], nMobile->mHits[1]);
            break;
        case kMSetEn:
            nMobile->mEnergyMod = nMod;
            BalanceMobile(nMobile, false);
            Msg("%s now has %hd%% energy points for its level (roughly %ld-%ldhp).", 
            *nMobile->mName, nMod, nMobile->mEnergy[0], nMobile->mEnergy[1]);
            break;
        case kMSetMv:
            nMobile->mMovesMod = nMod;
            BalanceMobile(nMobile, false);
            Msg("%s now has %hd%% move points for its level (roughly %ld-%ldhp).", 
            *nMobile->mName, nMod, nMobile->mMoves[0], nMobile->mMoves[1]);
            break;
    }
}



void OLC::MSetStats(STRINGCW& hArg, SHORT hOpt)
{
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    hOpt -= 17;

    CHAR nAmount;
    if(!hArg.ResolveChar(&mBuilder, nAmount, 3, 100))
        return;
    
    GetOpenMobile()->mStatMod[hOpt] = nAmount;
    BalanceMobile(GetOpenMobile(), false);
    Msg("%s now has a %+hd to %s.", *GetOpenMobile()->mName, (SHORT)nAmount, gStatTable[hOpt].mName);
    AreaChanged();
}



void OLC::MSetResist(STRINGCW& hArg)
{
    STRINGCW nResistStr, nAmountStr;
    hArg = hArg.OneArgument(nResistStr);
    hArg = hArg.OneArgument(nAmountStr);

    SHORT nResist = TableLookup(gResistTable, nResistStr, false);
    if(nResist == -1)
    {
        const CHAR* nShortHelp = 
        "Valid resistances are...\n"
        "    cold, fire, acid, electrical, poison, mind, sleep, and charm\n";
        mBuilder.Send(nShortHelp);
        return;
    }

    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;
    
    CHAR nAmount;
    if(!nAmountStr.ResolveChar(&mBuilder, nAmount, -100, 100))
        return;
    
    GetOpenMobile()->mResistMod[nResist] = nAmount;
    BalanceMobile(GetOpenMobile(), false);
    Msg("%s now has a %+hd%% to %s resistance.", *GetOpenMobile()->mName, (SHORT)nAmount, gResistTable[nResist].mName);
    AreaChanged();
}



void OLC::MSetRidable(STRINGCW& hArg)
{
    SHORT nHowMany;
    if(!hArg.ResolveShort(&mBuilder, nHowMany, 0))
    {
        mBuilder.Send("Values are from 0 (not ridable) or above (ridable).\n");
        return;
    }   

    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    GetOpenMobile()->mRidable = nHowMany;
    Msg("%s may now be ridden by %d people.", *GetOpenMobile()->mName, nHowMany);
    AreaChanged();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void OLC::MSetAttack(STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid options for mset attack... \n"
        "^!new^x                     : Make a new attack.\n"
        "^!list^x                    : List out all the mobs attacks.\n"
        "<num> ^!delete^x            : Delete an attack.\n"
        "<num> ^!chance^x <%chance>  : Set the % chance out of 100% it goes off.\n"
        "<num> ^!type^x   <type>     : Attacks area, tank, random, or script <#>.\n"
        "<num> ^!flags^x  <flag>     : Toggle attack flags. Plural, Etc...\n"
        "<num> ^!damage^x <type>     : Set damage type, cold, fire, acid, etc.\n"
        "<num> ^!hitroll^x <%mod>    : Set the hitroll modifier from normal.\n"
        "<num> ^!damroll^x <%mod>    : Set the damroll modifier from normal.\n"
        "<num> ^!message^x <message> : Set the attack message.\n";
        
        mBuilder.Send(nShortHelp);
        return;
    }

    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    
    STRINGCW nArg1;
    hArg.OneArgument(nArg1);
    
    if(!nArg1.IsNumber())
    {
        hArg = hArg.OneArgument(nArg1);
        const CHAR* nMenu1[] = {"new", "list", "\n"};
        SHORT nOption = mBuilder.OLCCheckMenu(kOLC_Check_MobileIndex, nMenu1, nArg1, false);
        switch(nOption)
        {
            case -1: return;
            case 0: MSetAttackNew(hArg); break;
            case 1: MSetAttackList(hArg); break;
        }
    }
    else
    {
        SHORT nAttNum;
        hArg = hArg.OneArgument(nArg1);
        if(!nArg1.ResolveShort(&mBuilder, nAttNum, 1))
            return;
        
        Attack* nAttack = FindAttack(GetOpenMobile()->mAttacks, nAttNum);
        if(nAttack == NULL)
        {
            mBuilder.Send("No such attack.\n");
            return;
        }
        
        const CHAR* nMenu2[] = {"delete", "chance", "type", "flags", "damage",
                                "damroll dr", "hitroll hr", "message", "\n"};       
        SHORT nOption = mBuilder.OLCCheckMenu(kOLC_Check_MobileIndex, nMenu2, hArg, false);
        switch(nOption)
        {
            case -1: return;
            case 0: MSetAttackDelete(hArg, nAttack); break;
            case 1: MSetAttackChance(hArg, nAttack); break;
            case 2: MSetAttackType(hArg, nAttack); break;
            case 3: MSetAttackFlags(hArg, nAttack); break;
            case 4: MSetAttackDamage(hArg, nAttack); break;
            case 5:
            case 6: MSetAttackHitDam(hArg, nAttack, nOption); break;
            case 7: MSetAttackMessage(hArg, nAttack); break;
        }
    }
}



void OLC::MSetAttackNew(STRINGCW& hArg)
{
    SHORT nAttNum = GetOpenMobile()->mAttacks.size()+1;
    Attack* nAttack;
    
    nAttack = new Attack();
    nAttack->mMessage       = hArg.Empty() ? STRINGCW("bite") : hArg;
    nAttack->mChance        = 100;
    nAttack->mType          = kAttTank;
    nAttack->mHitroll[0]    = 4;
    nAttack->mHitroll[1]    = 7;
    nAttack->mDamroll[0]    = 3;
    nAttack->mDamroll[1]    = 6;
    nAttack->mDamageType    = kDamPhysical;
    nAttack->mNumber        = nAttNum;
    
    GetOpenMobile()->mAttacks.push_back(nAttack);
    ASSERT(GetOpenMobile()->mAttacks.back() == nAttack);
    BalanceMobile(GetOpenMobile(), false);
    Msg("Created attack #%hd \"%s\" for %s.", nAttNum, *nAttack->mMessage, *GetOpenMobile()->mName);
    AreaChanged();
}



void OLC::MSetAttackList(STRINGCW& hArg)
{
    MobileIndex* nMobile = GetOpenMobile();
    for(ULONG i = 0; i < nMobile->mAttacks.size(); ++i)
        nMobile->mAttacks[i]->StatListTo(&mBuilder);
}



void OLC::MSetAttackDelete(STRINGCW& hArg, Attack* hAttack)
{
    AttackArray& nMobAttacks = GetOpenMobile()->mAttacks;
    AttackArray::iterator nAttackIter = std::find(nMobAttacks.begin(), nMobAttacks.end(), hAttack);
    ASSERT(nAttackIter != nMobAttacks.end());
    nMobAttacks.erase(nAttackIter);
    
    //Renumber the attacks
    for(ULONG i = 0; i < nMobAttacks.size(); ++i)
        nMobAttacks[i]->mNumber = i+1;

    Msg("Deleted attack #%hd \"%s\" for %s.", hAttack->mNumber, *hAttack->mMessage, *GetOpenMobile()->mName);
    AreaChanged();
    delete hAttack;
}



void OLC::MSetAttackChance(STRINGCW& hArg, Attack* hAttack)
{
    SHORT nChance;
    if(!hArg.ResolveShort(&mBuilder, nChance, 0, 100))
        return;

    hAttack->mChance = nChance;
    Msg("The \"%s\" attack now has a %hd%% chance to go off.", *hAttack->mMessage, nChance);
    AreaChanged();
}



void OLC::MSetAttackType(STRINGCW& hArg, Attack* hAttack)
{
    STRINGCW nArg1, nArg2;
    hArg = hArg.OneArgument(nArg1);
    hArg = hArg.OneArgument(nArg2);
    
    SHORT nType = SearchMenu(nArg1, gAttackTypeStrs, false);
    if(nType == -1)
    {
        const CHAR* nShortHelp =
        "Invalid type. Valid attack types are...\n"
        "   tank, area, random, and script <#>.\n";
        
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(nType == kAttScript)
    {
        SHORT nScript;
        if(!nArg2.ResolveShort(&mBuilder, nScript, 1))
            return;
        bool nExists = GetOpenMobile()->GetScript(nScript);
        hAttack->mDamageType = nScript;
        Msg("Attack #%hd now runs mob script #%hd.", hAttack->mNumber, nScript);
        if(nExists != true)
            Msg("Warning no such script exists currently.");
        return;
    }
    
    
    hAttack->mType = nType;
    Msg("The \"%s\" is now a(n) %s attack.", *hAttack->mMessage, gAttackTypeStrs[nType]);
    AreaChanged();
}



void OLC::MSetAttackFlags(STRINGCW& hArg, Attack* hAttack)
{
    if(gAttackFlags.FlagSet(&mBuilder, hArg, hAttack->mFlags))
    {
        Msg("The \"%s\" flags are now: %s.", *hAttack->mMessage, 
        *gAttackFlags.ListBitNames(hAttack->mFlags, &mBuilder));
        AreaChanged();
    }
}



void OLC::MSetAttackDamage(STRINGCW& hArg, Attack* hAttack)
{
    SHORT nDType = TableLookupL(gDamageTable, hArg, false);
    
    if(nDType == -1)
    {
        const CHAR* nShortHelp =
        "Invalid type. Valid attack types are...\n"
        "   physical, cold, fire, electricity, acid, and mind.\n";
        
        mBuilder.Send(nShortHelp);
        return;
    }
    
    hAttack->mDamageType = nDType;
    Msg("The \"%s\" now does %s damage.", *hAttack->mMessage, gDamageTable[nDType].mLName);
    AreaChanged();
}



void OLC::MSetAttackHitDam(STRINGCW& hArg, Attack* hAttack, SHORT hOpt)
{
    #define kMSetAttackDamroll  5
    #define kMSetAttackHitroll  6

    STRINGCW nModArg;
    hArg = hArg.OneArgument(nModArg);
    
    SHORT nMod;
    if(!nModArg.ResolveShort(&mBuilder, nMod, 0))
        return;
    
    switch(hOpt)
    {
        case kMSetAttackDamroll:
            hAttack->mDamMod = nMod;
            BalanceMobile(GetOpenMobile(), false);
            Msg("The \"%s\" attack now damages at %hd%% (roughly %ld-%ld).", *hAttack->mMessage, nMod,
            hAttack->mDamroll[0], hAttack->mDamroll[1]);
            break;
        case kMSetAttackHitroll:
            hAttack->mHitMod = nMod;
            BalanceMobile(GetOpenMobile(), false);
            Msg("The \"%s\" attack now hits at %hd%% (roughly %ld-%ld%% of the time).", *hAttack->mMessage, nMod,
            hAttack->mHitroll[0], hAttack->mHitroll[1]);
            break;
        default: ASSERT(false); return;
    }
    AreaChanged();
}



void OLC::MSetAttackMessage(STRINGCW& hArg, Attack* hAttack)
{
    if(hArg.Empty())
    {
        mBuilder.Send("You must specify a message for the attack.\n");
        return;
    }

    hAttack->mMessage = hArg;
    Msg("Attack #%hd is now the \"%s\" attack.", hAttack->mNumber, *hArg);
    AreaChanged();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void OLC::MSetPart(STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid options for mset part... \n"
        "^!list^x                 : List out all the mobs body parts.\n"
        "^!new^x <partname>       : Make a new body part.\n"
        "^!map^x <bodymap>        : Copy a set of predefined parts onto the mob.\n"
        "<name> ^!delete^x        : Delete the part.\n"
        "<name> ^!ac^x <#>        : Armor class of the part.\n"
        "<name> ^!size^x <#>      : Size of the part.\n"
        "<name> ^!name^x <string> : Rename the part.\n"
        "\n"
        "Note: For a list of bodymaps use 'table bodymaps'.\n";
        
        mBuilder.Send(nShortHelp);
        return;
    }
    

    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;


    const CHAR* nMenu1[] = {"new", "list",  "map", "\n"};
    const CHAR* nMenu2[] = {"delete", "ac", "size", "name", "\n"};
    

    STRINGCW nArg1;
    hArg = hArg.OneArgument(nArg1);
    SHORT nOption = SearchMenu(nArg1, nMenu1, false);
    if(nOption != -1)
    {
        switch(nOption)
        {
            case 0: MSetPartNew(hArg);      break;
            case 1: MSetPartList(hArg);     break;
            case 2: MSetPartMap(hArg);      break;
        }
    }
    else
    {
        Part* nPart = FindPart(GetOpenMobile()->mParts, nArg1);
        if(nPart == NULL)
        {
            mBuilder.Send("No such part.\n");
            return;
        }
        
        STRINGCW nArg2;
        hArg = hArg.OneArgument(nArg2);
        switch(SearchMenu(nArg2, nMenu2, false))
        {
            case 0: MSetPartDelete(hArg, nPart);    break;
            case 1: MSetPartAC(hArg, nPart);        break;
            case 2: MSetPartSize(hArg, nPart);      break;
            case 3: MSetPartName(hArg, nPart);      break;
            default:
                mBuilder.Send("Invalid option. Type 'mset part' for a list of valid options.\n");
                return;
        }
    }
}




void OLC::MSetPartNew(STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        mBuilder.Send("You have to specify a part name.\n");
        return;
    }
    
    Part *nPart;
    nPart           = new Part();
    nPart->mName    = hArg;
    nPart->mSize    = 5;
    
    GetOpenMobile()->mParts.Add(nPart);
    BalanceMobile(GetOpenMobile(), false);
    Msg("Created part '%s' on %s.", *nPart->mName, *GetOpenMobile()->mName);
    AreaChanged();
}




void OLC::MSetPartList(STRINGCW& hArg)
{
    MobileIndex* nMobile = GetOpenMobile();
    for(ULONG i = 0; i < nMobile->mParts.size(); ++i)
        nMobile->mParts[i]->StatListTo(&mBuilder);
}




void OLC::MSetPartMap(STRINGCW& hArg)
{
    Msg("Not Implemented");
}




void OLC::MSetPartDelete(STRINGCW& hArg, Part* nPart)
{
    PartArray& nMobParts = GetOpenMobile()->mParts;

    if(nMobParts.size() <= 1)
    {
        mBuilder.Send("A mobile must always have at least one part.\n");
        return;
    }
    
    
    PartArray::iterator nPartIter = std::find(nMobParts.begin(), nMobParts.end(), nPart);
    ASSERT(nPartIter != nMobParts.end());
    nMobParts.erase(nPartIter);
    
    
    Msg("Deleted '%s' part from %s.", *nPart->mName, *GetOpenMobile()->mName);
    AreaChanged();
    delete nPart;
}




void OLC::MSetPartAC(STRINGCW& hArg, Part* nPart)
{
    SHORT nValue;
    if(!hArg.ResolveShort(&mBuilder, nValue))
        return; 
    
    nPart->mAC = nValue;
    Msg("The '%s' part now has an AC of %hd.", *nPart->mName, nValue);
    AreaChanged();
}




void OLC::MSetPartSize(STRINGCW& hArg, Part* nPart)
{
    SHORT nValue;
    if(!hArg.ResolveShort(&mBuilder, nValue))
        return;
    
    nPart->mSize = nValue;
    Msg("The '%s' part size to %hd.", *nPart->mName, nValue);
    AreaChanged();
}




void OLC::MSetPartName(STRINGCW& hArg, Part* nPart)
{
    if(hArg.Empty())
    {
        mBuilder.Send("Change the part's name to what?\n");
        return;
    }
    
    Msg("Renamed '%s' part to '%s'.", *nPart->mName, *hArg);
    nPart->mName = hArg;
    AreaChanged();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void OLC::MSetSkin(STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid options for mset skin... \n"
        "^!new^x                 : Create a new skin.\n"
        "^!list^x                : List the skins for the mob.\n"
        "<num> ^!delete^x        : Delete a skin.\n"
        "<num> ^!vref^x   <vref> : Set vref of the object produced by the skin.\n"
        "<num> ^!chance^x <#>    : Set % chance this item will be produced.\n";
        
        mBuilder.Send(nShortHelp);
        return;
    }

    
    if(!mBuilder.OLCCheck(kOLC_Check_MobileIndex))
        return;

    
    STRINGCW nArg1;
    hArg.OneArgument(nArg1);
    
    if(!nArg1.IsNumber())
    {
        const CHAR* nMenu1[] = {"new", "list", "\n"};
        SHORT nOption = mBuilder.OLCCheckMenu(kOLC_Check_MobileIndex, nMenu1, hArg, false);
        switch(nOption)
        {
            case -1: return;
            case 0: MSetSkinNew(hArg); break;
            case 1: MSetSkinList(hArg); break;
        }
    }
    else
    {
        SHORT nSkinNum;
        hArg = hArg.OneArgument(nArg1);
        if(!nArg1.ResolveShort(&mBuilder, nSkinNum, 1))
            return;
        
        Skin* nSkin = FindSkin(GetOpenMobile()->mSkins, nSkinNum);
        if(nSkin == NULL)
        {
            mBuilder.Send("No such skin.\n");
            return;
        }
        
        const CHAR* nMenu2[] = {"delete", "vref", "chance", "\n"};      
        SHORT nOption = mBuilder.OLCCheckMenu(kOLC_Check_MobileIndex, nMenu2, hArg, false);
        switch(nOption)
        {
            case -1: return;
            case 0: MSetSkinDelete(hArg, nSkin); break;
            case 1: MSetSkinVRef(hArg, nSkin); break;
            case 2: MSetSkinChance(hArg, nSkin); break;
        }
    }
}



void OLC::MSetSkinNew(STRINGCW& hArg)
{
    ObjectIndex* nSkinObject = MUD::Get()->ResolveObjectVRef(&mBuilder, hArg);
    if(nSkinObject == NULL)
        return;

    SHORT nSkinNum = GetOpenMobile()->mSkins.size()+1;
    Skin* nSkin;
    
    nSkin = new Skin();
    nSkin->mVRef    = nSkinObject->VRef();
    nSkin->mChance  = 100;
    nSkin->mNumber  = nSkinNum;
    
    GetOpenMobile()->mSkins.push_back(nSkin);
    ASSERT(GetOpenMobile()->mSkins.back() == nSkin);
    Msg("Created skin #%hd, [%s] %s on %s.", nSkinNum, *nSkinObject->GetIdentTo(&mBuilder),
    *nSkinObject->mName, *GetOpenMobile()->mName);
    AreaChanged();
}



void OLC::MSetSkinList(STRINGCW& hArg)
{
    MobileIndex* nMobile = GetOpenMobile();
    for(ULONG i = 0; i < nMobile->mSkins.size(); ++i)
        nMobile->mSkins[i]->StatListTo(&mBuilder);
}



void OLC::MSetSkinDelete(STRINGCW& hArg, Skin* hSkin)
{
    SkinArray& nMobSkins = GetOpenMobile()->mSkins;
    SkinArray::iterator nSkinIter = std::find(nMobSkins.begin(), nMobSkins.end(), hSkin);
    ASSERT(nSkinIter != nMobSkins.end());
    nMobSkins.erase(nSkinIter);
    
    //Renumber the skins
    for(ULONG i = 0; i < nMobSkins.size(); ++i)
        nMobSkins[i]->mNumber = i+1;

    ObjectIndex* nSkinObject = MUD::Get()->GetObjectIndex(hSkin->mVRef);
    STRINGCW nName = nSkinObject ? nSkinObject->mName : STRINGCW("A NON-EXISTANT OBJECT");
    Msg("Deleted skin #%hd, [%hd/%hd] %s from %s.", hSkin->mNumber, GetArea(hSkin->mVRef),
    GetVnum(hSkin->mVRef), *nName, *GetOpenMobile()->mName);
    AreaChanged();
    delete hSkin;
}



void OLC::MSetSkinVRef(STRINGCW& hArg, Skin* hSkin)
{
    ObjectIndex* nSkinObject = MUD::Get()->ResolveObjectVRef(&mBuilder, hArg);
    if(nSkinObject == NULL)
        return;
    
    hSkin->mVRef = nSkinObject->VRef();
    Msg("Skin #%hd now produces %s.", hSkin->mNumber, *nSkinObject->mName);
    AreaChanged();
}



void OLC::MSetSkinChance(STRINGCW& hArg, Skin* hSkin)
{
    CHAR nChance;
    if(!hArg.ResolveChar(&mBuilder, nChance, 0, 100))
        return;

    hSkin->mChance = nChance;
    ObjectIndex* nSkinObject = MUD::Get()->GetObjectIndex(hSkin->mVRef);
    STRINGCW nName = nSkinObject ? nSkinObject->mName : STRINGCW("A NON-EXISTANT OBJECT");
    Msg("Skin #%hd now produces %s about %hd%% of the time.", hSkin->mNumber, *nName, (SHORT) nChance);
    AreaChanged();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_MSpot(Immortal* hCh, STRINGCW& hArg)
{
    //Msg("Not implemented.");
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_MScript(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid mscript options...\n"
        "mscript ^!list^x             : List all the scripts on the mobile.\n"
        "mscript ^!open^x <id>        : Open a script by the id number, or close it.\n"
        "mscript ^!close^x            : Close the mobile script you're working with.\n"
        "mscript ^!new^x              : Create a new mobile script.\n"
        "mscript ^!delete^x <id num>  : Delete a mobile script by its id number.\n"
        "mscript ^!code^x <editor>    : Modify the code for the script (also ocode).\n"
        "mscript ^!trigger^x <type>   : What triggers the script; enter, leave, phrase, etc.\n"
        "mscript ^!commands^x <words> : Set command words to trigger the phrase script.\n"
        "mscript ^!keywords^x <words> : Set the keywords that work with command words.\n"
        "mscript ^!compile^x          : Try to compile the script you're working on.\n"
        "mscript ^!text^x ...         : Edit text variables through the editor interface.\n"
        "mstat ^!script^x <id num>    : Info on a script.\n";
        hCh->Send(nShortHelp);
        return;
    }
    
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithMobileScripts();
    Do_Script(hCh, hArg, hCh->mOLC ? hCh->mOLC->GetOpenMobile() : NULL, kOLC_Check_MobileIndex);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//** Mob Editing Shortcuts **//
void Do_MNew(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_OLC))
        return;
    hCh->mOLC->MEditNew(hArg);
}

//** Mob Setting Shortcuts **//
void Do_MDesc(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_MobileIndex))
        return;
    hCh->mOLC->MSetStrings(hArg, kMSetDescription);
}



//** Mob Scripting Shortcuts **//
void Do_MCode(Immortal* hCh, STRINGCW& hArg)
{
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithMobileScripts();
    if(!hCh->OLCCheck(kOLC_Check_Script))
        return;
    
    hCh->mOLC->ScriptCode(hArg);            
}


void Do_MText(Immortal* hCh, STRINGCW& hArg)
{
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithMobileScripts();
    if(!hCh->OLCCheck(kOLC_Check_ScriptText))
        return;
    
    hCh->mOLC->ScriptTextEdit(hArg);
}


void Do_Part(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_MobileIndex))
        return;
        
    hCh->mOLC->MSetPart(hArg);
}

void Do_Attack(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->OLCCheck(kOLC_Check_MobileIndex))
        return;
        
    hCh->mOLC->MSetAttack(hArg);
}
