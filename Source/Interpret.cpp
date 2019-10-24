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
#include "Interpret.h"
#include "MUD.h"
#include "OrangeScript.h"
#include "Player.h"
#include "Social.h"
#include "StringMagic.h"
#include "StringUtils.h"


#if 0
#pragma mark CommandTable
#endif

const LONG kCommandsMax = 104;

#define CF(a,b,c,d,e,f,g,h,i)       {a, (void*) b, c, d, e, f, g, h, i}
////////////////////////////////////////////////////////////////////////////////////
CmdDef CommandTable[kCommandsMax] =
{
    //ShortCuts
    //Name                Function            MinPosition      Who  Lv  Log Lag Show
    CF("'"              , Do_Say            , RESTING   , NON, ALL, 0   , 0, 0, 0 ),
    CF("@"              , Do_At             , RESTING   , NON, IMM, AVT , 0, 0, 0 ),

    //Name                Function            MinPosition      Who  Lv  Log Lag Show
    CF("north"          , Do_North          , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("east"           , Do_East           , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("south"          , Do_South          , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("west"           , Do_West           , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("up"             , Do_Up             , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("down"           , Do_Down           , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),

    CF("at"             , Do_At             , RESTING   , NON, IMM, 0   , 0, 0, 1 ),
    CF("inventory"      , Do_Inventory      , SLEEPING  , NON, ALL, 0   , 0, 0, 1 ),
    CF("equipment"      , Do_Equipment      , SLEEPING  , NON, ALL, 0   , 0, 0, 1 ),
    CF("get"            , Do_Get            , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("kill"           , Do_Kill           , STANDING  , NON, ALL, 0   , 0, 0, 1 ),
    CF("look"           , Do_Look           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("stand"          , Do_Stand          , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("sit"            , Do_Sit            , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("rest"           , Do_Rest           , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("score"          , Do_Score          , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("sleep"          , Do_Sleep          , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("wake"           , Do_Wake           , DEAD      , NON, ALL, 0   , 0, 0, 1 ),

    CF("afk"            , Do_AFK            , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("color"          , Do_Color          , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("commands"       , Do_Commands       , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("close"          , Do_Close          , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("description"    , Do_Description    , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("draw"           , Do_Draw           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("drop"           , Do_Drop           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("emote"          , Do_Emote          , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("exits"          , Do_Exits          , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("help"           , Do_Commands       , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("hold"           , Do_Wear           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("info"           , Do_Info           , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("give"           , Do_Give           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("gossip"         , Do_Gossip         , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("lock"           , Do_Lock           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("open"           , Do_Open           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("ooc"            , Do_OOC            , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("option"         , Do_Option         , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("prompt"         , Do_Prompt         , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("put"            , Do_Put            , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("quit"           , Do_Quit           , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("remove"         , Do_Remove         , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("reply"          , Do_Reply          , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("sacrifice"      , Do_Sacrifice      , STANDING  , MOV, ALL, 0   , 0, 0, 1 ),
    CF("save"           , Do_Save           , DEAD      , NON, PC , 0   , 0, 0, 1 ),
    CF("say"            , Do_Say            , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("sayto"          , Do_SayTo          , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("scan"           , Do_Scan           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("sheath"         , Do_Sheath         , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("tell"           , Do_Tell           , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("to"             , Do_SayTo          , RESTING   , NON, ALL, 0   , 0, 0, 0 ),
    CF("unlock"         , Do_Unlock         , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("wear"           , Do_Wear           , RESTING   , NON, ALL, 0   , 0, 0, 1 ),
    CF("who"            , Do_Who            , DEAD      , NON, ALL, 0   , 0, 0, 1 ),
    CF("wield"          , Do_Wield          , RESTING   , NON, ALL, 0   , 0, 0, 1 ),


    //** Immortal Commands **//
    CF("alist"          , Do_AList          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("astat"          , Do_AStat          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("compile"        , Do_Compile        , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("force"          , Do_Force          , RESTING   , NON, IMM, AVT , 0, 0, 1 ),
    CF("goto"           , Do_Goto           , RESTING   , NON, IMM, AVT , 0, 0, 1 ),
    CF("invis"          , Do_Invis          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("map"            , Do_Map            , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("mcreate"        , Do_MCreate        , RESTING   , NON, IMM, AVT , 0, 0, 1 ),
    CF("mlist"          , Do_MList          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("mstat"          , Do_MStat          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("ocreate"        , Do_OCreate        , RESTING   , NON, IMM, AVT , 0, 0, 1 ),
    CF("olist"          , Do_OList          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("ostat"          , Do_OStat          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("repop"          , Do_Repop          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("rlist"          , Do_RList          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("rstat"          , Do_RStat          , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("shutdown"       , Do_Shutdown       , DEAD      , NON, IMM, IMP , 0, 0, 1 ),
    CF("strings"        , Do_Strings        , DEAD      , NON, IMM, AVT , 0, 0, 1 ),
    CF("trigger"        , Do_Trigger        , DEAD      , NON, IMM, AVT , 0, 0, 1 ),


    //** Immortal OLC Commands **//
    CF("aedit"          , Do_AEdit          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("aset"           , Do_ASet           , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("attack"         , Do_Attack         , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("mdesc"          , Do_MDesc          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("medit"          , Do_MEdit          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("mnew"           , Do_MNew           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("mobile"         , Do_MSet           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("mcode"          , Do_MCode          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("mscript"        , Do_MScript        , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("mset"           , Do_MSet           , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("mspot"          , Do_MSpot          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("mtext"          , Do_MText          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("object"         , Do_OSet           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("oedit"          , Do_OEdit          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("olc"            , Do_OLC            , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("oscript"        , Do_OScript        , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("oset"           , Do_OSet           , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("ospot"          , Do_OSpot          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("part"           , Do_Part           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("rcode"          , Do_RCode          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("rdesc"          , Do_RDesc          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("redit"          , Do_REdit          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("rexit"          , Do_RExit          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("rname"          , Do_RName          , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("rnew"           , Do_RNew           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("room"           , Do_RSet           , DEAD      , NON, IMM, BLD , 0, 0, 0 ),
    CF("rscript"        , Do_RScript        , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("rset"           , Do_RSet           , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("rspot"          , Do_RSpot          , DEAD      , NON, IMM, BLD , 0, 0, 1 ),
    CF("rtext"          , Do_RText          , DEAD      , NON, IMM, BLD , 0, 0, 0 )
};
////////////////////////////////////////////////////////////////////////////////////
#undef CF

typedef Vector<CmdDef*>     CmdDefArray;
typedef Vector<CmdDefArray> CmdDefHashTable;
CmdDefHashTable CommandHash;


void InitCommandHash()
{
    static bool sInitedCmdHash = false;

    ASSERT(sInitedCmdHash == false);
    sInitedCmdHash = true;

    CommandHash.resize(255);
    for(LONG i = 0; i < kCommandsMax; ++i)
    {
        CmdDefArray& nCmds = CommandHash[(UCHAR)CommandTable[i].mName[0]];
        nCmds.push_back(&CommandTable[i]);
    }
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Person::Interpret(STRINGCW& hCmd, bool hCheck)
{
    LONG        nSpace = hCmd.Find(" ");
    STRINGCW    nCommand;
    STRINGCW    nArguments;
    
    
    if(hCmd.Length() >= 1 && !isalnum(hCmd[0]))
    {
        nCommand    = hCmd.SubString(0, 1);
        nArguments  = hCmd.SubString(1, cwsStringEnd);
    }
    else
    if(nSpace != cwsNotFound)
    {
        nCommand = hCmd.SubString(0, nSpace);
        if(nSpace+1 < hCmd.Length())
            nArguments = hCmd.SubString(nSpace+1, cwsStringEnd);
        else
            nArguments = "";
    }else
    {
        nCommand =  hCmd;
        nArguments = "";
    }

    Interpret(nCommand, nArguments, hCheck);
}




void Person::Interpret(STRINGCW& hCmd, STRINGCW& hArgs, bool hCheck)
{
    static const CHAR* sCantPositionMsgs[kPosMax] =
    {
        /* Dead  */ "You might've been able to do that, if you weren't ^!DEAD^x!.\n",
        /* Mortl */ "^WThe tunnel of white light has you distracted.^x\n"           ,
        /* Incap */ "You are incapacitated, you should be ^!pray^xing instead.\n"   ,   
        /* Stunn */ "You are completely stunned at the moment.\n"                   ,
        /* Sleep */ "You cannot do that in your sleep.\n"                           ,
        /* Restn */ "You cannot do that while resting.\n"                           ,
        /* Sittn */ "Perhaps you should stand up first.\n"                          ,
        /* Fight */ "You're too busy fighting!\n"                                   ,
        /* Huntd */ "You're being ^!HUNTED^x and cannot do that right now!\n"       ,
        /* Stand */ "** CANT DO WHILE STANDING!?? - BUG **\n"
    };
    ULONG   i;
    SHORT   nWho, nOurTrust = GetTrust();
    
    if(hCmd.Empty())
        return;
    
    
    if(hCheck) //check scripts
    {
        if(TrigPhrase(this, hCmd, hArgs))
            return;
    }
    
    
    CmdDefArray& nPossibleCmds = CommandHash[(UCHAR)hCmd[0]];
    
    for(i = 0; i < nPossibleCmds.size(); ++i)
    if(nPossibleCmds[i]->mLevel <= nOurTrust)
    if((nWho = nPossibleCmds[i]->mWho) == ALL || (nWho == PC && isPlayer) || (nWho == NPC && isMobile) || (nWho == IMM && isPlayer && isPlayer->isImm))
    if(!StrNCmp(&nPossibleCmds[i]->mName[1], &hCmd[1], hCmd.Length()-1))
    {
        if(mPosition < nPossibleCmds[i]->mPosition)
        {
            Send(sCantPositionMsgs[mPosition]);
            return;
        }
        
        if(nPossibleCmds[i]->mWho == ALL)
            (*((CmdFuncAll*)nPossibleCmds[i]->mCall))(this, hArgs);
        else if(nPossibleCmds[i]->mWho == PC)
            (*((CmdFuncPC*) nPossibleCmds[i]->mCall))(isPlayer, hArgs);
        else if(nPossibleCmds[i]->mWho == NPC)
            (*((CmdFuncNPC*)nPossibleCmds[i]->mCall))(isMobile, hArgs);
        else if(nPossibleCmds[i]->mWho == IMM)
            (*((CmdFuncImm*)nPossibleCmds[i]->mCall))(isPlayer->isImm, hArgs);
        return; 
    }
    
    Social* nSocial = MUD::Get()->FindSocial(hCmd);
    if(nSocial)
    {
        nSocial->Do(this, hArgs);
        return;
    }
    
    Send("No Such function.\n");
}


