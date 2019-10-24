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
#include "Balance.h"
#include "Immortal.h"
#include "MUD.h"
#include "ObjectIndex.h"
#include "OLC.h"
#include "OrangeScript.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"
//
//ToDo: OStat
//      OEdit
//      OSet
//      OSpot
//



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_OList
//
//   Desc:: Object listing function.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_OList(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid olist options are...\n"
        "olist <area>         : List objects from an area file.\n"
        "olist <keyword>      : List objects partially matching the keyword(s).\n"
        "olist <area/keyword> : List objects in area(s) matching the keyword(s).\n"
        "olist all            : List ALL objects.\n";
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
        nTitle.SPrintF("[ ^TObjects In The %s Area^x ]", *nArea->mName);
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
            nTitle.SPrintF("[ ^TObjects In The %s Area^x ]", *nArea->mName);
        }else{
            nTitle.SPrintF("[ ^TObjects Matching \"%s\" In The %s Area^x ]", *nMatchArg, *nArea->mName);
        }           
    }
    else
    {
        nMatchArg = hArg;
        if(nMatchArg == "all" || nMatchArg == "*")
        {
            nMatchAll = true;
            nTitle.SPrintF("[ ^TAll Objects^x ]", *nMatchArg);
        }else{
            nTitle.SPrintF("[ ^TObjects Matching \"%s\"^x ]", *nMatchArg);
        }
        for(AreaIter i = nMUD->GetAreas().begin(); i != nMUD->GetAreas().end(); ++i)
            nCheckAreas.push_back(i->second);

    }

    
    //Match every ObjectIndex in nCheckAreas against nMatchArg
    ULONG nMatched = 0;
    bool nLeft = true;
    STRINGCW nBuf;
    
    hCh->SendPaged(OverlayCenterLine(nTitle, kGfxSep));
    hCh->SendPaged("\n\n^UVRef   ^x  ^UObject Name...               ^x    ^UVRef   ^x  ^UObject Name...               ^x");
    for(List<Area*>::iterator i = nCheckAreas.begin(); i != nCheckAreas.end(); ++i)
    for(IObjectIter j = (*i)->GetObjects().begin(); j != (*i)->GetObjects().end(); ++j)
    {
        ObjectIndex* nIndex = j->second;
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
//   Name:: Do_OStat
//
//   Desc:: Object information command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_OStat(Immortal* hCh, STRINGCW& hArg)
{
    ObjectIndex* nIndex;
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

    if(nArg1.Empty() && hCh->mOLC != NULL && hCh->mOLC->mObject != NULL)
        nIndex = hCh->mOLC->mObject;
    else
    if(!(nIndex = MUD::Get()->ResolveObjectVRef(hCh, nArg1)))
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
//   Name:: Do_OEdit
//
//   Desc:: Object editing command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_OEdit(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid medit options are...\n"
        "oedit ^!open^x <vref>      : Edit an object.\n"
        "oedit ^!close^x            : Stop editing an object.\n"
        "oedit ^!new^x [vnum]       : Create an object. [vnum] is optional.\n"
        "oedit ^!delete^x <vref>    : Delete an object.\n"
        "oedit ^!copy^x <from> [to] : Copy an object. [to] is optional.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = { "open", "close", "new", "delete", "copy", "\n" };
    LONG nChoice = hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg, true);
    if(nChoice == -1)
        return;

    if(nChoice != 0 /*OEditOpen*/)
    {
        if(!hCh->OLCCheck(kOLC_Check_Area))
            return;
    }
    
    switch(nChoice)
    {
        case -1: return;
        case 0: hCh->mOLC->OEditOpen(hArg);     break;
        case 1: hCh->mOLC->OEditClose(hArg);    break;
        case 2: hCh->mOLC->OEditNew(hArg);      break;
        case 3: hCh->mOLC->OEditDelete(hArg);   break;
        case 4: hCh->mOLC->OEditCopy(hArg);     break;
    }
}



void OLC::OEditOpen(STRINGCW& hArg)
{   
    ObjectIndex* nObject;
    if(mArea == NULL)
    {
        nObject = MUD::Get()->ResolveObjectVRef(&mBuilder, hArg);
        if(nObject == NULL)
            return;
        VNUM nAreaNum = GetArea(nObject->VRef());
        Area* nArea = MUD::Get()->GetArea(nAreaNum);
        STRINGCW nArg = nArea->mFileName;
        AEditOpen(nArg);
        if(!mArea)
            return;
    }
    else
    {
        if(NULL == (nObject = mArea->FindObjectIndex(hArg))
        && NULL == (nObject = MUD::Get()->ResolveObjectVRef(NULL, hArg)))
        {       
            mBuilder.SendF("No objects found matching \"%s\".\n", *hArg);
            return;
        }
    }
    
    if(nObject->ANum() != mArea->mANum)
    {
        mBuilder.SendF("Object %s is not from %s.\n", *nObject->mName, *mArea->mName);
        return;
    }
        
    if(GetOpenObject())
    {
        STRINGCW nNoArg;
        OEditClose(nNoArg);
    }

    Msg("You begin editing the \"%s\" object.", *nObject->mName);
    mObject = nObject;
}


void OLC::OEditClose(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    //Close mobile scripts
    STRINGCW nNoArg;
    WorkingWithObjectScripts();
    if(GetOpenScript())
        ScriptClose(nNoArg);

    Msg("You close the \"%s\" object.", *GetOpenObject()->mName);
    mObject = NULL;
}



void OLC::OEditNew(STRINGCW& hArg)
{
    VNUM nUseVNum = -1;
    
    
    if(hArg.IsNumber())
    {
        if(!hArg.ResolveShort(&mBuilder, nUseVNum, 0))
            return;
    }
    else if(!hArg.Empty())
    {
        mBuilder.Send("You may only specify a vnum for the object.\n");
        return;
    }


    //** Pick An Unused Vnum **//
    if(nUseVNum == -1)
    {
        for(nUseVNum = 0; nUseVNum < SHRT_MAX; ++nUseVNum)
        if(!GetOpenArea()->GetObjectIndex(nUseVNum))
            break;
    
        if(nUseVNum == SHRT_MAX)
        {
            mBuilder.SendF("The area is full and has no more object vnums left.\n");
            return;
        }
    }
    

    //** Mob Used? **//
    if(GetOpenArea()->GetObjectIndex(nUseVNum))
    {
        mBuilder.SendF("The %hd object vnum is already taken.\n", nUseVNum);
        return;
    }



    //** Finally Get Around To Making The Mob **//
    const CHAR* nBaseDesc = "Under Construction.";

    ObjectIndex* nObject = new ObjectIndex();
    nObject->mKeywords          = "proto clay protoclay";
    nObject->mName              = "a sphere of protoclay";
    nObject->mPName             = "spheres of protoclay";
    nObject->mShortDesc         = "A dark aqua sphere of protoclay ripples here.";
    nObject->mPShortDesc        = "$t dark aqua spheres of protoclay ripple here.";
    nObject->mDescription       = nBaseDesc;
    nObject->mItemType          = kItemJunk;
    nObject->mWeight            = 1.0;
    nObject->mLevel             = 1;
    nObject->mVRef              = MakeVRef(GetOpenArea()->mANum, nUseVNum);

    GetOpenArea()->AddObjectIndex(nObject);
    Msg("New object #%hd created in %s.", nUseVNum, *GetOpenArea()->mName);
    AreaChanged();


    //** Finish Up **//
    if(GetOpenObject())
        OEditClose(hArg = "");
    hArg.SPrintF("%hd", nUseVNum);
    OEditOpen(hArg);
}



void OLC::OEditDelete(STRINGCW& hArg)
{
    Msg("Not implemented");
}



void OLC::OEditCopy(STRINGCW& hArg)
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
//   Name:: Do_OSet
//
//   Desc:: Object setting command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_OSet(Immortal* hCh, STRINGCW& hArg)
{
    ObjectIndex* nObj = hCh->mOLC ? hCh->mOLC->GetOpenObject() : NULL;

    const CHAR* nMenu[] = {"name", "pname","keywords", "short", "pshort", "description",
    "materials", "extra", "wear", "type", "level", "weight", "cost" , "durability", "\n"};
    const CHAR* nLighMenu[] = {"life", "\n"};
    const CHAR* nWeapMenu[] = {"damagemod", "class", "\n"};
    const CHAR* nArmoMenu[] = {"ac", "\n"};
    const CHAR* nFurnMenu[] = {"people", "hit", "mana", "move", "flags", "\n"};
    const CHAR* nContMenu[] = {"capacity", "opening", "multiplier", "flags", "kkey", "\n"};
    const CHAR* nDConMenu[] = {"capacity", "current", "liquid", "\n"};
    const CHAR* nFoodMenu[] = {"nourishment", "filling", "high", "\n"};
    const CHAR* nReagMenu[] = {"charges", "\n"};
    const CHAR* nInstMenu[] = {"class", "\n"};
    const CHAR** nObjMenus[kItemMax] =
    {
        /* Light    */  nLighMenu,
        /* Scroll   */  NULL,
        /* Wands    */  NULL,
        /* Staff    */  NULL,
        /* Potion   */  NULL,
        /* Pill     */  NULL,
        /* Weapon   */  nWeapMenu,
        /* Armor    */  nArmoMenu,
        /* Furniture*/  nFurnMenu,
        /* Container*/  nContMenu,
        /* DrinkCon */  nDConMenu,
        /* Fountain */  nDConMenu,
        /* Food     */  nFoodMenu,
        /* Currency */  NULL,
        /* Boat     */  NULL,
        /* CorpseNPC*/  NULL,
        /* CorpsePC */  NULL,
        /* Reagent  */  nReagMenu,
        /* Stones   */  NULL,
        /* Luxury   */  NULL,
        /* Skin     */  NULL,
        /* Junk     */  NULL,
        /* Other    */  NULL,
        /* Instrument*/ nInstMenu
    };  


    //** Short Help **//
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "You may set the following strings...\n"
        "    ^!name^x, ^!pname^x, ^!keywords^x, ^!short^x, ^!pshort^x\n"
        "Valid oset options...\n"
        "oset ^!desc^x <editor>    : Edit object description.\n"
        "oset ^!type^x <type>      : Set the type of the item.\n"
        "oset ^!wear^x <locs>      : Set the wear locations, including take.\n"
        "oset ^!cost^x <cost>      : Set the cost of the item.\n"
        "oset ^!level^x  <level>   : Set the minimum level to use the item.\n"
        "oset ^!extra^x  <flags>   : Set flags like nodrop, glow, hum, etc.\n"
        "oset ^!weight^x <weight>  : Set the weight in pounds for an item.\n"
        "oset ^!material^x <flags> : Set the materials the item is composed of.\n"
        "oset ^!durability^x <%>   : Set to 'none' for undamagable, or 0-100%%.\n";
        hCh->Send(nShortHelp);
        
        if(nObj && nObjMenus[nObj->mItemType]) //Show object specific variables
        {
            STRINGCW nSpecificHelp;
            nSpecificHelp.SPrintF("Specific to ^H%s^x objects...\n    ", gItemTable[nObj->mItemType].mName);
            for(SHORT i = 0; nObjMenus[nObj->mItemType][i][0] != '\n'; ++i)
            {
                STRINGCW nAdd;
                nAdd.SPrintF("^!%s^x, ", nObjMenus[nObj->mItemType][i]);
                nSpecificHelp += nAdd;
            }
            nSpecificHelp.Resize(nSpecificHelp.Length()-2);
            nSpecificHelp += "\n";
            hCh->Send(nSpecificHelp);
        }       
        return; 
    }
    
    
    if(!hCh->OLCCheck(kOLC_Check_ObjectIndex))
        return;


    //** Handle Item Type-Specific Option **//
    const CHAR** nItemMenu = nObjMenus[nObj->mItemType];
    SHORT nOption = -1;
    STRINGCW nArg1;

    hArg = hArg.OneArgument(nArg1);
    if(nItemMenu != NULL)
        nOption = SearchMenu(nArg1, nItemMenu, false);

    if(nOption != -1)
    {
        switch(nObj->mItemType)
        {
            case kItemLight     : hCh->mOLC->OSetItemLight(hArg, nOption);      break;
            case kItemScroll    : ASSERT(false);                                break;
            case kItemWand      : ASSERT(false);                                break;
            case kItemStaff     : ASSERT(false);                                break;
            case kItemPotion    : ASSERT(false);                                break;
            case kItemPill      : ASSERT(false);                                break;
            case kItemWeapon    : hCh->mOLC->OSetItemWeapon(hArg, nOption);     break;
            case kItemArmor     : hCh->mOLC->OSetItemArmor(hArg, nOption);      break;
            case kItemFurniture : hCh->mOLC->OSetItemFurniture(hArg, nOption);  break;
            case kItemContainer : hCh->mOLC->OSetItemContainer(hArg, nOption);  break;
            case kItemDrinkCon  : 
            case kItemFountain  : hCh->mOLC->OSetItemDrinkCon(hArg, nOption);   break;
            case kItemFood      : hCh->mOLC->OSetItemFood(hArg, nOption);       break;
            case kItemCurrency  : ASSERT(false);                                break;
            case kItemBoat      : ASSERT(false);                                break;
            case kItemCorpseNPC : ASSERT(false);                                break;
            case kItemCorpsePC  : ASSERT(false);                                break;
            case kItemReagent   : hCh->mOLC->OSetItemReagent(hArg, nOption);    break;
            case kItemStones    : ASSERT(false);                                break;
            case kItemLuxury    : ASSERT(false);                                break;
            case kItemSkin      : ASSERT(false);                                break;
            case kItemJunk      : ASSERT(false);                                break;
            case kItemOther     : ASSERT(false);                                break;
            case kItemInstrument: hCh->mOLC->OSetItemInstrument(hArg, nOption); break;
            default: ASSERT(false); break;
        }
        return;
    }
    
    
    //** Handle Normal Option **//
    if((nOption = SearchMenu(nArg1, nMenu, false)) == -1)
    {
        hCh->Send("Invalid option. Type oset for a list of valid options.\n");
        return;
    }
    
    switch(nOption)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: hCh->mOLC->OSetStrings(hArg, nOption);      break;
        case 6: hCh->mOLC->OSetMaterials(hArg);             break;
        case 7: hCh->mOLC->OSetExtraFlags(hArg);            break;
        case 8: hCh->mOLC->OSetWear(hArg);                  break;
        case 9: hCh->mOLC->OSetType(hArg);                  break;
        case 10: hCh->mOLC->OSetLevel(hArg);                break;
        case 11: hCh->mOLC->OSetWeight(hArg);               break;
        case 12: hCh->mOLC->OSetCost(hArg);                 break;
        case 13: hCh->mOLC->OSetDurability(hArg);           break;
        default: ASSERT(false); break;
    }   
}



#define kOSetName           0
#define kOSetPName          1
#define kOSetKeywords       2
#define kOSetShort          3
#define kOSetPShort         4
#define kOSetDescription    5
void OLC::OSetStrings(STRINGCW& hArg, SHORT hOpt)
{
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    if(hOpt != kOSetDescription && hArg.Empty())
    {
        mBuilder.Send("Set it to what?\n");
        return;
    }
    
    switch(hOpt)
    {
        case kOSetName:
            Msg("Name set to \"%s\".", *hArg);
            GetOpenObject()->mName = hArg;
            AreaChanged();
            break;
        case kOSetPName:
            Msg("Plural name set to \"%s\".", *hArg);
            GetOpenObject()->mPName = hArg;
            AreaChanged();
            break;
        case kOSetKeywords:
            Msg("Set mobile keywords to \"%s\".", *hArg);
            GetOpenObject()->mKeywords = hArg;
            AreaChanged();
            break;
        case kOSetShort:
            hArg.Replace("^x", "^x^&");
            Msg("SHORT: ^&%s^x", *hArg);
            GetOpenObject()->mShortDesc = hArg;
            AreaChanged();
            break;
        case kOSetPShort:
            hArg.Replace("^x", "^x^&");
            Msg("SHORT: ^&%s^x", *hArg);
            GetOpenObject()->mPShortDesc = hArg;
            AreaChanged();
            if(hArg.Find("$t") == cwsNotFound)
                Msg("Warning: Plural short description doesn't contain a $t for the amount.");          
            break;
        case kOSetDescription:
            if(mBuilder.DoEditor(hArg, GetOpenObject()->mDescription, "[ Object Description Cleared ]"))
                AreaChanged();
            break;
    }
}



void OLC::OSetMaterials(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    ObjectIndex* nObject = GetOpenObject();
    if(gMaterialTable.FlagSet(&mBuilder, hArg, nObject->mMaterials))
    {
        AreaChanged();
        BalanceObject(GetOpenObject());
        Msg("The object is composed of: %s.", *gMaterialTable.ListBitNames(nObject->mMaterials, &mBuilder));        
    }
}



void OLC::OSetExtraFlags(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    ObjectIndex* nObject = GetOpenObject();
    if(gObjectFlags.FlagSet(&mBuilder, hArg, nObject->mObjectFlags))
    {
        AreaChanged();
        BalanceObject(GetOpenObject());
        Msg("The object flags are now: %s.", *gObjectFlags.ListBitNames(nObject->mObjectFlags, &mBuilder));     
    }
}



void OLC::OSetWear(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    ObjectIndex* nObject = GetOpenObject();
    if(gWearTable.FlagSet(&mBuilder, hArg, nObject->mWearFlags))
    {
        AreaChanged();
        Msg("The wear flags are now: %s.", *gWearTable.ListBitNames(nObject->mWearFlags, true));        
    }
}



void OLC::OSetType(STRINGCW& hArg)
{
    ObjectIndex* nObject = GetOpenObject();
    SHORT nType;
    
    if((nType = TableLookup(gItemTable, *hArg, false)) == -1)
    {
        STRINGCW nShortHelp = "Invalid object type. Choose from the following types...\n";
        for(ULONG i = 0; i < gItemTable.size(); ++i)
        {
            nShortHelp += gItemTable[i].mName;
            if((i+1) != gItemTable.size())
                nShortHelp += ", ";
        }
        mBuilder.Send(nShortHelp);
        return;
    }
    
    if(!mBuilder.OLCCheck(kOLC_Check_RoomIndex))
        return;

    nObject->mValue[0]  = 0;
    nObject->mValue[1]  = 0;
    nObject->mValue[2]  = 0;
    nObject->mValue[3]  = 0;
    nObject->mValue[4]  = 0;
    nObject->mItemType = nType;
    Msg("The object is now of type %s.", gItemTable[nType].mName);
    BalanceObject(nObject);
    AreaChanged();
}



void OLC::OSetLevel(STRINGCW& hArg)
{
    SHORT nLevel;
    if(!hArg.ResolveShort(&mBuilder, nLevel, 0, mBuilder.GetTrust()))
        return;
    
    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    GetOpenObject()->mLevel = nLevel;
    BalanceObject(GetOpenObject());
    Msg("%s is now a level %hd object.", *GetOpenObject()->mName, nLevel);
    AreaChanged();
}



void OLC::OSetWeight(STRINGCW& hArg)
{
    float nWeight;
    if(!hArg.ResolveFloat(&mBuilder, nWeight, 0))
        return;

    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    GetOpenObject()->mWeight = nWeight;
    BalanceObject(GetOpenObject());
    Msg("%s now weighs %f lbs.", *GetOpenObject()->mName, nWeight);
    AreaChanged();
}



void OLC::OSetCost(STRINGCW& hArg)
{
    LONG nCost;
    if(!hArg.ResolveLong(&mBuilder, nCost, 0))
        return;

    if(!mBuilder.OLCCheck(kOLC_Check_ObjectIndex))
        return;

    GetOpenObject()->mCost = nCost;
    BalanceObject(GetOpenObject());
    Msg("%s now costs %ld copper.", *GetOpenObject()->mName, nCost);
    AreaChanged();
}



void OLC::OSetDurability(STRINGCW& hArg)
{
}



void OLC::OSetItemLight(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemWeapon(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemArmor(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemFurniture(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemContainer(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemDrinkCon(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemFood(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemReagent(STRINGCW& hArg, SHORT hOpt)
{
}



void OLC::OSetItemInstrument(STRINGCW& hArg, SHORT hOpt)
{
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_OSpot(Immortal* hCh, STRINGCW& hArg)
{
    //Msg("Not implemented.");
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_OScript(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid oscript options...\n"
        "oscript ^!list^x             : List all the scripts on the object.\n"
        "oscript ^!open^x <id>        : Open a script by the id number, or close it.\n"
        "oscript ^!close^x            : Close the object script you're working with.\n"
        "oscript ^!new^x              : Create a new object script.\n"
        "oscript ^!delete^x <id num>  : Delete an object script by its id number.\n"
        "oscript ^!code^x <editor>    : Modify the code for the script (also ocode).\n"
        "oscript ^!trigger^x <type>   : What triggers the script; enter, leave, phrase, etc.\n"
        "oscript ^!commands^x <words> : Set command words to trigger the phrase script.\n"
        "oscript ^!keywords^x <words> : Set the keywords that work with command words.\n"
        "oscript ^!compile^x          : Try to compile the script you're working on.\n"
        "oscript ^!text^x ...         : Edit text variables through the editor interface.\n"
        "ostat ^!script^x <id num>    : Info on a script.\n";
        hCh->Send(nShortHelp);
        return;
    }
    
    if(hCh->mOLC)
        hCh->mOLC->WorkingWithObjectScripts();
    Do_Script(hCh, hArg, hCh->mOLC ? hCh->mOLC->GetOpenObject() : NULL, kOLC_Check_ObjectIndex);
}


