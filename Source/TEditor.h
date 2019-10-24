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
#ifndef TEDITOR_H
#define TEDITOR_H 1



class TEditor
{
    //** Class Functions **//
    public:
    static void     RemoveTrailingSpaces(CHAR* hLine);
    
    //** Public Member Functions **//
    public: TEditor(CHAR* hOnText);
    public: ~TEditor();
    void    Update();
    bool    Handle          (Person* hCh, STRINGCW& hCommand, CHAR* hClearStr);
    void    InsertLine      (CHAR* hLine, LONG hWhereLine);
    void    ReplaceLine     (CHAR* hLine, LONG hWhereLine);
    void    AddLineToEnd    (CHAR* hLine);
    void    DeleteLine      (LONG hWhereLine);
    void    FormatWidth     (ULONG hWide);
    bool    ParseLine       (CHAR*& hSource, CHAR* hLineBuf);
    void    PrefixLines     (const CHAR*  hWithStr);
    CHAR*   GetNextWord(CHAR* hText, LONG& hOffset);
    


    //** Public Data Members **//
    public:
    CHAR*   mBefore;
    STRING  mAfter;
    LONG    mNumLines;
    CHAR    mEOL;
    CHAR    mEOLStr[2];
    SHORT   mEOLLen;
};



//** Helper Functions **//
inline 
void CopyString(CHAR*& hTo, const CHAR* hFrom)
{
    LONG nLen = strlen(hFrom);
    hTo = new CHAR[nLen+1];
    memcpy(hTo, hFrom, nLen+1);
}


inline 
void FreeString(CHAR*& hStr)
{
    ASSERT(hStr != NULL);
    delete [] hStr;
    hStr = NULL;
}



#endif //ifndef TEDITOR_H
