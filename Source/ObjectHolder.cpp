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
#include "ObjectHolder.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "Person.h"
#include "StringMagic.h"
#include "StringUtils.h"





CombinedObjects::CombinedObjects(Object* hObject, LONG hAmt)
    : mObject(hObject)
    , mAmount(hAmt)
{
}



CombinedObjects::~CombinedObjects()
{
}



STRINGCW CombinedObjects::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    if(mAmount == 1
    || hCode == 'w'
    || hCode == 'l'
    || hCode == 'k')
        return mObject->GetTextForCodeTo(hTo, hCode);
        
    
    STRINGCW nText;
    
    if(hCode == 'p' || hCode == '?')
    {
        nText = hTo->CanSee(mObject) ? *mObject->GetPNameTo(hTo, mAmount) : "something";
    }else

    if(hCode == 'o')
    {
        nText = hTo->CanSee(mObject) ? *mObject->GetPNameTo(hTo, mAmount) : "something";
    }else

    if(hCode == 't')
    {
        nText = "{MOVEVERB}"; //mMovement;
    }

    //Unknown Code
    else
    {
        nText.SPrintF("{Unrecognized_Code(%c)_On_CombinedObjects}", hCode);
    }

    return nText;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ObjectHolder::ObjectHolder()
    : mInsideThis(NULL)
{
}


ObjectHolder::~ObjectHolder()
{
    ClearObjects();
}


ObjectHolder& ObjectHolder::operator =(const ObjectHolder& hFrom)
{
    AddObjects(&((ObjectHolder&) hFrom), false);
    return *this;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




STRINGCW ObjectHolder::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    return "{GetTextForCode On ObjectHolder}";
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::CombineTo
//
//   Desc:: Combine a bunch of objects into the condensed output form. It is
//          done in relation to how the hTo person views all the objects in
//          the ObjectHolder.
//          Strict means it will not group all matching objects, only those
//          immediately following the original object, once an object does not
//          match it will quit checking. This is done to keep "strict" control
//          over the combination so 2.Foo and such will match up with how it
//          should be.
//
//  Input:: hTo: Combine in relation to how this Person sees everything.
//          hCompareFunc: Tells CombineTo when to Objects are "equal".
//          hStrict: Default is off. Turns on strict combining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::CombineTo(Person* hTo, ObjCompare* hCompareFunc, bool hStrict)
{
    Vector<LONG>    nCombined;
    ObjectIter      nReScan, nScan      = Begin();
    ULONG           i, nObjectsSize     = Size();
    ULONG           nScanNum, nReScanNum;

    
    //Combined All The Objects
    mCombinedInfo.clear();
    nCombined.resize(nObjectsSize);
    for(i = 0; i < nObjectsSize; ++i)
        nCombined[i] = -1;


    for(nScanNum = 0; nScanNum < nObjectsSize; ++nScan, nScanNum++)
    if(nCombined[nScanNum] == -1)
    {
        nCombined[nScanNum] = 1;
        
        nReScan     = nScan;
        nReScanNum  = nScanNum;
        for(++nReScan, nReScanNum++; nReScanNum < nObjectsSize; ++nReScan, nReScanNum++)
        if(nCombined[nReScanNum] == -1)
        {
            if((*hCompareFunc)(hTo, *nScan, *nReScan))
            {
                nCombined[nReScanNum] = 0;
                nCombined[nScanNum]++;
            }else
            if(hStrict)
                break;
        }       
    }


    //Add The Results Into mCombinedInfo
    for(nScan = Begin(), i = 0; i < nObjectsSize; ++nScan, i++)
    if(nCombined[i] > 0)
    {
        Object* nCombinedObject = *nScan;
        LONG    nCombinedAmount = nCombined[i];
        
        ASSERT(nCombinedObject != NULL);
        mCombinedInfo.Add(new CombinedObjects(nCombinedObject, nCombinedAmount));
    }
}


void ObjectHolder::CombinedClear()
{
    for(ULONG i = 0; i < mCombinedInfo.size(); ++i)
        delete mCombinedInfo[i];
    
    mCombinedInfo.clear();
}


ULONG ObjectHolder::CombinedSize()
{
    return mCombinedInfo.size();
}


CombinedObjects* ObjectHolder::CombinedAt(ULONG hIndex)
{
    ASSERT(hIndex >= 0 && hIndex < mCombinedInfo.size());
    
    return mCombinedInfo[hIndex];
}



bool ObjectVRefSame(Person *hTo, Object *hObj1, Object *hObj2)
{
    ASSERT(hObj1);
    ASSERT(hObj2);
    return hObj1->mIndex->VRef() == hObj2->mIndex->VRef();
}


bool ObjectLookNameSame(Person *hTo, Object *hObj1, Object *hObj2)
{
    ASSERT(hObj1);
    ASSERT(hObj2);
        
    if(!hObj1->IsCustom() && !hObj2->IsCustom())
        return hObj1->mIndex == hObj2->mIndex;
    
    if(hObj1->GetNameTo(hTo).CaseCmp(hObj2->GetNameTo(hTo)) != 0)
        return false;
    
    if(hObj1->GetPNameTo(hTo, 2).CaseCmp(hObj2->GetPNameTo(hTo, 2)) != 0)
        return false;

    return true;
}



bool ObjectLookShortSame(Person *hTo, Object *hObj1, Object *hObj2)
{
    ASSERT(hObj1);
    ASSERT(hObj2);
    
    if(hObj1->ItemType() == kItemFurniture && hObj1->GetUsers(NULL))
        return false;
    if(hObj2->ItemType() == kItemFurniture && hObj2->GetUsers(NULL))
        return false;

    if(!hObj1->IsCustom() && !hObj2->IsCustom())
        return hObj1->mIndex == hObj2->mIndex;
    
    if(hObj1->GetShortDescTo(hTo).CaseCmp(hObj2->GetShortDescTo(hTo)) != 0)
        return false;
    
    if(hObj1->GetPShortDescTo(hTo, 2).CaseCmp(hObj2->GetPShortDescTo(hTo, 2)) != 0)
        return false;

    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ULONG ObjectHolder::Size()
{
    return mObjectsInside.size();
}


bool ObjectHolder::Empty()
{
    return mObjectsInside.empty();
}



ObjectIter ObjectHolder::Begin()
{
    return mObjectsInside.begin();
}




ObjectIter ObjectHolder::End()
{
    return mObjectsInside.end();
}


Object* ObjectHolder::Front()
{
    ASSERT(!mObjectsInside.empty());
    return  mObjectsInside.front();
}



Object* ObjectHolder::Back()
{
    ASSERT(!mObjectsInside.empty());
    return  mObjectsInside.back();
}



ObjectHolder* ObjectHolder::Base()
{
    ObjectHolder* nBase = this;
    
    while(nBase && nBase->mInsideThis)
        nBase = nBase->mInsideThis;
        
    return nBase;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::Exists
//
//   Desc:: Clears all the objects out of the ObjectHolder. Deletes them if
//          we own them.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::ClearObjects()
{
    //Delete Objects We Own
    for(ObjectIter nScan = Begin(); nScan != End(); ++nScan)
    if((*nScan)->mInsideThis == this)
        delete *nScan;
    
    
    //Delete Offsets Map Memory
    for(OHOffsetsIter nScan = mVRefOffsets.begin(); nScan != mVRefOffsets.end(); ++nScan)
        delete (*nScan).second;

    mObjectsInside.clear();
    mVRefOffsets.clear();
    CombinedClear();    
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::Exists
//
//   Desc:: Checks if an Object exists in this ObjectHolder. You can also pass
//          in a pointer to a ObjectIter which will be set to the ObjectIter
//          pointing to hObject inside this ObjectHolder if they are found. If
//          you used that in a later call to RemoveObject(ObjectIter&) it would
//          be faster because it doesn't have to locate the Object again.
//
//  Input:: hPerson: Object to search for inside this ObjectHolder
//          hItsIter: Optional. Set to the ObjectIter if it's found.
//
// Output:: True if it exists inside this ObjectHolder, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ObjectHolder::Exists(Object* hObject, ObjectIter* hItsIter)
{
    OHOffsetsIter   nIter;
    OHOffsets*      nOffsets;
    VREF            nVRef = hObject->mIndex->VRef();
    
    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        return false;

    nOffsets = (*nIter).second;
    ObjectIter  nBeginSearch    = nOffsets->mBegin;
    ObjectIter  nEndSearch      = nOffsets->mEnd;
    ObjectIter  nResult         = std::find(nBeginSearch, ++nEndSearch, hObject);
    
    if(nResult == nEndSearch)
        return false;
    
    if(hItsIter)
        *hItsIter = nResult;
    return true;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::AddObject
//
//   Desc:: Adds an Object into this ObjectHolder. If we own the object it is
//          deleted when this ObjectHolder is deleted and its mInsideThis field
//          is set to this ObjectHolder.
//
//  Input:: hObject: The Object to put into this ObjectHolder.
//          hWeOwn: Do we own the Object's memory?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::AddObject(Object* hObject, bool hWeOwn)
{
    ASSERT(hObject != this);    //Dont Put Us Inside Ourself!
    
    //** Add An Object Into The Segmented List **//
    VREF            nVRef = hObject->mIndex->VRef();
    OHOffsetsIter   nIter;
    OHOffsets*      nOffsets;
    

    if(hWeOwn)
        hObject->mInsideThis = this;

    if((nIter = mVRefOffsets.find(nVRef)) != mVRefOffsets.end())
    {
        nOffsets = (*nIter).second;

        ObjectIter nWhere = nOffsets->mEnd;
        mObjectsInside.insert(++nWhere, hObject);
        ++nOffsets->mEnd;
        return;
    }
    
    
    //** Add Object & New VRef-Object Offsets **//
    mObjectsInside.AddBack(hObject);
    ObjectIter nWhere = mObjectsInside.end();
    
    nWhere--;
    mVRefOffsets[nVRef] = new OHOffsets(nWhere, nWhere);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::AddObjects
//
//   Desc:: Adds all the Objects in an ObjectHolder to this ObjectHolder. If we
//          own the Objects they are deleted when this ObjectHolder is deleted
//          and their mInsideThis field is set to this ObjectHolder.
//
//  Input:: hObjects: ObjectHolder of objects to put into this ObjectHolder.
//          hWeOwn: Do we own all of the Objects' memory?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::AddObjects(ObjectHolder* hObjects, bool hWeOwn)
{
    //Optimize later
    for(ObjectIter i = hObjects->Begin(); i != hObjects->End(); ++i)
        AddObject(*i, hWeOwn);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RemoveObject
//
//   Desc:: Removes an Object from this ObjectHolder. If we own the object then
//          its mInsideThis field is set to NULL when it is removed, it isn't
//          deleted though.
//
//  Input:: hObject: Object to remove from this ObjectHolder.
//          hWeOwn: Do we "own" it?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::RemoveObject(Object* hObject, bool hWeOwn)
{
    //** Remove Object & VRef-Object Offsets **//
    OHOffsetsIter   nIter;
    OHOffsets*      nOffsets;
    VREF            nVRef = hObject->mIndex->VRef();
    
    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        ASSERT(!"No Vnum-Offset range in the object list");

    nOffsets = (*nIter).second;
    ObjectIter  nBeginSearch    = nOffsets->mBegin;
    ObjectIter  nEndSearch      = nOffsets->mEnd;
    ObjectIter  nResult         = std::find(nBeginSearch, ++nEndSearch, hObject);
    
    if(nResult == nEndSearch)
        ASSERT(!"Removal of non-existant Object");
    
    RemoveObject(nResult, hWeOwn);  
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RemoveObject
//
//   Desc:: Removes an Object from this ObjectHolder. If we own the object then
//          its mInsideThis field is set to NULL when it is removed, it isn't
//          deleted though.
//
//  Input:: hObject: ObjectIter to the Object to remove in this ObjectHolder.
//          hWeOwn: Do we "own" it?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::RemoveObject(ObjectIter& hObject, bool hWeOwn)
{
    //** Remove Object & VRef-Object Offsets **//
    OHOffsetsIter   nIter;
    OHOffsets*      nOffsets;
    VREF            nVRef = (*hObject)->mIndex->VRef();
    

    if(hWeOwn)
        (*hObject)->mInsideThis = NULL;

    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        ASSERT(!"Removed non-existant vnum list");

    nOffsets = (*nIter).second;
    if(nOffsets->mBegin == nOffsets->mEnd)
    {
        mVRefOffsets.erase(nIter);
        mObjectsInside.erase(hObject);
        delete nOffsets;
        return;
    }
    
    if(hObject == nOffsets->mEnd)
        --nOffsets->mEnd;
    else
    if(hObject == nOffsets->mBegin)
        ++nOffsets->mBegin;
    
    
    mObjectsInside.erase(hObject);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RemoveObjects
//
//   Desc:: Removes an ObjectHolder's worth of Objects from this ObjectHolder.
//          If we own the Objects then their mInsideThis fields are set to NULL
//          when they are removed, nothing is deleted though.
//
//  Input:: hObjects: ObjectHolder of  Objects to remove in this ObjectHolder.
//          hWeOwn: Do we "own" them all?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::RemoveObjects(ObjectHolder* hObjects, bool hWeOwn)
{
    //Optimize later
    for(ObjectIter i = hObjects->Begin(); i != hObjects->End(); ++i)
        RemoveObject(*i, hWeOwn);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RemoveObject
//
//   Desc:: Basically tells every Object in this ObjectHolder to remove
//          themselves from their true containers, which may or may not be this
//          ObjectHolder depending on if we own them or not.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::RemoveFromOwners()
{
    for(ObjectIter nScan = Begin(); nScan != End(); ++nScan)
    {
        ASSERT((*nScan)->mInsideThis != NULL);
        (*nScan)->mInsideThis->RemoveObject(*nScan);
    }
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RetrieveObjectAs
//
//   Desc:: Attempts to find an Object with a certain VREF inside this 
//          ObjectHolder as though hTo Person was looking at it.
//
//  Input:: hTo: The relative Person. May be NULL.
//          hVRef: VREF of an Object we're looking for.
//
// Output:: Non-NULL if an Object was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Object* ObjectHolder::RetrieveObjectAs(Person* hTo, VREF hVRef)
{
    OHOffsetsIter   nIter;
    if((nIter = mVRefOffsets.find(hVRef)) == mVRefOffsets.end())
        return NULL;
    
    OHOffsets* nOffsets = (*nIter).second;

    ObjectIter  nScan   = nOffsets->mBegin;
    ObjectIter  nEnd    = nOffsets->mEnd;
    Object*     nObject;
    
    nEnd++;
    for(; nScan != nEnd; ++nScan)
    {
        nObject = *nScan;
        
        if(!hTo || hTo->CanSee(nObject))
            return nObject;
    }
    
    return NULL;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RetrieveObjectsAs
//
//   Desc:: Finds Objects matched by a textual argument relative to a Person.
//          The results are stored in the supplied ObjectHolder. When a command
//          like "get 10*sword" is typed this is used to resolve "10*sword".
//
//  Input:: hTo: The relative Person.
//          hArg: Textual argument to match Objects to.
//          hHolder: An ObjectHolder to store the resulting matches in.
//
// Output:: True if anything was found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ObjectHolder::RetrieveObjectsAs(Person* hTo, const CHAR* hArg, ObjectHolder& hHolder)
{
    LONG nFound     = 0;
    LONG nCollected = 0;
    return RetrieveObjectsAs(hTo, hArg, hHolder, nFound, nCollected);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::RetrieveObjectsAs
//
//   Desc:: Finds Objects matched by a textual argument relative to a Person.
//          The results are stored in the supplied ObjectHolder. When a command
//          like "get 10*sword" is typed this is used to resolve "10*sword".
//          This method takes in the number already matched and the number
//          already collected (added into hHolder) so it can be called across
//          multiple ObjectHolders with something like "get 10*sword all".
//
//  Input:: hTo: The relative Person.
//          hArg: Textual argument to match Objects to.
//          hHolder: An ObjectHolder to store the resulting matches in.
//          hFound: How many object's matched the "sword" part.
//          hCollected: How many have been collected so far.
//
// Output:: True if anything was found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ObjectHolder::RetrieveObjectsAs(Person* hTo, const CHAR* hArg, ObjectHolder& hHolder, LONG& hFound, LONG& hCollected)
{
    if(Empty())
        return false;
    
    LONG            nStartWhere;        //The 4 in "3*4.Object"
    LONG            nStopAmount;        //The 3 in "3*4.Object"
    STRINGCW        nMatchArg;          //The Object in "3*4.Object"
    bool            nAll;               //Did they request "all"

    Vector<bool>    nPlainObjectFailed;
    LONG            nOffsetIndex    = 0;
    Object*         nLastObject     = NULL;
    Object*         nObject;
    ObjectIter      nScan;


    nPlainObjectFailed.resize(mVRefOffsets.size());
    for(ULONG i = 0; i < nPlainObjectFailed.size(); ++i)
        nPlainObjectFailed[i] = false;


    ParseMatching(hArg, nStartWhere, nStopAmount, nMatchArg, nAll);
    for(nScan = mObjectsInside.begin(); nScan != mObjectsInside.end(); ++nScan)
    {
        nObject = *nScan;
        if(nLastObject && nLastObject->GetIndex() != nObject->GetIndex())
            nOffsetIndex++;
        nLastObject = nObject;
    
        //Can We Even See It?
        if(!hTo->CanSee(nObject))
            continue;


        //Did A Similar Object Already Fail To Match?
        if(nPlainObjectFailed[nOffsetIndex] && !nObject->IsCustom())
            continue;
        
        
        //Check If It Fails To Match
        if(!(nAll || nObject->Keywords().IsNamed(nMatchArg)))
        {
            nPlainObjectFailed[nOffsetIndex] = true;
            continue;
        }
        
        
        //WooHoo It Matches, Can We Add It Yet?
        if(++hFound < nStartWhere)
            continue;


        //Add It And Continue           
        hHolder.AddObject(nObject);
        if(++hCollected == nStopAmount)     //-1 is all*item
            break;
    }   
    
    return hCollected ? true : false;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::ShowNameObjectsTo
//
//   Desc:: Shows this ObjectHolder to a Person, listing the Objects by their 
//          names.
//
//  Input:: hTo: Relative to this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::ShowNameObjectsTo(Person* hTo)
{
    CombineTo(hTo, ObjectLookNameSame);
    if(!CombinedSize())
        return;
        
    LONG nAmt;
    for(ULONG i = 0; i < CombinedSize(); ++i)
    if((nAmt = CombinedAt(i)->mAmount) == 1)
        hTo->SendF("%s\n", *CombinedAt(i)->mObject->GetNameTo(hTo));
    else
        hTo->SendF("%s\n", *CombinedAt(i)->mObject->GetPNameTo(hTo, nAmt));
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::ShowShortObjectsTo
//
//   Desc:: Shows this ObjectHolder to a Person, listing the Objects by their
//          short descriptions. Like you would see in a room.
//
//  Input:: hTo: Relative to this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::ShowShortObjectsTo(Person* hTo)
{
    CombineTo(hTo, ObjectLookShortSame);
    if(!CombinedSize())
        return;
        
    LONG nAmt;
    for(ULONG i = 0; i < CombinedSize(); ++i)
    if((nAmt = CombinedAt(i)->mAmount) == 1)
        hTo->SendF("%s\n", *CombinedAt(i)->mObject->GetShortDescTo(hTo));
    else
        hTo->SendF("%s\n", *CombinedAt(i)->mObject->GetPShortDescTo(hTo, nAmt));
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ObjectHolder::ShowShortObjectsTo
//
//   Desc:: Shows this ObjectHolder to a Person, listing the Objects in the
//          columned inventory form.
//
//  Input:: hTo: Relative to this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ObjectHolder::ShowInventoryTo(Person* hTo)
{
    CombineTo(hTo, ObjectLookNameSame);
    if(!CombinedSize())
        return;
        
    List<STRINGCW> nShortNames, nLongNames;
    bool nLeft = true;
    STRINGCW nNameStr;
    CHAR nBuf[512];
    LONG nAmount;
    float nWeight;
    
    for(ULONG i = 0; i < CombinedSize(); ++i)
    {
        CombinedObjects* nCombined = CombinedAt(i);
        nNameStr    = nCombined->mObject->GetNameTo(hTo);
        nAmount     = nCombined->mAmount;
        nWeight     = nAmount*nCombined->mObject->TotalWeight();
        if(CStrLen(nNameStr) > 28)
        {
            csprintf(nBuf, "\n%-71s %3ld %4.1f", *nNameStr, nAmount, nWeight);
            nLongNames.AddBack(nBuf);
        }
        else
        {
            if(nLeft)
                csprintf(nBuf, "\n%-28s  %3d %4.1f", *nNameStr, nAmount, nWeight);
            else
                csprintf(nBuf, "    %-28s  %3d %4.1f", *nNameStr, nAmount, nWeight);
            nShortNames.AddBack(nBuf);
            nLeft = !nLeft;
        }
    }
    
    //** Send Results **//
    hTo->SendF("^U%-28s^x  ^U%3s^x ^U%-4s^x    ^U%-28s^x  ^U%3s^x ^U%4s^x", 
    "Item", "Num", "Wght", "Item", "Num", "Wght");
    for(List<STRINGCW>::iterator i = nShortNames.begin(); i != nShortNames.end(); ++i)
        hTo->Send(*i);
    if(!nShortNames.empty() && !nLongNames.empty())
        hTo->Send("\n");
    for(List<STRINGCW>::iterator i = nLongNames.begin(); i != nLongNames.end(); ++i)
        hTo->Send(*i);
    hTo->Send("\n");
}




