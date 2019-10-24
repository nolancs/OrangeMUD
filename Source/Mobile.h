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
#ifndef MOBILE_H
#define MOBILE_H 1
#include "Person.h"
#include "VRefThing.h"


class Mobile 
    : public Person
    , public VRefThing
{
    //** De/constructors **//
    public: Mobile(MobileIndex* hFromIndex);
    public: virtual ~Mobile();

    //** SaveLoadable **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName)  {}
    virtual void    SaveValuesTo(XMLWriter& hFile)                  {}
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag)                      { return false; }

    //** Public Member Functions **//
    virtual void    PlaceInGame(PersonHolder* hTo, bool hLoggingIn);
    virtual void    RemoveFromGame(bool hLoggingOut);
    virtual void    PlaceIn(PersonHolder* hTo);
    virtual void    RemoveFrom();
    virtual bool    GetRoundAttacks(AttackArray& hAttacks);
    virtual void    StatTo(Person* hTo);


    //** Runtime Data Members **//
    public:
    MobileIndex*        mIndex;
    MobilePopulation*   mPopulation;
};






//** Mobile Flags **//
#define kMobNPC             (A)     /* Auto set for mobs    */
#define kMobSentinal        (B)     /* Stays in one room    */
#define kMobScavenger       (C)     /* Picks up objects     */
#define kMobSentient        (D)
#define kMobUnnatural       (E)     /* Will attack druids   */
#define kMobAggressive      (F)     /* Attacks PC's         */
#define kMobStayArea        (G)     /* Won't leave area     */
#define kMobWimpy           (H)
#define kMobPet             (I)     /* Auto set for pets    */
#define kMobTrain           (J)     /* Can train PC's       */
#define kMobPractice        (K)     /* Can practice PC's    */

#define kMobUndead          (O) 

#define kMobGhost           (Q)
#define kMobShopkeeper      (R)
#define kMobNightOnly       (S)
#define kMobDayOnly         (T)
#define kMobNoKill          (U)
#define kMobNoPurge         (V)
#define kMobOutdoors        (W)
#define kMobIndoors         (Y)





#endif //ifndef MOBILE_H
