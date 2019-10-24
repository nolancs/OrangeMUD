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
#ifndef ACT_H
#define ACT_H 1
#include "ActThing.h"



//Disemination
void Act(const CHAR* hFormat, Person* hCh, ActThing* hAct1, ActThing* hAct2, LONG hTargets, SHORT hMinPos = kPosResting, PersonHolder* hSentTo = NULL);
void Act(const CHAR* hFormat, RoomIndex* hRoom, ActThing* hAct1, ActThing* hAct2, LONG hTargets, SHORT hMinPos = kPosResting, PersonHolder* hSentTo = NULL);

//Textualization
STRINGCW ActParseTo(Person* hTo, const CHAR* hFormat, Person* hCh, ActThing* hAct1, ActThing* hAct2);



//## Specific Act Targets ##//
#define kActRoom        (A) //Always assumed, shouldn't need to use this
#define kActWorld       (B) //Send to world instead of ch->inRoom
#define kActChar        (C) //To ch
#define kActVict        (D) //To vict
#define kActNoChar      (E) //Don't include ch
#define kActNoVict      (F) //Don't include vict
#define kActGChar       (G) //Filter for only group members of ch
#define kActGVict       (H) //Filter for only group members of vict
#define kActNoGChar     (I) //Filter out all of ch's group members
#define kActNoGVict     (J) //Filter out all of vict's group members
#define kActMort        (K) //Only mortals
#define kActImm         (L) //Only immortals
#define kActOnlySeeChar (M) //Wont show 'someones', only to those who can see ch
#define kActOnlySeeVict (N) //Wont show 'someones', only to those who can see ch

//## Generalized Act Target Shortcuts ##//
#define ToRoom      (kActRoom|kActNoChar)       //All but ch
#define ToNotVict   (kActNoChar|kActNoVict)     //To all but ch and vict
#define ToVict      (kActVict)                  //To vict
#define ToChar      (kActChar)                  //To ch
#define ToAll       (kActRoom)                  //To everyone in the room
#define ToGChar     (kActGChar|kActNoChar)      //To everyone in the room grouped w/ char
#define ToGVict     (kActGVict|kActNoVict)      //To everyone in the room grouped w/ victim
#define ToGNotVict  (kActNoGChar|kActNoGVict)   //To the rest of the people not grouped w/ either
#define ToGRoom     (kActGChar)                 //To everyone in the group in the room


/*
#define kActInfoConnect (M)
#define kActInfoQuit    (N)
#define kActInfoLinks   (O)
#define kActInfoLevels  (P)
#define kActInfoDeaths  (Q)
#define kActInfoNotes   (R)
#define kActInfoImm     (S)
#define kActInfoRepops  (T)
#define kActInfoRewards (U)
#define kActInfoCreate  (V)
#define kActInfoPunish  (W)
#define kActInfoDebug   (X)
#define kActInfoOLC     (Y) 

#define TO_INFO_CONNECT (T_WORLD|T_NOCHAR|T_INFO_CONNECT)
#define TO_INFO_QUIT    (T_WORLD|T_NOCHAR|T_INFO_QUIT)
#define TO_INFO_LINKS   (T_WORLD|T_NOCHAR|T_INFO_LINKS)
#define TO_INFO_LEVELS  (T_WORLD|T_INFO_LEVELS)
#define TO_INFO_DEATHS  (T_WORLD|T_INFO_DEATHS)
#define TO_INFO_NOTES   (T_WORLD|T_INFO_NOTES)
#define TO_INFO_IMM     (T_WORLD|T_INFO_IMM)
#define TO_INFO_REPOPS  (T_WORLD|T_INFO_REPOPS)
#define TO_INFO_REWARDS (T_WORLD|T_INFO_REWARDS)
#define TO_INFO_CREATE  (T_WORLD|T_INFO_CREATE)
#define TO_INFO_PUNISH  (T_WORLD|T_INFO_PUNISH)
#define TO_INFO_SNOOP   (T_WORLD|T_INFO_SNOOP)
#define TO_INFO_OLC     (T_WORLD|T_INFO_OLC)
*/

#endif //ifndef ACT_H
