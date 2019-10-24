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
#ifndef INTERPRET_H
#define INTERPRET_H 1
#include "Person.h"

//## Functions ##//
void InitCommandHash();


//## TypeDefs ##//
typedef void            CmdFuncAll  (Person *hCh, STRINGCW& hArg);
typedef void            CmdFuncPC   (Player *hCh, STRINGCW& hArg);
typedef void            CmdFuncNPC  (Mobile *hCh, STRINGCW& hArg);
typedef void            CmdFuncImm  (Immortal *hCh, STRINGCW& hArg);
typedef struct CmdDef   CmdDef;


//## Prototypes ##//
CmdFuncImm          Do_AEdit;
CmdFuncPC           Do_AFK;
CmdFuncImm          Do_AList;
CmdFuncImm          Do_ASet;
CmdFuncImm          Do_AStat;
CmdFuncImm          Do_At;
CmdFuncImm          Do_Attack;
CmdFuncPC           Do_Color;
CmdFuncAll          Do_Commands;
CmdFuncImm          Do_Compile;
CmdFuncAll          Do_Close;
CmdFuncPC           Do_Description;
CmdFuncAll          Do_Down;
CmdFuncAll          Do_Draw;
CmdFuncAll          Do_Drop;
CmdFuncAll          Do_East;
CmdFuncAll          Do_Emote;
CmdFuncAll          Do_Equipment;
CmdFuncAll          Do_Exits;
CmdFuncImm          Do_Force;
CmdFuncAll          Do_Get;
CmdFuncAll          Do_Give;
CmdFuncAll          Do_Gossip;
CmdFuncImm          Do_Goto;
CmdFuncPC           Do_Info;
CmdFuncAll          Do_Inventory;
CmdFuncImm          Do_Invis;
CmdFuncAll          Do_Kill;
CmdFuncAll          Do_Lock;
CmdFuncAll          Do_Look;
CmdFuncImm          Do_Map;
CmdFuncImm          Do_MCode;
CmdFuncImm          Do_MCreate;
CmdFuncImm          Do_MDesc;
CmdFuncImm          Do_MEdit;
CmdFuncImm          Do_MList;
CmdFuncImm          Do_MNew;
CmdFuncImm          Do_MScript;
CmdFuncImm          Do_MSet;
CmdFuncImm          Do_MSpot;
CmdFuncImm          Do_MStat;
CmdFuncImm          Do_MText;
CmdFuncAll          Do_North;
CmdFuncImm          Do_OCreate;
CmdFuncImm          Do_OEdit;
CmdFuncImm          Do_OLC;
CmdFuncImm          Do_OList;
CmdFuncAll          Do_OOC;
CmdFuncAll          Do_Open;
CmdFuncPC           Do_Option;
CmdFuncImm          Do_OScript;
CmdFuncImm          Do_OSet;
CmdFuncImm          Do_OSpot;
CmdFuncImm          Do_OStat;
CmdFuncImm          Do_Part;
CmdFuncAll          Do_Put;
CmdFuncPC           Do_Prompt;
CmdFuncPC           Do_Quit;
CmdFuncImm          Do_RCode;
CmdFuncImm          Do_RDesc;
CmdFuncImm          Do_REdit;
CmdFuncImm          Do_RExit;
CmdFuncAll          Do_Remove;
CmdFuncPC           Do_Reply;
CmdFuncImm          Do_Repop;
CmdFuncAll          Do_Rest;
CmdFuncImm          Do_RName;
CmdFuncImm          Do_RNew;
CmdFuncImm          Do_RList;
CmdFuncImm          Do_RScript;
CmdFuncImm          Do_RSet;
CmdFuncImm          Do_RSpot;
CmdFuncImm          Do_RStat;
CmdFuncImm          Do_RText;
CmdFuncAll          Do_Sacrifice;
CmdFuncPC           Do_Save;
CmdFuncAll          Do_Say;
CmdFuncAll          Do_SayTo;
CmdFuncAll          Do_Scan;
CmdFuncAll          Do_Score;
CmdFuncAll          Do_Sheath;
CmdFuncImm          Do_Shutdown;
CmdFuncAll          Do_Sit;
CmdFuncAll          Do_Sleep;
CmdFuncAll          Do_South;
CmdFuncAll          Do_Stand;
CmdFuncImm          Do_Strings;
CmdFuncAll          Do_Tell;
CmdFuncImm          Do_Trigger;
CmdFuncAll          Do_Unlock;
CmdFuncAll          Do_Up;
CmdFuncAll          Do_Wake;
CmdFuncAll          Do_Wear;
CmdFuncAll          Do_West;
CmdFuncAll          Do_Who;
CmdFuncAll          Do_Wield;

//## Externals ##//
extern CmdDef   CommandTable[];
extern const LONG kCommandsMax;

//## Structures ##//
struct CmdDef
{
    const CHAR* mName;
    void*       mCall;
    SHORT       mPosition;
    SHORT       mFilter;
    SHORT       mWho;
    SHORT       mLevel;
    SHORT       mLog;
    SHORT       mLag;
    SHORT       mShow;      //Show In Commands/WizHelp
};

#define NON kWaitNone
#define GEN kWaitGeneral
#define CBT kWaitCombat
#define MOV kWaitMove

#define ALL 0
#define PC  1
#define NPC 2
#define IMM 3

#endif //ifndef INTERPRET_H



