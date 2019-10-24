/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: December 2000                                         [Crossplatform]
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
#ifndef STRINGUTILS_H
#define STRINGUTILS_H 1
#include <cstdarg>

SHORT   StrCmp(const CHAR* hStr1, const CHAR* hStr2);
SHORT   StrNCmp(const CHAR* hStr1, const CHAR* hStr2, LONG hLen);
bool    StrPrefix(const CHAR* hStr, const CHAR* hPrefix);
bool    StrSuffix(const CHAR* hStr, const CHAR* hSuffix);
SHORT   SearchMenu(CHAR* hArg, const CHAR** hList, bool hExact);
bool    IsNumber(const STRING& hStr);
LONG    AsNumber(const STRING& hStr);
STRING  StripWS(STRING& nStr);
UCHAR*  CToPStr(STRING hStr);
STRING  QBuf(const CHAR* hFormat, ...);
ULONG   CountLines(const CHAR* hText);
ULONG   CountOffLines(const CHAR* hText, ULONG hWantLines);
void    Base64Encode(const CHAR* hData, ULONG hLen, STRING& hBase64);
bool    Base64Decode(const CHAR* hData, ULONG hLen, STRING& hOutBuf);


//## Inline Overloads ##//
inline SHORT    StrCmp(const STRING& hStr1, const STRING& hStr2)    { return StrCmp(hStr1.c_str(), hStr2.c_str()); }
inline SHORT    StrCmp(const STRING& hStr1, const CHAR* hStr2)      { return StrCmp(hStr1.c_str(), hStr2); }
inline SHORT    StrCmp(const CHAR* hStr1, const STRING& hStr2)      { return StrCmp(hStr1, hStr2.c_str()); }


#endif //ifndef(STRINGUTILS_H)
