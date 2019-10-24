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
#ifndef PERSONHOLDER_H
#define PERSONHOLDER_H 1
#include "ActThing.h"



class PHOffsets;
class CombinedPeople;
typedef Map<LONG, PHOffsets*>       PHOffsetsMap;
typedef PHOffsetsMap::iterator      PHOffsetsIter;
typedef bool PerCompare (Person *hTo, Person *hPer1, Person *hPer2);
typedef INT PerSort(const void* nElement1, const void* nElement2);


class PersonHolder
    : virtual public ActThing
{
    //** De/constructors **//
    public: PersonHolder();
    public: virtual ~PersonHolder();
    public: PersonHolder& operator =(const PersonHolder& hFrom);

    //** Public Member Functions **//
    // ActThing
    virtual STRINGCW    GetTextForCodeTo(Person* hTo, CHAR hCode);
    // Combinine
    void            CombineTo(Person* hTo, PerCompare* hCompareFunc, bool hStrict = true);
    void            CombinedClear();
    ULONG           CombinedSize();
    CombinedPeople* CombinedAt(ULONG hIndex);
    // List Iteration
    ULONG           Size();
    bool            Empty();
    PersonIter      Begin();
    PersonIter      End();
    Person*         Front();
    Person*         Back();

    // Input & Output
    void            ClearPeople     ();
    bool            Exists          (Person*        hPerson, PersonIter* hItsIter = NULL);
    void            AddPerson       (Person*        hPerson);
    void            AddPeople       (PersonHolder*  hPeople);
    void            RemovePerson    (Person*        hPerson);
    void            RemovePerson    (PersonIter&    hPerson);
    void            RemovePeople    (PersonHolder*  hPeople);
    // Display
    void            SortPeopleTo(Person* hTo, PersonList& hSorted, PerSort* hPerSortFunc);
    void            ShowPeopleToAs  (Person* hTo);
    Person*         RetrievePersonAs(Person* hTo, VREF hVRef);
    bool            RetrievePeopleAs(Person* hTo, CHAR* hArg, PersonHolder& hFound, bool hSight = true);

    
    

    //** Protected Data Members **//
    protected:
    PersonList                  mPeopleInside;
    PHOffsetsMap                mVRefOffsets;
    Vector<CombinedPeople*>     mCombinedInfo;
};




class CombinedPeople
    : public ActThing
{
    //** De/constructors **//
    public: CombinedPeople(Person* hPerson, LONG hAmt);
    public: virtual ~CombinedPeople();

    //** ActThing Member Functions **//
    public:
    virtual STRINGCW GetTextForCodeTo(Person* hTo, CHAR hCode);
    
    
    //** Public Data Members **//
    public:
    LONG        mAmount;
    Person*     mPerson;
};




class PHOffsets
{
    public: PHOffsets() {}
    public: PHOffsets(PersonIter& hBegin, PersonIter& hEnd) : mBegin(hBegin), mEnd(hEnd) {}
    
    public:
    PersonIter  mBegin;
    PersonIter  mEnd;
};



PerCompare  PersonVRefSame;
PerCompare  PersonLookNameSame;
PerCompare  PersonLookShortSame;
PerSort     PersonLevelSort;
PerSort     PersonNameSort;

#endif //ifndef PERSONHOLDER_H



