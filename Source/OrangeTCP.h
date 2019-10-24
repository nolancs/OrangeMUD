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
#ifndef ORANGETCP_H
#define ORANGETCP_H 1
#include "TCPLink.h"


class OrangeTCP
{
    //** De/constructors **//
    public: OrangeTCP(TCPLink* hLink);
    public: ~OrangeTCP();
    
    //** Public Member Functions **//
    public:
    void        ProcessInput();
    void        ProcessOutput();
    void        Write(const CHAR* hData, LONG hLen = -1);
    void        WritePaged(const CHAR* hData, LONG hLen = -1);
    void        WriteNow(const CHAR* hData, LONG hLen = -1);
    const CHAR* GrabColorCode(CHAR hCode, LONG& hCodeLen);

    //** Protected Member Functions **//
    LONG        WriteToLinkInANSI(const CHAR* hData, LONG hLen);
    

    //** Runtime Data Members **//
    public:
    APtr<TCPLink>       mLink;
    OrangeSession*      mSession;
    bool                mAtPrompt;
    bool                mMoreInPaged;
    STRINGCW            mInput;
    STRINGCW            mOutputDirect;
    STRINGCW            mOutputPaged;

    protected:
    bool                mColorPaused;
};


//## Telnet Stuff ##//
//Telnet Command Form: IAC <WILL|WONT|DO|DONT> <SomeOption>
#define tAIC                    255     //Interpret As Command
#define tWill                   251     //Server Will Perform <THIS>
#define tWont                   252     //Server Wont Perform <THIS>
#define tDo                     253     //Please Do   Do <THIS>
#define tDont                   254     //Please Dont Do <THIS>
#define tSubNegStart            250
#define tSubNegEnd              240
#define tIs                     0       
#define tGA                     249     //You may reverse the line

#define tEcho                   1       //Server Sends Echod Text (Off by default)
#define tSuppressGA             3       //Do not send Go-Aheads
#define tTermType               24      //Subnegotiate terminal type

const UCHAR kMakeClientStopEchoing[]    = { tAIC, tWill, tEcho, '\0' };
const UCHAR kMakeClientStartEchoing[]   = { 10, 13, tAIC, tWont, tEcho, '\0' };
const UCHAR kGoAhead[]                  = { tAIC, tGA, '\0' };


//HOW-WHY:
//By telling the client that we will send echod text of everything they type
//we stop the clients telnet application from doing it. We actually DONT send
//anything back to echo it, so we're really just lying. This is how we stop a
//telnet app from showing your password as you type it.
//
//Most telnet apps will send a command right back at you if you send one to
//them. This is usually acknowledgement of the requested option change. You
//have to scan your input and check for kTelAICs, if one exists you have to
//skip the next 2 bytes so it doesn't interfere with player input. This is
//done in ProcessInput().

//INFO:
//A good URL for learning about this, and where I got all this info is from
//http://www.ebsnetinc.com/EBSmanuals/rtipman30/telnet.html or you can read
//the telnet RFCs


#endif //ifndef ORANGETCP_H
