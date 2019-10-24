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
#include "MUD.h"
#include "Help.h"
#include "StringUtils.h"
#include "Player.h"
#include "OSUtils.h"
#include "RoomIndex.h"
#include "Act.h"
#include "Channel.h"


OrangeSession::OrangeSession(OrangeTCP* hTCP)
    : mState(kStateSplash)
    , mPerson(NULL)
    , mIdleSince(CurrentTime())
    , mTCP(hTCP)
    , mReconnectSession(NULL)
{
    mTCP->mSession = this;
}


OrangeSession::~OrangeSession()
{
    if(mPerson)
        delete mPerson;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void OrangeSession::OnLoseLink()
{
    if(mPerson && mState == kStatePlaying)
    {
        Channel::Broadcast(-1, NULL, 
            kInfoLinks, "^]Info || ", "$n has lost $s link.^x", 
            mPerson, NULL, NULL, kActWorld|kActNoChar);
    }

    cout << "Session(" << mTCP->mLink->GetPeerName() << ") lost link" << endl;  
    mTCP->mLink = NULL;
    
    
    //If you lose link and you aren't playing then you are gone for good.
    if(mState != kStatePlaying)
        MUD::Get()->KillSession(this);
}




void OrangeSession::OnReconnect(OrangeSession* hFromSession)
{
    ASSERT(mPerson && mState == kStatePlaying);
    
    Channel::Broadcast(-1, NULL, 
        kInfoLinks, "^]Info || ", "$n has reconnected.^x", 
        mPerson, NULL, NULL, kActWorld|kActNoChar);
    
    Act("$n has reconnected.\n", mPerson, NULL, NULL, ToRoom);
    mTCP->Write("\n");
}


bool OrangeSession::IsLinkless()
{
    ASSERT(!mTCP.IsNull());
    
    return mTCP->mLink.IsNull();    
}

CHAR OrangeSession::LoadPlayer(STRINGCW& hName)
{
    return kLoadPlayer_Success;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void OrangeSession::EnterMUDLoading()
{
    mState = kStateMUDLoading;
    mTCP->Write("Age of Wonder is loading, please wait...\n");
    mTCP->Write(MUD::Get()->mLoadingProgress);
}


void OrangeSession::EnterSplash()
{
    mState = kStateSplash;
    Help* nHelpGreeting = MUD::Get()->mHelpGreeting;
    
    if(nHelpGreeting)
    {
        STRINGCW nPercentS = "%s";
        if (nHelpGreeting->mText.IsNamed(nPercentS))
        {
            // If the splash screen contains a %s write MUD statistics to it
            ULONG nCount = MUD::Get()->mPlayers.Size();
            STRINGCW nInfoStr, nGreeting;
            nInfoStr.SPrintF("%lu player%s online", nCount, nCount == 1 ? "" : "s");
            nGreeting.SPrintF(nHelpGreeting->mText, *nInfoStr);
            mTCP->Write(nGreeting.CStr(), nGreeting.Length());
        }
        else
        {
            mTCP->Write(nHelpGreeting->mText.CStr(), nHelpGreeting->mText.Length());
        }
    }
    else
    {
        mTCP->Write("[No 'greeting' Splash Screen]\n");
    }
}




void OrangeSession::EnterMenu()
{
    mState = kStateMenu;
    Help* nHelpMenu = MUD::Get()->mHelpMenu;
    
    if(nHelpMenu)
        mTCP->Write(nHelpMenu->mText.CStr(), nHelpMenu->mText.Length());
    else
        mTCP->Write("[No 'menu' Login Menu]\n");
}




void OrangeSession::EnterMOTD()
{
    mState = kStateMOTD;
    Help* nHelpMOTD = MUD::Get()->mHelpMOTD;
    
    if(nHelpMOTD)
        mTCP->Write(nHelpMOTD->mText.CStr(), nHelpMOTD->mText.Length());
    else
        mTCP->Write("[No 'motd' Message Of The Day]\n");
}


void OrangeSession::EnterPlaying()
{
    ASSERT(mPerson && mPerson->isPlayer);
    
    mState = kStatePlaying;
    
    mTCP->Write("\n^y** Entering The Game **^x\n");
    if(!mPerson->mWasInRoom)
        mPerson->mWasInRoom = RoomIndex::Get(kVRef_Room_Start);
        
    mPerson->PlaceInGame(mPerson->mWasInRoom, true);
    //Do_Look();
}



void OrangeSession::EnterOldName()
{
    mState = kStateOldName;
    mTCP->Write("Name: ");
}


void OrangeSession::EnterOldPass()
{
    mState = kStateOldPass;
    mTCP->Write("Password: ");
    mTCP->Write((CHAR*) kMakeClientStopEchoing);
}



void OrangeSession::EnterReconnectEmptyLink()
{
    ASSERT(mReconnectSession != NULL);
    
    mState = kStateReconnectEmptyLink;
    mTCP->Write("Reconnecting.\n");
    mTCP->ProcessOutput();
    
    mReconnectSession->mTCP->mLink = mTCP->mLink.Detach();
    mReconnectSession->OnReconnect(this);
    MUD::Get()->KillSession(this);
}



void OrangeSession::EnterReconnectOverLink()
{
    ASSERT(mReconnectSession != NULL);

    mState = kStateReconnectOverLink;
    mTCP->Write("You are already connected, override other link?\n");
    mTCP->Write("[Y]es/[N]o?\n");
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void OrangeSession::ProcessCommands()
{
    while(!mInputLines.empty())
    {
        STRINGCW nInput = mInputLines.front();
        
        mInputLines.pop();

        //** Handle Paged Text Prompt **//
        if(!mTCP.IsNull() && mTCP->mMoreInPaged)
        {
            if(nInput.Empty())  //Continue Paging
            {
                mTCP->mMoreInPaged = false;
                return;
            }else{              //Stop Paging
                mTCP->mOutputPaged.Clear();
                mTCP->mMoreInPaged = false;
                return;
            }
            
        }
        
        //Handle Handle Handle
        switch(mState)
        {
            case kStateMUDLoading:
                break;
            case kStateSplash:
                HandleSplash(nInput);
                break;
            case kStateMenu:
                HandleMenu(nInput);
                break;
            case kStateMOTD:
                HandleMOTD(nInput);
                break;
            case kStatePlaying:
                HandlePlaying(nInput);
                break;
            
            //Logging In Existing Characters
            case kStateOldName:
                HandleOldName(nInput);
                break;
            case kStateOldPass:
                HandleOldPass(nInput);
                break;
            case kStateReconnectOverLink:
                HandleReconnectOverLink(nInput);
                break;

            //Creating A New Character
            
            default:
                ASSERT(!"Unhandled OrangeSession mState!");
                break;          
        }
    }
}




void OrangeSession::HandleSplash(STRINGCW& hCmd)
{
    EnterMenu();
}



void OrangeSession::HandleMenu(STRINGCW& hCmd)
{
    if(hCmd.Empty())
    {
        EnterMenu();
        return;
    }
    
    enum
    {
        kPlayExistingCharacter  = 1,
        kCreateNewCharacter     = 2,
        kLetMeQuit              = 3
    };
    
    
    //** Load An Old Player **//
    if(!hCmd.IsNumber())
    {
        mState = kStateOldName;
        HandleOldName(hCmd);
        return;
    }

    //** Choose A Number Off The Menu **//
    LONG nChoice = hCmd.AsNumber();
    
    if(nChoice == kLetMeQuit)
    {
        mTCP->WriteNow("Goodbye.\n");
        MUD::Get()->KillSession(this);
        return;
    }
    
    if(nChoice == kPlayExistingCharacter)
    {
        EnterOldName();
        return;
    }

    if(nChoice == kCreateNewCharacter)
    {
        mTCP->Write("New players cannot be created yet. Hit Return To Continue.\n");
        return;
    }
    
    
    mTCP->Write("Invalid Choice. Hit Return To Continue.\n");
}




void OrangeSession::HandleMOTD(STRINGCW& hCmd)
{
    EnterPlaying();
}




void OrangeSession::HandlePlaying(STRINGCW& hCmd)
{
    mPerson->Interpret(hCmd);
}




void OrangeSession::HandleOldName(STRINGCW& hName)
{
    Player* nPlayer = NULL;
    
    
    //Load new xml file
    switch(MUD::Get()->LoadPlayer(hName, nPlayer))
    {
        case kLoadPlayer_ErrWithFile:
            mTCP->Write("Error Loading File.\n");
            if(nPlayer)
                delete nPlayer;
            mState = kStateMenu;
            break;
        case kLoadPlayer_Success:
            mPerson = nPlayer->isPlayer;
            mPerson->mSession = this;
            EnterOldPass();
            break;
    }
    
    
    //Attempt to load an old player file
    if(nPlayer == NULL)
    switch(MUD::Get()->LoadPlayerOld(hName, nPlayer))
    {
        case kLoadPlayer_ErrWithFile:
            mTCP->Write("Error Loading File.\n");
            if(nPlayer)
                delete nPlayer;
            mState = kStateMenu;
            break;
        case kLoadPlayer_NonExistant:
            mTCP->Write("No Such Player.\n");
            mState = kStateMenu;
            break;
        case kLoadPlayer_Success:
            mPerson = nPlayer->isPlayer;
            mPerson->mSession = this;
            EnterOldPass();
            break;
    }
}


void OrangeSession::HandleOldPass(STRINGCW& hPass)
{
    ASSERT(mPerson && mPerson->isPlayer);
        
    mTCP->Write((CHAR*) kMakeClientStartEchoing);
    mTCP->Write("\n");

    STRING nPass64;
    Base64Encode(hPass, hPass.Length(), nPass64);
    if(nPass64.empty() || mPerson->isPlayer->mPassword != nPass64.c_str())
    {
        mTCP->Write("Invalid password.\n");
        mState = kStateMenu;
        delete mPerson;
        mPerson = NULL;
        return;
    }
    

    //** Check If We're Logged In Already! **//
    MUD* nMUD = MUD::Get();
    for(SessionIter i = nMUD->mSessions.begin(); i != nMUD->mSessions.end(); ++i)
    {
        OrangeSession* nSession = *i;
        
        if(nSession == this || !nSession->mPerson || !nSession->mPerson->isPlayer)
            continue;
        if(nSession->mPerson->mName != mPerson->mName)
            continue;
        if(!nSession->mTCP.Ptr())
            continue;
        
        mReconnectSession = nSession;
        if(nSession->mTCP->mLink == NULL)
            EnterReconnectEmptyLink();
        else
            EnterReconnectOverLink();
        return;
    }
        

    EnterMOTD();
}



void OrangeSession::HandleReconnectOverLink(STRINGCW& hCmd)
{
    ASSERT(mReconnectSession != NULL);
    static STRINGCW sYes("Yes");
    static STRINGCW sNo("No");
    
    if(sYes.IsNamed(hCmd))
    {
        mTCP->Write("Verywell then. Reconnecting.\n");
        mTCP->ProcessOutput();
        mReconnectSession->mTCP->mLink = mTCP->mLink.Detach();
        mReconnectSession->OnReconnect(this);
        MUD::Get()->KillSession(this);
    }
    else
    if(sNo.IsNamed(hCmd))
    {
        mTCP->Write("Verywell. Goodbye then.\n");
        mTCP->ProcessOutput();
        MUD::Get()->KillSession(this);
    }
    else
    {
        mTCP->Write("[Y]es/[N]o?\n");
    }
}




