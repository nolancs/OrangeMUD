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
#ifndef OBJECTPROXY_H
#define OBJECTPROXY_H 1


class ObjectProxy
{
    //** De/constructors **//
    public: ObjectProxy();
    public: virtual ~ObjectProxy();

    
    //** Member Functions **//
    public:
    bool            IsCustom();
    void            Customize();
    void            Revert();

    //** Abstract Member Functions **//
    public:
    virtual ObjectIndex*    GetIndex() const = 0;

    //** Data Acessing Functions **//
    public:     
    const STRINGCW&     Name() const;
    const STRINGCW&     PName() const;
    const STRINGCW&     Keywords() const;
    const STRINGCW&     ShortDesc() const;
    const STRINGCW&     PShortDesc() const;
    const STRINGCW&     Description() const;
    LONG                Value(SHORT hIndex) const;
    LONG                WearFlags() const;
    SHORT               ItemType() const;
    float               Weight() const;
    
    //** Data Setting Functions **//
    public:
    STRINGCW&           xName();
    STRINGCW&           xPName();
    STRINGCW&           xKeywords();
    STRINGCW&           xShortDesc();
    STRINGCW&           xPShortDesc();
    STRINGCW&           xDescription();
    LONG&               xValue(SHORT hIndex);



    //** Public Data Members **//
    public:
    ObjectData*         mObject;
    APtr<ObjectData>    mCustomObject;
};



//Object data source
class ObjectData
{
    //** De/constructors **//
    public: ObjectData(ObjectIndex* hLikeObj);
    public: ~ObjectData();
    
    
    //** Data Members **//
    STRINGCW    mName;
    STRINGCW    mPName;
    STRINGCW    mKeywords;
    STRINGCW    mShortDesc;
    STRINGCW    mPShortDesc;
    STRINGCW    mDescription;
    //Variables
    LONG        mAge;   
    STRINGCW    mOwner;
    SHORT       mTimer;
    BYTE        mEnchantment;
    SHORT       mItemType;      
    SHORT       mLevel;
    float       mWeight;
    LONG        mMaterials;
    LONG        mAntiFlags;     //*6 align, X race, Y classes
    LONG        mExtraFlags;
    LONG        mObjectFlags;
    LONG        mValue[5];
};


#endif //ifndef OBJECTPROXY_H



