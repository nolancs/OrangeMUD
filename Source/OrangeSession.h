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
#ifndef ORANGESESSION_H
#define ORANGESESSION_H 1
#include "LoadOld.h"
#include "OrangeTCP.h"
#include "ANSI.h"

enum
{
    kLoadPlayer_ErrWithFile ,
    kLoadPlayer_NonExistant ,
    kLoadPlayer_Success
};


enum
{
    kStateMUDLoading,
    kStateSplash,
    kStateMenu,
    kStateMOTD,
    kStatePlaying,
    kStateReconnectEmptyLink,
    kStateReconnectOverLink,
    //Logging In
    kStateOldName,
    kStateOldPass
    //New Characters
};


class OrangeSession
{
    //** De/constructors **//
    public: OrangeSession(OrangeTCP* hTCP);
    public: ~OrangeSession();

    //** Public Member Functions **//
    public:
    bool    IsLinkless();
    void    OnLoseLink();
    void    OnReconnect(OrangeSession* hFromSession);
    CHAR    LoadPlayer(STRINGCW& hName);
    //Command-State Transitions
    void    EnterMUDLoading();
    void    EnterSplash();
    void    EnterMenu();
    void    EnterOldName();
    void    EnterOldPass();
    void    EnterMOTD();
    void    EnterPlaying();
    void    EnterReconnectEmptyLink();
    void    EnterReconnectOverLink();
    //Command-State Processing
    void    ProcessCommands();
    void    HandleSplash(STRINGCW& hCmd);
    void    HandleMenu(STRINGCW& hCmd);
    void    HandleOldName(STRINGCW& hCmd);
    void    HandleOldPass(STRINGCW& hCmd);
    void    HandleMOTD(STRINGCW& hCmd);
    void    HandlePlaying(STRINGCW& hCmd);
    void    HandleReconnectOverLink(STRINGCW& hCmd);
    

    //** Persistant Data Members **//
    public:
    SHORT                   mState;
    Person*                 mPerson;
    queue<STRINGCW>         mInputLines;
    Vector<STRINGCW>        mColorPrefStrs;
    TIMET                   mIdleSince;
    APtr<OrangeTCP>         mTCP;
    OrangeSession*          mReconnectSession;
};




#endif //ifndef ORANGESESSION_H
