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
#include "Attack.h"
#include "Mobile.h"
#include "Object.h"
#include "OrangeScript.h"
#include "Person.h"
#include "RoomIndex.h"
#include "Spot.h"
#include "StringUtils.h"
#include "Tables.h"


//## Tables ##//
LiquidTable         gLiqTable;
CurrencyTable       gCurrencyTable;
ItemTable           gItemTable;
SexTable            gSexTable;
SizeTable           gSizeTable;
PosTable            gPosTable;
WearTable           gWearTable;
WearFormatTable     gWearFormatTable;
WearMessageTable    gWearMessageTable;
WeaponTable         gWeaponTable;
TerrainTable        gTerrainTable;
TriggerTable        gTriggerTable;
StatTable           gStatTable;
ResistTable         gResistTable;
DamageTable         gDamageTable;
DamageMessageTable  gPhysTable(kDamPhysical);
DamageMessageTable  gColdTable(kDamCold);
DamageMessageTable  gFireTable(kDamFire);
DamageMessageTable  gElecTable(kDamElectricity);
DamageMessageTable  gAcidTable(kDamAcid);
DamageMessageTable  gMindTable(kDamMind);
MaterialTable       gMaterialTable;

OptionFlags         gOptionFlags;
InfoFlags           gInfoFlags;
RoomFlags           gRoomFlags;
ObjectFlags         gObjectFlags;
MobileFlags         gMobileFlags;
ContFlags           gContFlags;
FurnFlags           gFurnFlags;
SpotFlags           gSpotFlags;
ExitFlags           gExitFlags;
AttackFlags         gAttackFlags;

const SHORT gRevDir[kDirMax] =
{
    kDirSouth,
    kDirWest,
    kDirNorth,
    kDirEast,
    kDirDown,
    kDirUp
};

const STRINGCW gDirName[kDirMax] =
{
    STRINGCW("north")   ,
    STRINGCW("east")    ,
    STRINGCW("south")   ,
    STRINGCW("west")    ,
    STRINGCW("up")      ,
    STRINGCW("down")
};
const CHAR* gDirNameStrs[kDirMax+1] =
{
    "north" ,
    "east"  ,
    "south" ,
    "west"  ,
    "up"    ,
    "down"  ,
    "\n"
};
const STRINGCW gRevDirName[kDirMax] =
{
    STRINGCW("south")   ,
    STRINGCW("west")    ,
    STRINGCW("north")   ,
    STRINGCW("east")    ,
    STRINGCW("down")    ,
    STRINGCW("up")
};
const CHAR* gRevDirMoveStrs[kDirMax] =
{
    "the south" ,
    "the west"  ,
    "the north" ,
    "the east"  ,
    "below"     ,
    "above" 
};





SHORT LookupHelp(const CHAR* hName)
{
    return 0;
}




ItemTable::ItemTable()
{
    static BasicShortType sItemTypes[kItemMax] =
    {
    { "light"           , kItemLight        }, /* 0 */
    { "scroll"          , kItemScroll       },
    { "wand"            , kItemWand         },
    { "staff"           , kItemStaff        },
    { "potion"          , kItemPotion       },
    { "pill"            , kItemPill         },
    { "weapon"          , kItemWeapon       },
    { "armor"           , kItemArmor        },
    { "furniture"       , kItemFurniture    },
    { "container"       , kItemContainer    },
    { "drink"           , kItemDrinkCon     },
    { "fountain"        , kItemFountain     },
    { "food"            , kItemFood         },
    { "currency"        , kItemCurrency     },
    { "boat"            , kItemBoat         },
    { "npc_corpse"      , kItemCorpseNPC    },
    { "pc_corpse"       , kItemCorpsePC     },
    { "reagent"         , kItemReagent      },
    { "stone"           , kItemStones       },
    { "luxury"          , kItemLuxury       },
    { "skin"            , kItemSkin         },
    { "junk"            , kItemJunk         },
    { "other"           , kItemOther        },
    { "instrument"      , kItemInstrument   }
    };

    for(SHORT i = 0; i < kItemMax; ++i)
        Add(sItemTypes[i]);
}




LiquidTable::LiquidTable()
{
    static LiquidType sLiqs[kLiqMax] =
    {
    //           Sip    = 25 mL         Thirst 100 Pts From Min (Dying) to Max,     50 Norm->Max
    //          Shot    = 30 mL           Food 100 Pts From Min (Starving) To Max,  50 Norm->Max
    //          Gulp    = 60 mL           Full  50 Pts To Full Stomach
    //      Can Soda    = 240 mL
    //   Bottle Soda    = 591 mL        ( Leave Room For Food, Main Full Filler )
    
    // Name               Color               Value   Proof Full|Thrst|Food Sip-mL High
    { "absinthe"        , "green"           , 20    , { 200,   1,   4,   0,  30,   0 }},
    { "ale"             , "brown"           , 5     , {  15,   1,   8,   1,  50,   0 }},
    { "amontillado"     , "burgundy"        , 15    , {  35,   2,   8,   1,  50,   0 }},
    { "aquavit"         , "clear"           , 15    , { 140,   1,   5,   0,  30,   0 }},
    { "beer"            , "amber"           , 10    , {  12,   1,   8,   1,  50,   0 }},
    { "benedictine wine", "burgundy"        , 10    , {  40,   1,   8,   1,  25,   0 }},
    { "blood"           , "red"             , 23    , {   0,   2,  -1,   2,  45,   0 }},
    { "brandy"          , "golden"          , 10    , {  80,   1,   5,   0,  35,   0 }},
    { "champagne"       , "golden"          , 15    , {  32,   1,   8,   1,  30,   0 }},
    { "chocolate milk"  , "light brown"     , 10    , {   0,   3,  10,   3,  55,   0 }},
    { "cocoa"           , "brown"           , 5     , {   0,   2,   4,   0,  25,   0 }},
    { "coffee"          , "black"           , 2     , {   0,   1,   8,   0,  35,   0 }},
    { "coke"            , "brown"           , 2     , {   0,   2,   9,   2,  40,   0 }}, 
    { "cordial"         , "shiny blue"      , 10    , { 100,   1,   5,   0,  35,   0 }},
    { "cranberry juice" , "red"             , 5     , {   0,   1,   9,   2,  50,   0 }},
    { "dark ale"        , "dark"            , 7     , {  16,   1,   8,   1,  50,   0 }},
    { "elvish wine"     , "green"           , 9     , {  35,   2,   8,   1,  40,   0 }},
    { "firebreather"    , "boiling red"     , 14    , { 190,   0,   4,   0,  30,   0 }},
    { "framboise"       , "red"             , 12    , {  50,   1,   7,   1,  35,   0 }},
    { "icewine"         , "purple"          , 12    , {  50,   2,   6,   1,  35,   0 }},

    // Name               Color               Value   Proof Full|Thrst|Food Sip-mL High
    { "lemonade"        , "pink"            , 5     , {   0,   1,   9,   2,  50,   0 }},
    { "local specialty" , "red-yellow"      , 5     , { 151,   1,   3,   0,  30,   0 }},
    { "mead"            , "honey-colored"   , 3     , {  34,   2,   8,   2,  50,   0 }},
    { "milk"            , "white"           , 3     , {   0,   2,   9,   3,  50,   0 }},
    { "milk plus"       , "white"           , 300   , {   0,   2,   9,   3,  50,   7 }},
    { "orange juice"    , "orange"          , 5     , {   0,   2,   9,   3,  40,   0 }}, 
    { "red wine"        , "burgundy"        , 7     , {  30,   1,   8,   1,  40,   0 }},
    { "root beer"       , "brown"           , 5     , {   0,   2,   9,   2,  40,   0 }},
    { "rose wine"       , "pink"            , 9     , {  26,   1,   8,   1,  40,   0 }},
    { "rum"             , "amber"           , 6     , { 151,   1,   4,   0,  30,   0 }},
    { "salt water"      , "clear"           , 0     , {   0,   1,  -2,   0,  60,   0 }},
    { "schnapps"        , "clear"           , 9     , {  90,   1,   5,   0,  35,   0 }},
    { "sherry"          , "red"             , 11    , {  38,   2,   7,   1,  30,   0 }},    
    { "slime mold juice", "green"           , 0     , {   0,   2,  -8,   1,  50,   0 }},
    { "soup"            , "soupy yellow"    , 4     , {   0,   2,   6,   4,  30,   0 }},
    { "stew"            , "thick brown"     , 4     , {   0,   2,   4,   5,  30,   0 }},
    { "tea"             , "tan"             , 5     , {   0,   1,   8,   0,  40,   0 }},
    { "vodka"           , "clear"           , 10    , { 130,   1,   5,   0,  30,   0 }},
    { "water"           , "clear"           , 0     , {   0,   1,  10,   0,  60,   0 }},
    { "whisky"          , "golden"          , 10    , { 120,   1,   5,   0,  30,   0 }},
    { "white wine"      , "golden"          , 10    , {  28,   1,   8,   1,  35,   0 }},

    //Poisons
    { "sting"           , "dark yellow"     , 100   , {  -2,   1,  -5,   0,   3,   0 }},
    { "bite"            , "purple"          , 150   , {  -3,   1,  -5,   0,   3,   0 }},
    { "venom"           , "bright green"    , 200   , {  -4,   1,  -5,   0,   3,   0 }},
    { "bane"            , "crimson"         , 300   , {  -5,   1,  -5,   0,   3,   0 }},
    { "rot"             , "black"           , 500   , {  -7,   1,  -5,   0,   3,   0 }},
    { "eldersap"        , "dark blue"       , 700   , { -10,   1,  -5,   0,   3,   0 }},
    { "hemlock"         , "dark green"      , 1000  , { -13,   1,  -5,   0,   3,   0 }}
    };
    
    for(SHORT i = 0; i < kLiqMax; ++i)
        Add(sLiqs[i]);
};




CurrencyTable::CurrencyTable()
{
    static CurrencyType sCurrencies[kCurrencyMax] = 
    {
    // Name        Abbrv  Currency Number     Value   VRef
    { "platinum", "pp"  , kCurrencyPlatinum , 1000  , MakeVRef(0,4) },
    { "gold"    , "gp"  , kCurrencyGold     , 100   , MakeVRef(0,3) },
    { "silver"  , "sp"  , kCurrencySilver   , 10    , MakeVRef(0,2) },
    { "copper"  , "cp"  , kCurrencyCopper   , 1     , MakeVRef(0,1) }
    };

    for(SHORT i = 0; i < kCurrencyMax; ++i)
        Add(sCurrencies[i]);
}




SexTable::SexTable()
{
    static SexType sSexTypes[kSexMax] =
    {   
    //                          HeShe       HimHer      HisHer
    { "none"    , kSexNeutral   , "it"      , "it"      , "its"     },
    { "male"    , kSexMale      , "he"      , "him"     , "his"     },
    { "female"  , kSexFemale    , "she"     , "her"     , "her"     },
    { "either"  , kSexEither    , "they"    , "them"    , "their"   }
    };

    for(SHORT i = 0; i < kSexMax; ++i)
        Add(sSexTypes[i]);
}




SizeTable::SizeTable()
{
    static SizeType sSizeTypes[kSizeMax] = 
    {
    { "insect"  , kSizeInsect   , kVRef_Object_CorpseNPC_Insect },  
    { "rodent"  , kSizeRodent   , kVRef_Object_CorpseNPC_Rodent },
    { "animal"  , kSizeAnimal   , kVRef_Object_CorpseNPC_Animal },  
    { "small"   , kSizeSmall    , kVRef_Object_CorpseNPC_Small  },
    { "normal"  , kSizeNormal   , kVRef_Object_CorpseNPC_Normal },  
    { "large"   , kSizeLarger   , kVRef_Object_CorpseNPC_Larger },
    { "giant"   , kSizeGiant    , kVRef_Object_CorpseNPC_Giant  },  
    { "titan"   , kSizeTitan    , kVRef_Object_CorpseNPC_Titan  }
    };

    for(SHORT i = 0; i < kSizeMax; ++i)
        Add(sSizeTypes[i]);
}




PosTable::PosTable()
{
    static PosType sPositions[kPosMax] =
    {
    { kPosDead      ,   0 , "dead"  , "dead"            , "DEAD!!"                  },
    { kPosMortal    ,  50 , "mort"  , "mortally wounded", "mortally wounded."       },
    { kPosIncap     ,  50 , "incap" , "incapacitated"   , "incapacitated."          },
    { kPosStunned   ,  50 , "stun"  , "stunned"         , "lying here stunned."     },
    { kPosSleeping  , 200 , "sleep" , "sleeping"        , "sleeping here."          },
    { kPosResting   , 170 , "rest"  , "resting"         , "resting here."           },
    { kPosSitting   , 140 , "sit"   , "sitting"         , "sitting here."           },
    { kPosFighting  , 100 , "fight" , "fighting"        , "here, fighting "         },
    { kPosHunted    , 100 , "hunt"  , "hunted"          , "here, being hunted by "  },
    { kPosStanding  , 100 , "stand" , "standing"        , "standing here."          }
    };

    for(SHORT i = 0; i < kPosMax; ++i)
        Add(sPositions[i]);
}




WearTable::WearTable()
{
    #define lNone kLocNone
    static WearType sWearPlaces[] =
    {   
    //Name              Type                Where This Item Type Can Be Worn
    { "take"        ,   kWearTake           ,{ lNone        , lNone         }},
    { "finger"      ,   kWearFinger         ,{ kLocFingerL  , kLocFingerR   }},
    { "neck"        ,   kWearNeck           ,{ kLocNeck1    , kLocNeck2     }},
    { "body"        ,   kWearBody           ,{ kLocBody     , lNone         }},
    { "inner body"  ,   kWearBodyPad        ,{ kLocInner    , lNone         }},
    { "outer body"  ,   kWearAbout          ,{ kLocOuter    , lNone         }},
    { "shoulders"   ,   kWearShoulders      ,{ kLocShoulders, lNone         }},
    { "plume"       ,   kWearPlume          ,{ kLocPlume    , lNone         }},
    { "helm"        ,   kWearHelm           ,{ kLocHelm     , lNone         }},
    { "cap"         ,   kWearCap            ,{ kLocCap      , lNone         }},
    { "face"        ,   kWearFace           ,{ kLocFace     , lNone         }},
    { "ears"        ,   kWearEars           ,{ kLocEars     , lNone         }},
    { "legs"        ,   kWearLegs           ,{ kLocLegs     , lNone         }},
    { "leg pads"    ,   kWearLegPads        ,{ kLocLegPads  , lNone         }},
    { "feet"        ,   kWearFeet           ,{ kLocFeet     , lNone         }},
    { "socks"       ,   kWearSocks          ,{ kLocSocks    , lNone         }}, 
    { "hands"       ,   kWearHands          ,{ kLocHands    , lNone         }}, 
    { "arms"        ,   kWearArms           ,{ kLocArms     , lNone         }},
    { "arm pads"    ,   kWearArmPads        ,{ kLocArmPads  , lNone         }},
    { "shield"      ,   kWearShield         ,{ kLocShield   , lNone         }},
    { "waist"       ,   kWearWaist          ,{ kLocWaist    , lNone         }},
    { "sheath"      ,   kWearSheath         ,{ kLocSheath   , lNone         }},
    { "wrist"       ,   kWearWrist          ,{ kLocWristL   , kLocWristR    }},
    { "bracer"      ,   kWearBracer         ,{ kLocBracerL  , kLocBracerR   }},
    { "wield"       ,   kWearWield          ,{ kLocWield    , lNone         }},
    { "offhand"     ,   kWearOffhand        ,{ kLocOffhand  , kLocWield     }},
    { "hold"        ,   kWearHold           ,{ kLocHold     , lNone         }},
    { "float"       ,   kWearFloat          ,{ kLocFloat    , lNone         }},
    { "wings"       ,   kWearWings          ,{ kLocWings    , lNone         }},
    { "tail"        ,   kWearTail           ,{ kLocTail     , lNone         }},
    { NULL, 0, { 0, 0 }}
    };

    for(SHORT i = 0; sWearPlaces[i].mName; ++i)
        Add(sWearPlaces[i]);
}

STRINGCW WearTable::ListBitNames(LONG hBits, bool hShowTake)
{
    STRINGCW nFlags;
    
    for(USHORT i = hShowTake ? 0 : 1; i < size(); ++i)
    if(IsSet(hBits, (*this)[i].mWearBit))
    {
        if(!nFlags.Empty())
            nFlags += ", ";
        nFlags += (*this)[i].mName;
    }

    if(nFlags.Empty())
        nFlags = "<none>";
    
    return nFlags;
}


bool WearTable::FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags)
{
    bool nSomeChanged = false;
    STRINGCW nFlagArg;
    USHORT i;

    while(true)
    {
        hArg = hArg.OneArgument(nFlagArg);
        if(nFlagArg.Empty())
            break;
            
        for(i = 0; i < size(); ++i)
        {
            STRINGCW nFlagName = (*this)[i].mName;
            if(nFlagName.IsNamed(nFlagArg))
            {
                ToggleBit(hFlags, (*this)[i].mWearBit);
                nSomeChanged = true;
                break;
            }
        }
        if(i == size() && hCh)
            hCh->SendF("\"%s\" is not a valid flag.\n", *nFlagArg);
    }

    if(!nSomeChanged && hCh)
    {
        STRINGCW nFlagsStr;
        for(i = 0; i < size(); ++i)
        {
            if(!nFlagsStr.Empty())
                nFlagsStr += ", ";
            nFlagsStr += (*this)[i].mName;
        }
        hCh->SendF("Valid flags are: %s.\n", *nFlagsStr); 
    }
    
    return nSomeChanged;
}



WearFormatTable::WearFormatTable()
{
    static WearFormatType sWearSlotFormats[] =
    {
    //Slot Name                 Wear Locations Encompassed By The Slot Name
    { "<worn on finger>    ",   { kLocFingerL   , lNone         , lNone         }},
    { "<worn on finger>    ",   { kLocFingerR   , lNone         , lNone         }},
    { "<worn around neck>  ",   { kLocNeck1     , kLocNeck2     , lNone         }},
    { "<worn on head>      ",   { kLocPlume     , kLocHelm      , kLocCap       }},
    { "<worn on face>      ",   { kLocFace      , lNone         , lNone         }},
    { "<worn on ears>      ",   { kLocEars      , lNone         , lNone         }},
    { "<worn on body>      ",   { kLocInner     , kLocBody      , lNone         }},
    { "<worn about body>   ",   { kLocOuter     , lNone         , lNone         }},
    { "<worn on wings>     ",   { kLocWings     , lNone         , lNone         }},
    { "<worn on tail>      ",   { kLocTail      , lNone         , lNone         }},
    { "<worn on shoulders> ",   { kLocShoulders , lNone         , lNone         }},
    { "<worn on arms>      ",   { kLocArmPads   , kLocArms      , lNone         }},
    { "<worn as shield>    ",   { kLocShield    , lNone         , lNone         }},
    { "<worn on hands>     ",   { kLocHands     , lNone         , lNone         }},
    { "<worn around waist> ",   { kLocWaist     , kLocSheath    , lNone         }},
    { "<worn on legs>      ",   { kLocLegPads   , kLocLegs      , lNone         }},
    { "<worn on feet>      ",   { kLocSocks     , kLocFeet      , lNone         }},
    { "<worn on wrist>     ",   { kLocBracerL   , kLocWristL    , lNone         }},
    { "<worn on wrist>     ",   { kLocBracerR   , kLocWristR    , lNone         }},
    { "<wielded offhand>   ",   { kLocOffhand   , lNone         , lNone         }},
    { "<wielded>           ",   { kLocWield     , lNone         , lNone         }},
    { "<sheathed>          ",   { kLocSheathed  , lNone         , lNone         }},
    { "<held in hand>      ",   { kLocHold      , lNone         , lNone         }},
    { "<held as light>     ",   { kLocLight     , lNone         , lNone         }},
    { "<floating nearby>   ",   { kLocFloat     , lNone         , lNone         }},
    {NULL , {0, 0, 0 }}
    };

    for(SHORT i = 0; sWearSlotFormats[i].mName; ++i)
        Add(sWearSlotFormats[i]);
}



WearMessageTable::WearMessageTable()
{
    static WearMsgType sWearMessages[kLocMax] = 
    {
    { kLocLight     , "You light $p and hold it.\n"                 , "$n lights $p and holds it.\n"            },
    { kLocFingerL   , "You slide $p over your finger.\n"            , "$n slides $p over $s finger.\n"          },
    { kLocFingerR   , "You slide $p over your finger.\n"            , "$n slides $p over $s finger.\n"          },
    { kLocNeck1     , "You wear $p around your neck.\n"             , "$n wears $p around $s neck.\n"           },
    { kLocNeck2     , "You wear $p around your neck.\n"             , "$n wears $p around $s neck.\n"           },
    { kLocPlume     , "You place $p atop your head.\n"              , "$n places $p atop $s head.\n"            },
    { kLocHelm      , "You wear $p on your head.\n"                 , "$n wears $p on $s head.\n"               },
    { kLocCap       , "You wear $p on your head.\n"                 , "$n wears $p on $s head.\n"               },
    { kLocOuter     , "You wear $p about your body.\n"              , "$n wears $p about $s body.\n"            },  
    { kLocBody      , "You wear $p on your body.\n"                 , "$n wears $p on $s body.\n"               },  
    { kLocInner     , "You wear $p on your body.\n"                 , "$n wears $p on $s body.\n"               },
    { kLocLegs      , "You wear $p on your legs.\n"                 , "$n wears $p on $s legs.\n"               },
    { kLocLegPads   , "You wear $p on your legs.\n"                 , "$n wears $p on $s legs.\n"               },  
    { kLocFeet      , "You wear $p on your feet.\n"                 , "$n wears $p on $s feet.\n"               },
    { kLocSocks     , "You wear $p on your feet.\n"                 , "$n wears $p on $s feet.\n"               },
    { kLocHands     , "You wear $p on your hands.\n"                , "$n wears $p on $s hands.\n"              },
    { kLocArms      , "You wear $p on your arms.\n"                 , "$n wears $p on $s arms.\n"               },
    { kLocArmPads   , "You wear $p on your arms.\n"                 , "$n wears $p on $s arms.\n"               },
    { kLocShield    , "You wear $p as your shield.\n"               , "$n wears $p as $s shield.\n"             },
    { kLocWaist     , "You wear $p on your waist.\n"                , "$n wears $p on $s waist.\n"              },
    { kLocBracerL   , "You wear $p on your wrist.\n"                , "$n wears $p on $s wrist.\n"              },
    { kLocWristL    , "You wear $p on your wrist.\n"                , "$n wears $p on $s wrist.\n"              },
    { kLocBracerR   , "You wear $p on your wrist.\n"                , "$n wears $p on $s wrist.\n"              },
    { kLocWristR    , "You wear $p on your wrist.\n"                , "$n wears $p on $s wrist.\n"              },
    { kLocWield     , "You wield $p.\n"                             , "$n wields $p.\n"                         },
    { kLocOffhand   , "You place $p in your offhand.\n"             , "$n places $p in $s offhand.\n"           },
    { kLocHold      , "You hold $p in your hand.\n"                 , "$n holds $p in $s hand.\n"               },
    { kLocFloat     , "You release $p and it floats next to you.\n" , "$n releases $p to float next to $m.\n"   },
    { kLocSheath    , "You wear $p on your waist.\n"                , "$n wears $p on $s waist.\n"              },
    { kLocShoulders , "You sling $p over your shoulders.\n"         , "$n slings $p over $s shoulders\n"        },
    { kLocFace      , "You wear $p on your face.\n"                 , "$n wears $p on $s face.\n"               },
    { kLocEars      , "You wear $p on your ears.\n"                 , "$n wears $p on $s ears.\n"               },
    { kLocWings     , "You wear $p on your wings.\n"                , "$n wears $p on $s wings.\n"              },
    { kLocTail      , "You put $p on your tail.\n"                  , "$n puts $p on $s tail.\n"                },
    { kLocSheathed  , "You sheath your $p.\n"                       , "$n sheaths $s $p.\n"                     }
    };

    for(SHORT i = 0; i < kLocMax; ++i)
        Add(sWearMessages[i]);
}




WeaponTable::WeaponTable()
{
    static WeaponType sWeaponTypes[] =
    {
        { "unarmed" , kWeapUnarmed  , "punch"   , 0 /*ABIL_UNARMED*/},  //0
        { "dagger"  , kWeapDagger   , "stab"    , 0 /*ABIL_DAGGER*/ },  
        { "sword"   , kWeapSword    , "slash"   , 0 /*ABIL_SWORD*/  },
        { "axe"     , kWeapAxe      , "chop"    , 0 /*ABIL_AXE*/    },
        { "polearm" , kWeapPolearm  , "chop"    , 0 /*ABIL_POLEARM*/},  //4
        { "spear"   , kWeapSpear    , "pierce"  , 0 /*ABIL_SPEAR*/  },
        { "staff"   , kWeapStaff    , "swing"   , 0 /*ABIL_STAFF*/  },
        { "mace"    , kWeapMace     , "pound"   , 0 /*ABIL_MACE*/   },
        { "whip"    , kWeapWhip     , "whip"    , 0 /*ABIL_WHIP*/   },
        { "flail"   , kWeapFlail    , "thresh"  , 0 /*ABIL_FLAIL*/  },  //9
        { "bow"     , kWeapBow      , "shot"    , 0 /*ABIL_BOW*/    },
        { "arrow"   , kWeapArrow    , "shot"    , 0 /*ABIL_BOW*/    },
        { "lance"   , kWeapLance    , "joust"   , 0 /*ABIL_LANCE*/  },
        { NULL, 0, NULL, 0 }
    };

    for(SHORT i = 0; sWeaponTypes[i].mName; ++i)
        Add(sWeaponTypes[i]);
}



TerrainTable::TerrainTable()
{
    static TerrainType sTerrainTypes[] =
    {
        { "inside"          , 1     , 0     },
        { "city"            , 2     , 0     },
        { "field"           , 3     , 0     },
        { "forest"          , 4     , 0     },
        { "hills"           , 4     , 0     },
        { "mountain"        , 6     , 0     },
        { "shallow_water"   , 10    , 0     },
        { "deep_water"      , 30    , 0     },
        { "air"             , 5     , 0     },
        { "desert"          , 4     , 0     },
        { "cavern"          , 4     , 0     },
        { NULL, 0, 0 }
    };

    for(SHORT i = 0; sTerrainTypes[i].mName; ++i)
        Add(sTerrainTypes[i]);
}



TriggerTable::TriggerTable()
{
    static TriggerType sTriggerTypes[] =
    {
        //ID Num                  Name        Room    Object      Mobile
        { kScriptTrig_None      , "none"    , true  ,   true    , true  },
        { kScriptTrig_Phrase    , "phrase"  , true  ,   true    , true  },  //Done
        { kScriptTrig_Wear      , "wear"    , false ,   true    , false },
        { kScriptTrig_Remove    , "remove"  , false ,   true    , false },
        { kScriptTrig_Attack    , "attack"  , false ,   true    , false },
        { kScriptTrig_Ask       , "ask"     , false ,   false   , true  },
        { kScriptTrig_Random    , "random"  , true  ,   true    , true  },  
        { kScriptTrig_Enter     , "enter"   , true  ,   true    , true  },  //Done
        { kScriptTrig_Leave     , "leave"   , true  ,   true    , true  },  //Done
        { kScriptTrig_Death     , "death"   , false ,   false   , true  },
        { kScriptTrig_Timer     , "timer"   , false ,   true    , false },  
        { kScriptTrig_Eat       , "eat"     , false ,   true    , false },
        { kScriptTrig_Light     , "light"   , false ,   true    , false },  
        { kScriptTrig_Identify  , "identify", false ,   true    , false },  
        { kScriptTrig_Assist    , "assist"  , false ,   false   , true  },
        { -1, NULL, false, false,  false }
    };
    
    for(SHORT i = 0; sTriggerTypes[i].mName; ++i)
        Add(sTriggerTypes[i]);  
}



StatTable::StatTable()
{
    static BasicShortType sStatTypes[kStatMax] =
    {
    { "strength"        , kStatStr      },
    { "dexterity"       , kStatDex      },
    { "intelligence"    , kStatInt      },
    { "wisdom"          , kStatWis      },
    { "constitution"    , kStatCon      }
    };

    for(SHORT i = 0; i < kStatMax; ++i)
        Add(sStatTypes[i]);
}



ResistTable::ResistTable()
{
    static BasicShortType sResistTypes[kResMax] =
    {
    { "cold"            , kResCold      },
    { "fire"            , kResFire      },
    { "acid"            , kResAcid      },
    { "electrical"      , kResElec      },
    { "poison"          , kResPois      },
    { "mind"            , kResMind      },
    { "sleep"           , kResSlee      },
    { "charm"           , kResChar      }
    };

    for(SHORT i = 0; i < kResMax; ++i)
        Add(sResistTypes[i]);
}


DamageTable::DamageTable()
{
    static DamageType sDamageTypes[] =
    {
    { "Phys"    , kDamPhysical      , "Physical"        , &gPhysTable   },
    { "Cold"    , kDamCold          , "Cold"            , &gColdTable   },
    { "Fire"    , kDamFire          , "Fire"            , &gFireTable   },
    { "Elec"    , kDamElectricity   , "Electricity"     , &gElecTable   },
    { "Acid"    , kDamAcid          , "Acid"            , &gAcidTable   },
    { "Mind"    , kDamMind          , "Mind"            , &gMindTable   },
    {NULL, 0, NULL, NULL}
    };

    for(SHORT i = 0; sDamageTypes[i].mSName; ++i)
        Add(sDamageTypes[i]);
}


DamageMessageTable::DamageMessageTable(SHORT hType)
{
    static DamageMessageType sPhysMsgs[] =
    {
    { 330   , "*** ANNIHILATE ***"  , "*** ANNIHILATES ***" },
    { 265   , "** EXTIRIPATE **"    , "** EXTIRIPATES **"   },
    { 200   , "** ERADICATE **"     , "** ERADICATES **"    },
    { 140   , "** DEMOLISH **"      , "** DEMOLISHES **"    },
    { 100   , "* DISMEMBER *"       , "* DISMEMBERS *"      },
    {  90   , "* MASSACRE *"        , "* MASSACRES *"       },
    {  80   , "* CRIPPLE *"         , "* CRIPPLES *"        },
    {  70   , "DISEMBOWEL"          , "DISEMBOWELS"         },
    {  50   , "PULVERIZE"           , "PULVERIZES"          },
    {  40   , "MUTILATE"            , "MUTILATES"           },
    {  32   , "SAVAGE"              , "SAVAGES"             },
    {  24   , "maim"                , "maims"               },
    {  18   , "devastate"           , "devastates"          },
    {  13   , "decimate"            , "decimates"           },
    {   9   , "maul"                , "mauls"               },
    {   6   , "wound"               , "wounds"              },
    {   4   , "hit"                 , "hits"                },
    {   2   , "scratch"             , "scratches"           },
    {   1   , "graze"               , "grazes"              },
    {-1, NULL, NULL}
    };

    static DamageMessageType sColdMsgs[] =
    {
    { 300   , "*** SHATTER ***"     , "*** SHATTERS ***"    },
    { 230   , "** PETRIFY **"       , "** PETRIFIES **"     },
    { 175   , "** SOLIDIFY **"      , "** SOLIDIFIES **"    },
    { 125   , "* FREEZE *"          , "* FREEZES *"         },
    {  80   , "* ICE *"             , "* ICES *"            },
    {  63   , "CRYSTALLIZE"         , "CRYSTALLIZES"        },
    {  45   , "STIFFEN"             , "STIFFENS"            },
    {  33   , "FROST"               , "FROSTS"              },
    {  21   , "blister"             , "blisters"            },
    {  14   , "numb"                , "numbs"               },
    {   9   , "chill"               , "chills"              },
    {   5   , "shiver"              , "shivers"             },
    {   1   , "cool"                , "cools"               },
    {-1, NULL, NULL}
    };
    
    static DamageMessageType sFireMsgs[] =
    {
    { 300   , "*** DISINTEGRATE ***", "*** DISINTEGRATES ***"},
    { 250   , "** INCINERATE **"    , "** INCINERATES **"   },
    { 195   , "** CREMATE **"       , "** CREMATES **"      },
    { 145   , "** MELT **"          , "** MELTS **"         },
    { 100   , "* IMMOLATE *"        , "* IMMOLATES *"       },
    {  95   , "* IGNITE *"          , "* IGNITES *"         },
    {  75   , "* CHAR *"            , "* CHARS *"           },
    {  60   , "BURN"                , "BURNS"               },
    {  50   , "FRY"                 , "FRIES"               },
    {  40   , "COOK"                , "COOKS"               },
    {  30   , "boil"                , "boils"               },
    {  20   , "scald"               , "scalds"              },
    {  15   , "scorch"              , "scorches"            },
    {  10   , "toast"               , "toasts"              },
    {   5   , "singe"               , "singes"              },
    {   1   , "warm"                , "warms"               },
    {-1, NULL, NULL}
    };  

    //blemish, irritates, burns, MARS, SCARS, * DISFIGURES *
    static DamageMessageType sAcidMsgs[] =
    {
    { 300   , "*** DELIQUESCE ***"  , "*** DELIQUESCES ***" },
    { 255   , "** LIQUEFY **"       , "** LIQUEFIES **"     },
    { 200   , "** DISSOLVE **"      , "** DISSOLVES **"     },
    { 160   , "* CONSUME *"         , "* CONSUMES *"        },
    { 120   , "* DERFORM *"         , "* DEFORMS *"         },
    {  90   , "CORRODE"             , "CORRODES"            },
    {  65   , "WITHER"              , "WITHERS"             },
    {  45   , "ERODE"               , "ERODES"              },
    {  27   , "fester"              , "festers"             },
    {  17   , "blister"             , "blisters"            },
    {  10   , "sizzle"              , "sizzles"             },
    {   1   , "sting"               , "stings"              },
    {-1, NULL, NULL}
    };

    // ** atomizes ** * ionizes *
    static DamageMessageType sElecMsgs[] =
    {
    { 300   , "*** VAPORIZE ***"    , "*** VAPORIZES ***"   },
    { 265   , "** ATOMIZE **"       , "** ATOMIZES **"      },
    { 210   , "** ELECTROCUTE **"   , "** ELECTROCUTES **"  },
    { 165   , "** INCANDESCE **"    , "** INCANDESCES **"   },
    { 120   , "* ELECTRIFY *"       , "* ELECTRIFIES *"     },
    {  80   , "* FLARE *"           , "* FLARES *"          },
    {  55   , "SHOCK"               , "SHOCKS"              },
    {  35   , "JOLT"                , "JOLTS"               },
    {  25   , "charge"              , "charges"             },
    {  18   , "zap"                 , "zaps"                },
    {   5   , "spark"               , "sparks"              },
    {   1   , "tickle"              , "tickles"             },
    {-1, NULL, NULL}
    };

    //horrifies
    static DamageMessageType sMindMsgs[] =
    {
    { 300   , "*** HEMORRHAGE ***"  , "*** HEMORRHAGES ***" },
    { 230   , "** ANGUISH **"       , "** ANGUISHES **"     },
    { 170   , "** AGONIZE **"       , "** AGONIZES **"      },
    { 120   , "* TERRORIZE *"       , "* TERRORIZES *"      },
    {  80   , "* TORMENT *"         , "* TORMENTS *"        },
    {  55   , "SEIZURE"             , "SEIZURES"            },
    {  35   , "SPASM"               , "SPASMS"              },
    {  24   , "convulse"            , "convulses"           },
    {  18   , "stagger"             , "staggers"            },
    {  13   , "twitch"              , "twitches"            },
    {   5   , "twinge"              , "twinges"             },
    {   1   , "tingle"              , "tingles"             },
    {-1, NULL, NULL}
    };




    DamageMessageType* sMessages = NULL;
    switch(hType)
    {
        case kDamPhysical: sMessages = sPhysMsgs; break;
        case kDamCold: sMessages = sColdMsgs; break;
        case kDamFire: sMessages = sFireMsgs; break;
        case kDamAcid: sMessages = sAcidMsgs; break;
        case kDamElectricity: sMessages = sElecMsgs; break;
        case kDamMind: sMessages = sMindMsgs; break;
        default: ASSERT(false); return;
    }
    
    for(SHORT i = 0; sMessages[i].mMsg; ++i)
        Add(sMessages[i]);
}




MaterialTable::MaterialTable()
{
    MaterialType sMaterialTypes[] = 
    {
        /*   Name       Durablty Casting? Bitname         */
        { "bronze"      , 15    , true  , kMatBronze        },  /* Metals */
        { "iron"        , 25    , true  , kMatIron          },
        { "steel"       , 40    , true  , kMatSteel         },
        { "electrum"    , 50    , false , kMatElectrum      },
        { "mithril"     , 80    , true  , kMatMithril       },
        { "adamantite"  , 90    , true  , kMatAdamantite    },
        { "venomite"    , 80    , true  , kMatVenomite      },
        { "copper"      , 7     , false , kMatCopper        },
        { "silver"      , 10    , false , kMatSilver        },
        { "gold"        , 7     , false , kMatGold          },
        { "platinum"    , 5     , true  , kMatPlatinum      },  
        { "leather"     , 3     , false , kMatLeather       },  /* Others */
        { "cloth"       , 3     , false , kMatCloth         },
        { "wood"        , 4     , false , kMatWood          },
        { "crystal"     , 10    , false , kMatCrystal       },
        { "gemstone"    , 10    , false , kMatGemstone      },
        { "stone"       , 20    , false , kMatStone         },
        { "bone"        , 15    , false , kMatBone          },
        { "organic"     , 10    , false , kMatOrganic       },
        { "glass"       , 1     , false , kMatGlass         },
        { "energy"      , 99    , false , kMatEnergy        },
        { "unknown"     , 35    , false , kMatUnknown       },
        { NULL, 0, false, 0 }
    };


    for(SHORT i = 0; sMaterialTypes[i].mName; ++i)
        Add(sMaterialTypes[i]);
}




bool MaterialTable::FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags)
{
    bool nSomeChanged = false;
    STRINGCW nFlagArg;
    USHORT i;

    while(true)
    {
        hArg = hArg.OneArgument(nFlagArg);
        if(nFlagArg.Empty())
            break;
            
        for(i = 0; i < size(); ++i)
        {
            STRINGCW nFlagName = (*this)[i].mName;
            if(nFlagName.IsNamed(nFlagArg))
            {
                ToggleBit(hFlags, (*this)[i].mBit);
                nSomeChanged = true;
                break;
            }
        }
        if(i == size() && hCh)
            hCh->SendF("\"%s\" is not a valid flag.\n", *nFlagArg);
    }

    if(!nSomeChanged && hCh)
    {
        STRINGCW nFlagsStr;
        for(i = 0; i < size(); ++i)
        {
            if(!nFlagsStr.Empty())
                nFlagsStr += ", ";
            nFlagsStr += (*this)[i].mName;
        }
        hCh->SendF("Valid flags are: %s.\n", *nFlagsStr); 
    }
    
    return nSomeChanged;
}




STRINGCW MaterialTable::ListBitNames(LONG hBits, Person* hTo)
{
    STRINGCW nFlags;
    
    for(USHORT i = 0; i < size(); ++i)
    if(IsSet(hBits, (*this)[i].mBit))
    {
        if(!nFlags.Empty())
            nFlags += ", ";
        nFlags += (*this)[i].mName;
    }

    if(nFlags.Empty())
        nFlags = "<none>";
    
    return nFlags;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




SHORT FlagTable::FindNamed(const CHAR* hName, Person* hCh)
{
    for(USHORT i = 0; i < size(); ++i)
    if(hCh && hCh->GetTrust() >= (*this)[i].mTrust)
    if(STRINGCW((*this)[i].mName).IsNamed(hName))
        return i;

    return -1;
}




STRINGCW FlagTable::ListBitNames(LONG hBits, Person* hCh)
{
    STRINGCW nFlags;
    
    for(USHORT i = 0; i < size(); ++i)
    //if(!hCh || hCh->GetTrust() >= (*this)[i].mTrust)
    if(IsSet(hBits, (*this)[i].mBit))
    {
        if(!nFlags.Empty())
            nFlags += ", ";
        nFlags += (*this)[i].mName;
    }
    
    if(nFlags.Empty())
        nFlags = "<none>";

    return nFlags;
}




bool FlagTable::FlagSet(Person* hCh, STRINGCW& hArg, LONG& hFlags)
{
    bool nSomeChanged = false;
    STRINGCW nFlagArg;
    USHORT i;

    while(true)
    {
        hArg = hArg.OneArgument(nFlagArg);
        if(nFlagArg.Empty())
            break;
            
        for(i = 0; i < size(); ++i)
        {
            if((*this)[i].mSettable)
            {
                STRINGCW nFlagName = (*this)[i].mName;
                if(nFlagName.IsNamed(nFlagArg))
                {
                    if(hCh && hCh->GetTrust() < (*this)[i].mTrust)
                    {
                        hCh->SendF("You don't have enough power to set the %s flag.\n", *nFlagName);
                        break;
                    }
                    
                    ToggleBit(hFlags, (*this)[i].mBit);
                    nSomeChanged = true;
                    break;
                }
            }
        }
        if(i == size() && hCh)
            hCh->SendF("\"%s\" is not a valid flag.\n", *nFlagArg);
    }

    if(!nSomeChanged && hCh)
    {
        STRINGCW nFlagsStr;
        for(i = 0; i < size(); ++i)
        if((*this)[i].mSettable)
        {
            if(!nFlagsStr.Empty())
                nFlagsStr += ", ";
            nFlagsStr += (*this)[i].mName;
        }
    
        hCh->SendF("Valid flags are: %s.\n", *nFlagsStr); 
    }
    
    return nSomeChanged;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




OptionFlags::OptionFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "AutoAssist"      , kOptAutoAssist    , 0     , true      },
    { "AutoLoot"        , kOptAutoLoot      , 0     , true      },
    { "AutoGold"        , kOptAutoGold      , 0     , true      },
    { "AutoSplit"       , kOptAutoSplit     , 0     , true      },
    { "AutoSkin"        , kOptAutoSkin      , 0     , true      },
    { "Color"           , kOptColor         , 0     , true      },
    { "AutoExits"       , kOptAutoExits     , 0     , true      },
    { "NoSummon"        , kOptNoSummon      , 0     , true      },
    { "Brief"           , kOptBrief         , 0     , true      },
    { "Prompt"          , kOptPrompt        , 0     , true      },
    { "Movement"        , kOptMovement      , 0     , true      },
    { "DamBySelf"       , kOptDamBySelf     , 0     , true      },
    { "DamByGroup"      , kOptDamByGroup    , 0     , true      },
    { "DamToSelf"       , kOptDamToSelf     , 0     , true      },
    { "DamToGroup"      , kOptDamToGroup    , 0     , true      },
    { "Incognito"       , kOptIncognito     , 0     , true      },
    { "Misses"          , kOptMisses        , 0     , true      },
    { "AutoScan"        , kOptAutoScan      , 0     , true      },
    { "Repeat"          , kOptRepeat        , 0     , true      },
    { "Info"            , kOptInfo          , 0     , true      },
    { "ImmPowers"       , kOptImmPowers     , AVT   , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




InfoFlags::InfoFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "Connect"         , kInfoConnect          , 0     , true      },
    { "Quit"            , kInfoQuit             , 0     , true      },
    { "Links"           , kInfoLinks            , 0     , true      },
    { "Levels"          , kInfoLevels           , 0     , true      },
    { "Deaths"          , kInfoDeaths           , 0     , true      },
    { "Notes"           , kInfoNotes            , 0     , true      },
    { "Chat"            , kInfoChannelChat      , 0     , true      },
    { "Gossip"          , kInfoChannelGossip    , 0     , true      },
    { "OOC"             , kInfoChannelOOC       , 0     , true      },
    { "Tell"            , kInfoChannelTell      , 0     , true      },
    { "Clan"            , kInfoChannelClan      , 0     , true      },
    { "Auction"         , kInfoChannelAuction   , 0     , true      },
    { "Immtalk"         , kInfoChannelImm       , AVT   , true      },
    { "Immortal"        , kInfoImm              , AVT   , true      },
    { "Repops"          , kInfoRepops           , AVT   , true      },
    { "Rewards"         , kInfoRewards          , AVT   , true      },
    { "Creates"         , kInfoCreate           , AVT   , true      },
    { "Punishments"     , kInfoPunish           , AVT   , true      },
    { "Debug"           , kInfoDebug            , AVT   , true      },
    { "OLC"             , kInfoOLC              , AVT   , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




RoomFlags::RoomFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "Dark"            , kRoomDark         , 0     , true      },
    { "Light"           , kRoomLight        , 0     , true      },
    { "Indoors"         , kRoomIndoors      , 0     , true      },
    { "Safe"            , kRoomSafe         , 0     , true      },
    { "Law"             , kRoomLaw          , 0     , true      },
    { "NoMob"           , kRoomNoMob        , 0     , true      },
    { "NoRecall"        , kRoomNoRecall     , 0     , true      },
    { "NoSpell"         , kRoomNoSpell      , 0     , true      },
    { "NoSound"         , kRoomNoSound      , 0     , true      },
    { "Bank"            , kRoomBank         , 0     , true      },
    { "Currency"        , kRoomCurrency     , 0     , true      },
    { "Underwater"      , kRoomUnderwater   , 0     , true      },
    { "SaveItems"       , kRoomSaveItems    , 0     , true      },
    { "Punish"          , kRoomPunish       , 0     , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




ObjectFlags::ObjectFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "Container"       , kObjectIsContainer, 0     , true      },
    { "Cursed"          , kObjectCursed     , 0     , true      },
    { "NoDrop"          , kObjectNoDrop     , 0     , true      },
    { "NoLiq"           , kObjectNoLiq      , 0     , true      },
    { "Enchanted"       , kObjectEnchanted  , 0     , true      },
    { "TwoHandled"      , kObjectTwoHandled , 0     , true      },
    { "Throwable"       , kObjectThrowable  , 0     , true      },
    { "Sharp"           , kObjectSharp      , 0     , true      },
    { "Bladed"          , kObjectBladed     , 0     , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




MobileFlags::MobileFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "npc"             , kMobNPC           , 0     , false     },
    { "sentinal"        , kMobSentinal      , 0     , true      },
    { "scavenger"       , kMobScavenger     , 0     , true      },
    { "sentient"        , kMobSentient      , 0     , true      },
    { "unnatural"       , kMobUnnatural     , 0     , true      },
    { "aggressive"      , kMobAggressive    , 0     , true      },
    { "stayArea"        , kMobStayArea      , 0     , true      },
    { "wimpy"           , kMobWimpy         , 0     , true      },
    { "pet"             , kMobPet           , 0     , false     },
    { "train"           , kMobTrain         , 0     , true      },
    { "practice"        , kMobPractice      , 0     , true      },
    { "undead"          , kMobUndead        , 0     , true      },
    { "ghost"           , kMobGhost         , 0     , true      },
    { "shopkeeper"      , kMobShopkeeper    , 0     , true      },
    { "nightOnly"       , kMobNightOnly     , 0     , true      },
    { "dayOnly"         , kMobDayOnly       , 0     , true      },
    { "noKill"          , kMobNoKill        , 0     , true      },
    { "noPurge"         , kMobNoPurge       , 0     , true      },
    { "outdoors"        , kMobOutdoors      , 0     , true      },
    { "indoors"         , kMobIndoors       , 0     , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




ContFlags::ContFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "Closeable"       , kContCloseable    , 0     , true      },
    { "Lockable"        , kContLockable     , 0     , true      },
    { "Pickable"        , kContPickable     , 0     , true      },
    { "Closed"          , kContClosed       , 0     , true      },
    { "Locked"          , kContLocked       , 0     , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




FurnFlags::FurnFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "StandAt"         , kStandAt          , 0     , true      },
    { "StandOn"         , kStandOn          , 0     , true      },
    { "StandIn"         , kStandIn          , 0     , true      },
    { "SitAt"           , kSitAt            , 0     , true      },
    { "SitOn"           , kSitOn            , 0     , true      },
    { "SitIn"           , kSitIn            , 0     , true      },
    { "RestAt"          , kRestAt           , 0     , true      },
    { "RestOn"          , kRestOn           , 0     , true      },
    { "RestIn"          , kRestIn           , 0     , true      },
    { "SleepAt"         , kSleepAt          , 0     , true      },
    { "SleepOn"         , kSleepOn          , 0     , true      },
    { "SleepIn"         , kSleepIn          , 0     , true      },
    { "PutAt"           , kPutAt            , 0     , true      },
    { "PutOn"           , kPutOn            , 0     , true      },
    { "PutIn"           , kPutIn            , 0     , true      },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




SpotFlags::SpotFlags()
{
    FlagType sFlagTypes[] =
    {   
    //Name                FlagBit             Trust   Settable?
    { "hidable"         , kSpotHidable      , 0     , true  },
    { "window"          , kSpotWindow       , 0     , true  },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}




ExitFlags::ExitFlags()
{
    FlagType sFlagTypes[] =
    {
    //Name            FlagBit             Trust   Settable?
    { "door"        , kExIsDoor         , 0     , true  },
    { "closed"      , kExClosed         , 0     , false },
    { "locked"      , kExLocked         , 0     , false },
    { "hidden"      , kExHidden         , 0     , false },
    { "nowander"    , kExNoWander       , 0     , true  },
    { "unused1"     , kExUnused1        , 0     , false },
    { "noclose"     , kExNoClose        , 0     , true  },
    { "lockbroken"  , kExLockBroken     , 0     , true  },
    { "nosound"     , kExNoSound        , 0     , true  },
    { "nopass"      , kExNoPass         , 0     , true  },
    { "nopick"      , kExNoPick         , 0     , true  },
    { "easy"        , kExEasy           , 0     , true  },
    { "hard"        , kExHard           , 0     , true  },
    { "infuriating" , kExInfuriating    , 0     , true  },
    { "plural"      , kExPlural         , 0     , true  },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}



AttackFlags::AttackFlags()
{
    FlagType sFlagTypes[] =
    {
    //Name            FlagBit             Trust   Settable?
    { "plural"      , kAttPlural        , 0     , true  },
    { "justmsg"     , kAttJustMsg       , 0     , true  },
    { "nopart"      , kAttNoPart        , 0     , true  },
    { NULL, 0, 0, true }
    };

    for(SHORT i = 0; sFlagTypes[i].mName; ++i)
        Add(sFlagTypes[i]);
}





