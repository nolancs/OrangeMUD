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
#ifndef MUDFILE_H
#define MUDFILE_H 1
#include "FileLink.h"


//## Defines ##//
#define EOT '\4'

//** File Saving Structure **//
#define     SAVE_SECT_START "##SECTION"
#define     SAVE_SECT_END   "##END_SECTION\n\n\n"
#define     LOAD_SECT_START "##SECTION"
#define     LOAD_SECT_END   "##END_SECTION"
        
#define     SAVE_ELEM_START "@START\n"
#define     SAVE_ELEM_END   "@END\n"
#define     LOAD_ELEM_START "@START"
#define     LOAD_ELEM_END   "@END"

#define     SAVE_SIGNAL_START   ">SIGNAL"
#define     SAVE_SIGNAL_END     "<SIGNAL\n"
#define     LOAD_SIGNAL_START   ">SIGNAL"
#define     LOAD_SIGNAL_END     "<SIGNAL"

#define     SAVE_BLL    "!BEGIN_LL_MEMBER\n"
#define     SAVE_ELL    "!END_LL_MEMBER\n"
#define     LOAD_BLL    "!BEGIN_LL_MEMBER"
#define     LOAD_ELL    "!END_LL_MEMBER"



//## Classes ##//

class MUDFile
{
    //** De/constructors **//
    public: MUDFile(FileLink* hOurLink);
    public: ~MUDFile();


    //** Member Functions - Reading **//
    public:
    bool    OK()        { return mOK; }
    void    SetNotOK()  { mOK = false; }
    
    CHAR    GetC();
    void    UnGetC(CHAR hC);
    CHAR    ReadLetter();   
    float   ReadFloat();
    LONG    ReadNumber();
    ULONG   ReadULong();
    LONG    ReadFlags();
    CHAR*   ReadString();       
    CHAR*   ReadStringEOL();
    void    ReadEOL();
    CHAR*   ReadWord();
    
    //** Member Functions - Writing **//
    public:
    bool    FilterChar(CHAR &c);
    void    ForceVar(const CHAR* hName, SHORT hValue);
    void    ForceVar(const CHAR* hName, LONG hValue);
    void    ForceVar(const CHAR* hName, ULONG hValue);
    void    ForceVar(const CHAR* hName, float hValue);
    void    ForceVar(const CHAR* hName, CHAR hValue);
    void    ForceStr(const CHAR* hName, const CHAR* hStr);
    void    ForceWrd(const CHAR* hName, const CHAR* hWrd);
    void    WriteVar(const CHAR* hName, ULONG   hValue, ULONG   hOrig = 0);
    void    WriteVar(const CHAR* hName, SHORT   hValue, SHORT   hOrig = 0);
    void    WriteVar(const CHAR* hName, LONG    hValue, LONG    hOrig = 0);
    void    WriteVar(const CHAR* hName, float   hValue, float   hOrig = 0);
    void    WriteVar(const CHAR* hName, CHAR    hValue, CHAR    hOrig = 0);
    void    WriteStr(const CHAR* hName, const CHAR* hStr);
    void    WriteStr(const CHAR* hName, const CHAR* hStr, const CHAR* hOrig);
    void    WriteFlg(const CHAR* hName, LONG hFlags);
    void    WriteWrd(const CHAR* hName, const CHAR* wrd);
    void    WriteWrdVar(const CHAR* hName, const CHAR* hWrd, SHORT hValue);

    //** Protected Data Members **//
    public:
    FileLink*   mLink;
    LONG        mAtLine;
    CHAR        mWord[kMaxInputLen];
    bool        mOK;
};



//## Free Function Prototypes ##//


#endif //ifndef MUDFILE_H



