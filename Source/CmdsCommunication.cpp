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
#include "Interpret.h"
#include "StringUtils.h"
#include "StringMagic.h"
#include "Player.h"
#include "MUD.h"
#include "Act.h"
#include "RoomIndex.h"
#include "OSUtils.h"


void Do_Quit(Player* hCh, STRINGCW& hArgs)
{
    hCh->SendF("%15s\n", "^!Quitting^x");
    if(hCh->mSession->mTCP.Ptr())
        hCh->mSession->mTCP->ProcessOutput();
    
    cout << "From Game" << endl;
    hCh->RemoveFromGame(true);
    cout << "Kill Session" << endl;
    MUD::Get()->KillSession(hCh->mSession);
}





#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Do_AFK(Player* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        ToggleBit(hCh->mFlagsStatus, kStatusAFK);
        if(hCh->IsStatus(kStatusAFK))
        {
            Act("You are (AFK) $t\n"    , hCh, ActStr(hCh->mTitleAFK), NULL, ToChar, kPosDead);
            Act("$n is now (AFK) $t\n"  , hCh, ActStr(hCh->mTitleAFK), NULL, ToRoom);
        }else{
            Act("You no longer AFK.\n"  , hCh, NULL, NULL, ToChar, kPosDead);
            Act("$n is no longer AFK.\n", hCh, NULL, NULL, ToRoom);     
        }
        return;
    }
    
    RemoveBit(hCh->mFlagsStatus, kStatusAFK);
    hCh->mTitleAFK = hArg;
    STRINGCW nEmpty;
    Do_AFK(hCh, nEmpty);
}




void Do_Emote(Person* hCh, STRINGCW& hArg)
{
    //** Show History **//
    if(hArg.Empty())
    {
        if(!hCh->isPlayer)
            return;
        
        hCh->SendF("%s\n", OverlayCenterLine("[ ^TEmote History^x ]", kGfxSep));
        for(BYTE i = 0; i < kMaxChannelHistory; ++i)
        if(!hCh->isPlayer->mChannels[kChannelEmote].mMessages[i].Empty())
            hCh->SendF("\n^H[Emote]^x: %s\n", *hCh->isPlayer->mChannels[kChannelEmote].mMessages[i]);
        
        return;
    
    }
    

    //** Emote Something & Add To History **//
    PersonHolder    nSentTo;
    Person*         nPerson;
    ActStr          nActArg(hArg);
    STRINGCW        nMsg;
    
    Act("$n $t\n", hCh, nActArg, NULL, ToAll, kPosResting, &nSentTo);
    for(PersonIter i = nSentTo.Begin(); i != nSentTo.End(); ++i)
    {
        if(!(nPerson = *i)->isPlayer)
            continue;
            
        nMsg = ActParseTo(nPerson, "$n $t", hCh, nActArg, NULL);
        nPerson->isPlayer->mChannels[kChannelEmote].AddMessage(nMsg);
    }
}




void Do_Gossip(Person* hCh, STRINGCW& hArg)
{
    
    //** Show Channel History **//
    if(hArg.Empty())
    {
        if(!hCh->isPlayer)
            return;
        
        hCh->SendF("%s\n", OverlayCenterLine("[ ^TGossip History^x ]", kGfxSep));
        for(BYTE i = 0; i < kMaxChannelHistory; ++i)
        if(!hCh->isPlayer->mChannels[kChannelGossip].mMessages[i].Empty())
            hCh->SendF("\n^H[Gossip]^x: %s\n", *hCh->isPlayer->mChannels[kChannelGossip].mMessages[i]);
        
        return;
    }
    
    
    
    //** Gossip Something **//
    ActStr nActArg(hArg);


    if(!hCh->IsInfo(kInfoChannelGossip))
    {
        hCh->Send("You have gossip turned off. Type 'info gossip' to turn it on.\n");
        return;
    }
    
    Channel::Broadcast(
        kChannelGossip, "You gossip \"$t\"", 
        kInfoChannelGossip  , NULL, "^+You gossip \"$t\"^x", 
        hCh, nActArg, NULL, kActWorld|kActChar);
    
    Channel::Broadcast(
        kChannelGossip, "$n gossips \"$t\"", 
        kInfoChannelGossip  , NULL, "^+$n gossips \"$t\"^x", 
        hCh, nActArg, NULL, kActWorld|kActNoChar);
}





void Do_OOC(Person* hCh, STRINGCW& hArg)
{
    
    //** Show Channel History **//
    if(hArg.Empty())
    {
        if(!hCh->isPlayer)
            return;
        
        hCh->SendF("%s\n", OverlayCenterLine("[ ^TOOC History^x ]", kGfxSep));
        for(BYTE i = 0; i < kMaxChannelHistory; ++i)
        if(!hCh->isPlayer->mChannels[kChannelOOC].mMessages[i].Empty())
            hCh->SendF("\n^H[OOC]^x: %s\n", *hCh->isPlayer->mChannels[kChannelOOC].mMessages[i]);
        
        return;
    }
    
    
    
    //** OOC Something **//
    ActStr nActArg(hArg);


    if(!hCh->IsInfo(kInfoChannelOOC))
    {
        hCh->Send("You have OOC turned off. Type 'info ooc' to turn it on.\n");
        return;
    }
    
    Channel::Broadcast(
        kChannelOOC, "$n: $t", 
        kInfoChannelOOC , NULL, "^}[OOC] $n: $t^x", 
        hCh, nActArg, NULL, kActWorld);
}




void Do_Reply(Player* hCh, STRINGCW& hArg)
{
    //** Show Where Reply Points **//
    if(hArg.Empty())
    {
        hCh->SendF("Reply points to: %s\n", *hCh->mReplyTo);        
        return;
    }
    
    
    //** Reply To The Last Person To Who Told To Us **//
    PersonHolder    nTargets;
    Person*         nTarget;
    ActStr          nActArg(hArg);

    //Find Target
    if(!MUD::Get()->mPlayers.RetrievePeopleAs(hCh, hCh->mReplyTo, nTargets))
    {
        hCh->SendF("There is no one named \"%s\" online right now.\n", *hCh->mReplyTo);
        return;
    }
    ASSERT(nTargets.Size() == 1);
    nTarget = nTargets.Front();
    
    
    //Check Target
    ASSERT(nTarget != hCh);
    if(!hCh->IsInfo(kInfoChannelTell))
    {
        hCh->Send("You have tells off, type 'info tell' to turn them back on.\n");
        return;
    }
    if(!nTarget->IsInfo(kInfoChannelTell))
    {
        Act("$N has tells turned off and cannot be told to.\n", hCh, NULL, nTarget, ToChar, kPosDead);
        return;
    }
    ASSERT(nTarget->mSession != NULL);
    if(nTarget->mSession->IsLinkless())
    {
        Act("$N is linkless.\n", hCh, NULL, nTarget, ToChar, kPosDead);
        return;
    }
    
    
    //Idle Long?
    TIMET nIdleSecs = gExactTime.mSeconds-nTarget->mSession->mIdleSince;
    if(nIdleSecs > 45)
    {
        STRINGCW    nIdleSecsStr;
        nIdleSecsStr.SPrintF("%ld", nIdleSecs);
        ActStr      nActSecs(nIdleSecsStr);
        Act("$N has been idle for $t seconds.\n", hCh, nActSecs, nTarget, ToChar, kPosDead);
    }
    
    
    //Tell Them
    STRINGCW nMsg;
    
    nMsg = ActParseTo(hCh, "You tell $N \"$t\"", hCh, nActArg, nTarget);
    hCh->SendF("^`%s^x\n", *nMsg);
    if(hCh->isPlayer)   
        hCh->isPlayer->mChannels[kChannelTell].AddMessage(nMsg);
    
    nMsg = ActParseTo(nTarget, "$n tells you \"$t\"", hCh, nActArg, nTarget);
    nTarget->SendF("^`%s^x\n", *nMsg);
    if(nTarget->isPlayer)
    {   
        nTarget->isPlayer->mReplyTo = hCh->mName;
        nTarget->isPlayer->mChannels[kChannelTell].AddMessage(nMsg);
    }
}




void Do_Say(Person* hCh, STRINGCW& hArg)
{
    //** Show Channel History **//
    if(hArg.Empty())
    {
        if(!hCh->isPlayer)
            return;
        
        hCh->SendF("%s\n", OverlayCenterLine("[ ^TSay History^x ]", kGfxSep));
        for(BYTE i = 0; i < kMaxChannelHistory; ++i)
        if(!hCh->isPlayer->mChannels[kChannelSay].mMessages[i].Empty())
            hCh->SendF("\n^H[Say]^x: %s\n", *hCh->isPlayer->mChannels[kChannelSay].mMessages[i]);
        
        return;
    }
    
    
    
    //** Say Something **//
    PersonHolder    nSentTo;
    ActStr          nActArg(hArg);
    STRINGCW        nMessage;
    Person*         nPerson;


    Act("^)You say, '$t'^x\n", hCh, nActArg, NULL, ToChar, kPosResting, &nSentTo);
    Act("^)$n says, '$t'^x\n", hCh, nActArg, NULL, ToRoom, kPosResting, &nSentTo);
    
    for(PersonIter i = nSentTo.Begin(); i != nSentTo.End(); ++i)
    {
        if(!(nPerson = *i)->isPlayer)
            continue;
                    
        if(hCh == nPerson)
            nMessage = ActParseTo(nPerson, "You say, '$t'", hCh, nActArg, NULL);
        else
            nMessage = ActParseTo(nPerson, "$n says, '$t'", hCh, nActArg, NULL);
            
        nPerson->isPlayer->mChannels[kChannelSay].AddMessage(nMessage);
    }
}



void Do_SayTo(Person* hCh, STRINGCW& hArg)
{
    //** Show Channel History **//
    if(hArg.Empty())
    {
        Do_Say(hCh, hArg);
        return;
    }
    
    
    //** SayTo Something **//
    PersonHolder    nSentTo, nTargets;
    STRINGCW        nTargetArg, nMessage;
    Person*         nPerson, *nTarget;


    hArg = hArg.OneArgument(nTargetArg);
    ActStr nActArg(hArg);
    if(hArg.Empty())
    {
        hCh->Send("Say what to them?\n");
        return;
    }
    
    
    //Find Target
    if(!hCh->mInRoom->RetrievePeopleAs(hCh, nTargetArg, nTargets))
    {
        hCh->SendF("You don't see any \"%s\".\n", *nTargetArg);
        return;
    }
    if(nTargets.Size() != 1)
    {
        hCh->Send("You can only say to one person at a time!\n");
        return;
    }
    nTarget = nTargets.Front();
    
    
    //Check Target
    if(nTarget == hCh)
    {
        hCh->Send("Talking to yourself shouldn't be done outloud.\n");
        return;
    }
    if(!nTarget->IsAwake())
    {
        hCh->Send("They aren't awake to hear you.\n");
        return;
    }
    
    
    //Say To It
    Act("^)You say to $N, '$t'^x\n" , hCh, nActArg, nTarget, ToChar, kPosResting, &nSentTo);
    Act("^)$n says to you, '$t'^x\n", hCh, nActArg, nTarget, ToVict, kPosResting, &nSentTo);
    Act("^)$n says to $N, '$t'^x\n" , hCh, nActArg, nTarget, ToNotVict, kPosResting, &nSentTo);
    for(PersonIter i = nSentTo.Begin(); i != nSentTo.End(); ++i)
    {
        if(!(nPerson = *i)->isPlayer)
            continue;
                    
        if(nPerson == hCh)
            nMessage = ActParseTo(nPerson, "You say to $N, '$t'", hCh, nActArg, nTarget);
        else if(nPerson == nTarget)
            nMessage = ActParseTo(nPerson, "$n says to you, '$t'", hCh, nActArg, nTarget);
        else
            nMessage = ActParseTo(nPerson, "$n says to $N, '$t'", hCh, nActArg, nTarget);
            
        nPerson->isPlayer->mChannels[kChannelSay].AddMessage(nMessage);
    }
}




void Do_Tell(Person* hCh, STRINGCW& hArg)
{
    //** Show Channel History **//
    if(hArg.Empty())
    {
        if(!hCh->isPlayer)
            return;
        
        hCh->SendF("%s\n", OverlayCenterLine("[ ^TTell History^x ]", kGfxSep));
        for(BYTE i = 0; i < kMaxChannelHistory; ++i)
        if(!hCh->isPlayer->mChannels[kChannelTell].mMessages[i].Empty())
            hCh->SendF("\n^H[Tell]^x: %s\n", *hCh->isPlayer->mChannels[kChannelTell].mMessages[i]);
        
        return;
    }
    
    
    //** Tell Someone Something **//
    STRINGCW        nTargetArg, nMessage;
    PersonHolder    nTargets;
    Person*         nTarget;


    hArg = hArg.OneArgument(nTargetArg);
    ActStr nActArg(hArg);
    if(hArg.Empty())
    {
        hCh->Send("Tell what to them?\n");
        return;
    }
    
    
    //Find Target
    if(!MUD::Get()->mPlayers.RetrievePeopleAs(hCh, nTargetArg, nTargets))
    {
        hCh->SendF("There is no one named \"%s\" online right now.\n", *nTargetArg);
        return;
    }
    if(nTargets.Size() != 1)
    {
        hCh->Send("You can only tell to one person at a time!\n");
        return;
    }
    nTarget = nTargets.Front();
    
    
    //Check Target
    if(nTarget == hCh)
    {
        hCh->Send("Telling to yourself?\n");
        return;
    }
    if(!hCh->IsInfo(kInfoChannelTell))
    {
        hCh->Send("You have tells off, type 'info tell' to turn them back on.\n");
        return;
    }
    if(!nTarget->IsInfo(kInfoChannelTell))
    {
        Act("$N has tells turned off and cannot be told to.\n", hCh, NULL, nTarget, ToChar, kPosDead);
        return;
    }
    ASSERT(nTarget->mSession != NULL);
    if(nTarget->mSession->IsLinkless())
    {
        Act("$N is linkless.\n", hCh, NULL, nTarget, ToChar, kPosDead);
        return;
    }
    
    
    //Idle Long?
    TIMET nIdleSecs = gExactTime.mSeconds-nTarget->mSession->mIdleSince;
    if(nIdleSecs > 45)
    {
        STRINGCW    nIdleSecsStr;
        nIdleSecsStr.SPrintF("%ld", nIdleSecs);
        ActStr      nActSecs(nIdleSecsStr);
        Act("$N has been idle for $t seconds.\n", hCh, nActSecs, nTarget, ToChar, kPosDead);
    }
    
    
    //Tell Them
    STRINGCW nMsg;
    
    nMsg = ActParseTo(hCh, "You tell $N \"$t\"", hCh, nActArg, nTarget);
    hCh->SendF("^`%s^x\n", *nMsg);
    if(hCh->isPlayer)   
        hCh->isPlayer->mChannels[kChannelTell].AddMessage(nMsg);
    
    nMsg = ActParseTo(nTarget, "$n tells you \"$t\"", hCh, nActArg, nTarget);
    nTarget->SendF("^`%s^x\n", *nMsg);
    if(nTarget->isPlayer)
    {   
        nTarget->isPlayer->mReplyTo = hCh->mName;
        nTarget->isPlayer->mChannels[kChannelTell].AddMessage(nMsg);
    }
}




