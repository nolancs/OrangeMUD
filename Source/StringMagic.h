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
#ifndef STRINGMAGIC_H
#define STRINGMAGIC_H 1
#include <cstdarg>

ULONG       CStrLen(const CHAR* hStr);
CHAR*       CCharAt(const CHAR* hStr, ULONG hNthChar);
CHAR*       CTrunc(const CHAR* hStr, ULONG hTruncLen);
STRINGCW&   StripColorCodes(STRINGCW& hStr);
LONG        csprintf(CHAR* hBuf, const CHAR* hFormat, ...);
LONG        csnprintf(CHAR* hBuf, size_t hTruncLen, const  CHAR* hFormat, ...);
LONG        vcsprintf(CHAR* buf, const CHAR* format, va_list ap);

STRINGCW    QCBuf(const CHAR* hFormat, ...);
CHAR*       NumToWord(LONG nNum);
void        ParseMatching(const CHAR* hStr, LONG& hStart, LONG& hAmount, STRINGCW& hArg, bool& hAll);
const CHAR* AmountPrefix(const CHAR* hArg, LONG& hPrefixAmount);
const CHAR* NumberPrefix(const CHAR* hArg, LONG& hPrefixNum);
CHAR*       CenterLine(const CHAR* hStr, SHORT hWidth = 80);
CHAR*       OverlayCenterLine(const CHAR* hStr, const CHAR* hOverStr, SHORT hWidth = -1);


//** qsort Compare Functions **//
INT StrAlphaSort(const void* nElement1, const void* nElement2);
INT CStrAlphaSort(const void* nElement1, const void* nElement2);

#endif //ifndef STRINGMAGIC_H
