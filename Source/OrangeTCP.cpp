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
#include "OrangeTCP.h"
#include "OrangeSession.h"
#include "MUD.h"
#include "OSUtils.h"
#include "Tables.h"
#include "Player.h"
#include "StringUtils.h"




OrangeTCP::OrangeTCP(TCPLink* hLink)
    : mLink(hLink)
    , mSession(NULL)
    , mAtPrompt(false)
    , mMoreInPaged(false)
    , mColorPaused(false)
{
}


OrangeTCP::~OrangeTCP()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


LONG OrangeTCP::WriteToLinkInANSI(const CHAR* hData, LONG hLen)
{
    LONG    nWrote = 0, nCodeLen = 0;
    CHAR*   nCode;
    CHAR    c;
    
    if(hLen == 0)
        return 0;
    
    //Send a NewLine if they're sitting at the prompt still
    if(mAtPrompt)
    {
        mAtPrompt = false;
        mLink->Write("\r\n", 2);
    }
    
    
    for(LONG i = 0; i < hLen; ++i)
    {
        if((c = hData[i]) == '^')
        {
            if(i+1 == hLen)
            {
                nWrote++;
                continue;
            }
            
            i++;
            nWrote += 2;
            nCode = (CHAR*) GrabColorCode(hData[i], nCodeLen);

            if(nCodeLen)
                mLink->Write(nCode, nCodeLen);
                
            continue;
        }
        
        if(c == '\n' || c == '\r')
        {
            mLink->Write("\r\n", 2);
            nWrote += 1;
            continue;
        }
        
        if(mLink->Write(&c, 1) < 1)
            return nWrote;
        
        nWrote++;
    }
    
    return nWrote;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void OrangeTCP::Write(const CHAR* hData, LONG hLen)
{
    ASSERT(mSession);

    if(hLen == -1)
        hLen = strlen(hData);

    mOutputDirect.Append(hData, hLen);
}


void OrangeTCP::WritePaged(const CHAR* hData, LONG hLen)
{
    ASSERT(mSession);

    if(hLen == -1)
        hLen = strlen(hData);

    //mOutputDirect.Append(hData, hLen);
    mOutputPaged.Append(hData, hLen);
}


void OrangeTCP::WriteNow(const CHAR* hData, LONG hLen)
{
    ASSERT(mSession);

    WriteToLinkInANSI(hData, hLen);
}



void OrangeTCP::ProcessInput()
{
    ASSERT(mSession);
    if(mLink.Ptr() && !mLink->ConnectionAlive())
    {
        mSession->OnLoseLink();
        return;
    }
    
    if(!mLink.Ptr() || !mLink->ReadReady())
        return;

    
    CHAR    nFiltered[kMaxInputLen];
    CHAR    nRaw[kMaxInputLen];
    LONG    nRead, f, r;
    
    nRead = mLink->Read(nRaw, kMaxInputLen);
    if(nRead == 0)
    {
        mSession->OnLoseLink();
        return;
    }
    mInput.Append(nRaw, nRead);
    
    
    //## MacOS Quirk ##//
    //Now we have to clean up the input because MacOS MUD clients like Rapscallion
    //only send a \n after every command, but telnet and Windows clients like
    //Pueblo send a \n after every line. Simplest way around it is just to screen
    //out all the \r characters and break commands apart by \n.
    UCHAR nInChar;
    
    #define kTrueEOL    '\n'
    #define kFakeEOL    '\r'
    
    for(r = f = 0; r < mInput.Length(); ++r)
    if(mInput[r] != kFakeEOL)
    {
        //** End Of Command Line **//
        if(mInput[r] == kTrueEOL)
        {
            nFiltered[f] = '\0';
            mSession->mInputLines.push(STRINGCW(nFiltered));
            f = 0;
        }
        else
        //** Filter Telnet Command Replies **//
        if((nInChar = mInput[r]) == tAIC)
        {
            //Telnet commands are always 3 bytes long and start with 255 (AIC),
            //unless you're working with option subnegotiation which we aren't.
            if(r+2 < mInput.Length())
                r += 2; //Skip the telnet data if we have it all
        }
        else
        {
            nFiltered[f++] = mInput[r];
        }
    }
    
    //Set Input to What Wasnt Processed
    nFiltered[f] = '\0';
    mInput = nFiltered;
    
    mSession->mIdleSince = CurrentTime();
    mAtPrompt = false;  //They just hit return to send this text
    ProcessInput();
}



void OrangeTCP::ProcessOutput()
{
    ASSERT(mSession);
    if(!mLink.Ptr() || !mLink->WriteReady())
        return;

    if(!mOutputDirect.Empty() || !mOutputPaged.Empty())
    {
        LONG nWrote = 0, nPaged = 0;

        try
        {
            //** Write Paged **//
            if(!mOutputPaged.Empty() && !mMoreInPaged)
            {
                SHORT nLinesToWrite =   mSession->mPerson->isPlayer ?
                                        mSession->mPerson->isPlayer->mScroll : 20;
                if(nLinesToWrite == 0)
                {
                    nPaged = WriteToLinkInANSI(mOutputPaged.CStr(), mOutputPaged.Length());
                    mMoreInPaged = false;
                }else{
                    LONG nLen = CountOffLines(mOutputPaged, nLinesToWrite);
                    mMoreInPaged = (nLen != mOutputPaged.Length());
                    nPaged = WriteToLinkInANSI(mOutputPaged.CStr(), nLen);
                }
            }


            //** Write Direct **//
            nWrote = WriteToLinkInANSI(mOutputDirect.CStr(), mOutputDirect.Length());
        }
        catch(...)
        {
            MUDBug("WriteToLinkInANSI threw an exception.");
            return;
        }


        //** Erase When Exceptions Are Safely Caught **//
        if(nPaged > 0)
            mOutputPaged.Erase(0, nPaged);
        if(nWrote > 0)
            mOutputDirect.Erase(0, nWrote);
    }
    
    
    //** Write Prompt & Paged Info **//
    try
    {
        if(!mSession->mPerson || mSession->mState != kStatePlaying)
            return;
        if(mAtPrompt)
            return;

        STRINGCW& nPrompt = mSession->mPerson->GetParsedPrompt();

        WriteToLinkInANSI("\n", 1);
        WriteToLinkInANSI(nPrompt.CStr(), nPrompt.Length());

        if(mMoreInPaged)
        {
            ULONG nPagedLines = CountLines(mOutputPaged);
            static CHAR     sPagedPromptBuf[250];
            static ULONG    sPagedPromptLen;

            ASSERT(nPagedLines != 0);
            sPagedPromptLen = sprintf(sPagedPromptBuf,
            "\n^I[ More -- %ld Lines -- Hit Return or (Q)uit ]^x", nPagedLines);
            WriteNow(sPagedPromptBuf, sPagedPromptLen);
        }

        mAtPrompt = true;
    }
    catch(...)
    {
        MUDBug("WriteToLinkInANSI threw an exception.");
        return;
    }
}


const CHAR* OrangeTCP::GrabColorCode(CHAR hCode, LONG& hCodeLen)
{
    ASSERT(mSession);
    //Check For Color-Toggling ColorPause
    if(hCode == kColorPause)
    {
        mColorPaused = !mColorPaused;
        hCodeLen = 0;
        return "";
    }
    

    if(mColorPaused)
    {
        static CHAR sGiveCodeBuf[3];
        
        hCodeLen = 2;
        sprintf(sGiveCodeBuf, "^%c", hCode);
        return sGiveCodeBuf;
    }
    

    //We Have Color On?
    if(mSession->mPerson)
    if(!mSession->mPerson->IsOpt(kOptColor))
    {
        hCodeLen = 0;
        return "";
    }
    
    
    ANSIType& nANSIInfo = gANSITable[hCode];
    if(nANSIInfo.mIsStatic)
    {
        hCodeLen = strlen(nANSIInfo.mCode);
        return nANSIInfo.mCode;
    }
    else
    {
        STRINGCW& nCodeStr = mSession->mColorPrefStrs[nANSIInfo.mPos];
        
        hCodeLen = nCodeStr.Length();
        return nCodeStr;
    }
}


