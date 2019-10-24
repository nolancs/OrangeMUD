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
#ifndef OBJECTINDEX_H
#define OBJECTINDEX_H 1
#include "VRefThing.h"
#include "SpottedThing.h"
#include "ScriptedThing.h"
#include "LoadOld.h"
#include "Object.h"


class ObjectIndex
    : public VRefThing
    , public SpottedThing
    , public ScriptedThing
    , public LoadOld
//  , public SaveLoadable //Comes from ScriptedThing/SpottedThing
{
    //** De/constructors **//
    public: ObjectIndex();
    public: virtual ~ObjectIndex();

    //** Class Functions **//
    public: static ObjectIndex* Get(VNUM hVnum);
    public: static ObjectIndex* Get(VREF hVRef);

    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag);

    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);

    //** Member Functions **//
    public:
    ObjectData*     GetFlyweight();
    void            StatTo(Person* hTo);
    void            StatSpecificsTo(Person* hTo, LONG* hInPlayValues = NULL);


    //** Persistant Data Members **//
    public:
    //Name Data
    STRINGCW            mName;
    STRINGCW            mPName;
    STRINGCW            mKeywords;
    STRINGCW            mShortDesc;
    STRINGCW            mPShortDesc;
    STRINGCW            mDescription;
    //Container Values
    float               mContCapacity;
    LONG                mContMultiplier;
    VREF                mContKey;
    BYTE                mContFlags;
    //Variables
    LONG                mCost;
    SHORT               mDurability;
    SHORT               mItemType;      
    SHORT               mLevel;
    float               mWeight;
    LONG                mMaterials;
    LONG                mAntiFlags;
    LONG                mObjectFlags;
    LONG                mWearFlags;
    LONG                mValue[5];
    
    
    //** Runtime Data Members **//
    public:
    LONG                mInPlay;
    
    
    //** Protected Data Members **//
    protected:
    APtr<ObjectData>    mObjectFlyweight;
};




#endif //ifndef OBJECTINDEX_H

