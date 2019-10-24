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
#ifndef OBJECT_H
#define OBJECT_H 1
#include "ObjectProxy.h"
#include "ObjectHolder.h"
#include "LoadOld.h"
#include "SaveLoadable.h"


class Object
    : public ObjectHolder
    , public ObjectProxy
    , public LoadOld
//  , public ActThing       //(Comes From ObjectHolder)
//  , public SaveLoadable   //(Comes From ObjectHolder)
{
    //** De/constructors **//
    public: Object(ObjectIndex* hFromIndex);
    public: virtual ~Object();

    //** Overriden Virtual Functions **//
    public:
    virtual void            SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void            SaveValuesTo(XMLWriter& hFile);
    virtual bool            MatchValues(XMLReader& hXML, XMLTag& hTag);
    virtual bool            MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool            MatchLists(MUDFile& hFp, CHAR* hName);
    static void             LoadInto(MUDFile& hFp, ObjectHolder* hInto);
    virtual ObjectIndex*    GetIndex() const;
    virtual STRINGCW        GetTextForCodeTo(Person* hTo, CHAR hCode);

    //** Public Member Functions *//
    public:
    STRINGCW            GetNameTo(Person* hTo);
    STRINGCW            GetPNameTo(Person* hTo, LONG hAmount);
    STRINGCW            GetKeywordsTo(Person* hTo);
    STRINGCW            GetShortDescTo(Person* hTo);
    STRINGCW            GetPShortDescTo(Person* hTo, LONG hAmount);
    STRINGCW            GetDescriptionTo(Person* hTo);
    bool                GetUsers(PersonHolder* hUsers);
    SHORT               CountUsers();
    void                RemoveFrom();
    bool                IsContainer();
    bool                IsClosed();
    bool                IsLocked();
    bool                IsUsableHow(Person* hTo, BYTE hPos, BYTE hHow);
    bool                IsWearable(LONG hPos);
    bool                IsTakeable();
    bool                IsLighting();
    void                ShowTo(Person* hTo);
    void                ShowContentsTo(Person* hTo);
    float               TotalWeight();
    float               ContCapacity();
    float               ContMultiplier();
    VREF                ContKey();

    //** Runtime Data Members **//
    public:
    ObjectIndex*        mIndex;
    ObjectPopulation*   mPopulation;
    
    //** Persistant Data Members **//
    public:
    CHAR                mWearLoc;
    BYTE                mContFlags;
};




//## Item Wear Flags ##//
#define kWearTake       (A)
#define kWearFinger     (B)
#define kWearNeck       (C)
#define kWearBodyPad    (D) //Body
#define kWearBody       (E)
#define kWearAbout      (F)
#define kWearShoulders  (Z)
#define kWearPlume      (G) //Head
#define kWearHelm       (H)
#define kWearCap        (I)
#define kWearLegs       (J) //Legs
#define kWearLegPads    (K)
#define kWearFeet       (L) //Feet
#define kWearSocks      (M)
#define kWearHands      (N) //Hands
#define kWearArms       (O) //Arms
#define kWearArmPads    (P)
#define kWearShield     (Q)
#define kWearWaist      (R)
#define kWearSheath     (Y)
#define kWearWrist      (S) //Wrist
#define kWearBracer     (T)
#define kWearWield      (U)
#define kWearOffhand    (V)
#define kWearHold       (W)
#define kWearFloat      (X)
#define kWearFace       (aa)
#define kWearEars       (bb)
#define kWearWings      (cc)
#define kWearTail       (ee)

//## Item Worn Where Locations ##//
#define kLocStuck       -4  //Arrow stuck in target
#define kLocBank        -3
#define kLocSell        -2
#define kLocNone        -1
#define kLocLight       0
#define kLocFingerL     1
#define kLocFingerR     2
#define kLocNeck1       3
#define kLocNeck2       4
#define kLocPlume       5
#define kLocHelm        6
#define kLocCap         7
#define kLocOuter       8
#define kLocBody        9
#define kLocInner       10
#define kLocLegs        11
#define kLocLegPads     12
#define kLocFeet        13
#define kLocSocks       14
#define kLocHands       15
#define kLocArms        16
#define kLocArmPads     17
#define kLocShield      18
#define kLocWaist       19
#define kLocBracerL     20
#define kLocWristL      21
#define kLocBracerR     22
#define kLocWristR      23
#define kLocWield       24
#define kLocOffhand     25
#define kLocHold        26
#define kLocFloat       27
#define kLocSheath      28
#define kLocShoulders   29
#define kLocFace        30
#define kLocEars        31
#define kLocWings       32
#define kLocTail        33
#define kLocSheathed    34
#define kLocMax         35


//## Item Types ##//
#define kItemLight          0   //* Balls of light, lanterns and such
#define kItemScroll         1   //-
#define kItemWand           2   //-
#define kItemStaff          3   //-
#define kItemPotion         4   //-
#define kItemPill           5   //-
#define kItemWeapon         6   //* Weaponsmiths will buy it
#define kItemArmor          7   //* Armorers will buy it
#define kItemFurniture      8   //* 
#define kItemContainer      9   //* Bags and such
#define kItemDrinkCon       10  //* Drink
#define kItemFountain       11  //* 
#define kItemFood           12  //* Yep food
#define kItemCurrency       13  //* Coinage baby
#define kItemBoat           14  //- Maybe implemented
#define kItemCorpseNPC      15  //*
#define kItemCorpsePC       16  //* 
#define kItemReagent        17  //* Used in magical spells
#define kItemStones         18  //* Something a jewler might buy
#define kItemLuxury         19  //* Misc pricey things of no real use
#define kItemSkin           20  //* Animal skins etc
#define kItemJunk           21  //* Crap, nobody will buy it
#define kItemOther          22  //* Weird out of the way item, pack rats will buy
#define kItemInstrument     23  // Something a bard can play
#define kItemMax            24


//## Object Generic (Extra) Flags ##//
#define kObjectIsContainer  (A)
#define kObjectCursed       (B)
#define kObjectNoDrop       (C)
#define kObjectNoLiq        (D)
#define kObjectEnchanted    (E)
#define kObjectTwoHandled   (F)
#define kObjectThrowable    (G)
#define kObjectSharp        (H)
#define kObjectBladed       (I)


//## Furniture Flags ##//
void CvtFurnFlagToPosHow(LONG hFurnFlag, BYTE* hPos, BYTE* hHow);
LONG CvtPosHowToFurnFlag(BYTE hPos, BYTE hHow);
enum
{
    kUseNone = 0    ,
    kUseAt          , 
    kUseOn          ,
    kUseIn          ,
    kUseMax
};
#define kStandAt        (A)
#define kStandOn        (B)
#define kStandIn        (C)
#define kSitAt          (D)
#define kSitOn          (E)
#define kSitIn          (F)
#define kRestAt         (G)
#define kRestOn         (H)
#define kRestIn         (I)
#define kSleepAt        (J)
#define kSleepOn        (K)
#define kSleepIn        (L)
#define kPutAt          (M)
#define kPutOn          (N)
#define kPutIn          (O)
#define kPutInside      (P)




//## Container Flags ##//
#define kContCloseable      (A)
#define kContLockable       (B)
#define kContPickable       (C)
#define kContClosed         (D)
#define kContLocked         (E)
//Outdated
#define kOldContCloseable   (A)
#define kOldContPickproof   (B)
#define kOldContClosed      (C)
#define kOldContLocked      (D)
#define kOldContLockable    (F)
#define kOldContPickable    (G)


//## Object Value[] Definitions ##//
#define kLightLife      0
#define kWeapClass      0
#define kWeapMin        1
#define kWeapMax        2
#define kWeapCoat       3
#define kWeapMod        4
#define kArmAC          0
#define kArmMod         1
#define kFurnPeople     0
#define kFurnHit        1
#define kFurnEnergy     2
#define kFurnMove       3
#define kFurnFlags      4
#define kDContMaxCap    0
#define kDContCurCap    1
#define kDContLiq       2
#define kReagNum        0
#define kReagMin        1
#define kReagMax        2
#define kFoodFull       0
#define kFoodHunger     1
#define kFoodHigh       2
#define kCorpseHost     4
//Outdated Container Object Stuff
#define kContCap        0
#define kContFlags      1
#define kContOpening    2
#define kContMulti      3
#define kContKey        4


//## Weapon Types ##//
#define kWeapUnarmed    0
#define kWeapDagger     1
#define kWeapSword      2
#define kWeapAxe        3
#define kWeapPolearm    4
#define kWeapSpear      5
#define kWeapStaff      6
#define kWeapMace       7
#define kWeapWhip       8
#define kWeapFlail      9
#define kWeapBow        10
#define kWeapArrow      11
#define kWeapLance      12


//## Instrument Types ##//
#define kInstLyre       0
#define kInstFlute      1
#define kInstDrum       2
#define kInstHorn       3
#define kInstHarp       4
#define kInstViolin     5




#endif //ifndef OBJECT_H

