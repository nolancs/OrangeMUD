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
#include <stdarg.h>
#include "CommonTypes.h"
#include "Area.h"
#include "MobileIndex.h"
#include "ObjectIndex.h"
#include "OLC.h"
#include "OrangeScript.h"
#include "Immortal.h"
#include "RoomIndex.h"
#include "StringUtils.h"
#include "Tables.h"




OLC::OLC(Immortal& hBuilder)
    : mBuilder(hBuilder)
    , mArea(NULL)
    , mMobile(NULL)
    , mObject(NULL)
    , mRoomScript(NULL)
    , mRoomScriptText(NULL)
    , mMobileScript(NULL)
    , mMobileScriptText(NULL)
    , mObjectScript(NULL)
    , mObjectScriptText(NULL)
    , mCurScript(mRoomScript)
    , mCurScriptText(mRoomScriptText)
{
    Msg("Entering Online Creation");
}


OLC::~OLC()
{
    Msg("Now exiting Online Creation.");
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::CanExitOLC
//
//   Desc:: Can the builder exit OLC.
//
// Output:: True if they can. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool OLC::CanExitOLC()
{
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::Msg
//
//   Desc:: Sends an OLC formated message to the builder.
//
//  Input:: Format and virtual argument list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void OLC::Msg(const CHAR* hMsg, ...)
{
    static CHAR sBuf[kMaxStringLen];
    va_list nArgs;
    
    sBuf[0] = '\0';
    strcat(sBuf, "^/[OLC] ");
    va_start(nArgs, hMsg);
    vcsprintf(&sBuf[strlen(sBuf)], hMsg, nArgs);
    va_end(nArgs);
    strcat(sBuf, "^x\n");
    
    mBuilder.Send(sBuf);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::AreaChanged
//
//   Desc:: Mark the open Area as modified.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void OLC::AreaChanged()
{
    ASSERT(mArea != NULL);
    mArea->mChanged = true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::GetOpenArea
//
//   Desc:: Get a pointer to the currently open Area.
//
// Output:: NULL if no Area is open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Area* OLC::GetOpenArea()
{
    return mArea;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::GetOpenMobile
//
//   Desc:: Get a pointer to the currently open MobileIndex.
//
// Output:: NULL if no MobileIndex is open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

MobileIndex* OLC::GetOpenMobile()
{
    return mMobile;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::GetOpenObject
//
//   Desc:: Get a pointer to the currently open ObjectIndex.
//
// Output:: NULL if no ObjectIndex is open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ObjectIndex* OLC::GetOpenObject()
{
    return mObject;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OLC::GetOpenRoom
//
//   Desc:: Get a pointer to the currently open RoomIndex (the room the builder
//          is standing in). Returns NULL if that room isn't editable, not in
//          the same Area as GetOpenArea().
//
// Output:: NULL if no RoomIndex is open.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* OLC::GetOpenRoom()
{
    return mBuilder.mInRoom->mInArea == mArea ? mBuilder.mInRoom : NULL;
}
    



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_OLC(Immortal* hCh, STRINGCW& hArg)
{
    if(!hCh->mOLC.IsNull() && hCh->mOLC->CanExitOLC())
    {
        //hCh->mOLC->ExitOLC();
        hCh->mOLC = NULL;
    }else
    if(hCh->mOLC.IsNull())
    {
        hCh->mOLC = new OLC(*hCh);
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Immortal::OLCCheckMenu
//
//   Desc:: Given an aspect of OLC, a menu of choices, and an argument this 
//          method will make sure the Immortal has satisfied the aspect, like
//          having an area open, and will then search menu to find the
//          choice that the argument matches.
//
// Output:: Format and virtual argument list.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT Immortal::OLCCheckMenu(BYTE hCheck, const CHAR* hMenu[], STRINGCW& hArg, bool hExact)
{
    STRINGCW nOurArg;
    
    hArg = hArg.OneArgument(nOurArg);
    if(!OLCCheck(hCheck))
        return -1;
    
    SHORT nMenuResult = SearchMenu(nOurArg, hMenu, hExact);
    if(nMenuResult == -1)
    {
        Send("Invalid argument. Try no arguments for a summary.\n");
        return -1;
    }
    return nMenuResult;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Immortal::OLCCheck
//
//   Desc:: Given an aspect of OLC, it will make sure the Immortal has
//          satisfied the aspect, like having an Area or ObjectIndex open. If
//          they haven't, it will inform them.
//
// Output:: True if they satisfied the requirement.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Immortal::OLCCheck(BYTE hCheck)
{
    if(mOLC.IsNull())
    {
        Send("You haven't entered OLC yet. Type olc.\n");
        return false;
    }
    
    switch(hCheck)
    {
        case kOLC_Check_OLC: break;
        case kOLC_Check_HelpFile:
            break;
        case kOLC_Check_SocialFile:
            break;
        case kOLC_Check_Help:
            break;
        case kOLC_Check_Social:
            break;
        case kOLC_Check_Area:
            if(!mOLC->mArea)
            {
                Send("You're not editing an area.\n");
                return false;
            }
            break;
        case kOLC_Check_RoomIndex:
            if(!OLCCheck(kOLC_Check_Area))
                return false;
            if(mInRoom->mInArea != mOLC->mArea)
            {
                Send("This room is not in the area you are editing.\n");
                return false;
            }
            break;
        case kOLC_Check_ObjectIndex:
            if(!OLCCheck(kOLC_Check_Area))
                return false;
            if(!mOLC->mObject)
            {
                Send("You're not editing an object index.\n");
                return false;
            }
            break;
        case kOLC_Check_MobileIndex:
            if(!OLCCheck(kOLC_Check_Area))
                return false;
            if(!mOLC->mMobile)
            {
                Send("You're not editing a mobile index.\n");
                return false;
            }
            break;
        case kOLC_Check_Script:
            if(!OLCCheck(kOLC_Check_Area))
                return false;
            if(mOLC->GetOpenScript() == NULL)
            {
                Send("You're not editing a script.\n");
                return false;
            }
            break;
        case kOLC_Check_ScriptText:
            if(!OLCCheck(kOLC_Check_Area))
                return false;
            if(mOLC->GetOpenScriptText() == NULL)
            {
                Send("You're not editing a script text.\n");
                return false;
            }
            break;
        case kOLC_Check_Spot:
            break;
        case kOLC_Check_Population:
            break;
        default:
            ASSERT(!"Unhandled OLCCheck");
            break;
    }
    
    return true;
}






#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif







void Do_Script(Immortal* hCh, STRINGCW& hArg, ScriptedThing* hOn, BYTE hCheck)
{
    const CHAR* nMenu[] = {"list", "open", "close", "new", "delete", 
    "code", "trigger", "commands", "keywords", "compile", "text", "\n"};
    switch(hCh->OLCCheckMenu(hCheck, nMenu, hArg, false))
    {
        case -1: return;
        case 0: hCh->mOLC->ScriptList(hArg, hOn);   break;
        case 1: hCh->mOLC->ScriptOpen(hArg, hOn);   break;
        case 2: hCh->mOLC->ScriptClose(hArg);       break;
        case 3: hCh->mOLC->ScriptNew(hArg, hOn);    break;
        case 4: hCh->mOLC->ScriptDelete(hArg, hOn); break;
        case 5: hCh->mOLC->ScriptCode(hArg);        break;
        case 6: hCh->mOLC->ScriptTrigger(hArg);     break;
        case 7: hCh->mOLC->ScriptCommands(hArg);    break;
        case 8: hCh->mOLC->ScriptKeywords(hArg);    break;
        case 9: hCh->mOLC->ScriptCompile(hArg);     break;
        case 10: hCh->mOLC->ScriptText(hArg);       break;
    }
}




void OLC::ScriptList(STRINGCW& hArg, ScriptedThing* hList)
{
    ASSERT(hList);
    hList->StatListTo(&mBuilder);
}




void OLC::ScriptOpen(STRINGCW& hArg, ScriptedThing* hList)
{
    OrangeScript* nScript;
    if(!hList->ResolveScript(&mBuilder, hArg, nScript))
        return;
    
    if(GetOpenScript() != NULL)
        ScriptClose(hArg);
    
    GetOpenScript() = nScript;
    Msg("You open script #%hd.", nScript->mID);
}




void OLC::ScriptClose(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;
    Msg("You close script #%hd.", GetOpenScript()->mID);
    GetOpenScript() = NULL;
}




void OLC::ScriptNew(STRINGCW& hArg, ScriptedThing* hOn)
{
    SHORT nType = kRoomScript;
    if(dynamic_cast<RoomIndex*>(hOn)) nType = kRoomScript;
    else if(dynamic_cast<ObjectIndex*>(hOn)) nType = kObjectScript;
    else if(dynamic_cast<MobileIndex*>(hOn)) nType = kMobileScript;
    else ASSERT(false);
    
    
    VNUM nID = 1;
    while(hOn->GetScript(nID))
        ++nID;
    
    
    OrangeScript* nScript = new OrangeScript();
    nScript->mType      = nType;
    nScript->mID        = nID;
    nScript->mOnThis    = hOn;
    hOn->mScriptMap[nID] = nScript;


    const CHAR* nTypeNames[] = {"Room", "Object", "Mobile"};
    const CHAR* nTName = nTypeNames[nType];
    
    
    AreaChanged();
    Msg("%s script #%hd created for %s.", nTName, nID, *nScript->OnName());
    STRINGCW nOpenArg;
    nOpenArg.SPrintF("%hd", nID);
    ScriptOpen(nOpenArg, hOn);
}




void OLC::ScriptDelete(STRINGCW& hArg, ScriptedThing* hList)
{
    OrangeScript* nScript;
    if(!hList->ResolveScript(&mBuilder, hArg, nScript))
        return;
    
    if(nScript->IsRunning())
    {
        mBuilder.Send("Copies of this script are currently running.\n");
        mBuilder.Send("Wait until they end or use ps/k9 to terminate them.\n");
        return;
    }

    if(GetOpenScript() == nScript)
        ScriptClose(hArg);


    const CHAR* nTypeNames[] = {"room", "object", "mobile"};
    const CHAR* nTName = nTypeNames[nScript->mType];    

    Msg("Deleted %s script #%d for %s.", nTName, nScript->mID, *nScript->OnName());
    OScriptMap nNewMap;
    OrangeScript* nScanScript;
    for(VNUM i = 1; (nScanScript = hList->GetScript(i)) != NULL; ++i)
    if(nScanScript != nScript)
    {
        if(nScanScript->mID > nScript->mID)
            --nScanScript->mID;
        nNewMap[nScanScript->mID] = nScanScript;
    }
    hList->mScriptMap = nNewMap;
    delete nScript;
    AreaChanged();
}




void OLC::ScriptCode(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;
    
    STRINGCW nSourceTmp = GetOpenScript()->mSource.c_str();
    if(mBuilder.DoEditor(hArg, nSourceTmp, "^/[OLC] Script Code Cleared.^x"))
    {
        GetOpenScript()->mSource = nSourceTmp;
        AreaChanged();
    }
}




void OLC::ScriptCompile(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;
        
    ScriptedThing::CompileTo(GetOpenScript(), &mBuilder);
}




void OLC::ScriptTrigger(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;

    if(hArg.Empty())
    {
        STRINGCW nShortHelp = "Valid trigger types are...\n";
        for(SHORT i = 0; gTriggerTable[i].mName; ++i)
        if((GetOpenScript()->mType == kRoomScript   && gTriggerTable[i].mForRooms)
        || (GetOpenScript()->mType == kObjectScript && gTriggerTable[i].mForObjects)
        || (GetOpenScript()->mType == kMobileScript && gTriggerTable[i].mForMobiles))
        {
            nShortHelp += gTriggerTable[i].mName;
            nShortHelp += ", ";
        }
        nShortHelp.Erase(nShortHelp.Length()-2, 2);
        nShortHelp += ".\n";
        mBuilder.Send(nShortHelp);
        return;
    }
    
    SHORT i;
    for(i = 0; gTriggerTable[i].mName; ++i)
    if((GetOpenScript()->mType == kRoomScript   && gTriggerTable[i].mForRooms)
    || (GetOpenScript()->mType == kObjectScript && gTriggerTable[i].mForObjects)
    || (GetOpenScript()->mType == kMobileScript && gTriggerTable[i].mForMobiles))
    if(STRINGCW(gTriggerTable[i].mName).IsNamed(hArg))
        break;
    

    if(!gTriggerTable[i].mName)
    {
        STRINGCW nNothing;
        ScriptTrigger(nNothing);
        return;
    }
    
    const CHAR* nTypeNames[] = {"room", "object", "mobile"};
    const CHAR* nTName = nTypeNames[GetOpenScript()->mType];
    GetOpenScript()->mTrigger = gTriggerTable[i].mType;
    Msg("The %s script is now ^!%s^x^/ triggered.", nTName, gTriggerTable[i].mName);
    AreaChanged();
}




void OLC::ScriptCommands(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;

    const CHAR* nTypeNames[] = {"room", "object", "mobile"};
    const CHAR* nTName = nTypeNames[GetOpenScript()->mType];
    
    GetOpenScript()->mCommands = hArg;
    Msg("The %s script will now trigger with a \"%s\" \"%s\" phrase.", 
    nTName, *GetOpenScript()->mCommands, *GetOpenScript()->mKeywords);
    AreaChanged();
}




void OLC::ScriptKeywords(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;

    const CHAR* nTypeNames[] = {"room", "object", "mobile"};
    const CHAR* nTName = nTypeNames[GetOpenScript()->mType];
    
    GetOpenScript()->mKeywords = hArg;
    Msg("The %s script will now trigger with a \"%s\" \"%s\" phrase.", 
    nTName, *GetOpenScript()->mCommands, *GetOpenScript()->mKeywords);
    AreaChanged();
}




void OLC::ScriptText(STRINGCW& hArg)
{
    if(mBuilder.OLCCheck(kOLC_Check_Script))
        return;
}




void OLC::ScriptTextNew(STRINGCW& hArg)
{
}




void OLC::ScriptTextDelete(STRINGCW& hArg)
{
}




void OLC::ScriptTextOpen(STRINGCW& hArg)
{
}




void OLC::ScriptTextClose(STRINGCW& hArg)
{
}




void OLC::ScriptTextList(STRINGCW& hArg)
{
}




void OLC::ScriptTextEdit(STRINGCW& hArg)
{
}




