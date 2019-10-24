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
#ifndef PLAYER_H
#define PLAYER_H 1
#include "Person.h"
#include "OrangeSession.h"
#include "Channel.h"


class Player 
    : public Person
{
    //** De/constructors **//
    public: Player();
    public: virtual ~Player();

    //** SaveLoadable **//
    bool            Save();
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag);
    
    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    virtual void    OnLoad();

    //** Member Functions **//
    virtual void    PlaceInGame(PersonHolder* hTo, bool hLoggingIn);
    virtual void    RemoveFromGame(bool hLoggingOut);
    virtual void    PlaceIn(PersonHolder* hTo);
    virtual void    RemoveFrom();
    virtual bool    GetRoundAttacks(AttackArray& hAttacks);
    virtual void    StatTo(Person* hTo);



    //** Runtime Data Members **//
    public:
    Immortal*           isImm;
    STRINGCW            mReplyTo;
    SHORT               mDamRoll;
    SHORT               mHitRoll;
    SHORT               mFlagsStatus;
    
    //** Persistant Data Members **//
    public:
    UID                 mUID;
    STRINGCW            mPassword;
    STRINGCW            mPrompt;
    STRINGCW            mTitle;
    STRINGCW            mTitleAFK;
    STRINGCW            mEmail;
    STRINGCW            mHomepage;
    TIMET               mLastLogin;
    TIMET               mTimePlayed;
    SHORT               mTrust;
    SHORT               mInvisLevel;
    SHORT               mScroll;
    LONG                mFlagsOptions;  
    LONG                mFlagsInfo;
    SHORT               mCondition[kCondMax];
    LONG                mColorPref[kColorPrefMax];
    Channel             mChannels[kChannelMax];
    ObjectHolder        mBank;
};







#endif //ifndef PLAYER_H

