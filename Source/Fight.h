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
#ifndef FIGHT_H
#define FIGHT_H 1

class Fight
{
    //** De/constructors **//
    public: Fight(Person* hTank, Person* hVict);
    public: ~Fight();
    
    //** Public Member Functions **//
    public:
    void        End();
    void        Add(Person* hPerson);
    void        Remove(Person* hPerson);
    void        DoRound();

    //** Public Data Members **//
    public:
    Person*     mVict;
    Person*     mKiller;
    PersonList  mKillers;
};



class Blow
{
    //** De/constructors **//
    public: Blow();
    public: Blow(Fight* hFight, Person* hKiller, Attack* hAttack = NULL);
    public: ~Blow();

    //** Public Member Functions **//
    public:
    void    DoCalculation();    //Calculate damage
    void    DoDisplay();        //Show calculated damage
    void    DoDamage();         //Actually do damage
    bool    Killed();
    bool    Missed();

    //** Public Data Members **//
    public:
    Person* mVict;
    Person* mKiller;
    Object* mWeapon;
    Part*   mPart;
    BYTE    mType;      //Tank, Random
    CHAR*   mMessage;
    BYTE    mDamageType;
    SHORT   mHitroll[2];
    SHORT   mDamroll[2];
    SHORT   mFlags;
    BYTE    mDistance;
    //Calculated Members
    SHORT   mDamage;
    UCHAR   mResultFlags;
    enum{kMissed = (A), kKilled = (B)};
};




#endif //ifndef FIGHT_H
