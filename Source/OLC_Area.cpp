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
#include "OLC_Area.h"
#include "StringMagic.h"
#include "Immortal.h"
#include "MUD.h"
#include "Area.h"
#include "StringUtils.h"
//
// ToDo:
// AEditNew
//




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_AList
//
//   Desc:: Area listing function.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_AList(Immortal* hCh, STRINGCW& hArg)
{
    
    //** The Title **//
    STRINGCW nTitle;

    if(hArg.Empty())
        nTitle = "[ ^TComplete Area List^x ]"; 
    else
        nTitle.SPrintF("[ ^TAreas Matching \"%s\"^x ]", *hArg);
        
    hCh->SendPaged(OverlayCenterLine(nTitle, kGfxSep));
    hCh->SendPaged(
    "\n"
    "\n"
    "^UAnum^x"                              "  "
    "^UFile Name      ^x"                   "  "
    "^UArea...                         ^x"  "  "
    "^UEdited By   ^x"                      "  "
    "^UChanged^x\n");
    
    
    //** Iterate Through All The Areas **//
    for(AreaIter i = MUD::Get()->GetAreas().begin(); i != MUD::Get()->GetAreas().end(); ++i)
    {
        Area* nArea = (*i).second;
        if(!hArg.Empty() 
        && !nArea->mFileName.IsNamed(hArg)
        && !nArea->mName.IsNamed(hArg))
            continue;
            
        nArea->StatListTo(hCh);
    }

    hCh->SendPagedF("\n%s\n", kGfxLine);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_AStat
//
//   Desc:: Area information command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_AStat(Immortal* hCh, STRINGCW& hArg)
{
    Area* nArea;
    
    if(hArg.Empty() && hCh->mOLC != NULL && hCh->mOLC->mArea != NULL)
        nArea = hCh->mOLC->mArea;
    else
        nArea = MUD::Get()->FindArea(hArg);
    
    if(!nArea)
    {
        hCh->SendF("No areas found matching \"%s\".\n", *hArg);
        return;
    }
    
    nArea->StatTo(hCh);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_AEdit
//
//   Desc:: Area editing command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_AEdit(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid aedit options are...\n"
        "aedit open <file_name> : Open an area by file name.\n"
        "aedit close            : Close an area to stop editing it.\n"
        "aedit save             : Save the area you are currently editing.\n"
        "aedit new <file_name> <area_number> : Make a new area and open it.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = {"open", "close", "close!", "save", "new", "\n"};
    switch(hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg, true))
    {
        case -1: return;
        case 0: hCh->mOLC->AEditOpen(hArg);         break;
        case 1: hCh->mOLC->AEditClose(hArg, false); break;
        case 2: hCh->mOLC->AEditClose(hArg, true);  break;
        case 3: hCh->mOLC->AEditSave(hArg);         break;
        case 4: hCh->mOLC->AEditNew(hArg);          break;
    }
}



void OLC::AEditOpen(STRINGCW& hArg)
{
    if(mArea)
    {
        mBuilder.SendF("You're ALREADY EDITING %s.\n", *mArea->mFileName);
        return;
    }
    
    Area* nArea = MUD::Get()->FindArea(hArg);
    if(!nArea)
    {
        mBuilder.SendF("No areas found matching \"%s\".\n", *hArg);
        return;
    }
    
    if(!HasPermission(&mBuilder, nArea))
    {
        mBuilder.SendF("You do not have permission to edit %s, \"%s\".\n", *nArea->mFileName, *nArea->mName);
        return;
    }
    
    Msg("You begin editing the \"%s\" area.", *nArea->mName);
    nArea->mEditors.AddBack(&mBuilder);
    mArea = nArea;
}



void OLC::AEditClose(STRINGCW& hArg, bool hCloseNow)
{
    if(!mBuilder.OLCCheck(kOLC_Check_Area))
        return;
    
    //Check for changes here
    if(GetOpenArea()->mChanged && !hCloseNow)
    {
        mBuilder.SendF("The \"%s\" area has unsaved changes.\n", *GetOpenArea()->mName);
        mBuilder.Send("You must save the area or use the 'aedit close!' command to leave it unsaved\n");
        mBuilder.Send("Unsaved changes will disappear only after the mud reboots, they still remain in memory\n");
        return;
    }
    
    
    //Close Room Stuff
    STRINGCW nNoArg;
    WorkingWithRoomScripts();
    if(GetOpenScript())
        ScriptClose(nNoArg);

    //Close Mobile Stuff
    if(mMobile)
        MEditClose(nNoArg);

    //Close Object Stuff
    //if(mObject)
    //  OEditClose(nNoArg);
    
    
    Msg("You close the \"%s\" area.", *GetOpenArea()->mName);
    mArea->mEditors.Remove(&mBuilder);
    mArea = NULL;
}



void OLC::AEditSave(STRINGCW& hArg)
{
    if(!mBuilder.OLCCheck(kOLC_Check_Area))
        return;

    if(mArea->Save())
    {
        mArea->mChanged = false;
        Msg("The \"%s\" area has been saved into file %s.", *mArea->mName, *mArea->mFileName);
    }else{
        Msg("Failed to save the \"%s\" to file %s.", *mArea->mName, *mArea->mFileName); 
    }
}



void OLC::AEditNew(STRINGCW& hArg)
{
    if(GetOpenArea())
    {
        mBuilder.SendF("You must close %s before creating a new area.\n", *GetOpenArea()->mFileName);
        return;
    }

    Area* nArea;
    STRINGCW nFileNameStr, nAreaNumStr; 
    hArg = hArg.OneArgument(nFileNameStr);
    hArg = hArg.OneArgument(nAreaNumStr);
    if(!StrSuffix(nFileNameStr, ".are"))
    {
        mBuilder.Send("The area file name must end with .are.\n");
        return;
    }
    if(NULL != (nArea = MUD::Get()->FindArea(nFileNameStr)))
    {
        mBuilder.SendF("%s already uses the file name %s.\n", *nArea->mName, *nFileNameStr);
        return;
    }
    
    
    VNUM nAreaNum;
    if(!nAreaNumStr.ResolveShort(&mBuilder, nAreaNum, 0))
        return;
    if(NULL != (nArea = MUD::Get()->GetArea(nAreaNum)))
    {
        mBuilder.SendF("%s already uses area number %d.\n", *nArea->mName, nAreaNum);
        return;
    }
    
    
    nArea = new Area();
    nArea->mANum = nAreaNum;
    nArea->mName = "New Area";
    nArea->mFileName = nFileNameStr;
    nArea->mBuilders.Add(mBuilder.GetUID());
    MUD::Get()->GetAreas()[nArea->mANum] = nArea;
    
    Msg("You create Area %d, %s.", nAreaNum, *nFileNameStr);
    AEditOpen(nFileNameStr);
    AEditSave(nFileNameStr);
    MUD::Get()->AddResourceFile(nFileNameStr);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Do_ASet
//
//   Desc:: Area setting command.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Do_ASet(Immortal* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        const CHAR* nShortHelp =
        "Valid aset options are...\n"
        "aset name    <string> : Set the area name.\n"
        "aset credits <string> : Modify the area credits.\n"
        "aset high/low <#>     : Set the high and low level ranges.\n"
        "aset respawn  <#>     : In minutes, time it takes for a respawn.\n";
        hCh->Send(nShortHelp);
        return;
    }

    const CHAR* nMenu[] = { "name", "credits", "low", "high", "respawn", "\n"};
    switch(hCh->OLCCheckMenu(kOLC_Check_OLC, nMenu, hArg))
    {
        case -1: return;
        case 0: hCh->mOLC->ASetName(hArg);      break;
        case 1: hCh->mOLC->ASetCredits(hArg);   break;
        case 2: hCh->mOLC->ASetLow(hArg);       break;
        case 3: hCh->mOLC->ASetHigh(hArg);      break;
        case 4: hCh->mOLC->ASetRespawn(hArg);   break;
    }
}



void OLC::ASetName(STRINGCW& hArg)
{
    ARGUMENT_REQUIRED(hArg);
    STRINGCW nOldName = mArea->mName;
    mArea->mName = hArg;
    Msg("The \"%s\" area is now known as the \"%s\" area.", *nOldName, *hArg);
    AreaChanged();
}



void OLC::ASetCredits(STRINGCW& hArg)
{
    ARGUMENT_REQUIRED(hArg);
    if(mArea->mBuilders.Edit(&mBuilder, hArg))
        AreaChanged();
}



void OLC::ASetLow(STRINGCW& hArg)
{
    ARGUMENT_REQUIRED(hArg);
    if(!hArg.ResolveShort(&mBuilder, mArea->mLowRange, 0, kLevelMax))
        return;
    
    Msg("Suggested low level for the \"%s\" area is now %ld.", *mArea->mName, mArea->mLowRange);
    AreaChanged();
}



void OLC::ASetHigh(STRINGCW& hArg)
{
    ARGUMENT_REQUIRED(hArg);
    if(!hArg.ResolveShort(&mBuilder, mArea->mHighRange, 0, kLevelMax))
        return;
    
    Msg("Suggested high level for the \"%s\" area is now %ld.", *mArea->mName, mArea->mHighRange);
    AreaChanged();
}



void OLC::ASetRespawn(STRINGCW& hArg)
{
    ARGUMENT_REQUIRED(hArg);
    if(!hArg.ResolveShort(&mBuilder, mArea->mRespawnRate, 1))
        return;
        
    Msg("The %s area now respawns every %ld minutes.", *mArea->mName, mArea->mRespawnRate);
    AreaChanged();
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



bool OLC::HasPermission(Person* hCh, Area* hArea)
{
    if(hCh->GetTrust() >= kAncient)
        return true;

    return hArea->mBuilders.Exists(hCh->GetUID());
}



void OLC::ModifyPermission(Person* hCh, Area* hArea)
{
}



