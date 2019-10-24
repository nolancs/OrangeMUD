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
#include <cstdarg>




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Random
//
//   Desc:: The heart of any chance based game, the random number generator.
//          Semi-out of place being in the string functions file, it really
//          doesn't need its own file though. You hand this function a min and
//          a max value and it will give you a random number from [min, max].
//
//  Input:: hMin: The minimum value.
//          hMax: The maximum value.
//
// Output:: The random result from [min, max].
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG Random(LONG hMin, LONG hMax)
{
    LONG nDiff = hMax - hMin;

    if(!nDiff)
        return hMin;

    if((hMax = hMax - hMin + 1) <= 1)
        return hMin;

    double i = nDiff*(rand()/((double)RAND_MAX));
    double round = i - LONG(i);
    
    i += round < .5 ? 0 : .5; 
    return hMin + LONG(i); //funny math to round it
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Randomize
//
//   Desc:: Takes a number like 100 and a percentage of randomness of 5.0% and
//          gives you back a number anywhere from 95 to 105. 
//
//  Input:: hNum: Some value.
//          hPercent: Randomize it up/down this many percent.
//
// Output:: The randomize result.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG Randomize(LONG hNum, float hPercent)
{
    LONG nHalf = (LONG) float(hNum * hPercent);
    LONG nResult = hNum - nHalf;

    return nResult + Random(0, nHalf*2);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CStrLen
//
//   Desc:: Will tell you the "visual" length of a string, disregarding any
//          color codes embedded into its text and giving you the length of
//          only what would be displayed on the screen.
//
//  Input:: The string.
//
// Output:: The "visual" length of the string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ULONG CStrLen(const CHAR* hStr)
{
    ULONG nLen, i;
    
    for(nLen = 0, i = 0; hStr[nLen+i] != '\0'; )
    if(hStr[nLen+i] == '^')
    {
        if(hStr[nLen+i+1] == '^')
        {
            nLen++;
            i++;
        }else{
            i += 2;
        }
    }else{
        nLen++;
    }
    
    return nLen;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CCharAt
//
//   Desc:: Since you can't just assume that hStr[5] will be the 5th character
//          visually seen by the end user you need a way to get a pointer to
//          that Nth visual character in a string. This is the way.
//
//  Input:: The string with possible color codes.
//
// Output:: Pointer to the Nth "visual" character in the string. Or NULL if
//          there wasn't one.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* CCharAt(const CHAR* hStr, ULONG hNthChar)
{
    ULONG nLen, i;
    
    for(nLen = 0, i = 0; hStr[nLen+i] != '\0'; )
    {
        if(hNthChar == nLen)
            return (CHAR*) &hStr[nLen+i];
    
        if(hStr[nLen+i] == '^')
        {
            if(hStr[nLen+i+1] == '^')
            {
                nLen++;
                i++;
            }else{
                i += 2;
            }
        }else{
            nLen++;
        }
    }
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CTrunc
//
//   Desc:: Truncuate a color coded string to it only appears to be N
//          characters long.
//
//  Input:: The string and length.
//
// Output:: Pointer to the Nth "visual" character in the string. Or NULL if
//          there wasn't one.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* CTrunc(const CHAR* hStr, ULONG hTruncLen)
{
    if(CStrLen(hStr) <= hTruncLen)
        return (CHAR*) hStr;

    static SHORT sOnBuf; //5 static buffers
    static CHAR sBuffers[5][kMaxStringLen];
    CHAR* nBuf = sBuffers[++sOnBuf > 5 ? (sOnBuf = 0) : sOnBuf];

    LONG nHowManyChars = CCharAt(hStr, hTruncLen)-hStr;
    strncpy(nBuf, hStr, nHowManyChars);
    nBuf[nHowManyChars] = '\0';
    return nBuf;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StripColorCodes
//
//   Desc:: Strips out color codes from a string.
//
//  Input:: The string.
//
// Output:: Reference to input string for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW& StripColorCodes(STRINGCW& hStr)
{
    for(LONG i = 0; i < hStr.Length(); ++i)
    if(hStr[i] == '^')
    {
        if(hStr[i+1] == '^')
        {
            ++i;
            continue;
        }
        if(hStr[i+1] == '\0')
            hStr.Erase(i, 1);
        else
            hStr.Erase(i, 2);
    }
    
    return hStr;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: csprintf
//
//   Desc:: Just like sprintf but it adjusts the intended string lengths to
//          compensate for unseen color codes. It allows you write your format
//          string using "visual" lengths no matter how many color codes are
//          embedded in the strings you want inserted through the %s %10s.
//
//  Input:: The same as sprintf.
//
// Output:: The non-visual, actual length of characters written into hBuf.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG csprintf(CHAR* hBuf, const CHAR* hFormat, ...)
{
    va_list nAp;
    LONG    nLen;
    
    va_start(nAp, hFormat);
    nLen = vcsprintf(hBuf, hFormat, nAp);
    va_end(nAp);
    
    return nLen;    
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: csnprintf
//
//   Desc:: Just like snprintf but it adjusts the intended string lengths to
//          compensate for unseen color codes. It allows you write your format
//          string using "visual" lengths no matter how many color codes are
//          embedded in the strings you want inserted through the %s %10s.
//
//  Input:: The same as snprintf.
//
// Output:: The non-visual, actual length of characters written into hBuf.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG csnprintf(CHAR* hBuf, size_t hTruncLen, const CHAR* hFormat, ...)
{
    va_list hAp;
    ULONG   nLen, i;
    
    
    va_start(hAp, hFormat);
    nLen = vcsprintf(hBuf, hFormat, hAp);
    va_end(hAp);
    
    //** Trunctuate **//
    if(CStrLen(hBuf) <= hTruncLen)
        return nLen;
        
    for(nLen = 0, i = 0; hBuf[nLen+i] != '\0' && nLen < hTruncLen;)
    if(hBuf[nLen+i] == '^')
    {
        if(hBuf[nLen+i+1] == '^')
        {
            nLen++;
            i++;
        }else{
            i += 2;
        }
    }else{
        nLen++;
    }
    
    hBuf[nLen+i] = '\0';
    
    return nLen+i;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: vcsprintf
//
//   Desc:: Just like vsprintf but it adjusts the intended string lengths to
//          compensate for unseen color codes. It allows you write your format
//          string using "visual" lengths no matter how many color codes are
//          embedded in the strings you want inserted through the %s %10s.
//
//  Input:: The same as vsprintf.
//
// Output:: The non-visual, actual length of characters written into hBuf.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

LONG vcsprintf(CHAR* hBuf, const CHAR* hFormat, va_list hAp)
{
    CHAR    c, nNewFormat[kMaxStringLen], nWorkBuf[50];
    LONG    nOldLen, nNewLen, n, f;
    va_list nCachedAp;
    CHAR*   nStrArg;

    va_copy(nCachedAp, hAp);
    
    for(n = 0, f = 0; hFormat[f] != '\0'; ++f)
    {
        if(hFormat[f] != '%')
        {
            nNewFormat[n++] = hFormat[f];
            continue;
        }
        
        //** Skip %% Strings **//
        if(hFormat[f+1] == '%')
        {
            nNewFormat[n++] = hFormat[f++];
            nNewFormat[n++] = hFormat[f];
            continue;
        }
        
        
        //** Get nOldLen If STRINGCW **//
        if(sscanf(&hFormat[f+1], "%d%c", (INT*) &nOldLen, &c) != 2 || c != 's')
        {
            LONG x = f+1;
            while(!isalpha(hFormat[x]))
            {
                if(hFormat[x] == '\0')
                    ASSERT(false);
                ++x;
            }
            
            CHAR nDTChar = hFormat[x];
            if(nDTChar == 'h'|| nDTChar == 'l')
                nDTChar = hFormat[x+1];
            switch(nDTChar)
            {
                case 's': va_arg(hAp, char*);   break;
                case 'f': va_arg(hAp, double);  break;
                case 'd': va_arg(hAp, int);     break;
                case 'u': va_arg(hAp, int);     break;
                case 'c': va_arg(hAp, int);     break;
                default: ASSERT(false);         break;
            }
                
            nNewFormat[n++] = hFormat[f];
            continue;
        }
        else
        {
            nStrArg = (CHAR*) va_arg(hAp, char*);
        }
        
        //** Skip Old "%-23s" **//
        while(hFormat[f] != 's')
            f++;
            
        //** Find nNewLen, Adjusted By Color **//
        nNewLen = abs((int)nOldLen)+(strlen(nStrArg)-CStrLen(nStrArg));
        nNewLen = nOldLen < 0 ? -nNewLen : nNewLen;
        
        
        //** Append New Length In Place Of Old **//
        sprintf(nWorkBuf, "%%%ds", (INT)nNewLen);
        INT w = 0;
        
        while(nWorkBuf[w] != '\0')
             nNewFormat[n++] = nWorkBuf[w++];
    }

    nNewFormat[n] = '\0';
    n = vsprintf(hBuf, nNewFormat, nCachedAp);
    return n;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: QCBuf
//
//   Desc:: Quick Color Buffer. Basically like csprintf above but does it on
//          the fly with its own internal buffer and returns the results in
//          a pristine STRINGCW object.
//
//  Input:: The sprintf like "format" string, and any additional arguments.
//
// Output:: The string results.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW QCBuf(const CHAR* hFormat, ...)
{
    static CHAR nBuf[kMaxStringLen];

    va_list hAp;
    
    va_start(hAp, hFormat);
    vcsprintf(nBuf, hFormat, hAp);
    va_end(hAp);
    
    return nBuf;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: NumToWord
//
//   Desc:: Turns a number like 43 into "forty-three". Works on any number less
//          than a million, but support for larger numbers could easily be
//          added.
//
//  Input:: The number.
//
// Output:: The string representing the number. Since it uses a static buffer
//          it is only valid until the next call to NumToWord.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* NumToWord(LONG nNum)
{
    /////////////////////////////////////////////////////////////////////////
    const CHAR* nStrOnes[10] = 
    { 
        "zero", "one", "two", "three", "four", "five",
        "six", "seven", "eight",  "nine"
    };
    const CHAR* nStrTeens[10] = 
    {
        "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
        "sixteen", "seventeen", "eighteen", "nineteen"
    };
    const CHAR* nStrTens[10] =
    {
        "zero", "ten", "twenty", "thirty", "forty", "fifty", 
        "sixty", "seventy",   "eighty", "ninety"
    };
    /////////////////////////////////////////////////////////////////////////
    LONG nThousands, nHundreds, nTens, nOnes;
    static CHAR nBuf[1024];

    
    nBuf[0] = '\0';
    
    if(nNum >= 1000000)
    {
        strcat(nBuf, "an obscene amount of");
        return nBuf;
    }
    
    nThousands = nNum/1000;
    nNum %= 1000;
    nHundreds = nNum/100;
    nNum %= 100;
    nTens = nNum/10;
    nNum %= 10;
    nOnes = nNum;
    
    //** THOUSANDS **//
    if(nThousands)
    {
        LONG tHundreds, tTens, tOnes;
                                    //Breaks the thousands down
        tHundreds = nThousands/100; //> one hundred thousand?
        nThousands %= 100;
        tTens = nThousands/10;      //> ten thousand?
        nThousands %= 10;
        tOnes = nThousands;         //always at least a thousand 
        
        if(tHundreds) //one hundred and twelve thousand three hundred sixty-two
        {
            strcat(nBuf, nStrOnes[tHundreds]); 
            if(!tTens && !tOnes)
                strcat(nBuf, " hundred ");
            else
                strcat(nBuf, " hundred and ");
        }
        if(tTens)
        {
            if(tTens == 1)  //It's in 10-19, nStrTeens (ishy)
            {
                strcat(nBuf, nStrTeens[tOnes]); strcat(nBuf, " ");
            }else{
                strcat(nBuf, nStrTens[tTens]); strcat(nBuf, (tOnes ? "-": " "));
            }
        }
        if(tOnes && tTens != 1)
        {
            strcat(nBuf, nStrOnes[tOnes]); strcat(nBuf, " ");
        }
        if(!nHundreds && (nTens || nOnes))
            strcat(nBuf, "thousand and ");
        else
            strcat(nBuf, "thousand ");
    }
    
    //** HUNDREDS **//
    if(nHundreds)
    {
        strcat(nBuf, nStrOnes[nHundreds]); 
        if(!nTens && !nOnes)
            strcat(nBuf, " hundred ");
        else
            strcat(nBuf, " hundred and ");
    }
    
    //** TENS **//
    if(nTens)
    {
        if(nTens == 1)  //It's in 10-19, nStrTeens (ishy)
        {
            strcat(nBuf, nStrTeens[nOnes]); strcat(nBuf, " ");
        }else{
            strcat(nBuf, nStrTens[nTens]); strcat(nBuf, (nOnes ? "-": " "));
        }
    }
    
    //** ONES **//
    if(nOnes && nTens != 1)
    {
        strcat(nBuf, nStrOnes[nOnes]);
    }
    
    //Pick off any spaces on the end
    while(isspace(nBuf[strlen(nBuf)-1]))
        nBuf[strlen(nBuf)-1] = '\0';
    
    return nBuf;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ParseMatching
//
//   Desc:: Takes the standard input for specifying a thing in the mud and
//          extracts relevant numbers and arguments for you to use. The text
//          is usually something like "thing" or "2*thing" or "all.thing".
//
//  Input:: hStr: The text argument to parse.
//
// Output:: hStart: Will contain where to start matching, 4 of "4.Object".
//          hAmount: Will contain how many to match, 3 of "3*Object".
//          hArg: An argument you can compare against an objects keywords.
//          hAll: Did they do an "all" or "all.Object"?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ParseMatching(const CHAR* hStr, LONG& hStart, LONG& hAmount, STRINGCW& hArg, bool& hAll)
{
    hStr = AmountPrefix(hStr, hAmount);
    hStr = NumberPrefix(hStr, hStart);
    hArg = hStr;
    hAll = hAmount == -1 && hArg.Empty();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: AmountPrefix
//
//   Desc:: Pulls the 86 off of a string that looks like "86*3.something". Puts
//          1 in hPrefixAmount if no number was found, nothing automatically
//          means they just want one. Returns a pointer inside the same string
//          you passed in to where "3.something" starts.
//
//  Input:: hArg: The string.
//          hPrefixAmount: Holds amount specified on return.
//
// Output:: The string new amount-removed string pointer.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

const CHAR* AmountPrefix(const CHAR* hArg, LONG& hPrefixAmount)
{
    CHAR nBuf[kMaxInputLen];
    
    hPrefixAmount = 1;
    
    if(!StrCmp("all", hArg))
    {
        hPrefixAmount = -1;
        return &hArg[3];
    }
    
    if(StrPrefix(hArg, "all.") || StrPrefix(hArg, "all*"))
    {
        hPrefixAmount = -1;
        return &hArg[4];
    }
    
    for(LONG i = 0; hArg[i] != '\0'; ++i)
    if(hArg[i] == '*' && i != 0)
    {
        nBuf[0] = '\0';
        strncat(nBuf, hArg, i);
        if(!IsNumber(nBuf))
            return hArg;
            
        hPrefixAmount = atol(nBuf);
        hPrefixAmount = hPrefixAmount <= 0 ? 1 : hPrefixAmount;
        
        return &hArg[i+1];
    }
    
    return hArg;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: NumberPrefix
//
//   Desc:: Pulls the 3 off of a string that looks like "86*3.something". Puts
//          1 in hPrefixNum if no number was found, nothing automatically means
//          they intented the first one. Returns a pointer inside the same
//          string you passed in to where "something" starts.
//
//  Input:: hArg: The string.
//          hPrefixNum: Holds number specified on return.
//
// Output:: The string new number-removed string pointer.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

const CHAR* NumberPrefix(const CHAR* hArg, LONG& hPrefixNum)
{
    CHAR nBuf[kMaxInputLen];
    
    hPrefixNum = 1;
    
    for(LONG i = 0; hArg[i] != '\0'; ++i)
    if(hArg[i] == '.' && i != 0)
    {
        nBuf[0] = '\0';
        strncat(nBuf, hArg, i);
        if(!IsNumber(nBuf))
            return hArg;
            
        hPrefixNum = atoi(nBuf);
        hPrefixNum = hPrefixNum <= 0 ? 1 : hPrefixNum;
        
        return &hArg[i+1];
    }
    
    return hArg;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CenterLine
//
//   Desc:: Given any string it will center it inside a given width. Blank
//          spaces pad the area to the left of the newly centered string. It
//          takes color codes into account so every string appears visually
//          center ot the end user.
//
//  Input:: hStr: The string.
//          hWidth: Width to center it inside. Default is 80.
//
// Output:: The centered string held inside a static buffer.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* CenterLine(const CHAR* hStr, SHORT hWidth)
{
    static CHAR sBuf[kMaxStringLen];
    LONG        nLeading, nLength, i;
    
    nLength     = CStrLen(hStr);
    nLeading    = (hWidth-nLength)/2;
    
    if(nLength >= hWidth)
    {
        sBuf[0] = '\0';
        strcat(sBuf, hStr);
        return sBuf;
    }
    
    for(i = 0; i < nLeading; ++i)
        sBuf[i] = ' ';
    
    sBuf[i] = '\0';
    strcat(sBuf, hStr);
    
    return sBuf;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OverlayCenterLine
//
//   Desc:: Like CenterLine but it overlays the centered string over another
//          string. This is used to make -=-=-=[ Title ]=-=-=- bar types of
//          things in the game. It takes color codes into account so every
//          string appears visually centered to the end user.
//
//  Input:: hStr: The string.
//          hOverStr: String to overlay the centered hStr on.
//          hWidth: Center hStr inside this width. Default is the CStrLen of
//          hOverStr.
//
// Output:: The centered string held inside a static buffer.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* OverlayCenterLine(const CHAR* hStr, const CHAR* hOverStr, SHORT hWidth)
{
    static CHAR sBuf[kMaxStringLen];
    LONG        nLeading, nLength;
    
    if(hWidth == -1)
        hWidth = CStrLen(hOverStr);

    nLength     = CStrLen(hStr);
    nLeading    = (hWidth-nLength)/2;
    
    if(nLength >= hWidth)
    {
        sBuf[0] = '\0';
        strcat(sBuf, hStr);
        return sBuf;
    }
    
    
    //Copy Using Visual Characters
    sBuf[0] = '\0';
    strncat(sBuf, hOverStr, CCharAt(hOverStr, nLeading)-hOverStr);
    strcat(sBuf, hStr);
    strcat(sBuf, CCharAt(hOverStr, nLeading+nLength));

    return sBuf;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StrAlphaSort
//
//   Desc:: Sorting method you can pass to the "qsort" function. Will make the
//          strings appear in alphabetical order.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

INT StrAlphaSort(const void* nElement1, const void* nElement2)
{
    CHAR*   String1 = *(CHAR**)nElement1;
    CHAR*   String2 = *(CHAR**)nElement2;
    LONG    nLength = UMAX(strlen(String1), strlen(String2));
    
    /* A - Z */
    return strncmp(String1, String2, nLength);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CStrAlphaSort
//
//   Desc:: Same as above but disregards color codes while sorting.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

INT CStrAlphaSort(const void* nElement1, const void* nElement2)
{
    STRINGCW String1 = *(CHAR**)nElement1;
    STRINGCW String2 = *(CHAR**)nElement2;
    StripColorCodes(String1);
    StripColorCodes(String2);
    LONG nLength = UMAX(String1.Length(), String2.Length());
    
    /* A - Z */
    return strncmp(String1, String2, nLength);
}

