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
#include "OrangeSession.h"
#include "Channel.h"
#include "TEditor.h"
#include "Player.h"
#include "MUD.h"
#include "Act.h"



Channel::Channel()
    : mMessages(kMaxChannelHistory)
{
}


Channel::~Channel()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Channel::Broadcast
//
//   Desc:: Global broadcast a message tied to a channel or a channel info 
//          option or both. Show it and/or store it in history.
//
//  Input:: hChanIndex: Put hHistFormat'd ACT text into history, -1 is none.
//          hHistFormat: Format that goes into the history.
//          hChanBit: Must have this channel turned on. 0 is no channel.
//          hChanPrefix: Like "Info || " or "Auction || ".
//          hFormat: Format of text after "Info || " hChanPrefix.
//          hCh:        Same as Act();
//          hAct1:      Same as Act();
//          hAct2:      Same as Act();
//          hTargets:   Same as Act(); 
//          hSentTo:    Same as Act();
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Channel::Broadcast(CHAR hChanIndex,
                        const CHAR* hHistFormat ,
                        LONG        hChanBit    , 
                        const CHAR* hChanPrefix , 
                        const CHAR* hFormat     , 
                        Person* hCh, ActThing* hAct1, ActThing* hAct2, 
                        LONG hTargets, PersonHolder* hSentTo)
{
    MUD*            nMUD    = MUD::Get();
    Person*         hVCh    = dynamic_cast<Person*>(hAct2);
    Person*         hTo     = NULL;
    OrangeSession*  nSess;
    SessionIter     i;


    for(i = nMUD->mSessions.begin(); i != nMUD->mSessions.end(); ++i)
    {
        nSess = *i;
        hTo = nSess->mPerson;
        if(!nSess->mPerson || nSess->mState != kStatePlaying)
            continue;

        //** Channel Filter **//
        if(hChanBit && !hTo->IsInfo(hChanBit))
            continue;
    
        //** General ACT Filter **//
        if(IsSet(hTargets, kActOnlySeeChar) && !hTo->CanSee(hCh))
            continue;
        if(IsSet(hTargets, kActOnlySeeVict) && !hTo->CanSee(hVCh))
            continue;
        if(IsSet(hTargets, kActChar) && hTo != hCh)
            continue;
        if(IsSet(hTargets, kActVict) && (hTo != hVCh || hTo == hCh))
            continue;
        if(IsSet(hTargets, kActNoChar) && hTo == hCh)
            continue;
        if(IsSet(hTargets, kActNoVict) && hTo == hVCh)
            continue;
        if(IsSet(hTargets, kActGChar) && !hTo->IsSameGroupAs(hCh))
            continue;
        if(IsSet(hTargets, kActGVict) && !hTo->IsSameGroupAs(hVCh))
            continue;
        if(IsSet(hTargets, kActNoGChar) && hTo->IsSameGroupAs(hCh))
            continue;
        if(IsSet(hTargets, kActNoGVict) && hTo->IsSameGroupAs(hVCh))
            continue;
        if(IsSet(hTargets, kActImm) && !hTo->IsImm())
            continue;
        if(IsSet(hTargets, kActMort) && hTo->IsImm())
            continue;       
        
        
        //Send
        STRINGCW nActText = ActParseTo(hTo, hFormat, hCh, hAct1, hAct2);
        
        if(hChanPrefix)
        {
            TEditor nFormat(nActText);
            nFormat.FormatWidth(80-strlen(hChanPrefix));
            nFormat.Update();
            nFormat.PrefixLines(hChanPrefix);
            nActText = nFormat.mAfter.c_str();
        }
        
        hTo->Send(nActText);
        hTo->Send("\n");
        
        if(hSentTo)
            hSentTo->AddPerson(hTo);
            
        if(hChanIndex != -1 && hTo->isPlayer)
        {
            nActText = ActParseTo(hTo, hHistFormat, hCh, hAct1, hAct2);
            hTo->isPlayer->mChannels[(UCHAR)hChanIndex].AddMessage(nActText);
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
//   Name:: Channel::AddMessage
//
//   Desc:: Adds a message to the channel's history. Removing the oldest 
//          message at the end of the list.
//
//  Input:: The Person to show this Spot to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Channel::AddMessage(STRINGCW& hMsg)
{
    mMessages.AddFront(hMsg);
    mMessages.resize(kMaxChannelHistory);
}

