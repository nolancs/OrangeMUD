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
#ifndef OBJECTHOLDER_H
#define OBJECTHOLDER_H 1
#include "ActThing.h"
#include "SaveLoadable.h"


class OHOffsets;
class CombinedObjects;
typedef Map<VREF, OHOffsets*>       OHOffsetsMap;
typedef OHOffsetsMap::iterator      OHOffsetsIter;
typedef bool ObjCompare (Person *hTo, Object *hObj1, Object *hObj2);




class ObjectHolder
    : virtual public ActThing
    , public SaveLoadable
{
    //** De/constructors **//
    public: ObjectHolder();
    public: virtual ~ObjectHolder();
    public: ObjectHolder& operator =(const ObjectHolder& hFrom);

    //** Overridden Functions **//
    public:
    virtual void        SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void        SaveValuesTo(XMLWriter& hFile);
    virtual bool        MatchValues(XMLReader& hXML, XMLTag& hTag);
    virtual STRINGCW    GetTextForCodeTo(Person* hTo, CHAR hCode);

    //** Public Member Functions **//
    public:
    void                CombineTo(Person* hTo, ObjCompare* hCompareFunc, bool hStrict = true);
    void                CombinedClear();
    ULONG               CombinedSize();
    CombinedObjects*    CombinedAt(ULONG hIndex);
    ULONG               Size();
    bool                Empty();
    ObjectIter          Begin();
    ObjectIter          End();
    Object*             Front();
    Object*             Back();
    ObjectHolder*       Base();
    void                ClearObjects();
    bool                Exists(Object* hObject, ObjectIter* hItsIter = NULL);
    void                AddObject(Object* hObject, bool hWeOwn = false);
    void                AddObjects(ObjectHolder* hObjects, bool hWeOwn = false);
    void                RemoveObject(Object* hObject, bool hWeOwn = false);
    void                RemoveObject(ObjectIter& hObject, bool hWeOwn = false);
    void                RemoveObjects(ObjectHolder* hObjects, bool hWeOwn = false);
    void                RemoveFromOwners();
    Object*             RetrieveObjectAs(Person* hTo, VREF hVRef);
    bool                RetrieveObjectsAs(Person* hTo, const CHAR* hArg, ObjectHolder& hHolder);
    bool                RetrieveObjectsAs(Person* hTo, const CHAR* hArg, ObjectHolder& hHolder, LONG& hFoundAmt, LONG& hCollectedAmt);
    void                ShowNameObjectsTo(Person* hTo);
    void                ShowShortObjectsTo(Person* hTo);
    void                ShowInventoryTo(Person* hTo);


    //** Public Data Members **//
    public:
    ObjectHolder*               mInsideThis;
    
    //** Protected Data Members **//
    protected:
    ObjectList                  mObjectsInside;
    OHOffsetsMap                mVRefOffsets;
    Vector<CombinedObjects*>    mCombinedInfo;
};




class CombinedObjects
    : public ActThing
{
    //** De/constructors **//
    public: CombinedObjects(Object* hObject, LONG hAmt);
    public: virtual ~CombinedObjects();

    //** ActThing Member Functions **//
    public:
    virtual STRINGCW GetTextForCodeTo(Person* hTo, CHAR hCode);
    
    
    //** Public Data Members **//
    public:
    LONG        mAmount;
    Object*     mObject;
};




class OHOffsets
{
    public: OHOffsets() {}
    public: OHOffsets(ObjectIter& hBegin, ObjectIter& hEnd) : mBegin(hBegin), mEnd(hEnd) {}

    public:
    ObjectIter  mBegin;
    ObjectIter  mEnd;
};




//## Prototypes ##//
ObjCompare  ObjectVRefSame;
ObjCompare  ObjectLookNameSame;
ObjCompare  ObjectLookShortSame;



#endif //ifndef OBJECTHOLDER_H
