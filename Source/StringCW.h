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
******************************************************************************
Description:
I'm a fan of the Standard Template Library string classes but wanted something
specific that coders could look through and modify easily since strings are
the heart of any good MUD. I also wanted a very lightweight string class that
would ease the transition from simple char* strings. By default you can pass a
StringCW object off any place you can pass a CHAR* since it has that typecast
operator overloaded.
******************************************************************************/
#ifndef STRINGCW_H
#define STRINGCW_H 1
#include <cstdarg>
#include <stdio.h>

//## External Prototypes ##//
LONG vcsprintf(char* hBuf, const char *hFormat, va_list hAp);


#if 0
#pragma mark StringCW
#endif

#define cwsStringEnd    -1
#define cwsNotFound     -2
#define cwsNoMin        LONG_MIN
#define cwsNoMax        LONG_MAX

class StringCW
{

    //** De/Constructors **//
    public: StringCW();
    public: StringCW(LONG hPreBuffer);
    public: StringCW(const StringCW& hFrom);
    public: StringCW(const CHAR* hFrom, LONG hFromLen = cwsStringEnd);
    public: virtual ~StringCW();

    
    //** Operators **//
    public:
    StringCW& operator =(const StringCW& hAddStr);
    StringCW& operator =(const STRING& hAddStr);
    StringCW& operator =(const CHAR* hAddStr);
    StringCW& operator +=(const StringCW& hAddStr);
    StringCW& operator +=(const string& hAddStr);
    StringCW& operator +=(const CHAR* hAddStr);
    StringCW& operator +=(const CHAR hAddChar);
    CHAR& operator [] (int hPos)                    { return mCStr[hPos]; }
    CHAR& operator [] (LONG hPos)                   { return mCStr[hPos]; }
    CHAR& operator [] (SHORT hPos)                  { return mCStr[hPos]; }
    CHAR* operator * () const                       { return mCStr; }
    operator CHAR* () const                         { return mCStr; }
    friend StringCW operator +(const StringCW& Str1, const StringCW& Str2);
    
    //** Comparison Operators **//
    bool operator < (const StringCW& hStr) const    { return (bool)(Compare(hStr) == -1);   }
    bool operator > (const StringCW& hStr) const    { return (bool)(Compare(hStr) == 1);    }
    bool operator <=(const StringCW& hStr) const    { return (bool)(Compare(hStr) != 1);    }
    bool operator >=(const StringCW& hStr) const    { return (bool)(Compare(hStr) != -1);   }
    bool operator ==(const StringCW& hStr) const    { return (bool)(Compare(hStr) == 0);    }
    bool operator !=(const StringCW& hStr) const    { return (bool)(Compare(hStr) != 0);    }
    bool operator < (const CHAR* hStr) const        { return (bool)(Compare(hStr) == -1);   }
    bool operator > (const CHAR* hStr) const        { return (bool)(Compare(hStr) == 1);    }
    bool operator <=(const CHAR* hStr) const        { return (bool)(Compare(hStr) != 1);    }
    bool operator >=(const CHAR* hStr) const        { return (bool)(Compare(hStr) != -1);   }
    bool operator ==(const CHAR* hStr) const        { return (bool)(Compare(hStr) == 0);    }
    bool operator !=(const CHAR* hStr) const        { return (bool)(Compare(hStr) != 0);    }
    
    //** Public Member Functions **//
    public:
    StringCW&       Append(const CHAR* hStr, LONG hLen);
    CHAR*           CStr() const;
    bool            Empty() const;
    void            Clear(bool hShrink = false);
    void            Erase(LONG hFrom, LONG hLength);
    LONG            Length() const;
    StringCW&       SPrintF(const CHAR* hFormat, ...);
    StringCW&       CSPrintF(const CHAR* hFormat, ...);
    
    //** String Manipulation **//
    public:
    StringCW&       Capitalize(bool hAll = false);
    StringCW&       StripWS();
    StringCW&       StripLeadingWS();
    StringCW&       StripTrailingWS();
    StringCW&       Resize(LONG hNewLen);
    StringCW&       Buffer(LONG hBufferLen);
    StringCW&       ShrinkToFit();
    StringCW&       Replace(const CHAR* hFrom, const CHAR* hTo);
    
    //** String Testing **//
    public:
    LONG            Cmp(const CHAR* hToStr) const;
    LONG            CaseCmp(const CHAR* hToStr) const;
    LONG            NCmp(const CHAR* hToStr, LONG hLen) const;
    LONG            CaseNCmp(const CHAR* hToStr, LONG hLen) const;
    StringCW        SubString(LONG hFrom, LONG hLength) const;
    void            Explode(Vector<StringCW>& hWords) const;
    void            Implode(Vector<StringCW>& hWords, const CHAR* hDelim = " ");
    LONG            Find(const CHAR* hNeedle, LONG hFromPos = 0) const;
    bool            IsNumber() const;
    LONG            AsNumber() const;   
    bool            IsFloat() const;
    float           AsFloat() const;    
    CHAR*           OneArgument(StringCW& hArg) const;
    bool            IsVRef(StringCW* hArea = NULL, StringCW* hObject = NULL) const;
    bool            IsNamed(const StringCW& hNeedles) const;
    bool            ResolveChar(Person* hCh, CHAR& hChar, LONG hMin = cwsNoMin, LONG hMax = cwsNoMax);
    bool            ResolveShort(Person* hCh, SHORT& hShort, LONG hMin = cwsNoMin, LONG hMax = cwsNoMax);
    bool            ResolveLong(Person* hCh, LONG& hLong, LONG hMin = cwsNoMin, LONG hMax = cwsNoMax);
    bool            ResolveFloat(Person* hCh, float& hFloat, float hMin = cwsNoMin, float hMax = cwsNoMax);

    //** Protected Member Functions **//
    protected:
    void            AddData(const CHAR* hData, LONG hLen);
    void            ResizeMem(LONG hToLen, bool hForceDown = false);
    LONG            Compare(const StringCW& hStr, bool hWithCase = true) const;
    LONG            Compare(const CHAR* hStr, LONG hLen = cwsStringEnd, bool hWithCase = true) const;
    bool            ThisIsUs(const StringCW* hStrCW, const CHAR* hStrData);
    
    

    //** Protected Data Members **//
    protected:
    CHAR*           mCStr;
    LONG            mLen;
    LONG            mBuffered;
};




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ThisIsUs
//
//   Desc:: Pass in a StringCW and/or a pointer to a spot in some character
//          memory and this function will check to see if that is actually
//          pointing to this StringCW. This is important when we are being
//          assigned with the += or = operators and the assigning data may
//          be coming from ourselves as well.
//
//  Input:: A StringCW pointer and/or a pointer into some character memory.
//
// Output:: True if the input points to us, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
bool StringCW::ThisIsUs(const StringCW* hStrCW, const CHAR* hStrData)
{
    if(hStrCW == this)
        return true;
    
    if(hStrData)
    {
        const CHAR* nDataLo = mCStr;
        const CHAR* nDataHi = &mCStr[mBuffered-1];
        if(hStrData >= nDataLo && hStrData <= nDataHi)
            return true;
    }
    
    return false;
}




inline 
void StringCW::ResizeMem(LONG hToLen, bool hForceDown)
{
    if(mBuffered == hToLen)
        return;
    
    if(!hForceDown && mBuffered >= hToLen)
        return;
    
    CHAR* nNewMem;
    if((nNewMem = (CHAR*) realloc((void*) mCStr, hToLen)) == NULL)
    {
        //Realloc failed, fucking panic
        free(mCStr);
        mCStr       = NULL;
        mLen        = 0;
        mBuffered   = 0;
        ASSERT(!"Out of StringCW Memory!!");
        return;
    }
    
    mBuffered   = hToLen;
    mCStr       = nNewMem;
}


inline 
void StringCW::AddData(const CHAR* hData, LONG hLen)
{
    LONG nAddLen = hLen;
    LONG nOldLen = mLen;

    ResizeMem(nOldLen+nAddLen+1);
    if(ThisIsUs(NULL, hData))
        memmove(&mCStr[nOldLen] , hData, nAddLen);
    else
        memcpy(&mCStr[nOldLen] , hData, nAddLen);

    //Cap string
    mCStr[nOldLen+nAddLen] = '\0';
    mLen += nAddLen;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::CStr
//
//   Desc:: Get the C-Style string for this StringCW. 
//
// Output:: The C-Style string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
CHAR* StringCW::CStr() const
{
    return mCStr;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Empty
//
//   Desc:: Find out if the string has nothing in it. 
//
// Output:: True if the string is empty, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
bool StringCW::Empty() const
{
    for(LONG i = 0; i < mLen; ++i)
    if(mCStr[i] != ' ')
        return false;
        
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Clear
//
//   Desc:: Erase the entire string, making it empty.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
void StringCW::Clear(bool hShrink)
{
    ResizeMem(1, hShrink);
    mCStr[0] = '\0';
    mLen = 0;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Erase
//
//   Desc:: Erase part of the string. You specify a beginning position and the
//          number of character to be erased from the string.
//
//  Input:: hFrom: The starting position.
//          hLength: Number of characters to erase, starting him hFrom.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
void StringCW::Erase(LONG hFrom, LONG hLength)
{
    ASSERT(hFrom+hLength <= mLen);
    memmove(mCStr+hFrom, mCStr+(hFrom+hLength), mLen-(hFrom+hLength)+1);
    mLen -= hLength;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Length
//
//   Desc:: Get the length of the string. The number of characters stored in it
//          such that nSomeStr[nSomeStr.Length()] would == '\0'.
//
//  Input:: The length.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::Length() const
{
    return mLen;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Append
//
//   Desc:: Add hLen amount of characters from the string pointed hAddStr.
//
//  Input:: hAddStr: The string.
//          hLen: The amount of characters to add from hAddStr.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW& StringCW::Append(const CHAR* hAddStr, LONG hLen)
{
    AddData(hAddStr, hLen);
    return *this;
}




inline 
LONG StringCW::Compare(const CHAR* hToStr, LONG hLen, bool hWithCase) const
{
    LONG i, nResult, nToLen = hLen == -1 ? strlen(hToStr) : hLen;

    if(mLen < nToLen)
        return -1;

    //** Case Matters **//
    if(hWithCase)
    {
        nResult = strncmp(mCStr, hToStr, nToLen);
        
        if(nResult > 0)
            return 1;
        else if(nResult < 0)
            return -1;
        else
            return 0;
    }
    else
    //** Case Doesnt Matter **//
    {
        for(i = 0; i < hLen; ++i)
        {
            if(tolower(mCStr[i]) < tolower(hToStr[i]))
                return -1;
            if(tolower(mCStr[i]) > tolower(hToStr[i]))
                return 1;
        }
    }
    
    return 0;
}



inline 
LONG StringCW::Compare(const StringCW& hToStr, bool hWithCase) const
{
    return Compare(*hToStr, hToStr.mLen, hWithCase);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




inline 
StringCW::StringCW()
    : mCStr(NULL)
    , mLen(0)
    , mBuffered(0)
{
    Clear();
}


inline
StringCW::StringCW(LONG hPreBuffer)
    : mCStr(NULL)
    , mLen(0)
    , mBuffered(0)
{
    ResizeMem(hPreBuffer+1);
}


inline 
StringCW::StringCW(const StringCW& hFrom)
    : mCStr(NULL)
    , mLen(0)
    , mBuffered(0)
{
    Clear();
    (*this) += hFrom;
}



inline 
StringCW::StringCW(const CHAR* hFrom, LONG hFromLen)
    : mCStr(NULL)
    , mLen(0)
    , mBuffered(0)
{
    Clear();
    AddData(hFrom, hFromLen == cwsStringEnd ? strlen(hFrom) : hFromLen);
}



inline 
StringCW::~StringCW()
{
    if(mCStr)
        free(mCStr);
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


inline 
StringCW operator +(const StringCW& hStr1, const StringCW& hStr2)
{
    StringCW nResult(hStr1);
    nResult += hStr2;
    return nResult;
}


inline 
StringCW& StringCW::operator =(const StringCW& hAddStr)
{
    if(ThisIsUs(&hAddStr, NULL))
        return *this;

    LONG nAddLen = hAddStr.Length();

    //Resize buffer to fit, but make string empty
    ResizeMem(nAddLen+mLen+1);
    mLen        = 0;
    mCStr[0]    = '\0';
    AddData(hAddStr.CStr(), nAddLen);
    return *this;
}


inline 
StringCW& StringCW::operator =(const STRING& hAddStr)
{
    LONG nAddLen = hAddStr.length();

    //Resize buffer to fit, but make string empty
    ResizeMem(nAddLen+mLen+1);
    mLen        = 0;
    mCStr[0]    = '\0';
    AddData(hAddStr.c_str(), nAddLen);
    return *this;
}


inline 
StringCW& StringCW::operator =(const CHAR* hAddStr)
{
    ASSERT(hAddStr);

    if(ThisIsUs(NULL, hAddStr))
    {
        CHAR* nHoldOldMemory = mCStr;
        mBuffered   = 0;
        mLen        = 0;
        mCStr       = NULL;
        AddData(hAddStr, strlen(hAddStr));
        free(nHoldOldMemory);
        return *this;
    }

    //Resize buffer to fit, but make string empty
    LONG nAddLen = strlen(hAddStr);

    ResizeMem(nAddLen+mLen+1);
    mLen        = 0;
    mCStr[0]    = '\0';
    AddData(hAddStr, nAddLen);
    return *this;
}


inline 
StringCW& StringCW::operator +=(const StringCW& hAddStr)
{
    AddData(hAddStr.CStr(), hAddStr.Length());
    return *this;
}



inline
StringCW&  StringCW::operator +=(const string& hAddStr)
{
    return (*this) += hAddStr.c_str();
}



inline 
StringCW& StringCW::operator +=(const CHAR* hAddStr)
{
    ASSERT(hAddStr);

    AddData(hAddStr, strlen(hAddStr));
    return *this;
}



inline 
StringCW& StringCW::operator +=(const CHAR hAddChar)
{
    AddData(&hAddChar, 1);
    return (*this); 
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::SPrintF
//
//   Desc:: Just like sprintf but it uses the StringCW as its buffer. What ever
//          existed in the StringCW before the call is lost and replaced with
//          the results. The string's memory is resized accordingly.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::SPrintF(const CHAR* hFormat, ...)
{
    CHAR        nBuf[kMaxStringLen];
    LONG        nLen;
    va_list     nAp;
    
    va_start(nAp, hFormat);
    nLen = vsprintf(nBuf, hFormat, nAp);
    va_end(nAp);
    
    ResizeMem(nLen+1);
    mLen = nLen;
    memcpy(mCStr, nBuf, nLen+1);
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::SPrintF
//
//   Desc:: Just like csprintf but it uses the StringCW as its buffer. It works
//          like sprintf but adjustes the length to account for color codes.
//          See the csprintf comments in StringMagic.cpp for more info on that.
//          What ever existed in the StringCW before the call is lost and
//          replaced with the results. The string's memory is resized
//          accordingly.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::CSPrintF(const CHAR* hFormat, ...)
{
    CHAR        nBuf[kMaxStringLen];
    LONG        nLen;
    va_list     nAp;
    
    va_start(nAp, hFormat);
    nLen = vcsprintf(nBuf, hFormat, nAp);
    va_end(nAp);

    ResizeMem(nLen+1);
    mLen = nLen;
    memcpy(mCStr, nBuf, nLen+1);
    return *this;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Cmp
//
//   Desc:: Compares a string to this string, ignoring case. Returns zero if
//          they're equal. Otherwise 1 if this string is greater, or -1 if
//          this string is less.
//
//  Input:: The string to compare it with.
//
// Output:: Zero if equal, otherwise -1 or 1.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::Cmp(const CHAR* hToStr) const
{
    LONG nToLen;
    
    nToLen = strlen(hToStr);
    if(Length() < nToLen)
        return -1;
    if(Length() > nToLen)
        return 1;
    
    return Compare(hToStr, nToLen, false);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::CaseCmp
//
//   Desc:: Compares a string to this string, taking case into account. Returns
//          zero if they're equal. Otherwise 1 if this string is greater, or -1
//          if this string is less.
//
//  Input:: The string to compare it with.
//
// Output:: Zero if equal, otherwise -1 or 1.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::CaseCmp(const CHAR* hToStr) const
{
    return Compare(hToStr, -1, true);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::NCmp
//
//   Desc:: Compares a string to this string, ignoring case. Returns zero if
//          they're equal. Otherwise 1 if this string is greater, or -1 if
//          this string is less.
//
//  Input:: The string to compare it with.
//
// Output:: Zero if equal, otherwise -1 or 1.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::NCmp(const CHAR* hToStr, LONG hLen) const
{
    return Compare(hToStr, hLen, false);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::CaseNCmp
//
//   Desc:: Compares a string to this string, taking case into account. Returns
//          zero if they're equal. Otherwise 1 if this string is greater, or -1
//          if this string is less.
//
//  Input:: The string to compare it with.
//
// Output:: Zero if equal, otherwise -1 or 1.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::CaseNCmp(const CHAR* hToStr, LONG hLen) const
{
    return Compare(hToStr, hLen, true);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::SubString
//
//   Desc:: Returns a substring of this string. You specify a starting position
//          and how many character from that position you want included int the
//          substring. You may use cwsStringEnd as hLength to say from hFrom, 
//          take everything till the end of the string.
//
//  Input:: hFrom: The starting position.
//          hLength: This many characters starting at hFrom.
//
// Output:: The substring.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW StringCW::SubString(LONG hFrom, LONG hLength) const
{
    if(hLength == cwsStringEnd)
        hLength = mLen-hFrom;
    
    ASSERT(hFrom >= 0 && hLength >= 0);
    ASSERT(hFrom+hLength <= mLen);
    
    return StringCW(mCStr+hFrom, hLength);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Find
//
//   Desc:: Attempts to locate a string somewhere inside this string. Case is
//          taken into account. hFromPos can also specify a position to start
//          searching from, the default is 0.
//
//  Input:: hNeedle: The string to search for.
//          hFromPos: Where to start looking from. Default is 0.
//
// Output:: A positive number indicating the position in this string where the
//          hNeedle was found. Otherwise cwsNotFound if it wasn't found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::Find(const CHAR* hNeedle, LONG hFromPos) const
{
    CHAR* nStartAt  = mCStr+hFromPos;
    CHAR* nFind     = strstr(nStartAt, hNeedle);
    
    return nFind ? nFind-nStartAt : cwsNotFound;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::IsNumber
//
//   Desc:: Checks if the StringCW text is an integer number.
//
// Output:: True if it is, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
bool StringCW::IsNumber() const
{
    LONG i = 0;
    bool nFoundDigit = false;
    if(mCStr[0] == '-')
        i = 1;

    for(; i < mLen; ++i)
    {
        if(!isdigit(mCStr[i]))
        {
            return false;
        }
        else
        {
            nFoundDigit = true;
        }
    }   
    return nFoundDigit;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::AsNumber
//
//   Desc:: Returns a LONG integer representation of the number stored in the
//          StringCW text, assuming of course it IS a number.
//
// Output:: The LONG number corresponding to the string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
LONG StringCW::AsNumber() const
{
    return atol(mCStr);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::IsFloat
//
//   Desc:: Checks if the StringCW text is a floating point number.
//
// Output:: True if it is, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
bool StringCW::IsFloat() const
{
    bool nDot = false;
    LONG i = 0;
    
    if(mCStr[0] == '-')
        i = 1;

    for(; i < mLen; ++i)
    if(!isdigit(mCStr[i]))
    {
        if(mCStr[i] == '.' && !nDot)
        {
            nDot = true;
            continue;
        }
        return false;
    }
        
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::AsFloat
//
//   Desc:: Returns a floating point representation of the number stored in the
//          StringCW text, assuming of course it IS a floating point number.
//
// Output:: The floating point number corresponding to the string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
float StringCW::AsFloat() const
{
    return atof(mCStr);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::IsVRef
//
//   Desc:: Tests if the string is a "area/thing" virtual reference and returns
//          true, extracting the "area" and "thing" sections out. Otherwise it
//          returns false.
//
//  Input:: hArea: Holds the "area" string once it's extracted.
//          hObject: Holds the "thing" string once it's extracted.
//
// Output:: True if the string is a virtual reference, otherwise false.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
bool StringCW::IsVRef(StringCW* hArea, StringCW* hObject) const
{
    LONG nSlashPos;
    
    nSlashPos = Find("/");
    if(nSlashPos == cwsNotFound)
        return false;
    
    if(hArea)
        *hArea = SubString(0, nSlashPos);
    if(hObject)
        *hObject = SubString(nSlashPos+1, cwsStringEnd);
    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Capitalize
//
//   Desc:: Capitalize the first character of the first word in the string if
//          hAll is false. Otherwise the first character of every word in the
//          string. It takes ^ color codes into account when searching for the
//          the first REAL character that the user would see.
//
//  Input:: False by default, true if you want every word capitalized.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW& StringCW::Capitalize(bool hAll)
{
    LONG nCapdWords;
    
    nCapdWords = 0;
    for(LONG i = 0; i < mLen; ++i)
    if(mCStr[i] == ' ' || nCapdWords == 0)
    {
        //Skip rest of whitespace
        while(mCStr[i] == ' ')
            i++;
        
        while(mCStr[i] == '^' && i+1 < mLen)
            i += 2;
            
        if(i >= mLen)
            break;
            
        mCStr[i] = UPPER(mCStr[i]);
        nCapdWords++;
        
        if(!hAll)
            break;
    }
    
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::StripWS
//
//   Desc:: Strip away both leading and trailing white space from this string.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW& StringCW::StripWS()
{
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::StripLeadingWS
//
//   Desc:: Strip away just leading white space from this string.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW& StringCW::StripLeadingWS()
{
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::StripTrailingWS
//
//   Desc:: Strip away just trailing white space from this string.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline 
StringCW& StringCW::StripTrailingWS()
{
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Resize
//
//   Desc:: Resizes the string to have a length of hNewLen. If the new length
//          is larger than the old length the new characters are padded with
//          spaces. A new length of 0 is the same as calling Clear().
//
//  Input:: The length you wish to resize the string to.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::Resize(LONG hNewLen)
{
    if(hNewLen == cwsStringEnd)
        hNewLen = mLen;
    
    ASSERT(hNewLen >= 0);
    LONG nOldLen, i;
        
    nOldLen = mLen;
    if(hNewLen == nOldLen)
        return *this;

    
    ResizeMem(hNewLen+1, true);
    mLen = hNewLen;
    mCStr[hNewLen] = '\0';
    
    
    if(hNewLen > nOldLen)
    for(i = nOldLen; i < hNewLen; ++i)
        mCStr[i] = ' ';
    
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Buffer
//
//   Desc:: Prebuffers to string to handle up to hBufferLen of characters
//          before needing another realloc. Very useful if you know you're
//          going to do a lot of work on a string. There is also the PreBuffer
//          constructor to StringCW which does the same thing and takes the
//          same arguments.
//
//  Input:: The prebuffer length.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::Buffer(LONG hBufferLen)
{
    if(mLen > hBufferLen)
    {
        mLen = hBufferLen;
        mCStr[hBufferLen] = '\0';
    }
    
    ResizeMem(mLen+1, true);
    return *this;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ShrinkToFit
//
//   Desc:: Discards all unused string memory (from things like Buffer()) and
//          makes the string take only the exact amount of memory it needs, and
//          no more.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::ShrinkToFit()
{
    ResizeMem(mLen+1, true);
    return *this;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Replace
//
//   Desc:: Replace all occurances of hFrom in this string with hTo.
//
// Output:: The StringCW reference for chaining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

inline
StringCW& StringCW::Replace(const CHAR* hFrom, const CHAR* hTo)
{   
    LONG nFound, nFromPos = 0;
    LONG nFromLen = strlen(hFrom);
    LONG nToLen = strlen(hTo);
    StringCW nNewString;
    
    while((nFound = Find(hFrom, nFromPos)) != cwsNotFound)
    {
        nNewString.Append(&(this->CStr()[nFromPos]), nFound-nFromPos);
        nNewString.Append(hTo, nToLen);
        nFromPos = nFound + nFromLen;
    }
    nNewString.Append(&(this->CStr()[nFromPos]), strlen(&(this->CStr()[nFromPos])));
    *this = nNewString;
    return *this;
}




#endif //ifndef STRINGCW_H
