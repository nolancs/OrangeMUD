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
#include "StringMagic.h"
#include "StringUtils.h"
#include "TEditor.h"
#include "StringCW.h"
#include "Person.h"



TEditor::TEditor(CHAR* hOnText)
{
    mEOL        = '\n';
    mEOLLen     = 1;
    mEOLStr[0]  = mEOL;
    mEOLStr[1]  = '\0';
    CopyString(mBefore, hOnText);
    mNumLines   = 1;

    //** Count Lines In Odd-Fashion **//
    for(ULONG b = 0; mBefore[b] != '\0'; ++b)
    if(!strncmp(&mBefore[b], mEOLStr, mEOLLen))
        mNumLines += 2;
}




TEditor::~TEditor()
{
    FreeString(mBefore);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void TEditor::Update()
{
    FreeString(mBefore);
    CopyString(mBefore, mAfter.c_str());
    mAfter = "";
}





bool TEditor::Handle(Person* hCh, STRINGCW& hCommand, CHAR* hClearStr)
{
    STRINGCW    nArg, nVar;
    LONG        nLineNum, nLen;


    if(hCommand.Empty())
    {
        hCh->SendLined(mBefore, true);
        return false;
    }
    
    //** Delete **//
    if(hCommand == "-")
    {
        hCh->SendPaged(mBefore);
        hCh->SendPagedF("\n%s\n", hClearStr);
        mAfter = "";
        return true;
    }
    
    //** Format To Width **//
    if(hCommand[0] == '=')
    {   
        hCommand.OneArgument(nVar);
        nVar = &nVar[1];
        
        if(!nVar.IsNumber())
        {
            hCh->Send("You must provide a width, a number after the =.\n");
            return false;
        }
        
        if((nLen = atol(nVar)) < 10)
        {
            hCh->Send("Illegal width.\n");
            return false;
        }
        
        FormatWidth(nLen);
        if(hCh->IsOpt(kOptRepeat))
            hCh->SendLined(mAfter.c_str(), false);
        return true;
    }
    
    nArg = hCommand.OneArgument(nVar);
    
    if(!nVar.IsNumber())
    {
        AddLineToEnd(hCommand);
    }else{
        nLineNum = atol(nVar);
        
        if(nLineNum > mNumLines)
            AddLineToEnd(nArg);
        else
        if(nLineNum % 2)
            ReplaceLine(nArg, nLineNum);
        else
            InsertLine(nArg, nLineNum);
    }
    
    if(hCh->IsOpt(kOptRepeat))
        hCh->SendLined(mAfter.c_str(), true);
    
    return true;
}





void TEditor::AddLineToEnd(CHAR* hLine)
{
    CHAR nAddLine[kMaxInputLen];

    strcpy(nAddLine, hLine);
    RemoveTrailingSpaces(nAddLine);
    
    if(mBefore[0] != '\0')
    {   
        mAfter.append(mBefore);
        mAfter.append(mEOLStr);
    }
    mAfter.append(nAddLine);    
}




void TEditor::InsertLine(CHAR* hLine, LONG hWhereLine)
{
    CHAR    nAddLine[kMaxInputLen], nLineBuf[kMaxStringLen];
    LONG    nCurLine = 1;
    CHAR*   nScan = mBefore;
    
    strcpy(nAddLine, hLine);
    RemoveTrailingSpaces(nAddLine);
    
    if(hWhereLine < 1)
    {
        mAfter.append(nAddLine);
        if(mNumLines >= 1)
            mAfter.append(mEOLStr);
    }
    
    while(ParseLine(nScan, nLineBuf))
    {
        mAfter.append(nLineBuf);
        if(nCurLine != mNumLines && !(hWhereLine > mNumLines))
            mAfter.append(mEOLStr);
        
        if(hWhereLine == nCurLine+1)
        {
            mAfter.append(nAddLine);
            if(nCurLine < mNumLines)
                mAfter.append(mEOLStr);
        }
        nCurLine += 2;
    }
    
    if(hWhereLine > nCurLine)
        mAfter.append(nAddLine);
}




void TEditor::ReplaceLine(CHAR* hLine, LONG hWhereLine)
{
    CHAR    nAddLine[kMaxInputLen], nLineBuf[kMaxStringLen];
    CHAR*   nScan = mBefore;
    LONG    nCurLine = 1;
    

    strcpy(nAddLine, hLine);
    RemoveTrailingSpaces(nAddLine);
    
    if(nAddLine[0] == '\0')
    {
        DeleteLine(hWhereLine);
        return;
    }
    
    if(hWhereLine == 1)
    {
        mAfter.append(nAddLine);
        if(mNumLines > 1)
            mAfter.append(mEOLStr);
    }
    
    while(ParseLine(nScan, nLineBuf))
    {
        if(nCurLine != hWhereLine)
        {
            mAfter.append(nLineBuf);
            if(nCurLine != mNumLines)
                mAfter.append(mEOLStr);
        }else
        if(hWhereLine != 1)
        {
            mAfter.append(nAddLine);
            if(mNumLines > hWhereLine)
                mAfter.append(mEOLStr);
        }
        nCurLine += 2;
    }
}




void TEditor::DeleteLine(LONG hWhereLine)
{
    CHAR    nLineBuf[kMaxStringLen];
    CHAR*   nScan = mBefore;
    long    nCurLine = 1;
    

    while(ParseLine(nScan, nLineBuf))
    {
        if(nCurLine != hWhereLine)
        {
            mAfter.append(nLineBuf);
            if(!(nCurLine == mNumLines)
            && !(hWhereLine == nCurLine+2 && nCurLine+2 >= mNumLines))
                mAfter.append(mEOLStr);
        }
        nCurLine += 2;
    }
}




void TEditor::FormatWidth(ULONG hWide)
{
    CHAR    nLine[kMaxStringLen];
    LONG    b = 0;
    ULONG   nCLen;
    bool    nFirst = true;
    CHAR*   nWord;
    
    nLine[0] = '\0';

    while(mBefore[b] != '\0')
    {   
        //** Don't Format Lines With ^? In Front Of Them **//
        if(!strncmp(&mBefore[b], "^?", 2))
        {
            CHAR* nTemp = &mBefore[b];
            
            if(nLine[0] != '\0')
            {
                mAfter.append(nLine);
                mAfter.append(mEOLStr);
                nLine[0] = '\0';
            }
            ParseLine(nTemp, nLine);
            b += (nTemp-(&mBefore[b]));
            mAfter.append(nLine);
            mAfter.append(mEOLStr);
            nLine[0] = '\0';
            nFirst = true;
            continue;
        }
        //** END **//
    
        nWord = GetNextWord(mBefore, b);
        nCLen = CStrLen(nWord);
        
        if((CStrLen(nLine) + nCLen + 1) > hWide)
        {
            mAfter.append(nLine);
            mAfter.append(mEOLStr);
            nLine[0] = '\0';
            strcat(nLine, nWord);
            nFirst = false;
        }else{
            if(!nFirst)
                strcat(nLine, " ");
            strcat(nLine, nWord);
            nFirst = false;
        }
        
        //** Setup Text For Next Pass **//
        while(mBefore[b] == ' ')
            b++;
        
        if(!strncmp(&mBefore[b]         , mEOLStr, mEOLLen))
        {
            if(!strncmp(&mBefore[b+mEOLLen] , mEOLStr, mEOLLen))
            {
                //** A Blank Line Or More, Keep It **//
                while(!strncmp(&mBefore[b], mEOLStr, mEOLLen))
                {
                    mAfter.append(nLine);
                    mAfter.append(mEOLStr);
                    nFirst = true;
                    nLine[0] = '\0';
                    b += mEOLLen;
                }
            }else{
                //** Just One Return, Skip It **//
                b += mEOLLen;
            }
        }
    }
    
    mAfter.append(nLine);
}




bool TEditor::ParseLine(CHAR*& hSource, CHAR* hLineBuf)
{
    SHORT   i = 0;
    CHAR    c;
    
    if(hSource[0] == '\0')
        return false;
    
    while((c = *hSource) != '\0' && strncmp(hSource, mEOLStr, mEOLLen))
    {
        hSource++;
        hLineBuf[i++] = c;
    }
    
    hLineBuf[i] = '\0';
    if(c != '\0' && !strncmp(hSource, mEOLStr, mEOLLen))
        hSource += mEOLLen;

    return true;
}




void TEditor::PrefixLines(const CHAR* hWithStr)
{
    ULONG nBegin = 0, nEnd;
    
    
    for(nEnd = nBegin; mBefore[nEnd] != '\0'; ++nEnd)
    if(!strncmp(&mBefore[nEnd], mEOLStr, mEOLLen))
    {
        mAfter.append(hWithStr);
        mAfter.append(&mBefore[nBegin], (nEnd+mEOLLen)-nBegin);
        nBegin = nEnd+mEOLLen;
        nEnd = nBegin;
    }
    
    if(nEnd != nBegin)
    {
        mAfter.append(hWithStr);
        mAfter.append(&mBefore[nBegin]);
    }
}




CHAR* TEditor::GetNextWord(CHAR* hText, LONG& hOffset)
{
    static CHAR sBuf[kMaxStringLen];
    CHAR c;
    LONG b;
    
    while(isspace(hText[hOffset]))
        hOffset++;
        
    b = 0;
    while((c = (hText[hOffset])) != '\0' && !isspace(c) && !strchr(mEOLStr, c))
    {
        sBuf[b++] = hText[hOffset++];
    }
    
    sBuf[b] = '\0';
    return sBuf;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif






void TEditor::RemoveTrailingSpaces(CHAR* hLine)
{
    LONG nLen = strlen(hLine);
    
    if(hLine[0] != '\0')
    while(isspace(hLine[nLen]))
        nLen--;
    
    hLine[nLen] = '\0';
}



