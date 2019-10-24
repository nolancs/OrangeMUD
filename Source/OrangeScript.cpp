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
#include "Area.h"
#include "Mobile.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "OrangeScript.h"
#include "OrangeScriptVariable.h"
#include "Person.h"
#include "RoomIndex.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"


OrangeScript::OrangeScript()
    : mOnThis(NULL)
    , mType(0)
    , mTrigger(0)
    , mFlags(0)
    , mID(0)
{
}


OrangeScript::~OrangeScript()
{
    for(OScriptVariableIter i = mTextVariables.begin(); i != mTextVariables.end(); ++i)
        delete *i;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


//Outdated
VREF CvtToVRef(VNUM hOldVnum);
void OrangeScript::Convert()
{
    Vector<STRINGCW> nFuncNames;
    nFuncNames.Add("get_room");
    nFuncNames.Add("create_obj");
    nFuncNames.Add("CHAR ");
    nFuncNames.Add("OBJ ");
    nFuncNames.Add("has_obj");
    nFuncNames.Add("mob_in_room");
        
    ULONG i;
    for(i = 0; i < nFuncNames.size(); ++i)
    {
        STRINGCW nOldSource = mSource.c_str();
        STRINGCW nNewSource, nOldCall, nNewCall;
        LONG nNumFound;
        LONG nOldVNum = 0;
        VREF nNewVRef;
        
        do
        {
            CHAR* nFoundFunc;
            if((nFoundFunc = strstr(nOldSource, nFuncNames[i])) != NULL)
            {
                nNewSource.Append(nOldSource, nFoundFunc - *nOldSource);
                nOldSource.Erase(0, nFoundFunc - *nOldSource);
                if(nFuncNames[i] == "CHAR ")
                {
                    nNewSource += "PERSON ";
                    nOldSource.Erase(0, strlen(nFuncNames[i]));
                }
                else
                if(nFuncNames[i] == "OBJ ")
                {
                    nNewSource += "OBJECT ";
                    nOldSource.Erase(0, strlen(nFuncNames[i]));
                }
                else
                {
                    nFoundFunc = strstr(nOldSource, ")");
                    ASSERT(nFoundFunc);
                    nOldCall.Clear();
                    nOldCall.Append(nOldSource, (nFoundFunc - *nOldSource)+1);
                    STRINGCW nNewCall;
                    if(i == 4 || i == 5)
                    {
                        printf("%s\n", *nOldCall);
                        CHAR nOldVarBuf[kMaxStringLen];
                        STRINGCW nSearchStr = nFuncNames[i] + "(%[^(),+], %ld)";
                        nNumFound = sscanf(nOldCall, *nSearchStr, nOldVarBuf, &nOldVNum);
                        if(nNumFound != 2)
                        {
                            nSearchStr = nFuncNames[i] + "(%[^(),+],%ld)";
                            nNumFound = sscanf(nOldCall, *nSearchStr, nOldVarBuf, &nOldVNum);
                        }
                        ASSERT(nNumFound == 2);
                        nNewVRef = CvtToVRef((VNUM)nOldVNum);
                        STRINGCW nAreaStr = MUD::Get()->GetArea(GetArea(nNewVRef))->mFileName;
                        nNewCall.SPrintF("%s(%s, \"%s/%d\")", *nFuncNames[i], nOldVarBuf, *nAreaStr, GetVnum(nNewVRef));
                    }
                    else
                    {
                        STRINGCW nSearchStr = nFuncNames[i] + "(%ld)";
                        nNumFound = sscanf(nOldCall, *nSearchStr, &nOldVNum);
                        ASSERT(nNumFound == 1);
                        nNewVRef = CvtToVRef((VNUM)nOldVNum);
                        STRINGCW nAreaStr = MUD::Get()->GetArea(GetArea(nNewVRef))->mFileName;
                        nNewCall.SPrintF("%s(\"%s/%d\")", *nFuncNames[i], *nAreaStr, GetVnum(nNewVRef));
                    }               
                    printf("conv: %s => %s\n", *nOldCall, *nNewCall);
                    nNewSource += nNewCall;
                    nOldSource = nFoundFunc+1;
                }
                
            }
            else
            {
                nNewSource += nOldSource;
                break;
            }
        }
        while(true);
    
        mSource = *nNewSource;
    }
    
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScript::IsRunning
//
//   Desc:: Returns true if this script is running currently in the MUD.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool OrangeScript::IsRunning()
{
    for(List<MUD::RunningScript*>::iterator i = MUD::Get()->mScripts.begin(); i != MUD::Get()->mScripts.end(); ++i)
    if((*i)->mScript == this)
        return true;

    return false;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScript::StatTo
//
//   Desc:: Gets the name of what this OrangeScript is attached to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW OrangeScript::OnName()
{
    RoomIndex* nRoom = dynamic_cast<RoomIndex*>(mOnThis);
    if(nRoom)
        return nRoom->mName;
        
    MobileIndex* nMobile = dynamic_cast<MobileIndex*>(mOnThis);
    if(nMobile)
        return nMobile->mName;

    ObjectIndex* nObject = dynamic_cast<ObjectIndex*>(mOnThis);
    if(nObject)
        return nObject->mName;
        
    return "#[Unknown]#";
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScript::StatTo
//
//   Desc:: Sends all the statistics about an OrangeScript to a Person.
//
//  Input:: The Person to send the statistics to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void OrangeScript::StatTo(Person* hTo)
{
    CHAR* nTypeNames[] = {"Room", "Object", "Mobile"};
    STRINGCW nBuf, nTitleBuf;

    nTitleBuf.SPrintF("[ ^T%s Script #%d for %s^x ]", nTypeNames[mType], mID, *OnName());
    nTitleBuf.Capitalize(true);
    nTitleBuf = OverlayCenterLine(nTitleBuf, kGfxSep);
    nBuf.SPrintF(
    //////////////////////////////////////////////////////////////////////////////////////////
    "%s\n"
    "  Commands: %-35s Trigger: %s\n"
    "  Keywords: %-35s   Flags: <none>\n"
    "     Texts: %s\n"
    "\n"
    "^USource Code...                                                                  ^x\n"
    "%s\n",
    //////////////////////////////////////////////////////////////////////////////////////////
    *nTitleBuf      , 
    *mCommands      , gTriggerTable[mTrigger].mName ,
    *mKeywords      ,
    "n/a yet"       ,
    mSource.c_str());
    
    hTo->SendPaged(nBuf);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PhraseMatch
//
//   Desc:: This is basically like IsNamed(), it takes a list of words and a
//          list of keys and returns true if all the words in the word list
//          match or partially match keys in the keylist. We don't use
//          IsNamed() because we only do partial matches on >= 3 characters.
//
//  Input:: hWordList: the word list, example "big troll".
//          hKeyList: the key list, example "big smelly green troll".
//
// Output:: True if the match succeeded.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PhraseMatch(STRINGCW& hWordList, STRINGCW& hKeyList)
{
    if(hWordList.Empty() || hKeyList.Empty())
        return false;
    
    
    //** Parse Word List **//
    STRINGCW nWord, nKeyword, nTheWords = hWordList, nTheKeys;

    while(true) 
    {
        nTheWords = nTheWords.OneArgument(nWord);

        if(nWord.Empty())
            return true;
        
        
        //** Parse Keyword List **//
        nTheKeys = hKeyList;
        while(true) 
        {
            nTheKeys = nTheKeys.OneArgument(nKeyword);
            
            if(nKeyword.Empty())
                return false;
            
            if(nWord.Length() < 3)
            {
                if(!strcmp(nKeyword, nWord))
                    break;
            }else{
                if(StrPrefix(nKeyword, nWord))
                    break;
            }
        }
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScript::CheckTrigger
//
//   Desc:: Given a command and list of arguments it checks to see if this
//          script is triggered by them. This works for all types of scripts.
//          The script is run and true is returned when triggered.
//
//  Input:: hCmd: the command, example "look".
//          hArg: the argument(s), example "east".
//          theCh: the character in this scripting instance.
//          theMob: the mobile in this scripting instance.
//          theObj: the object in this scripting instance.
//          theVict: the victim in this scripting instance.
//
// Output:: True if the script was triggered.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool OrangeScript::CheckTrigger(STRINGCW& hCmd, STRINGCW& hArg, Person* theCh, Mobile* theMob, Object* theObj, Person* theVict)
{
    RoomIndex* theRoom = theCh->mInRoom;
    void* nTarget = NULL;
    void* nFound = NULL;
    

    if(mTrigger != kScriptTrig_Phrase || hCmd.Empty())
        return false;

    if(!PhraseMatch(hCmd, mCommands))
        return false;
    

    //** Command Followed By Anything/Nothing **//
    if(mKeywords.Empty())
    {
        MUD::Get()->RunScript(this, theRoom, theCh, theMob, theObj, theVict);
        return true;
    }


    //** Object/Mobile Targeted In Keyword **//
    if(mKeywords == "*" && mType != kRoomScript)
    {
        if(mType == kObjectScript)
        {
            ObjectHolder nRetrieved;
            if(theCh->RetrieveObjectsHere(hArg, nRetrieved, kFromInvWearRoom) && nRetrieved.Size() == 1)
                nFound = nRetrieved.Front();
            nTarget = theObj;
        }
        else 
        if(mType == kMobileScript)
        {
            PersonHolder nRetrieved;
            if(theCh->mInRoom->RetrievePeopleAs(theCh, hArg, nRetrieved) && nRetrieved.Size() == 1)
                nFound = nRetrieved.Front();
            nTarget = theMob;
        }
        else
        {
            return false;
        }
        
        if(nFound == nTarget)
        {
            MUD::Get()->RunScript(this, theRoom, theCh, theMob, theObj, theVict);
            return true;
        }
    }
    else
    //** Command & Keyword Match **//
    if(PhraseMatch(hArg, mKeywords))
    {
        MUD::Get()->RunScript(this, theRoom, theCh, theMob, theObj, theVict);
        return true;
    }

    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TrigPhrase
//
//   Desc:: Attempts to trigger any script when a Person issues a command.
//
//  Input:: hCh: the character issuing the command.
//          hCmd: the command, example "look".
//          hArg: the argument(s), example "east".
//
// Output:: True if a script was triggered.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TrigPhrase(Person* hCh, STRINGCW& hCmd, STRINGCW& hArg)
{
    ObjectIter  nObjectScan;
    OScriptIter nScriptScan;

    
    //** Object Scripts (on Person) **//
    for(nObjectScan = hCh->ObjectHolder::Begin(); nObjectScan != hCh->ObjectHolder::End(); ++nObjectScan)
    {
        for(nScriptScan = (*nObjectScan)->mIndex->mScriptMap.begin(); nScriptScan != (*nObjectScan)->mIndex->mScriptMap.end(); ++nScriptScan)
        {
            if((*nScriptScan).second->CheckTrigger(hCmd, hArg, hCh, NULL, *nObjectScan, NULL))
                return true;
        }
    }
    


    //** Object Scripts (in Room) **//
    for(nObjectScan = hCh->mInRoom->ObjectHolder::Begin(); nObjectScan != hCh->mInRoom->ObjectHolder::End(); ++nObjectScan)
    {
        for(nScriptScan = (*nObjectScan)->mIndex->mScriptMap.begin(); nScriptScan != (*nObjectScan)->mIndex->mScriptMap.end(); ++nScriptScan)
        {
            if((*nScriptScan).second->CheckTrigger(hCmd, hArg, hCh, NULL, *nObjectScan, NULL))
                return true;
        }
    }
    


    //** Mobile Scripts **//
    PersonIter nPersonScan;
    for(nPersonScan = hCh->mInRoom->PersonHolder::Begin(); nPersonScan != hCh->mInRoom->PersonHolder::End(); ++nPersonScan)
    {
        if((*nPersonScan)->isMobile)
        {
            for(nScriptScan = (*nPersonScan)->isMobile->mIndex->mScriptMap.begin(); nScriptScan != (*nPersonScan)->isMobile->mIndex->mScriptMap.end(); ++nScriptScan)
            {
                if((*nScriptScan).second->CheckTrigger(hCmd, hArg, hCh, (*nPersonScan)->isMobile, NULL, NULL))
                    return true;
            }
        }
    }



    //** Room Scripts **//
    for(nScriptScan = hCh->mInRoom->mScriptMap.begin(); nScriptScan != hCh->mInRoom->mScriptMap.end(); ++nScriptScan)
    {
        if((*nScriptScan).second->CheckTrigger(hCmd, hArg, hCh, NULL, NULL, NULL))
            return true;
    }

    

    return false;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: TrigMove
//
//   Desc:: Attempts to trigger any script when a Person moves using the two
//          movement triggers, enter and leave.
//
//  Input:: hCh: the character issuing the command.
//          hType: kScriptTrig_Enter or kScriptTrig_Leave.
//          hDirName: The direction they're leaving to or entering from.
//
// Output:: True if a script was triggered.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool TrigMove(Person* hCh, SHORT hType, const STRINGCW& hDirName)
{
    OScriptIter nScriptScan;
    OrangeScript* nScript;
    

    //** Room Scripts **//
    for(nScriptScan = hCh->mInRoom->mScriptMap.begin(); nScriptScan != hCh->mInRoom->mScriptMap.end(); ++nScriptScan)
    {
        nScript = (*nScriptScan).second;
        if(nScript->mTrigger == hType)
        if(nScript->mKeywords.Empty() || nScript->mKeywords.IsNamed(hDirName))
        {
            MUD::Get()->RunScript(nScript, hCh->mInRoom, hCh, NULL, NULL, NULL);
            return true;
        }
    }


    //** Mobile Scripts **//
    PersonIter nPersonScan;
    for(nPersonScan = hCh->mInRoom->PersonHolder::Begin(); nPersonScan != hCh->mInRoom->PersonHolder::End(); ++nPersonScan)
    {
        if((*nPersonScan)->isMobile)
        {
            for(nScriptScan = (*nPersonScan)->isMobile->mIndex->mScriptMap.begin(); nScriptScan != (*nPersonScan)->isMobile->mIndex->mScriptMap.end(); ++nScriptScan)
            {
                nScript = (*nScriptScan).second;
                if(nScript->mTrigger == hType)
                if(nScript->mKeywords.Empty() || nScript->mKeywords.IsNamed(hDirName))
                {
                    MUD::Get()->RunScript(nScript, hCh->mInRoom, hCh, (*nPersonScan)->isMobile, NULL, NULL);
                    return true;
                }
            }
        }
    }


    //** Object Scripts (on Person) **//
    ObjectIter  nObjectScan;
    for(nObjectScan = hCh->ObjectHolder::Begin(); nObjectScan != hCh->ObjectHolder::End(); ++nObjectScan)
    {
        for(nScriptScan = (*nObjectScan)->mIndex->mScriptMap.begin(); nScriptScan != (*nObjectScan)->mIndex->mScriptMap.end(); ++nScriptScan)
        {
            nScript = (*nScriptScan).second;
            if(nScript->mTrigger == hType)
            if(nScript->mKeywords.Empty() || nScript->mKeywords.IsNamed(hDirName))
            {
                MUD::Get()->RunScript(nScript, hCh->mInRoom, hCh, NULL, *nObjectScan, NULL);
                return true;
            }
        }
    }
    

    //** Object Scripts (in Room) **//
    for(nObjectScan = hCh->mInRoom->ObjectHolder::Begin(); nObjectScan != hCh->mInRoom->ObjectHolder::End(); ++nObjectScan)
    {
        for(nScriptScan = (*nObjectScan)->mIndex->mScriptMap.begin(); nScriptScan != (*nObjectScan)->mIndex->mScriptMap.end(); ++nScriptScan)
        {
            nScript = (*nScriptScan).second;
            if(nScript->mTrigger == hType)
            if(nScript->mKeywords.Empty() || nScript->mKeywords.IsNamed(hDirName))
            {
                MUD::Get()->RunScript(nScript, hCh->mInRoom, hCh, NULL, *nObjectScan, NULL);
                return true;
            }
        }
    }
    
    
    return false;
}





#if 0

bool TrigAsk(Person *ch, Person *nMob, char *about)
{
    char text[MAX_INPUT_LENGTH], Var[MAX_INPUT_LENGTH];
    bool found = false;
    Script *sScan;
    
    if(!IS_NPC(nMob))
        return false;
    
    //** Sort Text **//
    text[0] = '\0';
    about = one_argument(about, Var);
    for(; Var[0] != '\0'; about = one_argument(about, Var))
    {               
        if(IsExactName(Var, "the"))
            continue;
        if(IsExactName(Var, "about"))
            continue;
            
        SmashChars(Var, "?.!", "   ");
        strcat(text, Var);
        strcat(text, " ");
    }
    
    for(sScan = nMob->isMobile->iMob->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == ASK)
    if(PhraseMatch(text, sScan->keywords))
    {
        found = true;
        sScan->Run(nMob->inRoom, ch, nMob, NULL, NULL);
    }

    return found;
}



bool TrigWear(Person *ch, Object *item)
{
    Script *sScan;
    bool ran = false;
    
    for(sScan = item->iObj->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == WEAR)
    {
        sScan->Run(ch->inRoom, ch, NULL, item, NULL);
        ran = true;
    }
    
    return ran;
}


bool TrigRemove(Person *ch, Object *item)
{
    Script *sScan;
    bool ran = false;
    
    for(sScan = item->iObj->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == REMOVE)
    {
        sScan->Run(ch->inRoom, ch, NULL, item, NULL);
        ran = true;
    }
    
    return ran;
}



void TrigAttack(Person *ch, Object *item, Person *victim)
{
    Script *sScan;
    
    for(sScan = item->iObj->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == ATTACK)
    {
        sScan->Run(ch->inRoom, ch, NULL, item, victim);
    }
}

bool TrigEat(Person *ch, Object *item)
{
    Script *sScan;
    bool ran = false;
    
    for(sScan = item->iObj->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == EAT)
    {
        sScan->Run(ch->inRoom, ch, NULL, item, NULL);
        ran = true;
    }
    
    return ran;
}


void TrigAssist(Person *ch, Person *victim, Person *checkMob)
{
    Script *sScan;

    if(!checkMob->isMobile)
        return;
    
    
    for(sScan = checkMob->isMobile->iMob->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == ASSIST)
    {
        sScan->Run(ch->inRoom, ch, checkMob, NULL, victim);
    }
}

void TrigDeath(Person *chKiller, Person *mobVict, Object *corpse)
{
    Script *sScan;

    if(!mobVict->isMobile)
        return;
    
    
    for(sScan = mobVict->isMobile->iMob->scripts; sScan; sScan = sScan->nextLocal)
    if(sScan->trigger == DEATH)
    {
        sScan->Run(mobVict->inRoom, chKiller, mobVict, corpse, NULL);
    }
}


#endif



