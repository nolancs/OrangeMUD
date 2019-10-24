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
#include "Player.h"
#include "Mobile.h"
#include "PersonHolder.h"
#include "Tables.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "RoomIndex.h"


CombinedPeople::CombinedPeople(Person* hPerson, LONG hAmt)
    : mPerson(hPerson)
    , mAmount(hAmt)
{
}



CombinedPeople::~CombinedPeople()
{
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: CombinedPeople::GetTextForCodeTo
//
//   Desc:: An ActThing function overriden so that whenever a $n, $m, etc is
//          encountered in relation to this class that it returns a string you
//          decide upon. The code is the 'n' part of a $n. Every ActThing
//          object is required to respond to at least the '?' code. 
//
//  Input:: hTo: Get the text for hCode relative to this person.
//          hCode: The code.
//
// Output:: The string results.
//
//   Note:: The codes n,e,m,s,v,c are reserved for Person/Player/Mobile classes
//          ONLY so you cannot use those as codes for any other classes to
//          respond to, but all other characters are usable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW CombinedPeople::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    //Would Format each $n $N $m accordling with the amount of people
    //that are combined in this group.
    
    if(mAmount == 1)
        return mPerson->GetTextForCodeTo(hTo, hCode);

    STRINGCW nText;
    
    if(hCode == 'n' || hCode == '?')
    {
        nText = hTo->CanSee(mPerson) ? *mPerson->GetPNameTo(hTo, mAmount) : "someones";
    }else
    if(hCode == 'e')
    {
        nText = gSexTable[kSexEither].mHeShe;
    }else
    if(hCode == 'm')
    {
        nText = gSexTable[kSexEither].mHimHer;
    }else
    if(hCode == 's')
    {
        nText = gSexTable[kSexEither].mHisHer;
    }else
    if(hCode == 'v')
    {
        nText = "{MOVEVERB}"; //mMovement;
    }else
    if(hCode == 'c')
    {
        nText = "{CONDITION}";
        //strcpy(buf, this->Condition(to, false, false));
    }else{
        nText.SPrintF("{Unrecognized_Code(%c)_On_CombinedPeople}", hCode);
    }

    return nText;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




PersonHolder::PersonHolder()
{

}




PersonHolder::~PersonHolder()
{
    ClearPeople();
}




PersonHolder& PersonHolder::operator =(const PersonHolder& hFrom)
{
    AddPeople(&((PersonHolder&) hFrom));
    return *this;
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: RoomIndex::GetTextForCodeTo
//
//   Desc:: An ActThing function overriden so that whenever a $n, $m, etc is
//          encountered in relation to this class that it returns a string you
//          decide upon. The code is the 'n' part of a $n. Every ActThing
//          object is required to respond to at least the '?' code. 
//
//  Input:: hTo: Get the text for hCode relative to this person.
//          hCode: The code.
//
// Output:: The string results.
//
//   Note:: The codes n,e,m,s,v,c are reserved for Person/Player/Mobile classes
//          ONLY so you cannot use those as codes for any other classes to
//          respond to, but all other characters are usable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW PersonHolder::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    return "{GetTextForCode On PersonHolder}";
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::CombineTo
//
//   Desc:: Combine a bunch of people into the condensed output form. It is
//          done in relation to how the hTo person views all the people in
//          the PersonHolder.
//          Strict means it will not group all matching people, only those
//          immediately following the original person, once a person does not
//          match it will quit checking. This is done to keep "strict" control
//          over the combination so 2.Foo and such will match up with how it
//          should be.
//
//  Input:: hTo: Combine in relation to how this Person sees everything.
//          hCompareFunc: Tells CombineTo when to People are "equal".
//          hStrict: Default is off. Turns on strict combining.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::CombineTo(Person* hTo, PerCompare* hCompareFunc, bool hStrict)
{
    Vector<LONG>    nCombined;
    PersonIter      nReScan, nScan  = Begin();
    ULONG           i, nPeopleSize  = Size();
    ULONG           nScanNum, nReScanNum;
    
    
    //Combined All The People
    mCombinedInfo.clear();
    nCombined.resize(nPeopleSize);
    for(i = 0; i < nPeopleSize; ++i)
        nCombined[i] = -1;
    
    
    for(nScanNum = 0; nScanNum < nPeopleSize; ++nScan, nScanNum++)
    if(nCombined[nScanNum] == -1)
    {
        nCombined[nScanNum] = 1;
        
        nReScan     = nScan;
        nReScanNum  = nScanNum;
        for(++nReScan, nReScanNum++; nReScanNum < nPeopleSize; ++nReScan, nReScanNum++)
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
    for(nScan = Begin(), i = 0; i < nPeopleSize; ++nScan, i++)
    if(nCombined[i] > 0)
    {
        Person* nCombinedPerson = *nScan;
        LONG    nCombinedAmount = nCombined[i];
        
        ASSERT(nCombinedPerson != NULL);
        mCombinedInfo.Add(new CombinedPeople(nCombinedPerson, nCombinedAmount));
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::CombinedClear
//
//   Desc:: Clears the results and memory used from a previous CombineTo call.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::CombinedClear()
{
    for(ULONG i = 0; i < mCombinedInfo.size(); ++i)
        delete mCombinedInfo[i];
    
    mCombinedInfo.clear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::CombinedSize
//
//   Desc:: Retrieves the number of CombinedPeople in the results of the last
//          CombineTo. Each CombinedPeople will be shown as a single chunk or
//          line of text like "three trolls". 
//
// Output:: The combined amount.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ULONG PersonHolder::CombinedSize()
{
    return mCombinedInfo.size();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::CombinedAt
//
//   Desc:: Retrieves a pointer to the CombinedPeople structure that represents
//          a bunch of people combined. The CombinedPeople has info pointing to
//          an object and how many of them got combined into it because they
//          appeared the same in the last CombineTo call.
//
//  Input:: The index of the CombinedPeople you wish to access.
//
// Output:: The CombinedPeople for the given Index.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

CombinedPeople* PersonHolder::CombinedAt(ULONG hIndex)
{
    ASSERT(hIndex >= 0 && hIndex < mCombinedInfo.size());
    
    return mCombinedInfo[hIndex];
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonVRefSame                          [Person Combining Function]
//
//   Desc:: A person combine function that will come people according to if
//          their VRef's match. You pass it as the CompareFunc argument in a
//          call to CombineTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PersonVRefSame(Person *hTo, Person *hPer1, Person *hPer2)
{
    ASSERT(hPer1);
    ASSERT(hPer2);
    
    if(hPer1->isPlayer || hPer2->isPlayer)
        return false;
        
    
    return hPer1->isMobile->mVRef == hPer2->isMobile->mVRef;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonLookNameSame                      [Person Combining Function]
//
//   Desc:: A person combine function that will come people according to if
//          their names look the same. You pass it as the CompareFunc argument
//          in a call to CombineTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PersonLookNameSame(Person *hTo, Person *hPer1, Person *hPer2)
{
    ASSERT(hPer1);
    ASSERT(hPer2);
    
    if(hPer1->GetNameTo(hTo).CaseCmp(hPer2->GetNameTo(hTo)) != 0)
        return false;
    
    if(hPer1->GetPNameTo(hTo, 2).CaseCmp(hPer2->GetPNameTo(hTo, 2)) != 0)
        return false;

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonLookShortSame                     [Person Combining Function]
//
//   Desc:: A person combine function that will come people according to if
//          their short descriptions look the same. You pass it as the
//          CompareFunc argument in a call to CombineTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PersonLookShortSame(Person *hTo, Person *hPer1, Person *hPer2)
{
    ASSERT(hPer1);
    ASSERT(hPer2);
    
    if(hPer1->mOnObject != hPer2->mOnObject
    || hPer1->mOnHow != hPer2->mOnHow)
        return false;
    
    if(hPer1->GetShortDescTo(hTo).CaseCmp(hPer2->GetShortDescTo(hTo)) != 0)
        return false;
    
    if(hPer1->GetPShortDescTo(hTo, 2).CaseCmp(hPer2->GetPShortDescTo(hTo, 2)) != 0)
        return false;

    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


ULONG PersonHolder::Size()
{
    return mPeopleInside.size();
}


bool PersonHolder::Empty()
{
    return mPeopleInside.empty();
}


PersonIter PersonHolder::Begin()
{
    return mPeopleInside.begin();
}




PersonIter PersonHolder::End()
{
    return mPeopleInside.end();
}



Person* PersonHolder::Front()
{
    ASSERT(!mPeopleInside.empty());
    return  mPeopleInside.front();
}



Person* PersonHolder::Back()
{
    ASSERT(!mPeopleInside.empty());
    return  mPeopleInside.back();
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::ClearPeople
//
//   Desc:: Removes all the people from a Person holder. Clears all memory
//          used and restores the PersonHolder to a pristine empty state.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::ClearPeople()
{
    //Delete People We Own
    for(PersonIter nScan = Begin(); nScan != End(); ++nScan)
    if((*nScan)->mInRoom == this)
    {
        (*nScan)->RemoveFromGame(false);
        delete *nScan;
    }
    
    //Delete Offsets Map Memory
    for(PHOffsetsIter nScan = mVRefOffsets.begin(); nScan != mVRefOffsets.end(); ++nScan)
        delete (*nScan).second;


    mPeopleInside.clear();
    mVRefOffsets.clear();
    CombinedClear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::Exists
//
//   Desc:: Checks if a Person exists in this PersonHolder. You can also pass
//          in a pointer to a PersonIter which will be set to the PersonIter
//          pointing to hPerson inside this PersonHolder if they are found. If
//          you used that in a later call to RemovePerson(PersonIter&) it would
//          be faster because it doesn't have to locate the Person again.
//
//  Input:: hPerson: Person to search for inside this PersonHolder
//          hItsIter: Optional. Set to their PersonIter if they're found.
//
// Output:: True if they exist inside this PersonHolder, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PersonHolder::Exists(Person* hPerson, PersonIter* hItsIter)
{
    //** Find A No-Vnum Player From The List **//
    if(hPerson->isPlayer)
    {
        for(PersonIter nScan = Begin(); nScan != End();)
        {
            if(*nScan == hPerson)
            {
                if(hItsIter)
                    *hItsIter = nScan;
                return true;
            }
            else
            if((*nScan)->isPlayer)
            {
                ++nScan;
                continue;
            }
        
            //Iterate Past This Chunk Of Mobiles
            ULONG nVnum = (*nScan)->isMobile->mVRef;
            nScan = mVRefOffsets[nVnum]->mEnd;
            ++nScan;
        }
        
        return false;
    }
    
    
    //** Remove Mobile & VRef-Person Offsets **//
    PHOffsetsIter   nIter;
    PHOffsets*      nOffsets;
    VREF            nVRef = hPerson->isMobile->mVRef;
    
    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        return false;

    nOffsets = (*nIter).second;
    PersonIter  nBeginSearch    = nOffsets->mBegin;
    PersonIter  nEndSearch      = nOffsets->mEnd;
    PersonIter  nResult         = std::find(nBeginSearch, ++nEndSearch, hPerson);
    
    if(nResult == nEndSearch)
        return false;
    
    if(hItsIter)
        *hItsIter = nResult;
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::AddPerson
//
//   Desc:: Adds a Person to this PersonHolder. They will be inserted at a
//          place determined by their vref and attributes. Players are always
//          added at the beginning (top when seen in a room).
//
//  Input:: The Person to add to this PersonHolder.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::AddPerson(Person* hPerson)
{
    //** Add A Player **//
    if(hPerson->isPlayer)
    {
        mPeopleInside.AddFront(hPerson);
        return;
    }
    
    
    //** Add A Mobile Into The Segmented List **//
    VREF            nVRef = hPerson->isMobile->mVRef;
    PHOffsetsIter   nIter;
    PHOffsets*      nOffsets;
    
    if((nIter = mVRefOffsets.find(nVRef)) != mVRefOffsets.end())
    {
        nOffsets = (*nIter).second;

        PersonIter nWhere = nOffsets->mEnd;
        mPeopleInside.insert(++nWhere, hPerson);
        ++nOffsets->mEnd;
        return;
    }
    
    
    //** Add Mobile & New VRef-Person Offsets **//
    mPeopleInside.AddBack(hPerson);
    PersonIter nWhere = mPeopleInside.end();
    
    nWhere--;
    mVRefOffsets[nVRef] = new PHOffsets(nWhere, nWhere);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::AddPeople
//
//   Desc:: Adds a bunch of people in a PersonHolder to this PersonHolder. They
//          will be inserted at places determined by their vref and attributes.
//          Players are always added at the beginning (top when seen in a 
//          room).
//
//  Input:: The PersonHolder to add to this PersonHolder.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::AddPeople(PersonHolder* hPeople)
{
    //Optimize later
    for(PersonIter i = hPeople->Begin(); i != hPeople->End(); ++i)
        AddPerson(*i);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::RemovePerson
//
//   Desc:: Removes a Person from this PersonHolder. Do not call this unless
//          you are sure they are inside the PersonHolder because it will
//          ASSERT if you try to remove a non-existant person.
//
//  Input:: The Person to remove from this PersonHolder.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::RemovePerson(Person* hPerson)
{
    //** Remove A No-Vnum Player From The List **//
    if(hPerson->isPlayer)
    {
        for(PersonIter nScan = Begin(); nScan != End();)
        {
            if(*nScan == hPerson)
            {
                mPeopleInside.erase(nScan);
                return;
            }
            else
            if((*nScan)->isPlayer)
            {
                ++nScan;
                continue;
            }
        
            //Iterate Past This Chunk Of Mobiles
            ULONG nVnum = (*nScan)->isMobile->mVRef;
            nScan = mVRefOffsets[nVnum]->mEnd;
            ++nScan;
        }
        
        ASSERT(!"Remove of non-existant Player");
        return;
    }
    
    
    //** Remove Mobile & VRef-Person Offsets **//
    PHOffsetsIter   nIter;
    PHOffsets*      nOffsets;
    VREF            nVRef = hPerson->isMobile->mVRef;
    
    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        ASSERT(!"No Vnum-Offset range in the person list");

    nOffsets = (*nIter).second;
    PersonIter  nBeginSearch    = nOffsets->mBegin;
    PersonIter  nEndSearch      = nOffsets->mEnd;
    PersonIter  nResult         = std::find(nBeginSearch, ++nEndSearch, hPerson);
    
    if(nResult == nEndSearch)
        ASSERT(!"Removal of non-existant Mobile");
    
    RemovePerson(nResult);  
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::RemovePerson
//
//   Desc:: Same as the above RemovePerson but it actually removes the Person
//          by their PersonIter in this specific PersonHolder. It saves time if
//          you are scanning the PersonHolder by PersonIters and need to remove
//          a Person you should call this rather than passing their Person*.
//
//  Input:: The Person to remove from this PersonHolder.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::RemovePerson(PersonIter& hPerson)
{
    //** Remove A Player **//
    if((*hPerson)->isPlayer)
    {
        mPeopleInside.erase(hPerson);
        return;
    }
    
    
    //** Remove Mobile & VRef-Person Offsets **//
    PHOffsetsIter   nIter;
    PHOffsets*      nOffsets;
    VREF            nVRef = (*hPerson)->isMobile->mVRef;
    
    
    if((nIter = mVRefOffsets.find(nVRef)) == mVRefOffsets.end())
        ASSERT(!"Removed non-existant vnum list");

    nOffsets = (*nIter).second;
    if(nOffsets->mBegin == nOffsets->mEnd)
    {
        mVRefOffsets.erase(nIter);
        mPeopleInside.erase(hPerson);
        delete nOffsets;
        return;
    }
    
    if(hPerson == nOffsets->mEnd)
        --nOffsets->mEnd;
    else
    if(hPerson == nOffsets->mBegin)
        ++nOffsets->mBegin;
    
    mPeopleInside.erase(hPerson);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::RemovePeople
//
//   Desc:: Removes a bunch of people in a PersonHolder from this PersonHolder.
//          If will ASSERT if any of the people do not actually exist in this
//          PersonHolder.
//
//  Input:: The PersonHolder of people to remove from this PersonHolder.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::RemovePeople(PersonHolder* hPeople)
{
    //Optimize later
    for(PersonIter i = hPeople->Begin(); i != hPeople->End(); ++i)
        RemovePerson(*i);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::SortPeopleTo
//
//   Desc:: Sorts all the people in this PersonHolder according to a PerSort
//          method. The sorted people are stored into a PersonList.
//
//  Input:: hTo: Sort them as they appear to this Person.
//          hSorted: A PersonList to hold the results of the sort.
//          hPerSortFunc: The sorting method to use.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::SortPeopleTo(Person* hTo, PersonList& hSorted, PerSort* hPerSortFunc)
{
    if(Empty()) return;
    Person**    nSortArray = new Person*[mPeopleInside.size()];
    PersonIter  nScan;
    ULONG       i = 0;
    
    
    for(nScan = Begin(); nScan != End(); ++nScan)
        nSortArray[i++] = *nScan;

    qsort(nSortArray, mPeopleInside.size(), sizeof(Person*), hPerSortFunc);
    
    for(i = 0; i < mPeopleInside.size(); ++i)
        hSorted.AddBack(nSortArray[i]);

    delete [] nSortArray;
}




INT PersonLevelSort(const void* nElement1, const void* nElement2)
{
    Person* nPer1 = (*(Person**) nElement1);
    Person* nPer2 = (*(Person**) nElement2);
    
    return nPer2->mLevel-nPer1->mLevel;
}




INT PersonNameSort(const void* nElement1, const void* nElement2)
{
    Person* nPer1   = (*(Person**) nElement1);
    Person* nPer2   = (*(Person**) nElement2);
    LONG    nLen    = UMAX(nPer1->mName.Length(), nPer2->mName.Length());
    
    /* A - Z */
    return strncmp(nPer1->mName, nPer2->mName, nLen);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::ShowPeopleToAs
//
//   Desc:: Shows a PersonHolder as the condensed form of short descriptions,
//          generic showing routine. RoomIndex has its own ShowPeopleTo().
//
//  Input:: The person to show it to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void PersonHolder::ShowPeopleToAs(Person* hTo)
{
    LONG nAmt;
    
    CombineTo(hTo, PersonLookShortSame);
    if(!CombinedSize())
        return;
        
    for(ULONG i = 0; i < CombinedSize(); ++i)
    {
        CombinedPeople* nScan = CombinedAt(i);
        
        if((nAmt = nScan->mAmount) == 1)
            hTo->SendF("%s\n", *nScan->mPerson->GetShortDescTo(hTo));
        else
            hTo->SendF("%s\n", *nScan->mPerson->GetPShortDescTo(hTo, nAmt));
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::RetrievePersonAs
//
//   Desc:: Attempts to find a Mobile with a certain VREF inside this 
//          PersonHolder as though hTo Person was looking at it.
//
//  Input:: hTo: The relative Person. May be NULL.
//          hVRef: VREF of a Mobile we're looking for.
//
// Output:: Non-NULL if a Mobile was found.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Person* PersonHolder::RetrievePersonAs(Person* hTo, VREF hVRef)
{
    PHOffsetsIter   nIter;
    if((nIter = mVRefOffsets.find(hVRef)) == mVRefOffsets.end())
        return NULL;
    
    PHOffsets* nOffsets = (*nIter).second;

    PersonIter  nScan   = nOffsets->mBegin;
    PersonIter  nEnd    = nOffsets->mEnd;
    Person*     nMobile;
    
    nEnd++;
    for(; nScan != nEnd; ++nScan)
    {
        nMobile = *nScan;
        
        if(!hTo || hTo->CanSee(nMobile))
            return nMobile;
    }
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: PersonHolder::RetrievePeopleAs
//
//   Desc:: Retrieves people from a PersonHolder. It searches the PersonHolder
//          as hTo, trying to find people that match hArg. It stores the people
//          found in hHolder. If hSight is false it will not check if hTo can
//          actually see the people, the default is true.
//
//  Input:: hCh: As this person.
//          hArg: Search for these people.
//          hHolder: And store them in this PersonHolder.
//          hSight: If true, only people I can see get found.
//
// Output:: True if any were found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool PersonHolder::RetrievePeopleAs(Person* hTo, CHAR* hArg, PersonHolder& hHolder, bool hSight)
{
    if(Empty())
        return false;
    
    LONG            nStartWhere;        //The 4 in "3*4.Object"
    LONG            nStopAmount;        //The 3 in "3*4.Object"
    STRINGCW        nMatchArg;          //The Object in "3*4.Object"
    bool            nAll;               //Did they request "all"

    LONG            nCollected = 0, nFound = 0;
    Person*         nPerson;
    PersonIter      nScan;
    bool            nSelf;



    ParseMatching(hArg, nStartWhere, nStopAmount, nMatchArg, nAll);
    nSelf = nMatchArg == "self";
    for(nScan = mPeopleInside.begin(); nScan != mPeopleInside.end(); ++nScan)
    {
        nPerson = *nScan;
    
        //Can We Even See It?
        if(!hTo->CanSee(nPerson))
            continue;


        //Check If It Fails To Match
        if(!(nAll || nPerson->mKeywords.IsNamed(nMatchArg) || (nSelf && nPerson == hTo)))
            continue;
        
        
        //WooHoo It Matches, Can We Add It Yet?
        if(++nFound < nStartWhere)
            continue;


        //Add It And Continue           
        hHolder.AddPerson(nPerson);
        if(++nCollected == nStopAmount)     //-1 is all*person
            break;
    }   
    
    return nCollected ? true : false;
}




