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
#ifndef TABLES_H
#define TABLES_H 1


//## Defines ##//
#define kLiqMax     48


//## Forward Class/Struct Declarations ##//
class   FlagTable;
class   ItemTable;
class   LiquidTable;
class   CurrencyTable;
class   SexTable;
class   SizeTable;
class   PosTable;
class   WearTable;
class   WearFormatTable;
class   WearMessageTable;
class   WeaponTable;
class   TerrainTable;
class   TriggerTable;
class   StatTable;
class   ResistTable;
class   DamageTable;
class   DamageMessageTable;
class   MaterialTable;

class   OptionFlags;
class   InfoFlags;
class   RoomFlags;
class   ObjectFlags;
class   MobileFlags;
class   ContFlags;
class   FurnFlags;
class   SpotFlags;
class   ExitFlags;
class   AttackFlags;

//## External Tables ##//
extern LiquidTable          gLiqTable;
extern ItemTable            gItemTable;
extern CurrencyTable        gCurrencyTable;
extern SexTable             gSexTable;
extern SizeTable            gSizeTable;
extern PosTable             gPosTable;
extern WearTable            gWearTable;
extern WearFormatTable      gWearFormatTable;
extern WearMessageTable     gWearMessageTable;
extern WeaponTable          gWeaponTable;
extern TerrainTable         gTerrainTable;
extern TriggerTable         gTriggerTable;
extern StatTable            gStatTable;
extern ResistTable          gResistTable;
extern DamageTable          gDamageTable;
extern DamageMessageTable   gPhysTable;
extern DamageMessageTable   gColdTable;
extern DamageMessageTable   gFireTable;
extern DamageMessageTable   gElecTable;
extern DamageMessageTable   gAcidTable;
extern DamageMessageTable   gMindTable;
extern MaterialTable        gMaterialTable;

extern OptionFlags          gOptionFlags;
extern InfoFlags            gInfoFlags;
extern RoomFlags            gRoomFlags;
extern ObjectFlags          gObjectFlags;
extern MobileFlags          gMobileFlags;
extern ContFlags            gContFlags;
extern FurnFlags            gFurnFlags;
extern SpotFlags            gSpotFlags;
extern ExitFlags            gExitFlags;
extern AttackFlags          gAttackFlags;

extern const SHORT          gRevDir[];
extern const STRINGCW       gDirName[];
extern const CHAR*          gDirNameStrs[];
extern const STRINGCW       gRevDirName[];
extern const CHAR*          gRevDirMoveStrs[];


//## Prototypes ##//
SHORT LookupHelp(const CHAR* hName);



////////////////////////////////////////////////////////////////////////////////
// ## Information Structures ##
////////////////////////////////////////////////////////////////////////////////

struct BasicShortType
{
    CHAR*   mName;
    SHORT   mValue;
};

struct BasicLongType
{
    CHAR*   mName;
    LONG    mValue;
};

struct FlagType
{
    CHAR*   mName;
    LONG    mBit;
    SHORT   mTrust;
    bool    mSettable;
};

struct LiquidType
{
    CHAR*   mName;
    CHAR*   mColor;
    SHORT   mValue;
    SHORT   mAffects[kLiqAffMax];
};

struct CurrencyType
{
    CHAR*   mName;  //quick word reference to this form of currency, for building mostly
    CHAR*   mAbbr;  //Abbreviation, platinum piece = pp, copper piece = cp. Like that
    SHORT   mType;  //Money type number => #define CURRENCY_COPPER 0
    SHORT   mValue; //The value of this currency is X of the base currency
                    //...Note: Base currency is always num=0 and value=1, everything else uses it.
    VREF    mVRef;  //Object vnum for this piece of money
};

struct SexType
{
    CHAR*   mName;
    SHORT   mValue;
    CHAR*   mHeShe;
    CHAR*   mHimHer;
    CHAR*   mHisHer;
};

struct SizeType
{
    CHAR*   mName;
    SHORT   mValue;
    VREF    mCorpseVRef;
};

struct PosType
{
    SHORT   mValue;
    float   mRegenMod;
    CHAR*   mUnused;
    CHAR*   mName;
    CHAR*   mLDesc;
};


#define kMaxWearTypeLocs 2
struct WearType
{
    CHAR*   mName;
    LONG    mWearBit;
    SHORT   mWearLocs[kMaxWearTypeLocs];
};

struct WearMsgType
{
    SHORT   mValue;
    CHAR*   mWearPerson;
    CHAR*   mWearOthers;
};

#define kMaxWearFormatTypeLocs 3
struct WearFormatType
{
    CHAR*   mName;
    SHORT   mLocations[kMaxWearFormatTypeLocs];
};

struct WeaponType
{
    CHAR*   mName;
    SHORT   mClass;
    CHAR*   mDamage;
    SHORT   mAbil;
};

struct TerrainType
{
    CHAR*   mName;
    SHORT   mMoveLoss;
    LONG    mFlags;
};

struct TriggerType
{
    SHORT   mType;
    CHAR*   mName;
    bool    mForRooms;
    bool    mForObjects;
    bool    mForMobiles;
};

struct DamageType
{
    CHAR*   mSName;
    SHORT   mValue;
    CHAR*   mLName;
    DamageMessageTable* mTable;
};

struct DamageMessageType
{
    LONG    mAbove;
    CHAR*   mMsg;
    CHAR*   mPMsg;
};

struct MaterialType
{
    CHAR*   mName;
    SHORT   mDurability;
    bool    mCasting;
    LONG    mBit;
};


////////////////////////////////////////////////////////////////////////////////
// ## Table Classes ##
////////////////////////////////////////////////////////////////////////////////

class FlagTable : public Vector<FlagType>
{
    public: FlagTable() {}
    public: virtual ~FlagTable() {}
    public: virtual SHORT FindNamed(const CHAR* hName, Person* hCh = NULL);
    public: virtual STRINGCW ListBitNames(LONG hBits, Person* hCh = NULL);
    public: virtual bool FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags);
};

class ItemTable : public Vector<BasicShortType>
{
    public: ItemTable();
};

class LiquidTable : public Vector<LiquidType>
{
    public: LiquidTable();
};

class CurrencyTable : public Vector<CurrencyType>
{
    public: CurrencyTable();
};

class SexTable : public Vector<SexType>
{
    public: SexTable();
};

class SizeTable : public Vector<SizeType>
{
    public: SizeTable();
};

class PosTable : public Vector<PosType>
{
    public: PosTable();
};

class WearTable : public Vector<WearType>
{
    public: WearTable();
    public: STRINGCW ListBitNames(LONG hBits, bool hShowTake);
    public: bool FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags);
};

class WearFormatTable : public Vector<WearFormatType>
{
    public: WearFormatTable();
};

class WearMessageTable : public Vector<WearMsgType>
{
    public: WearMessageTable();
};

class WeaponTable : public Vector<WeaponType>
{
    public: WeaponTable();
};

class TerrainTable : public Vector<TerrainType>
{
    public: TerrainTable();
};

class TriggerTable : public Vector<TriggerType>
{
    public: TriggerTable();
};

class StatTable : public Vector<BasicShortType>
{
    public: StatTable();
};

class ResistTable : public Vector<BasicShortType>
{
    public: ResistTable();
};

class DamageTable : public Vector<DamageType>
{
    public: DamageTable();
};

class DamageMessageTable : public Vector<DamageMessageType>
{
    public: DamageMessageTable(SHORT hType);
};

class MaterialTable : public Vector<MaterialType>
{
    public: MaterialTable();
    public: STRINGCW ListBitNames(LONG hBits, Person* hTo = NULL);
    public: bool FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags);
};

////////////////////////////////////////////////////////////////////////////////
// Flags
////////////////////////////////////////////////////////////////////////////////

class OptionFlags   : public FlagTable { public: OptionFlags(); };
class InfoFlags     : public FlagTable { public: InfoFlags();   };
class RoomFlags     : public FlagTable { public: RoomFlags();   };
class ObjectFlags   : public FlagTable { public: ObjectFlags(); };
class MobileFlags   : public FlagTable { public: MobileFlags(); };
class ContFlags     : public FlagTable { public: ContFlags();   };
class FurnFlags     : public FlagTable { public: FurnFlags();   };
class SpotFlags     : public FlagTable { public: SpotFlags();   };
class ExitFlags     : public FlagTable { public: ExitFlags();   };
class AttackFlags   : public FlagTable { public: AttackFlags(); };

////////////////////////////////////////////////////////////////////////////////
// Generic Templated Lookup Methods
////////////////////////////////////////////////////////////////////////////////
SHORT StrCmp(const CHAR* hStr1, const CHAR* hStr2);

template<typename TableT>
SHORT TableLookup(TableT& hTable, const CHAR* hName, bool hExact = true)
{
    for(USHORT i = 0; i < hTable.size(); ++i)
    {   
        if(hExact)
        {   if(!StrCmp(hName, hTable[i].mName)) return i;
        }else
        {   if(STRINGCW(hTable[i].mName).IsNamed(hName))    return i;
        }
    }
    return -1;
}

template<typename TableT>
SHORT TableLookupL(TableT& hTable, const CHAR* hName, bool hExact = true)
{
    for(USHORT i = 0; i < hTable.size(); ++i)
    {   
        if(hExact)
        {   if(!StrCmp(hName, hTable[i].mLName))    return i;
        }else
        {   if(STRINGCW(hTable[i].mLName).IsNamed(hName))   return i;
        }
    }
    return -1;
}


#endif //ifndef TABLES_H

