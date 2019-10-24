/******************************************************************************
  Author: Matthew Nolan                                      OrangeMUD Codebase
    Date: April 2001                                            [Crossplatform]
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
#include "Act.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "Object.h"
#include "ObjectIndex.h"
#include "OrangeScript.h"
#include "OrangeScriptBinary.h"
#include "OrangeScriptDataType.h"
#include "OrangeScriptLibrary.h"
#include "OrangeScriptVariable.h"
#include "Person.h"
#include "RoomIndex.h"


#define SCRIPTING_ASSERT(x)     do{if(!(x)){printf("Scripting Assertion ("#x") failed\n");return NULL;}}while(0)

//## Macros For Easy Function Setting ##//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define Arg0                                            0,{xx,xx,xx,xx,xx,xx,xx},{NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#define Neg1(t1,n1)                                    -1,{t1,xx,xx,xx,xx,xx,xx},{n1,NULL,NULL,NULL,NULL,NULL,NULL}
#define Arg1(t1,n1)                                     1,{t1,xx,xx,xx,xx,xx,xx},{n1,NULL,NULL,NULL,NULL,NULL,NULL}
#define Arg2(t1,n1,t2,n2)                               2,{t1,t2,xx,xx,xx,xx,xx},{n1,n2,NULL,NULL,NULL,NULL,NULL}
#define Arg3(t1,n1,t2,n2,t3,n3)                         3,{t1,t2,t3,xx,xx,xx,xx},{n1,n2,n3,NULL,NULL,NULL,NULL}
#define Arg4(t1,n1,t2,n2,t3,n3,t4,n4)                   4,{t1,t2,t3,t4,xx,xx,xx},{n1,n2,n3,n4,NULL,NULL,NULL}
#define Arg5(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5)             5,{t1,t2,t3,t4,t5,xx,xx},{n1,n2,n3,n4,n5,NULL,NULL}
#define Arg6(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6)       6,{t1,t2,t3,t4,t5,t6,xx},{n1,n2,n3,n4,n5,n6,NULL}
#define Arg7(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6,t7,n7) 7,{t1,t2,t3,t4,t5,t6,t7},{n1,n2,n3,n4,n5,n6,n7}

#define OF(xName,xFunc,xOpLev,xRet,xARGSx)  {xName,xFunc,xOpLev,xRet,xARGSx,NULL}
#define ScriptFn(xName,xRet,xARGSx)         {#xName,xName,0,xRet,xARGSx
#define ScriptF2(xFunc,xName,xRet,xARGSx)   {xName,xFunc,0,xRet,xARGSx
#define DT(xName,xType,xCreate)             {xName,xType,xCreate
#define CT(xName,xType,xDef,xValue)         {xName,xType, new ScriptVariable(xDef,(void*)xValue, true)}
#define NoHelp                              ,NULL}
#define WithHelp(xHelp)                     ,xHelp}
#define EmptyOperatorDecl                   {NULL,NULL,0,0,Arg0, NULL}
#define EmptyFunctionDecl                   {NULL,NULL,0,0,Arg0, NULL}
#define EmptyDataTypeDecl                   {NULL,0,NULL, NULL}
#define EmptyConstantDecl                   CT(NULL,0,gDTDef_None,NULL)

#define xx kScriptDT_None
#define fu kScriptDT_None
#define st kScriptDT_STRING
#define in kScriptDT_INT
#define ch kScriptDT_PERSON
#define ob kScriptDT_OBJECT
#define ro kScriptDT_ROOM
#define di kScriptDT_DIR
#define bo kScriptDT_BOOL
#define af kScriptDT_AFFECT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
{ "carrying"     ,  &s_carrying,     0, ch, Arg1(ob,"item")                                 , "Returns the character who is carrying the item. Returns NULL if noone is carrying it, it's in the room." },
{ "class"        ,  &s_class,        0, in, Arg1(ch,"character")                            , "Returns the character's class. -1 if it's a mobile." },
{ "close"        ,  &s_close,        0, xx, Arg2(ro,"room", di,"direction")                 , "From room it will close the exit in the direction you specify. It closes both sides." },
{ "closed"       ,  &s_closed,       0, bo, Arg2(ro,"room", di,"direction")                 , "From room it will tell you if the exit in direction is closed. Returns TRUE if closed, FALSE if open." },
{ "create_mob"   ,  &s_create_mob,   0, ch, Arg2(in,"vnum", ro,"in_room")                   , "Create a mobile from the vnum, it will appear in the in_room. It also returns the mobile if you need it." },
{ "create_obj"   ,  &s_create_obj,   0, ob, Arg1(in,"vnum")                                 , "Create an object using its virtual number. It returns the object for use in obj_to_char or obj_to_room." },

{ "damage"       ,  &s_damage   ,    0, bo, Arg5(ch,"victim", in,"dam_type", in,"damage", st,"message", in,"flags") , "Damages the victim. Returns true if it killed them." },

{ "eat"          ,  &s_eat,          0, xx, Arg2(ch,"character", ob,"food")                 , "Will make the character attempt to eat the food object. Bypasses any eat triggers." },
{ "energy"       ,  &s_energy,       0, in, Arg1(ch,"character")                            , "Returns the character's current energy points." },
{ "engage"       ,  &s_engage,       0, bo, Arg3(ch,"character", ch,"victim", bo,"attack")  , "Make character begin fighting victim, shows engage message. Attacks are done if attack is true. Returns false if character couldn't engage victim." },
{ "has_obj"      ,  &s_has_obj,      0, ob, Arg2(ch,"character", in,"vnum")                 , "Does character have an object with the vnum in their inventory. Returns NULL if they don't have it, else it returns the object." },
{ "hits"         ,  &s_hits,         0, in, Arg1(ch,"character")                            , "Returns the character's current hit points." },
{ "junk_obj"     ,  &s_junk_obj,     0, xx, Arg1(ob,"object")                               , "Will junk the object, removing it from play. Calling this on the object of an oscript will terminate the script immediately after." },

{ "level"        ,  &s_level,        0, in, Arg1(ch,"character")                            , "Returns the character's level." },
{ "lock"         ,  &s_lock,         0, xx, Arg2(ro,"room", di,"direction")                 , "From room it will lock the exit in the direction you specify. It locks both sides." },
{ "locked"       ,  &s_locked,       0, bo, Arg2(ro,"room", di,"direction")                 , "From room it will tell you if the exit in direction is locked. Returns TRUE if locked, FALSE if unlocked." },
{ "moves"        ,  &s_moves,        0, in, Arg1(ch,"character")                            , "Returns the character's current move points." },
{ "obj_in_room"  ,  &s_obj_in_room,  0, ob, Arg2(ro,"room", in,"vnum")                      , "Is there an object with the vnum in the room. Returns NULL if there isn't, else it returns the object." },
{ "obj_to_char"  ,  &s_obj_to_char,  0, xx, Arg2(ob,"object", ch,"character")               , "Place the given object into the characters inventory." },
{ "obj_to_obj"   ,  &s_obj_to_obj,   0, xx, Arg2(ob,"object", ob,"container")               , "Place the given object into the container object." },
{ "obj_to_room"  ,  &s_obj_to_room,  0, xx, Arg2(ob,"object", ro,"to_room")                 , "Place the given object into the room contents." },
{ "open"         ,  &s_open,         0, xx, Arg2(ro,"room", di,"direction")                 , "From room it will open the exit in the direction you specify. It opens both sides." },

{ "race"        ,   &s_race,         0, in, Arg1(ch,"character")                            , "Returns the character's race. -1 if it's a mobile." },
{ "remove"      ,   &s_remove,       0, xx, Arg2(ob,"item", bo,"show")                      , "This will make the character remove the item. Show determines if it shows the standard item removal messages." },
{ "retaliate"   ,   &s_retaliate,    0, xx, Arg2(ch,"character", ch,"victim")               , "Make victim react to an attack from character. Not needed if character already engaged victim." },

{ "sprintf"     ,   &s_sprintf,      0, st, Arg4(st,"format", xx,"arg", xx,"arg", xx, "arg"), "Standard sprintf function. %d to insert a number %s for a string." },

{ "unlock"      ,   &s_unlock,       0, xx, Arg2(ro,"room", di,"direction")                 , "From room it will unlock the exit in the direction you specify. It unlocks both sides." },
{ "vnum"        ,   &s_vnum,         0, in, Arg1(ch,"character")                            , "Returns the mobile's vnum. -1 if it's a player." },

{ "wear"        ,   &s_wear,         0, xx, Arg2(ob,"item" , bo,"show")                     , "This will make the character holding the item wear it. Show determines if it shows the standard wear messages." },
{ "wearing_obj" ,   &s_wearing_obj,  0, ob, Arg2(ch,"character", in,"vnum") 
*/

ScriptingFunc   act_to_char;
ScriptingFunc   act_to_victim;
ScriptingFunc   act_to_rest;
ScriptingFunc   act_to_room;
ScriptingFunc   act_to_all;
ScriptingFunc   act_to_victim;
ScriptingFunc   attack;
ScriptingFunc   carrying;
ScriptingFunc   class_;
ScriptingFunc   close;
ScriptingFunc   closed;
ScriptingFunc   create_mob;
ScriptingFunc   create_obj;
ScriptingFunc   get_room;
ScriptingFunc   interpret;
ScriptingFunc   mob_in_room;
ScriptingFunc   open;
ScriptingFunc   random;
ScriptingFunc   run_mscript;
ScriptingFunc   run_oscript;
ScriptingFunc   run_rscript;
ScriptingFunc   send_char;
ScriptingFunc   send_room;
ScriptingFunc   transfer;
ScriptingFunc   wait;

//** OrangeMUD Function Table **//
ScriptFuncDef gOrangeFunctionTable[] = 
{
    ScriptFn(act_to_char    ,   xx, Arg4(st,"text", ch,"character", ob,"object", ch,"victim"))
    WithHelp("Send ACT text to the character ($n). Optional object ($p) and victim ($N) may be used as well."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(act_to_victim  ,   xx, Arg4(st,"text", ch,"character", ob,"object", ch,"victim"))
    WithHelp("Send ACT text to the victim. Character ($n) and victim ($N) are needed but the object ($p) is optional."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(act_to_rest    ,   xx, Arg4(st,"text", ch,"character", ob,"object", ch,"victim"))
    WithHelp("Send ACT text to everyone in the room besides the character ($n) and the victim ($N). Only object ($p) is optional."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(act_to_room    ,   xx, Arg4(st,"text", ch,"character", ob,"object", ch,"victim"))
    WithHelp("Send ACT text to everyone but the character ($n) in the room. Optional victim ($N) and object ($p) as well."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(act_to_all     ,   xx, Arg4(st,"text", ch,"character", ob,"object", ch,"victim"))
    WithHelp("Send ACT text to everyone in the character's ($n) room. Optional object ($p) and victim ($N) may be used as well."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(attack         ,   bo, Arg7(ch,"from", ch,"to", in,"hitroll", in,"dam_type", in,"damage", st,"message", in,"flags"))
    WithHelp("From attacks to with hitroll and damage. Returns true if victim died. Doesn't set them fighting, use engage for that."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(carrying       ,   ch, Arg1(ob,"item"))
    WithHelp("Returns the character who is carrying the item. Returns NULL if noone is carrying it, it's in the room."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptF2(class_, "class",   in, Arg1(ch,"character"))
    WithHelp("Returns the character's class. -1 if it's a mobile."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(close          ,   xx, Arg2(ro,"room", di,"direction"))
    WithHelp("From room it will close the exit in the direction you specify. It closes both sides."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(closed         ,   bo, Arg2(ro,"room", di,"direction"))
    WithHelp("From room it will tell you if the exit in direction is closed. Returns TRUE if closed, FALSE if open."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(create_mob     ,   ch, Arg2(st,"vref", ro,"in_room"))
    WithHelp("Create a mobile from the area/vnum, it will appear in the in_room. It also returns the mobile if you need it."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(create_obj     ,   ob, Arg1(st,"vref"))
    WithHelp("Create an object from the area/vnum. It returns the object for use in obj_to_char or obj_to_room."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(get_room       ,   ro, Arg1(st,"vref"))
    WithHelp("Will return the room with the area/vnum you specify, if it exists."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(interpret      ,   xx, Arg3(ch,"character", st,"command", ch,"victim"))
    WithHelp("Do a command as the character, victim (optional) may be NULL. Use %s in the command to specify the victim's name."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(mob_in_room    ,   ch, Arg2(ro,"room", st,"vref"))
    WithHelp("Is there a mobile (by vref) in the room. Returns NULL if there isn't, otherwise it returns the mobile."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(open           ,   xx, Arg2(ro,"room", di,"direction"))
    WithHelp("From room it will open the exit in the direction you specify. It opens both sides."),
    //-----------------------------------------------------------------------------------------------------------------------------//

    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(random         ,   in, Arg2(in,"minimum", in,"maximum"))
    WithHelp("Will return a random number anywhere between and including minimum and maximum."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(run_mscript    ,   xx, Arg2(ch,"mobile", in,"script_id"))
    WithHelp("Run a mobile script, it uses the existing ch, room, obj, mob, victim from the calling script."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(run_oscript    ,   xx, Arg2(ob,"item", in,"script_id"))
    WithHelp("Run an object script, it uses the existing ch, room, obj, mob, victim from the calling script."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(run_rscript    ,   xx, Arg2(ro,"room", in,"script_id"))
    WithHelp("Run a room script, it uses the existing ch, room, obj, mob, victim from the calling script."),
    //-----------------------------------------------------------------------------------------------------------------------------//

    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(send_char      ,   xx, Arg2(st,"text", ch,"to_char"))
    WithHelp("Send text to a character, if you need it Act(ion) parsed use act_to_char."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(send_room      ,   xx,  Arg2(st,"text", ro,"to_room"))
    WithHelp("Send text to everyone in the room you specify."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(transfer       ,   xx,  Arg3(ch,"character", ro,"to_room", bo,"look"))
    WithHelp("Transfer the character to the to_room, they will \"look\" at the new room if look is TRUE."),
    //-----------------------------------------------------------------------------------------------------------------------------//

    //-----------------------------------------------------------------------------------------------------------------------------//
    ScriptFn(wait           ,   xx,  Arg1(in,"duration"))
    WithHelp("Pause the script for 3 seconds * duration. Wait(2); would wait 6 seconds, and so on."),
    //-----------------------------------------------------------------------------------------------------------------------------//
    
    EmptyFunctionDecl
};



//** OrangeMUD DataType Table **//
ScriptDataTypeDef gOrangeDataTypeTable[] = 
{
    DT("PERSON", kScriptDT_PERSON, CreateDT_PERSON)
    WithHelp("A player or mobile, any kind of character in the game."),

    DT("OBJECT", kScriptDT_OBJECT, CreateDT_OBJECT)
    WithHelp("An object variable."),

    DT("ROOM", kScriptDT_ROOM, CreateDT_ROOM)
    WithHelp("A room variable."),

    DT("DIR", kScriptDT_DIR, CreateDT_DIR)
    WithHelp("A direction. Has constant values like DIR_NORTH, etc."),

    DT("AFFECT", kScriptDT_AFFECT, CreateDT_AFFECT)
    WithHelp("A spell or skill affect created using affect() function."),

    EmptyDataTypeDecl
};
ScriptDataTypeDef* gDTDef_PERSON    = &gOrangeDataTypeTable[0];
ScriptDataTypeDef* gDTDef_OBJECT    = &gOrangeDataTypeTable[1];
ScriptDataTypeDef* gDTDef_ROOM      = &gOrangeDataTypeTable[2];
ScriptDataTypeDef* gDTDef_DIR       = &gOrangeDataTypeTable[3];
ScriptDataTypeDef* gDTDef_AFFECT    = &gOrangeDataTypeTable[4];



//** OrangeMUD Constant Tables Index **//
ScriptConstIndex gOrangeConstantTables[] =
{
    { "Base"    , "Basic constants for coding."             , gBaseConstantTable    },
    { "General" , "General constants for an OrangeMUD."     , gOrangeConstantTable  },
    { "Affect"  , "Affect constants for specifying affects.", gAffectConstantTable  },
    { "Race"    , "Racial constants for specifying a race." , gRaceConstantTable    },
    { "Class"   , "Class constants for specifying a class." , gClassConstantTable   },
    { "Attack"  , "Damaging and attacking constants."       , gAttackConstantTable  },
    {NULL, NULL, NULL}
};



//** OrangeMUD Constant Tables **//
ScriptConstDef gOrangeConstantTable[] = 
{
    CT("DIR_NORTH"      , kScriptDT_DIR , gDTDef_DIR    , kDirNorth ),
    CT("DIR_EAST"       , kScriptDT_DIR , gDTDef_DIR    , kDirEast  ),
    CT("DIR_SOUTH"      , kScriptDT_DIR , gDTDef_DIR    , kDirSouth ),
    CT("DIR_WEST"       , kScriptDT_DIR , gDTDef_DIR    , kDirWest  ),
    CT("DIR_UP"         , kScriptDT_DIR , gDTDef_DIR    , kDirUp    ),
    CT("DIR_DOWN"       , kScriptDT_DIR , gDTDef_DIR    , kDirDown  ),
    EmptyConstantDecl
};

ScriptConstDef gAffectConstantTable[] = 
{
    EmptyConstantDecl
};

ScriptConstDef gRaceConstantTable[] = 
{
    CT("WARRIOR"        , kScriptDT_INT , gDTDef_INT    , kClassWarrior     ),
    CT("PALADIN"        , kScriptDT_INT , gDTDef_INT    , kClassPaladin     ),
    CT("DEMONKNIGHT"    , kScriptDT_INT , gDTDef_INT    , kClassDemonknight ),
    CT("RANGER"         , kScriptDT_INT , gDTDef_INT    , kClassRanger      ),
    CT("ARCHER"         , kScriptDT_INT , gDTDef_INT    , kClassArcher      ),
    CT("ROGUE"          , kScriptDT_INT , gDTDef_INT    , kClassRogue       ),
    CT("NINJA"          , kScriptDT_INT , gDTDef_INT    , kClassNinja       ),
    CT("PRIEST"         , kScriptDT_INT , gDTDef_INT    , kClassPriest      ),
    CT("SHAMAN"         , kScriptDT_INT , gDTDef_INT    , kClassShaman      ),
    CT("DRUID"          , kScriptDT_INT , gDTDef_INT    , kClassDruid       ),
    CT("MAGICIAN"       , kScriptDT_INT , gDTDef_INT    , kClassMagician    ),
    CT("ELEMENTALIST"   , kScriptDT_INT , gDTDef_INT    , kClassElementalist),
    CT("NECROMANCER"    , kScriptDT_INT , gDTDef_INT    , kClassNecromancer ),
    CT("PSION"          , kScriptDT_INT , gDTDef_INT    , kClassPsion       ),
    CT("BARD"           , kScriptDT_INT , gDTDef_INT    , kClassBard        ),
    EmptyConstantDecl
};

ScriptConstDef gClassConstantTable[] = 
{
    CT("HUMAN"          , kScriptDT_INT , gDTDef_INT    , kRaceHuman    ),
    CT("AVARIEL"        , kScriptDT_INT , gDTDef_INT    , kRaceAvariel  ),
    CT("DWARF"          , kScriptDT_INT , gDTDef_INT    , kRaceDwarf    ),
    CT("ELF"            , kScriptDT_INT , gDTDef_INT    , kRaceElf      ),
    CT("HOBBIT"         , kScriptDT_INT , gDTDef_INT    , kRaceHobbit   ),
    CT("KZINTI"         , kScriptDT_INT , gDTDef_INT    , kRaceKzinti   ),
    CT("MORPH"          , kScriptDT_INT , gDTDef_INT    , kRaceMorph    ),
    CT("TROLL"          , kScriptDT_INT , gDTDef_INT    , kRaceTroll    ),
    CT("VAMPIRE"        , kScriptDT_INT , gDTDef_INT    , kRaceVampire  ),
    CT("VYAN"           , kScriptDT_INT , gDTDef_INT    , kRaceVyan     ),
    EmptyConstantDecl
};

ScriptConstDef gAttackConstantTable[] = 
{
    EmptyConstantDecl
};




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif

///////////////////////////////////////////////////////////////////////////////
// Some Macros For Working With ScriptVariables of X DataType
///////////////////////////////////////////////////////////////////////////////
#define AsDT_INT(xVar)      ((LONG)  (xVar)->mValue->mValuePtr)
#define AsDT_STRING(xVar)   (((STRING*)(xVar)->mValue->mValuePtr)->c_str())



void* act_to_char(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    Object* nObj    = (Object*) hArgList[2];
    Person* nVictim = (Person*) hArgList[3];
    
    SCRIPTING_ASSERT(nText && nCh);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nCh, nObj, nVictim, ToChar, kPosResting);
    return NULL;
}


void* act_to_victim(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    Object* nObj    = (Object*) hArgList[2];
    Person* nVictim = (Person*) hArgList[3];
    
    SCRIPTING_ASSERT(nText && nCh && nVictim);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nCh, nObj, nVictim, ToVict, kPosResting);
    return NULL;
}


void* act_to_rest(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    Object* nObj    = (Object*) hArgList[2];
    Person* nVictim = (Person*) hArgList[3];
    
    SCRIPTING_ASSERT(nText && nCh && nVictim);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nCh, nObj, nVictim, ToNotVict, kPosResting);
    return NULL;
}


void* act_to_room(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    Object* nObj    = (Object*) hArgList[2];
    Person* nVictim = (Person*) hArgList[3];
    
    SCRIPTING_ASSERT(nText && nCh);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nCh, nObj, nVictim, ToRoom, kPosResting);
    return NULL;
}


void* act_to_all(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    Object* nObj    = (Object*) hArgList[2];
    Person* nVictim = (Person*) hArgList[3];
    
    SCRIPTING_ASSERT(nText && nCh);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nCh, nObj, nVictim, ToAll, kPosResting);
    return NULL;
}


void* attack(void* hArgList[])
{
    SCRIPTING_ASSERT(!"Write Me!");
    return NULL;
}


void* carrying(void* hArgList[])
{
    SCRIPTING_ASSERT(!"Write Me!");
    return NULL;
}


void* class_(void* hArgList[])
{
    SCRIPTING_ASSERT(!"Write Me!");
    return NULL;
}


void* close(void* hArgList[])
{
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[0];
    LONG        nInDir  = (LONG)        hArgList[1];
    Exit*       nExit;

    SCRIPTING_ASSERT(nRoom != NULL);
    SCRIPTING_ASSERT(nInDir >= 0 && nInDir < kDirMax);
    
    if(!(nExit = nRoom->mExits[nInDir]))
        return NULL;
    
    nExit->Close(true);
    return NULL;
}


void* closed(void* hArgList[])
{
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[0];
    LONG        nInDir  = (LONG)        hArgList[1];
    Exit*       nExit;


    SCRIPTING_ASSERT(nRoom != NULL);
    SCRIPTING_ASSERT(nInDir >= 0 && nInDir < kDirMax);
    
    if(!(nExit = nRoom->mExits[nInDir]))
        return NULL;
    
    return (void*) IsSet(nExit->mFlags, kExClosed);
}


void* create_mob(void* hArgList[])
{
    SCRIPTING_ASSERT(!"Write Me!");
    return NULL;
}


void* create_obj(void* hArgList[])
{
    SCRIPTING_ASSERT(!"Write Me!");
    return NULL;
}


void* get_room(void* hArgList[])
{
    STRINGCW nVRefStr = (CHAR*) ((STRING*)hArgList[0])->c_str();
    return MUD::Get()->ResolveRoomVRef(NULL, nVRefStr);
}


void* interpret(void* hArgList[])
{
    Person*     nCharacter  = (Person*) hArgList[0];
    STRINGCW    nCommand    = (CHAR*)   ((STRING*)hArgList[1])->c_str();
    Person*     nVictim     = (Person*) hArgList[2];


    SCRIPTING_ASSERT(nCharacter != NULL);   

    if(nVictim)
        nCommand.SPrintF(nCommand, *nVictim->GetNameTo(nCharacter));
    nCharacter->Interpret(nCommand);
    return NULL;
}


void* mob_in_room(void* hArgList[])
{
    RoomIndex*  nRoom       = (RoomIndex*)  hArgList[0];
    STRINGCW    nVRefStr    = (CHAR*)       ((STRING*)hArgList[1])->c_str();
    MobileIndex* nMobIndex  = MUD::Get()->ResolveMobileVRef(NULL, nVRefStr);


    SCRIPTING_ASSERT(nRoom != NULL);    
    SCRIPTING_ASSERT(nMobIndex != NULL);
    
    VREF nVRef = nMobIndex->VRef();
    Person* nMobile = nRoom->RetrievePersonAs(NULL, nVRef);
    return nMobile;
}


void* open(void* hArgList[])
{
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[0];
    LONG        nInDir  = (LONG)        hArgList[1];
    Exit*       nExit;


    SCRIPTING_ASSERT(nRoom != NULL);
    SCRIPTING_ASSERT(nInDir >= 0 && nInDir < kDirMax);
    
    if(!(nExit = nRoom->mExits[nInDir]))
        return NULL;
    
    nExit->Open(true);
    return NULL;
}


void* random(void* hArgList[])
{
    LONG nMin = (LONG) hArgList[0];
    LONG nMax = (LONG) hArgList[1];
    
    return (void*) Random(nMin, nMax);
}


void* run_mscript(void* hArgList[])
{
    Person* nMobile = (Person*) hArgList[0];
    LONG    nID     = (LONG)    hArgList[1];
    
    
    SCRIPTING_ASSERT(nMobile != NULL);
    SCRIPTING_ASSERT(nMobile->isMobile != NULL);
    OrangeScript* nScriptToRun = nMobile->isMobile->mIndex->GetScript(nID);
    SCRIPTING_ASSERT(nScriptToRun != NULL);

    MUD::RunningScript* nRunningNow = MUD::Get()->mRunningNow;
    SCRIPTING_ASSERT(nRunningNow != NULL);  
    OrangeScriptBinary* nBinary = static_cast<OrangeScriptBinary*>(nRunningNow->mScript->mBinary.Ptr());
    SCRIPTING_ASSERT(nBinary != NULL);
    
    RoomIndex*  nSaveRoom   = (RoomIndex*)  nBinary->mRoom->mValue->mValuePtr;
    Person*     nSaveCh     = (Person*)     nBinary->mCh->mValue->mValuePtr;
    Mobile*     nSaveMob    = (Mobile*)     nMobile->isMobile;
    Object*     nSaveObj    = (Object*)     nBinary->mObj->mValue->mValuePtr;
    Person*     nSaveVict   = (Person*)     nBinary->mVictim->mValue->mValuePtr;
    
    MUD::Get()->RunScript(nScriptToRun, nSaveRoom, nSaveCh, nSaveMob, nSaveObj, nSaveVict);
    MUD::Get()->mRunningNow = nRunningNow;
    return NULL;
}


void* run_oscript(void* hArgList[])
{
    Object* nObject = (Object*) hArgList[0];
    LONG    nID     = (LONG)    hArgList[1];
    
    SCRIPTING_ASSERT(nObject != NULL);
    OrangeScript* nScriptToRun = nObject->mIndex->GetScript(nID);
    SCRIPTING_ASSERT(nScriptToRun != NULL);

    MUD::RunningScript* nRunningNow = MUD::Get()->mRunningNow;
    SCRIPTING_ASSERT(nRunningNow != NULL);  
    OrangeScriptBinary* nBinary = static_cast<OrangeScriptBinary*>(nRunningNow->mScript->mBinary.Ptr());
    SCRIPTING_ASSERT(nBinary != NULL);
    
    RoomIndex*  nSaveRoom   = (RoomIndex*)  nBinary->mRoom->mValue->mValuePtr;
    Person*     nSaveCh     = (Person*)     nBinary->mCh->mValue->mValuePtr;
    Mobile*     nSaveMob    = (Mobile*)     nBinary->mMob->mValue->mValuePtr;
    Object*     nSaveObj    = (Object*)     nObject;
    Person*     nSaveVict   = (Person*)     nBinary->mVictim->mValue->mValuePtr;
    
    MUD::Get()->RunScript(nScriptToRun, nSaveRoom, nSaveCh, nSaveMob, nSaveObj, nSaveVict);
    MUD::Get()->mRunningNow = nRunningNow;
    return NULL;
}


void* run_rscript(void* hArgList[])
{
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[0];
    LONG        nID     = (LONG)        hArgList[1];
    
    SCRIPTING_ASSERT(nRoom != NULL);
    OrangeScript* nScriptToRun = nRoom->GetScript(nID);
    SCRIPTING_ASSERT(nScriptToRun != NULL);

    MUD::RunningScript* nRunningNow = MUD::Get()->mRunningNow;
    SCRIPTING_ASSERT(nRunningNow != NULL);  
    OrangeScriptBinary* nBinary = static_cast<OrangeScriptBinary*>(nRunningNow->mScript->mBinary.Ptr());
    SCRIPTING_ASSERT(nBinary != NULL);
    
    RoomIndex*  nSaveRoom   = (RoomIndex*)  nRoom;
    Person*     nSaveCh     = (Person*)     nBinary->mCh->mValue->mValuePtr;
    Mobile*     nSaveMob    = (Mobile*)     nBinary->mMob->mValue->mValuePtr;
    Object*     nSaveObj    = (Object*)     nBinary->mObj->mValue->mValuePtr;
    Person*     nSaveVict   = (Person*)     nBinary->mVictim->mValue->mValuePtr;
    
    MUD::Get()->RunScript(nScriptToRun, nSaveRoom, nSaveCh, nSaveMob, nSaveObj, nSaveVict);
    MUD::Get()->mRunningNow = nRunningNow;
    return NULL;
}


void* send_char(void* hArgList[])
{
    CHAR*   nText   = (CHAR*)   ((STRING*)hArgList[0])->c_str();
    Person* nCh     = (Person*) hArgList[1];
    
    SCRIPTING_ASSERT(nText && nCh);
    nCh->Send(nText);
    nCh->Send("\n");
    return NULL;
}


void* send_room(void* hArgList[])
{
    CHAR*       nText   = (CHAR*)       ((STRING*)hArgList[0])->c_str();
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[1];
    
    SCRIPTING_ASSERT(nText && nRoom);
    STRINGCW nFormat(nText); nFormat += '\n';
    Act(nFormat, nRoom, NULL, NULL, ToAll, kPosDead);
    return NULL;
}


void* transfer(void* hArgList[])
{
    Person*     nCh     = (Person*)     hArgList[0];
    RoomIndex*  nRoom   = (RoomIndex*)  hArgList[1];
    bool        nLook   = (bool)        hArgList[2];
    
    SCRIPTING_ASSERT(nCh && nRoom);
    nCh->RemoveFrom();
    nCh->PlaceIn(nRoom);
    if(nLook)
        nCh->mInRoom->ShowTo(nCh, false);
        
    return NULL;
}


void* wait(void* hArgList[])
{
    LONG nWait = (LONG) hArgList[0];
    SCRIPTING_ASSERT(MUD::Get()->mRunningNow);
    MUD::Get()->mRunningNow->mExecutor.mWait += nWait;
    return NULL;
}

