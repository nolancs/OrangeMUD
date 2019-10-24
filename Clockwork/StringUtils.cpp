/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: April, 2001                                           [Crossplatform]
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
#include "CommonTypesCW.h"
#include "StringUtils.h"




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StrCmp
//
//   Desc:: Case insenstiive strcmp
//
//  Input:: Same as strcmp
//
// Output:: Same as strcmp
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT StrCmp(const CHAR* hStr1, const CHAR* hStr2)
{
    SHORT nDiff = 0;
    CHAR nChar1, nChar2;
    
    while (true)
    {
        nChar1 = tolower(*(hStr1++));
        nChar2 = tolower(*(hStr2++));
        nDiff = nChar1 - nChar2;

        // Both strings ended without any difference
        if (nDiff == 0 && nChar1 == '\0')
            return 0;
    
        // Difference detected, return
        if (nDiff)
            break;
    }
    
    return nDiff;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StrNCmp
//
//   Desc:: Case insenstiive strncmp
//
//  Input:: Same as strcmp
//
// Output:: Same as strcmp
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT StrNCmp(const CHAR* hStr1, const CHAR* hStr2, LONG hLen)
{
    SHORT nDiff = 0;
    CHAR nChar1, nChar2;
    INT nScannedLen = 0;
    
    while (nScannedLen < hLen)
    {
        nChar1 = tolower(*(hStr1++));
        nChar2 = tolower(*(hStr2++));
        nDiff = nChar1 - nChar2;
        
        // Both strings ended without any difference
        if (nDiff == 0 && nChar1 == '\0')
            return 0;
        
        // Difference detected, return
        if (nDiff)
            break;
        
        ++nScannedLen;
    }
    
    return nDiff;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StrPrefix
//
//   Desc:: Check if hStr starts with hPrefix. Case insensitive.
//
// Output:: True if hStr starts with hPrefix. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StrPrefix(const CHAR* hStr, const CHAR* hPrefix)
{
    INT nLen = strlen(hPrefix);
    return StrNCmp(hStr, hPrefix, nLen) ? false : true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StrSuffix
//
//   Desc:: Check if hStr ends with hSuffix. Case insensitive.
//
// Output:: True if hStr ends with hSuffix. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StrSuffix(const CHAR* hStr, const CHAR* hSuffix)
{
    INT nStrLen = strlen(hStr);
    INT nSuffixLen = strlen(hSuffix);
    if (nStrLen < nSuffixLen)
        return false;
    return StrCmp(hStr+(nStrLen-nSuffixLen), hSuffix) ? false : true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: IsNumber
//
//   Desc:: Check if hStr is a decimal number, negative or positive.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool IsNumber(const STRING& hStr)
{
    if (hStr == "0")
        return true;
    if (atol(hStr.c_str()) == 0) // returns 0 if no conversion possible
        return false;
    return true;
}


LONG AsNumber(const STRING& hStr)
{
    return (LONG) atol(hStr.c_str());
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: SearchMenu
//
//   Desc:: Search an array of strings (aka menu) that is terminated with
//          an "\n" entry. Eg: nDirMenu = { "North", "South", "\n" };
//
//  Input:: hArg: The input to match against the menu.
//          hList: The menu / list of string, terminated with an "\n" entry.
//          hExact: False = prefix matches count, true = exact matches only.
//
// Output:: Matched index, or -1 if no match is found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT SearchMenu(CHAR* hArg, const CHAR** hList, bool hExact)
{
    SHORT nIndex = 0;
    
    while (hList && hList[nIndex] && hList[nIndex][0] != '\n')
    {
        if (hExact && !StrCmp(hList[nIndex], hArg))
            return nIndex;
        else if (!hExact && StrPrefix(hList[nIndex], hArg))
            return nIndex;
        ++nIndex;
    }
    
    return -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StripWS
//
//   Desc:: Strip leading and trailing whitespace from a string and return it.
//
//  Input:: The string.
//
// Output:: The whitespace stripped string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING StripWS(STRING& nStr)
{
    INT i, nLeadingWS = 0, nTrailingWS = 0, nTotalLen = nStr.length();
    
    // Count leading white space
    i = 0;
    while (i < nTotalLen && isspace(nStr[i++]))
        ++nLeadingWS;
    
    // All white space, or zero-length string, return an empty string
    if (nLeadingWS >= nTotalLen)
        return "";
    
    // Count trailing whitespace
    i = nTotalLen-1;
    while (i >= 0 && isspace(nStr[i--]))
        ++nTrailingWS;

    return nStr.substr(nLeadingWS, nTotalLen-nLeadingWS-nTrailingWS);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CToPStr
//
//   Desc:: Convert a c-style string to a pascal-style string. Pascal strings
//          store the length in the first byte. Only used for Mac toolbox
//          calls.
//
//  Input:: The c-string.
//
// Output:: The p-string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UCHAR* CToPStr(STRING hStr)
{
    // Deprecated: Only used by pre-OSX Mac toolbox calls
    assert(!"NOT IMPLEMENTED: CToPStr");
    return NULL;
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: QBuf
//
//   Desc:: Quick buffer. Quickly sprintf some text and return it as a string.
//
//  Input:: The sprintf like "format" string, and any additional arguments.
//
// Output:: The string results.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING QBuf(const CHAR* hFormat, ...)
{
    static CHAR nBuf[10960];
    va_list hAp;
    
    va_start(hAp, hFormat);
    vsprintf(nBuf, hFormat, hAp);
    va_end(hAp);
    
    return nBuf;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CountLines
//
//   Desc:: Return the string length that contains X lines of text. For writing
//          paged output.
//
//  Input:: hText: The text data.
//          hWantLines: The number of lines you want.
//
// Output:: The substring length, starting from zero, of hText that contains
//          hWantLines lines of text. May contain less than hWantLines, but
//          never more.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ULONG CountLines(const CHAR* hText)
{
    ULONG nFoundLines = 0;
    
    while (*hText++ != '\0')
    {
        if (*hText == '\n')
            ++nFoundLines;
        ++hText;
    }
    
    return nFoundLines;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CountOffLines
//
//   Desc:: Return the string length that contains X lines of text. For writing
//          paged output.
//
//  Input:: hText: The text data.
//          hWantLines: The number of lines you want.
//
// Output:: The substring length, starting from zero, of hText that contains
//          hWantLines lines of text. May contain less than hWantLines, but
//          never more.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ULONG CountOffLines(const CHAR* hText, ULONG hWantLines)
{
    ULONG nSubStrLen = 0;
    
    while (hText[nSubStrLen] != '\0' && hWantLines > 0)
    {
        if (hText[nSubStrLen] == '\n')
            --hWantLines;
        nSubStrLen++;
    }
    
    return nSubStrLen;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Base64Encode
//
//   Desc:: Base64 encode some string data.
//
//  Input:: hData: String data to Base64 encode.
//          hLen: Length of hData.
//          hBase64: Base64 encoded string data.
//
// Output:: True on successful decode. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Base64Encode(const CHAR* hData, ULONG hLen, STRING& hBase64)
{
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";
    UCHAR char_array_3[3], char_array_4[4];
    INT i = 0, j = 0;
    
    while (hLen--) {
        char_array_3[i++] = *(hData++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; (i <4) ; i++)
                hBase64 += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; (j < i + 1); j++)
            hBase64 += base64_chars[char_array_4[j]];
        
        while(i++ < 3)
            hBase64 += '=';
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Base64Decode
//
//   Desc:: Base64 decode some string data.
//
//  Input:: hData: Base64 encoded string data.
//          hLen: Length of hData.
//          hOutBuf: Decoded string data.
//
// Output:: True on successful decode. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Base64Decode(const CHAR* hData, ULONG hLen, STRING& hOutBuf)
{
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";
    #define IS_BASE64(c) (isalnum(c) || (c == '+') || (c == '/'))
    UCHAR char_array_4[4], char_array_3[3];
    INT i = 0, j = 0, in_ = 0;
    
    while (hLen-- && hData[in_] != '=' && IS_BASE64(hData[in_])) {
        char_array_4[i++] = hData[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                hOutBuf += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++)
            hOutBuf += char_array_3[j];
    }
    
    return true;
}

