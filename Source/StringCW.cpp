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
#include "StringCW.h"
#include "Person.h"




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Explode
//
//   Desc:: Breaks the string apart by words and stores each word as a StringCW
//          in hWords. Whitespace can be tabs or spaces. I named this function
//          after a similar one in PHP.
//
//  Input:: A place to hold all the words.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void StringCW::Explode(Vector<StringCW>& hWords) const
{
    LONG nWordStart, nWordEnd = 0;
    
    while(true)
    {
        //Find First Thing That Isn't A Space
        nWordStart = nWordEnd;
        while(isspace(mCStr[nWordStart]))
            nWordStart++;
    
        if(mCStr[nWordStart] == '\0')
            return;
        
        
        //Find The End Of The Word, Space Or \0
        nWordEnd = nWordStart+1;
        while(!isspace(mCStr[nWordEnd]) && mCStr[nWordEnd] != '\0')
            nWordEnd++;


        //Add This Word To The Array Of Words
        if(mCStr[nWordEnd] == '\0')
        {
            hWords.Add(SubString(nWordStart, cwsStringEnd));
            return;
        }
        else
        {
            hWords.Add(SubString(nWordStart, nWordEnd-nWordStart));
        }
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::Implode
//
//   Desc:: Reverse of the Explode() operation. Takes a list of words and puts
//          them into this string with hDelim in between each of them, which is
//          a space by default. I named this function after a similar one in
//          PHP.
//
//  Input:: hWords: All the words/tokens.
//          hDelim: What put in between each word/token.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void StringCW::Implode(Vector<StringCW>& hWords, const CHAR* hDelim)
{
    if(hWords.empty())
    {
        Clear();
        return;
    }
    
    LONG nDelimLen = strlen(hDelim);
    LONG nAllocTo = (hWords.size()-1) * nDelimLen;
    
    for(ULONG i = 0; i < hWords.size(); ++i)
        nAllocTo += hWords[i].Length();

    ResizeMem(nAllocTo+1);
    mLen = 0;
    mCStr[0] = '\0';
    for(ULONG i = 0; i < hWords.size(); ++i)
    {
        AddData(hWords[i].CStr(), hWords[i].Length());
        if(i+1 < hWords.size())
            AddData(hDelim, nDelimLen);     
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::OneArgument
//
//   Desc:: Breaks off an argument from the front of the string. An argument
//          being a chunk of text deliminated by spaces, or enclosed by some
//          quotes like of " or '. So this.is.an.argument and "so is this".
//
//  Input:: A string to hold the argument once it is extracted.
//
// Output:: A pointer to the rest of the string after hArg, you can use it
//          like nStr = nStr.OneArgument(nArg); to keep grabbing one argument
//          at a time off a StringCW or just ignore it if you want to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CHAR* StringCW::OneArgument(StringCW& hArg) const
{
    LONG nWhere = 0;
    CHAR nEndDelim = ' ';
    
    hArg.Clear();
    while(isspace(mCStr[nWhere]))
        nWhere++;
    
    if(mCStr[nWhere] == '\'')
    {
        nEndDelim = '\'';
        nWhere++;
    }else
    if(mCStr[nWhere] == '\"')
    {
        nEndDelim = '\"';
        nWhere++;
    }
    
    while(mCStr[nWhere] != nEndDelim && mCStr[nWhere] != '\0')
    {
        if((mCStr[nWhere] == '.'|| mCStr[nWhere] == '*') && nEndDelim == ' ')
        {
            if(mCStr[nWhere+1] == '\'') nEndDelim = '\'';
            if(mCStr[nWhere+1] == '\"') nEndDelim = '\"';
            if(nEndDelim != ' ')
            {
                hArg += mCStr[nWhere++];    //Add .
                nWhere++;                   //Skip "    
            }
        }
        hArg += mCStr[nWhere++];
    }
    if(mCStr[nWhere] != '\0' && mCStr[nWhere+1] != '\0')
        nWhere++;
    
    return &mCStr[nWhere];
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::IsNamed
//
//   Desc:: Searches this string and returns true if all the words (space
//          deliminated) in hNeedles are existant or partially existant. This
//          function would return true if our string was "Wavy Black Cloak" and
//          hNeedles was "wav cloa" or "black wavy" etc.
//
//  Input:: The needles (words) to search for the haystack (this string).
//
// Output:: True if this string matched, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StringCW::IsNamed(const StringCW& hNeedles) const
{
    LONG                nCurNeedleLength, nMatchedNeedles = 0;
    LONG                nNeedleStart, nNeedleEnd = 0;
    Vector<StringCW>    nHaystackWords;
    ULONG               i;
    
    
    if(Empty() || hNeedles.Empty())
        return false;
    
    
    Explode(nHaystackWords);
    if(nHaystackWords.empty())
        return false;
    
    
    while(true)
    {
        //Find The Start Of A Needle Word
        nNeedleStart = nNeedleEnd;
        while(isspace(hNeedles[nNeedleStart]))
            nNeedleStart++;
    
        if(hNeedles[nNeedleStart] == '\0')
            break;
        
        
        //Find The End Of A Needle Word
        nNeedleEnd = nNeedleStart+1;
        while(!isspace(hNeedles[nNeedleEnd]) && hNeedles[nNeedleEnd] != '\0')
            nNeedleEnd++;
        
        nCurNeedleLength = nNeedleEnd - nNeedleStart;
        if(nCurNeedleLength == 0)
            break;
        
        
        //Make Sure The Needle Word Matches One Word In The Haystack
        for(i = 0; i < nHaystackWords.size(); ++i)
        if(!nHaystackWords[i].NCmp(&hNeedles.CStr()[nNeedleStart], nCurNeedleLength))
            break;
        
        
        //Didnt Match Any Words In Haystack?
        if(i == nHaystackWords.size()) 
            return false;
        
        
        nMatchedNeedles++;
    }
    
    
    return nMatchedNeedles ? true : false;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ResolveChar
//
//   Desc:: Resolves this string into a char value. Takes cares of notifying 
//          the Person if the string is not a valid number and if it is not
//          within the optional min/max range.
//
//  Input:: hCh: Person to notify if resolution fails.
//          hChar: Resolved char stored here.
//          hMin: Optional, define a min/max acceptable range.
//          hMax: Optional, define a min/max acceptable range.
//
// Output:: True if everything worked.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StringCW::ResolveChar(Person* hCh, CHAR& hChar, LONG hMin, LONG hMax)
{
    if(!IsNumber())
    {
        hCh->SendF("%s is not a number.\n", CStr());
        return false;
    }
    
    CHAR nValue = (CHAR) atol(CStr());
    if(hMin != cwsNoMin && hMax == cwsNoMax && (nValue < hMin))
    {
        hCh->SendF("Must be more than or equal to %ld and \"%ld\" is not.\n", hMin, nValue);
        return false;
    }else
    if(hMin == cwsNoMin && hMax != cwsNoMax && (nValue > hMax))
    {
        hCh->SendF("Must be less than or equal to %ld and \"%ld\" is not.\n", hMax, nValue);
        return false;
    }else   
    if(hMin != cwsNoMin && hMax != cwsNoMax && (nValue < hMin || nValue > hMax))
    {
        hCh->SendF("Must be within the %ld to %ld range and \"%ld\" is not.\n", hMin, hMax, nValue);
        return false;
    }
        
    hChar = nValue;
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ResolveShort
//
//   Desc:: Resolves this string into a short integer value. Takes cares of
//          notifying the Person if the string is not a valid number and if
//          it is not within the optional min/max range.
//
//  Input:: hCh: Person to notify if resolution fails.
//          hShort: Resolved short stored here.
//          hMin: Optional, define a min/max acceptable range.
//          hMax: Optional, define a min/max acceptable range.
//
// Output:: True if everything worked.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StringCW::ResolveShort(Person* hCh, SHORT& hShort, LONG hMin, LONG hMax)
{
    if(!IsNumber())
    {
        hCh->SendF("%s is not a number.\n", CStr());
        return false;
    }
    
    SHORT nValue = (SHORT) atol(CStr());
    if(hMin != cwsNoMin && hMax == cwsNoMax && (nValue < hMin))
    {
        hCh->SendF("Must be more than or equal to %ld and \"%ld\" is not.\n", hMin, nValue);
        return false;
    }else
    if(hMin == cwsNoMin && hMax != cwsNoMax && (nValue > hMax))
    {
        hCh->SendF("Must be less than or equal to %ld and \"%ld\" is not.\n", hMax, nValue);
        return false;
    }else   
    if(hMin != cwsNoMin && hMax != cwsNoMax && (nValue < hMin || nValue > hMax))
    {
        hCh->SendF("Must be within the %ld to %ld range and \"%ld\" is not.\n", hMin, hMax, nValue);
        return false;
    }
        
    hShort = nValue;
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ResolveLong
//
//   Desc:: Resolves this string into a long integer value. Takes cares of
//          notifying the Person if the string is not a valid number and if
//          it is not within the optional min/max range.
//
//  Input:: hCh: Person to notify if resolution fails.
//          hLong: Resolved long stored here.
//          hMin: Optional, define a min/max acceptable range.
//          hMax: Optional, define a min/max acceptable range.
//
// Output:: True if everything worked.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StringCW::ResolveLong(Person* hCh, LONG& hLong, LONG hMin, LONG hMax)
{
    if(!IsNumber())
    {
        hCh->SendF("%s is not a number.\n", CStr());
        return false;
    }
    
    LONG nValue = atol(CStr());
    if(hMin != cwsNoMin && hMax == cwsNoMax && (nValue < hMin))
    {
        hCh->SendF("Must be more than or equal to %ld and \"%ld\" is not.\n", hMin, nValue);
        return false;
    }else
    if(hMin == cwsNoMin && hMax != cwsNoMax && (nValue > hMax))
    {
        hCh->SendF("Must be less than or equal to %ld and \"%ld\" is not.\n", hMax, nValue);
        return false;
    }else   
    if(hMin != cwsNoMin && hMax != cwsNoMax && (nValue < hMin || nValue > hMax))
    {
        hCh->SendF("Must be within the %ld to %ld range and \"%ld\" is not.\n", hMin, hMax, nValue);
        return false;
    }
        
    hLong = nValue;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StringCW::ResolveFloat
//
//   Desc:: Resolves this string into a floating point value. Takes cares of
//          notifying the Person if the string is not a valid number and if
//          it is not within the optional min/max range.
//
//  Input:: hCh: Person to notify if resolution fails.
//          hLong: Resolved float stored here.
//          hMin: Optional, define a min/max acceptable range.
//          hMax: Optional, define a min/max acceptable range.
//
// Output:: True if everything worked.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool StringCW::ResolveFloat(Person* hCh, float& hFloat, float hMin, float hMax)
{
    if(!IsFloat())
    {
        hCh->SendF("%s is not a floating point number.\n", CStr());
        return false;
    }
    
    float nValue = atof(CStr());
    if(hMin != cwsNoMin && hMax == cwsNoMax && (nValue < hMin))
    {
        hCh->SendF("Must be more than %f and \"%f\" is not.\n", hMin, nValue);
        return false;
    }else
    if(hMin == cwsNoMin && hMax != cwsNoMax && (nValue > hMax))
    {
        hCh->SendF("Must be less than %f and \"%f\" is not.\n", hMax, nValue);
        return false;
    }else   
    if(hMin != cwsNoMin && hMax != cwsNoMax && (nValue < hMin || nValue > hMax))
    {
        hCh->SendF("Must be within the %f to %f range and \"%f\" is not.\n", hMin, hMax, nValue);
        return false;
    }
    
    hFloat = nValue;
    return true;
}
