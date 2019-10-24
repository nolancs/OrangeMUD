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
#include "MUD.h"
#include "MUDFile.h"


void MFBug(MUDFile& hFile, CHAR* hStr, ...)
{
    cout << "Line: " << hFile.mAtLine << " $$ " << hFile.mWord << " $$ " << hStr << endl;
}


MUDFile::MUDFile(FileLink* hOurLink)
{
    mLink       = hOurLink;
    mAtLine     = 0;
    mWord[0]    = '\0';
    mOK         = true;
}



MUDFile::~MUDFile()
{
    if(mLink)
        delete mLink;
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//Takes all the \r out of strings with \n terminated lines
bool MUDFile::FilterChar(CHAR &c)
{
    if(c == '\r')
        return true;
    if(c == EOT)
        return true;
    
    return false;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



CHAR MUDFile::GetC()
{
    CHAR c = mLink->GetC();
    
    if(c == '\n' || c == '\r')
        mAtLine++;
    
    return c;
}



void MUDFile::UnGetC(CHAR hC)
{
    if(hC == '\n' || hC == '\r')
        mAtLine--;

    mLink->UnGetC(hC);
}



CHAR MUDFile::ReadLetter()
{
    CHAR c;

    do
    {
        c = GetC();
    }
    while(isspace(c));

    return c;
}



float MUDFile::ReadFloat()
{
    bool    nIsNegative = false;
    STRINGCW    nFloatStr;
    float   nFloatNum;
    CHAR    c;


    //Read whitespace
    do
    {
        c = GetC();
    }
    while(isspace(c));


    //Read signedness
    if(c == '+')
    {
        c = GetC();
    }
    else if(c == '-')
    {
        nIsNegative = true;
        c = GetC();
    }

    if(!isdigit(c) && c != '.')
        MUDFatalBug("MUDFile: ReadFloat: bad format");

    while(isdigit(c) || c == '.')
    {
        nFloatStr += c;
        c = GetC();
    }
    

    nFloatNum = atof(nFloatStr);
    
    if(nIsNegative)
        nFloatNum = -nFloatNum;

    if(c == '|')
        nFloatNum += ReadFloat();
    else 
    if(c != ' ')
        UnGetC(c);

    return nFloatNum;
}



LONG MUDFile::ReadNumber()
{
    bool hIsNegative = false;
    LONG hNumber = 0;
    char c;


    //Read whitespace
    do
    {
        c = GetC();
    }
    while(isspace(c));

    
    //Read signedness
    if(c == '+')
    {
        c = GetC();
    }
    else if(c == '-')
    {
        hIsNegative = true;
        c = GetC();
    }

    if(!isdigit(c))
        MUDFatalBug("File: ReadNumber: bad format");


    while(isdigit(c))
    {
        hNumber = hNumber * 10 + c - '0';
        c = GetC();
    }

    if(hIsNegative)
        hNumber = -hNumber;

    if(c == '|')
        hNumber += ReadNumber();
    else 
    if(c != ' ')
        UnGetC(c);

    return hNumber;
}



ULONG MUDFile::ReadULong()
{
    bool hIsNegative = false;
    LONG hNumber = 0;
    char c;


    //Read whitespace
    do
    {
        c = GetC();
    }
    while(isspace(c));

    
    //Read signedness
    if(c == '+')
    {
        c = GetC();
    }
    else if(c == '-')
    {
        hIsNegative = true;
        c = GetC();
    }

    if(!isdigit(c))
        MUDFatalBug("File: ReadNumber: bad format");


    while(isdigit(c))
    {
        hNumber = hNumber * 10 + c - '0';
        c = GetC();
    }

    if(hIsNegative)
        hNumber = -hNumber;

    if(c == '|')
        hNumber += ReadNumber();
    else 
    if(c != ' ')
        UnGetC(c);

    return hNumber;
}



LONG MUDFile::ReadFlags()
{
    LONG    hNumber = 0;
    LONG    i;
    
    STRINGCW nFlagStr = ReadString();
    
    for(i = 0; nFlagStr[i] != '\0'; ++i)
    switch(nFlagStr[i])
    {
        case '0': hNumber = 0; break;
        case 'A': SetBit(hNumber, A); break;
        case 'B': SetBit(hNumber, B); break;
        case 'C': SetBit(hNumber, C); break;
        case 'D': SetBit(hNumber, D); break;
        case 'E': SetBit(hNumber, E); break;
        case 'F': SetBit(hNumber, F); break;
        case 'G': SetBit(hNumber, G); break;
        case 'H': SetBit(hNumber, H); break;
        case 'I': SetBit(hNumber, I); break;
        case 'J': SetBit(hNumber, J); break;
        case 'K': SetBit(hNumber, K); break;
        case 'L': SetBit(hNumber, L); break;
        case 'M': SetBit(hNumber, M); break;
        case 'N': SetBit(hNumber, N); break;
        case 'O': SetBit(hNumber, O); break;
        case 'P': SetBit(hNumber, P); break;
        case 'Q': SetBit(hNumber, Q); break;
        case 'R': SetBit(hNumber, R); break;
        case 'S': SetBit(hNumber, S); break;
        case 'T': SetBit(hNumber, T); break;
        case 'U': SetBit(hNumber, U); break;
        case 'V': SetBit(hNumber, V); break;
        case 'W': SetBit(hNumber, W); break;
        case 'X': SetBit(hNumber, X); break;
        case 'Y': SetBit(hNumber, Y); break;
        case 'Z': SetBit(hNumber, Z); break;
        case 'a': SetBit(hNumber, aa); break;
        case 'b': SetBit(hNumber, bb); break;
        case 'c': SetBit(hNumber, cc); break;
        case 'd': SetBit(hNumber, dd); break;
        case 'e': SetBit(hNumber, ee); break;
        default:
            ASSERT(!"Unknown character bit.");
            break;
    }
    
    return hNumber;
}



CHAR* MUDFile::ReadString()
{
    static STRINGCW nsReadStringStr;
    CHAR            c;
    
    
    do
    {
        c = GetC();
    }
    while(isspace(c));
    
    nsReadStringStr = "";   
    while(true)
    {
        if(c == EOF)
            return (CHAR*) (nsReadStringStr = "");
        if(c == EOT)
            break;          

//      if(c == '\r')
//          nsReadStringStr += '\n';
    
        if(c == '\n' || c == '\r')
            nsReadStringStr += "\n";
        else
            nsReadStringStr += c;
    
            
        c = GetC();
    }

    return (CHAR*) nsReadStringStr;
}


    
CHAR* MUDFile::ReadStringEOL()
{
    static STRINGCW nsReadStringEOLStr;
    CHAR            c;
    
    
    do
    {
        c = GetC();
    }
    while(isspace(c));
    
    nsReadStringEOLStr = "";    
    while(true)
    {
        if(c == EOF)
            return (CHAR*) (nsReadStringEOLStr = "");
        if(c == '\n' || c == '\r')
            break;          

        nsReadStringEOLStr += c;
        c = GetC();
    }

    return (CHAR*) nsReadStringEOLStr;
}



void MUDFile::ReadEOL()
{
    CHAR c;
    
    do
    {
        c = GetC();
    }
    while(c != '\n' && c != '\r');
}



CHAR* MUDFile::ReadWord()
{
    static STRINGCW nsReadWordStr;
    CHAR            nDelim, c;

    do
    {
        nDelim = GetC();
    }
    while(isspace(nDelim) || nDelim == '\n' || nDelim == '\r');

    if(nDelim != '\'' && nDelim != '"')
    {
        UnGetC(nDelim);
        nDelim = ' ';
    }
    
    nsReadWordStr = "";
    do
    {
        c = GetC();
        
        if(c == nDelim || c == '\n' || c == '\r')
            break;
        
        if(c == EOF)
        {
            nsReadWordStr.Clear();
            nsReadWordStr += (CHAR) EOF;
            return (CHAR*) nsReadWordStr;
        }
        
        nsReadWordStr += c;
    }
    while(true);

    
    return (CHAR*) nsReadWordStr;
}







#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void MUDFile::ForceVar(const CHAR* hName, SHORT hValue)
{
}


void MUDFile::ForceVar(const CHAR* hName, LONG hValue)
{
}


void MUDFile::ForceVar(const CHAR* hName, ULONG hValue)
{
}


void MUDFile::ForceVar(const CHAR* hName, float hValue)
{
}


void MUDFile::ForceVar(const CHAR* hName, CHAR hValue)
{
}


void MUDFile::ForceStr(const CHAR* hName, const CHAR* hStr)
{
}


void MUDFile::ForceWrd(const CHAR* hName, const CHAR* hWrd)
{
}


void MUDFile::WriteVar(const CHAR* hName, ULONG hValue, ULONG hOrig)
{
}


void MUDFile::WriteVar(const CHAR* hName, SHORT hValue, SHORT hOrig)
{
}


void MUDFile::WriteVar(const CHAR* hName, LONG  hValue, LONG hOrig)
{
}


void MUDFile::WriteVar(const CHAR* hName, float hValue, float hOrig)
{
}


void MUDFile::WriteVar(const CHAR* hName, CHAR hValue, CHAR hOrig)
{
}


void MUDFile::WriteStr(const CHAR* hName, const CHAR* hStr)
{
}


void MUDFile::WriteStr(const CHAR* hName, const CHAR* hStr, const CHAR* hOrig)
{
}


void MUDFile::WriteFlg(const CHAR* hName, LONG hFlags)
{
}


void MUDFile::WriteWrd(const CHAR* hName, const CHAR* wrd)
{
}


void MUDFile::WriteWrdVar(const CHAR* hName, const CHAR* hWrd, SHORT hValue)
{
}










