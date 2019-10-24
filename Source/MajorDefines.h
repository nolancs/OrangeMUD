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
#ifndef MAJORDEFINES_H
#define MAJORDEFINES_H 1

#define kMaxInputLen    4096        //4k
#define kMaxStringLen   10960       //10k

//** VRef Functions **//
#define kThisArea   -1
inline VREF MakeVRef(VNUM hArea, VNUM hVNum) {return (VREF(hArea) << 16) | hVNum;}
inline VNUM GetArea(VREF hRef) {return VNUM(hRef >> 16);} //0xFF00 Left 
inline VNUM GetVnum(VREF hRef) {return VNUM(hRef);      } //0x00FF Right 

//** Well Known VRefs **//
#define kVRef_Room_Start                MakeVRef(0, 0)      /* 1401 */
#define kVRef_Room_Limbo                MakeVRef(0, 2)
#define kVRef_Room_Purgatory            MakeVRef(0, 3)
#define kVRef_Room_MobLounge            MakeVRef(0, 4)
#define kVRef_Object_Null               MakeVRef(0, 0)
#define kVRef_Object_Privacy            MakeVRef(0, 25)
#define kVRef_Object_CorpseNPC          MakeVRef(0, 10)
#define kVRef_Object_CorpsePC           MakeVRef(0, 11)
#define kVRef_Object_CorpseNPC_Insect   MakeVRef(0, 12)
#define kVRef_Object_CorpseNPC_Rodent   MakeVRef(0, 13)
#define kVRef_Object_CorpseNPC_Animal   MakeVRef(0, 14)
#define kVRef_Object_CorpseNPC_Small    MakeVRef(0, 15)
#define kVRef_Object_CorpseNPC_Normal   MakeVRef(0, 16)
#define kVRef_Object_CorpseNPC_Larger   MakeVRef(0, 17)
#define kVRef_Object_CorpseNPC_Giant    MakeVRef(0, 18)
#define kVRef_Object_CorpseNPC_Titan    MakeVRef(0, 19)


//** Levels **//
#define kLevelMax       110
#define kLevelImmortal  101
#define kLevelHero      100
enum
{   
    L1  = kLevelImmortal,       // Avatar
    L2  ,                       // Builder
    L3  ,
    L4  ,
    L5  ,                       // DemiGod
    L6  ,
    L7  ,                       // God
    L8  ,   
    L9  ,                       // Ancient
    ML  = kLevelMax             // Implementor
};
#define kImplementor    kLevelMax       //110
#define kAncient        L9              //108..109
#define kGod            L7              //106..107
#define kDemiGod        L5              //103..104..105
#define kBuilder        L2              //102
#define kAvatar         L1              //101
#define kHero           kLevelHero      //100
#define IMP             kImplementor
#define ANC             kAncient
#define GOD             kGod
#define DEM             kDemiGod
#define BLD             kBuilder
#define AVT             kAvatar
#define HER             kHero


//** Names For Each Bit - Thanks ROM! **//
#define A           1
#define B           2
#define C           4
#define D           8
#define E           16
#define F           32
#define G           64
#define H           128
#define I           256
#define J           512
#define K           1024
#define L           2048
#define M           4096
#define N           8192
#define O           16384
#define P           32768
#define Q           65536
#define R           131072
#define S           262144
#define T           524288
#define U           1048576
#define V           2097152
#define W           4194304
#define X           8388608
#define Y           16777216
#define Z           33554432
#define aa          67108864
#define bb          134217728
#define cc          268435456    
#define dd          536870912
#define ee          1073741824


//** Types Of Day **//
enum
{
    kSunNight = 0   ,
    kSunDay         ,
    kSunMax
};


//** Directions **//
enum
{
    kDirNorth = 0   ,
    kDirEast        ,
    kDirSouth       ,
    kDirWest        ,
    kDirUp          ,
    kDirDown        ,
    kDirMax
};


//** Currencies **//
enum
{
    kCurrencyCopper = 0 ,
    kCurrencySilver     ,
    kCurrencyGold       ,
    kCurrencyPlatinum   ,
    kCurrencyMax
};


//** Stat Indexes **//
enum
{
    kStatStr = 0,
    kStatDex    ,
    kStatInt    ,
    kStatWis    ,
    kStatCon    ,
    kStatMax
};


//** Sex Types  **//
enum
{
    kSexNeutral = 0 ,
    kSexMale        ,
    kSexFemale      ,
    kSexEither      ,
    kSexMax
};


//** Size Types **//
enum
{
    kSizeInsect = 0 ,
    kSizeRodent     ,
    kSizeAnimal     ,
    kSizeSmall      ,
    kSizeNormal     ,
    kSizeLarger     ,
    kSizeGiant      ,
    kSizeTitan      ,
    kSizeMax
};



//** Damage Types **//
enum
{
    kDamPhysical = 0    ,
    kDamCold            ,
    kDamFire            ,
    kDamElectricity     ,
    kDamAcid            ,
    kDamMind
};


//** Resistance Indexes **//
enum 
{
    kResCold = 0,
    kResFire    ,
    kResAcid    ,
    kResElec    ,
    kResPois    ,
    kResMind    , 
    kResSlee    ,
    kResChar    ,
    kResMax
};


//** Wimpy Indexes **//
#define kWimpyOff   LONG_MIN
enum
{
    kWimpyFlee = 0  ,
    kWimpyBlink     ,
    kWimpyRecall    ,
    kWimpyPray      ,
    kWimpyMax 
};


//** Condition Indexes **//
enum
{
    kCondDrunk = 0  ,
    kCondFull       ,
    kCondThirst     ,
    kCondHunger     ,
    kCondHigh       ,
    kCondWithdrawl  , 
    kCondMax
};
#define kNowHungry  40
#define kNowThirsty 40
#define kCondUpdate 254     //Passed As Value For Normal Update
#define kCondNever  -1      //Never update conditions, like old -1 on hunger/thirst
#define kMaxFull    100


//** Positions **//
enum
{
    kPosDead = 0    ,
    kPosMortal      ,
    kPosIncap       ,
    kPosStunned     , 
    kPosSleeping    ,
    kPosResting     ,
    kPosSitting     ,
    kPosFighting    , 
    kPosHunted      ,
    kPosStanding    ,
    kPosMax
};

#define DEAD        kPosDead
#define MORTAL      kPosMortal
#define INCAP       kPosIncap
#define STUNNED     kPosStunned
#define SLEEPING    kPosSleeping
#define RESTING     kPosResting
#define SITTING     kPosSitting
#define FIGHTING    kPosFighting
#define HUNTED      kPosHunted
#define STANDING    kPosStanding



//** Liquid Affect Indexes **//
enum
{
    kLiqProof = 0   ,
    kLiqFull        ,
    kLiqThirst      ,
    kLiqFood        ,
    kLiqSip         ,
    kLiqHigh        ,
    kLiqAffMax
};


//** Class Types **//
enum
{
    kClassWarrior       = 0 ,
    kClassPaladin           ,
    kClassDemonknight       ,
    kClassRanger            ,
    kClassArcher            ,
    kClassRogue             ,
    kClassNinja             ,
    kClassPriest            ,
    kClassShaman            ,
    kClassDruid             ,
    kClassMagician          ,
    kClassElementalist      ,
    kClassNecromancer       ,
    kClassPsion             ,
    kClassBard              ,
    kClassMax
};



//** Race Types **//
enum 
{                       //      Size    Speed   Strngth Brains  Wisdom  Toughness   Special
    kRaceHuman  = 0 ,   //1  GB Normal  Normal  Normal  Normal  Normal  Normal      
    kRaceAvariel    ,   //2  GB Normal  Faster  Strong  Smart   Wise    Fragile     ¥Wings
    kRaceDwarf      ,   //3  G  Smaller Slow    Strong  Dumb    Normal  Tough       ¥+Axes  
    kRaceElf        ,   //4  G  Normal  Fast    Weak    Smarter Wiser   Weak
    kRaceFaery      ,
    kRaceHobbit     ,   //5  G  Smaller Fast    Normal  Dumb    Wiser   Normal
    kRaceKzinti     ,   //6  GB Large   Fast    Strong  Dumber  Dumb    Tough       ¥Eyes/Claws
    kRaceMorph      ,   //7  GB Normal  -Slow   -Weak   Normal  Normal  Weak        ¥Morphing
    kRaceOrc        ,   //8  B  Smaller Slow    Strong  Normal  Dumb    Tougher
    kRaceTroll      ,   //8  B  Larger  Slower  Strongr Dumber  Dumber  Tougher
    kRaceVampire    ,   //9  B  Normal  Normal  Weak    Normal  Normal  Weak        ¥Blood+ Light-
    kRaceVyan       ,   //10 B  Normal  Fast    Weak    Smarter Wiser   Weak
    //RACE_GOBLIN,      //¥  B  Smaller Fast    Normal  Dumber  Dumb    Tough
    kRaceMax
};




//** Command Categories **//
enum
{
    kWaitGeneral = 0,   //General Commands
    kWaitCombat,        //Combat Commands
    kWaitMove,          //Movement Commands
    kWaitMax         
};
#define kWaitNone -1


//** Object Material Bits **//
#define kMatBronze      (A)     /*  1   */  //=======[_METALS_]==========>
#define kMatIron        (B)
#define kMatSteel       (C)
#define kMatElectrum    (D)
#define kMatMithril     (E)
#define kMatAdamantite  (F)
#define kMatVenomite    (H)
#define kMatCopper      (I)
#define kMatSilver      (J)
#define kMatGold        (K)     /*  10  */
#define kMatPlatinum    (L)                 //=======[_VARIOUS_]=========>
#define kMatLeather     (M)
#define kMatCloth       (N)
#define kMatWood        (O)
#define kMatCrystal     (P)
#define kMatGemstone    (Q)
#define kMatStone       (R)
#define kMatBone        (S)
#define kMatOrganic     (T)
#define kMatGlass       (U)     /*  20  */
#define kMatEnergy      (V)
#define kMatUnknown     (W)



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark Macros
#endif

//One Function Prototype
LONG Random(LONG hMin, LONG hMax); //Both defined in StringMagic.cpp
LONG Randomize(LONG hNum, float hPercent);


//#define   ASSERT(x)           assert(x)

#define IsSet(i, bit)       ((i) & (bit))
#define NotSet(i, bit)      (!((i) & (bit)))
#define SetBit(i, bit)      ((i) |= (bit))
#define RemoveBit(i, bit)   ((i) &= ~(bit))
#define ToggleBit(i, bit)   (((i) & (bit)) ? ((i) &= ~(bit)) : ((i) |= (bit)))
#define Tenth(i)            (((float)i)/10.0)
#define Hundreth(i)         (((float)i)/100.0)
#define Avg(x,y)            ((x+y)/2)
#define ObjInPerson(o, x)       (o->mInsideThis == static_cast<ObjectHolder*>(x))
#define ObjInObject(o, x)       (o->mInsideThis == static_cast<ObjectHolder*>(x))
#define ObjInRoom(o, x)         (o->mInsideThis == static_cast<ObjectHolder*>(x))
#define ObjInAnything(o)        (o->mInsideThis != NULL)


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark GraphicText
#endif

#define kGfxSep     "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
#define kGfxSep2    "-==============================================================================-"
#define kGfxThinSep "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define kGfxLine    "--------------------------------------------------------------------------------"
#define kUnderLine  "^U                                                                                ^x"

#endif //ifndef MAJORDEFINES_H


