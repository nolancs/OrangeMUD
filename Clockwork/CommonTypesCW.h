/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December, 2000                                        [Crossplatform]
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
#ifndef COMMONTYPESCW_H
#define COMMONTYPESCW_H 1
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <cassert>
#include <algorithm>

using namespace std;

#if(TARGET_WIN)
#pragma warning(disable:4786) 
#endif //(TARGET_WIN)

//** Common Portable Primatives **//
#if(!TARGET_WIN)
typedef int                 INT;
typedef unsigned int        UINT;
typedef char                CHAR;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef short               SHORT;
typedef unsigned short      USHORT;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef string              STRING;
typedef time_t              TIMET;
#else
#include <afxwin.h>
#include <Winsock.h>
#include "WinIncludes.h"
#endif


//** Standard Template Library **//
#include "STLFacades.h"
using namespace std;


//** Declare All Class Names Ahead Of Time **//
#include "ForwardDeclCW.h"
#include "Exception.h"

#if DEBUG
    #ifndef DEBUG
    #define DEBUG(x)    assert(x)
    #endif
    #ifndef ASSERT
    #define ASSERT(x)   assert(x)
    #endif
#else
    #ifndef DEBUG
    #define DEBUG(x)
    #endif
    #ifndef ASSERT
    #define ASSERT(x)   assert(x)
    #endif
#endif

#define UMIN(x, y)      (x > y ? y : x)
#define UMAX(x, y)      (x > y ? x : y)
#define LOWER(c)        ((char) ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c)))
#define UPPER(c)        ((char) ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c)))


//##########################################################################//
//##########################################################################//




//** Hmmmm Portable Classes Somewhere Else? **//
class CWRECT
{
    public:
    CWRECT() { Set(0, 0, 0, 0); }
    CWRECT(SHORT hLeft, SHORT hTop, SHORT hRight, SHORT hBot) { Set(hLeft, hTop, hRight, hBot); }
    
    
    public:
    void Set(SHORT hLeft, SHORT hTop, SHORT hRight, SHORT hBot)
    {
        mLeft = hLeft, mTop = hTop, mRight = hRight, mBottom = hBot;
    }
    
    bool operator==(const CWRECT& nComp)
    {
        return (nComp.mLeft == mLeft && nComp.mTop == mTop && nComp.mRight == mRight && nComp.mBottom == mBottom);
    }
    
    public:
    SHORT   mLeft;
    SHORT   mTop;
    SHORT   mRight;
    SHORT   mBottom;
};




#endif //ifndef COMMONTYPESCW_H

