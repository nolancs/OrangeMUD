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
#ifndef AMBIENCE_H
#define AMBIENCE_H 1
#include "LoadOld.h"
#include "SaveLoadable.h"

class Ambience
    : public LoadOld
    , public SaveLoadable
{
    //** De/constructors **//
    public: Ambience();
    public: virtual ~Ambience();
    
    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag); 

    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    static void     LoadListInto(MUDFile& hFp, AmbienceArray& hList);

    //** Public Member Functions **//
    public:
    void            AddOntoRoom(VREF hRef);
    void            AddOntoMobile(VREF hRef);
    void            RemoveFromRoom(VREF hRef);
    void            RemoveFromMobile(VREF hRef);


    //** Persistant Data Members **//
    public:
    SHORT           mNumber;
    STRINGCW        mMessage;
    Vector<VNUM>    mVnums;
    
    STRINGCW        mOldVnums;      //Outdated
};




#endif //ifndef AMBIENCE_H
