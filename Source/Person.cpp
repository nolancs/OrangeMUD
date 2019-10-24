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
#include "Act.h"
#include "Area.h"
#include "Classes.h"
#include "Fight.h"
#include "Immortal.h"
#include "Interpret.h"
#include "Mobile.h"
#include "MobileIndex.h"
#include "MUD.h"
#include "Object.h"
#include "OrangeScript.h"
#include "OrangeSession.h"
#include "Player.h"
#include "Races.h"
#include "RoomIndex.h"
#include "Spot.h"
#include "StringMagic.h"
#include "StringUtils.h"
#include "Tables.h"
#include "TEditor.h"



Person::Person()
    //Runtime Data
    : mSession(NULL)
    , isPlayer(NULL)
    , isMobile(NULL)
    , mInRoom(NULL)
    , mWasInRoom(NULL)
    , mInSpot(NULL)
    , mOnObject(NULL)
    , mOnHow(kUseNone)
    , mRiding(NULL)
    , mTimer(0)
    , mFighting(NULL)
    , mFollowing(NULL)
    , mImFighting(NULL)
    , mFightingMe(NULL)

    //Persistant Data
    , mSex(kSexNeutral)
    , mLevel(0)
    , mRace(0)
    , mClass(0)
    , mAlignment(0)
    , mSize(0)
    , mPosition(kPosStanding)
    , mHits(0)  , mHitsMax(0)
    , mEnergy(0), mEnergyMax(0)
    , mMoves(0) , mMovesMax(0)
    , mHitRegen(0), mEnergyRegen(0), mMoveRegen(0)
    , mFocus(0)
    , mExpTNL(0)
    , mLevelLast(0)
    , mLevelHitT(0)
    , mLevelEnergyT(0)
    , mLevelMoveT(0)
    , mHitT(0), mEnergyT(0), mMoveT(0)
    , mAttacksLeft(0)
{
    for(LONG i = 0; i < kWaitMax; ++i)
        mLags[i] = 0;

    for(LONG i = 0; i < kWimpyMax; ++i)
        mWimpy[i] = kWimpyOff;

    for(LONG i = 0; i < kResMax; ++i)
        mResist[i] = 0;

    for(LONG i = 0; i < kStatMax; ++i)
        mStat[i] = 0;
}




Person::~Person()
{
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void Person::Send(const CHAR* hData, LONG hLen)
{
    if(!mSession || !mSession->mTCP.Ptr())
        return;

    LONG nSendSize = hLen;
    
    if(nSendSize == -1)
        nSendSize = strlen(hData);
    else
        ASSERT(nSendSize >= hLen);
    
    mSession->mTCP->Write(hData, nSendSize);    
}


void Person::Send(const STRINGCW& hData, LONG hLen)
{
    if(!mSession || !mSession->mTCP.Ptr())
        return;
    
    LONG nSendSize = hLen;
    
    if(nSendSize == -1)
        nSendSize = hData.Length();
    else
        ASSERT(nSendSize >= hLen);
    
    mSession->mTCP->Write(hData.CStr(), nSendSize);
}



void Person::SendF(const CHAR* hData, ...)
{
    CHAR    nBuf[kMaxStringLen];
    va_list hAp;
    
    va_start(hAp, hData);
    LONG n = vcsprintf(nBuf, hData, hAp);
    va_end(hAp);
    Send(nBuf, n);
}


void Person::SendPaged(const CHAR* hData, LONG hLen)
{
    if(!mSession || !mSession->mTCP.Ptr())
        return;

    LONG nSendSize = hLen;
    
    if(nSendSize == -1)
        nSendSize = strlen(hData);
    else
        ASSERT(nSendSize >= hLen);
    
    mSession->mTCP->WritePaged(hData, nSendSize);   
}


void Person::SendPaged(const STRINGCW& hData, LONG hLen)
{
    if(!mSession || !mSession->mTCP.Ptr())
        return;
    
    LONG nSendSize = hLen;
    
    if(nSendSize == -1)
        nSendSize = hData.Length();
    else
        ASSERT(nSendSize >= hLen);
    
    mSession->mTCP->WritePaged(hData.CStr(), nSendSize);
}



void Person::SendPagedF(const CHAR* hData, ...)
{
    CHAR    nBuf[kMaxStringLen];
    va_list hAp;
    
    va_start(hAp, hData);
    vcsprintf(nBuf, hData, hAp);
    va_end(hAp);
    SendPaged(nBuf);
}



void Person::SendLined(const CHAR* hText, bool hShowColorCodes)
{
    CHAR        nBuf[kMaxStringLen];
    LONG        x = 0, i = 1, nLine = 1;
    CHAR        nNumBuf[6];
    STRINGCW    nOutput;
    
    if(hShowColorCodes)
        nOutput += kColorPauseStr;
        
    for(;;)
    {       
        sprintf(nNumBuf,"[%2ld] ", nLine);
        for(i = 0; hText[x+i] != '\n'; ++i)
        {
            nBuf[i] = hText[x+i];
            if(hText[x+i] == '\0')
            {
                nOutput += nNumBuf;
                nOutput += nBuf;
                nOutput += "\n";
                if(hShowColorCodes)
                    nOutput += kColorPauseStr;
                SendPaged(nOutput);
                return;
            }
        }
        
        nBuf[i] = hText[x+i]; i++;
        nBuf[i] = '\0';
        nOutput += nNumBuf;
        nOutput += nBuf;
        x       += i;
        nLine   += 2;
    }
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


void Person::PlaceInGame(PersonHolder* hTo, bool hLoggingIn)
{
    ASSERT(hTo != NULL);    
    PlaceIn(hTo);
}



void Person::RemoveFromGame(bool hLoggingOut)
{
    RemoveFrom();
    if(mImFighting)
        mImFighting->Remove(this);
    if(mFightingMe)
        mFightingMe->End();
}



void Person::PlaceIn(PersonHolder* hTo)
{
    ASSERT(hTo != NULL);
    hTo->AddPerson(this);
    mInRoom = dynamic_cast<RoomIndex*>(hTo);
    ASSERT(mInRoom);
    
    //Add Lights
    for(ObjectIter i = mWearing.Begin(); i != mWearing.End(); ++i)
    {
        Object* nLight = *i;
        if(nLight->IsLighting())
            ++mInRoom->mLights;
    }
}



void Person::RemoveFrom()
{
    if(!mInRoom)
        return;

    //Remove Lights
    for(ObjectIter i = mWearing.Begin(); i != mWearing.End(); ++i)
    {
        Object* nLight = *i;
        if(nLight->IsLighting())
            --mInRoom->mLights;
    }

    mInRoom->RemovePerson(this);
    mInRoom     = NULL;
    mOnObject   = NULL;
    mOnHow      = kUseNone;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetNameTo
//
//   Desc:: Gets a string corresponding to what this Person's name appears to
//          be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's name relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetNameTo(Person* hTo)
{
    return mName;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetPNameTo
//
//   Desc:: Gets a string corresponding to what this Person's plural name
//          appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's plural name relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetPNameTo(Person* hTo, LONG hAmount)
{
    STRINGCW nPName(100);
    
    if(mPName.Empty())
        nPName = mName + "s";
    else
        nPName = mPName;

    return nPName.SPrintF("%s %s", NumToWord(hAmount), *nPName);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetKeywordsTo
//
//   Desc:: Gets a string corresponding to what this Person's keywords appear
//          to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's keywords relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetKeywordsTo(Person* hTo)
{
    return mKeywords;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetShortDescTo
//
//   Desc:: Gets a string corresponding to what this Person's short description
//          appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's short description relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetShortDescTo(Person* hTo)
{
    STRINGCW nText(200);
    bool nCanSee = hTo->CanSee(this);
    
    //** Mobile In Default Position **//
    if(isMobile && !mInSpot && mPosition == isMobile->mIndex->mDefaultPos)
    {
        return ActParseTo(hTo, mShortDesc, this, NULL, NULL);
    }
    
    
    //** Generic Short Description **//
    if(mOnObject)
    {
        const char* nHowStrs[] = { "", "at", "on", "in" };
        nText.SPrintF("    -%s is here, %s %s $p.", *GetNameTo(hTo).Capitalize(), gPosTable[mPosition].mName, nHowStrs[mOnHow]);
        return ActParseTo(hTo, nText, this, mOnObject, NULL);
    }

    nText += GetNameTo(hTo) + " ";
    
    if(mInSpot)
        nText += QBuf("(hiding behind %s) ", *mInSpot->mName);
    if(isPlayer && isPlayer->isImm && isPlayer->isImm->mInvisLevel && nCanSee)
        nText += "(invis) ";

    nText += QBuf("is %s", gPosTable[mPosition].mLDesc);
    
    if(mPosition == kPosFighting && mFighting)
    {
        nText += mFighting->GetTextForCodeTo(hTo, 'n');
        nText += "!";
    }
    else
    if(mPosition == kPosFighting)   
        nText += "someone who just left!?";
        
    return nText.Capitalize();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetPShortDescTo
//
//   Desc:: Gets a string corresponding to what this Person's plural short
//          description appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's plural short description relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetPShortDescTo(Person* hTo, LONG hAmount)
{
    STRINGCW nText(200);


    if(mOnObject)
    {
        const char* nHowStrs[] = { "", "at", "on", "in" };
        nText.SPrintF("    -%s are here, %s %s $p.", *GetPNameTo(hTo, hAmount).Capitalize(), gPosTable[mPosition].mName, nHowStrs[mOnHow]);
        nText = ActParseTo(hTo, nText, this, mOnObject, NULL);
        return nText;
    }

    nText += GetPNameTo(hTo, hAmount) + " ";
    
    if(mInSpot)
        nText += QBuf("(hiding behind %s) ", *mInSpot->mName);
    
    nText += QBuf("are %s", gPosTable[mPosition].mLDesc);
    
    if(mPosition == kPosFighting && mFighting)
    {
        nText += mFighting->GetTextForCodeTo(hTo, 'n');
        nText += "!";
    }
    else
    if(mPosition == kPosFighting)   
        nText += "someone who just left!?";
    
    return ActParseTo(hTo, nText, this, ActStr(NumToWord(hAmount)), NULL);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetDescriptionTo
//
//   Desc:: Gets a string corresponding to what this Person's description
//          appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's description relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetDescriptionTo(Person* hTo)
{
    static const CHAR* nDefault = "You notice nothing especially noteworthy about $m.";
    
    if(mDescription.Empty())
        return ActParseTo(hTo, nDefault, this, NULL, NULL);

    return mDescription;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetConditionTo
//
//   Desc:: Gets a string corresponding to what this Person's current health
//          condition as it appears relative to another Person (hTo). You can
//          get both a short or long version, (short for in-prompt stuff) and
//          you can get them with or without color.
//
//  Input:: hTo: As seen by this Person.
//          hInColor: Want color with that?
//          hShort: Want the compact version of the condition?
//
// Output:: This Person's condition relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetConditionTo(Person *to, bool hInColor, bool hShort)
{
    #define kNumConds 10
    static const CHAR* sConds[2*kNumConds][2] =
    {
        { "^wdead^x"                , "is ^wDEAD!^x"                    },
        { "^r^!dying^x"             , "is ^r^!dying!^x"                 },              
        { "^rleaking guts^x"        , "is ^yleaking guts.^x"            },              
        { "^rcovered in blood^x"    , "is ^rcovered in blood.^x"        },
        { "^rbleeding freely^x"     , "is ^rbleeding freely.^x"         },              
        { "^ynastily wounded^x"     , "is ^yvery nastily wounded.^x"    },              
        { "^yhurt and bleeding^x"   , "is ^yhurt and bleeding.^x"       },
        { "^gminor cuts^x"          , "has some ^gcuts and bruises.^x"  },      
        { "^wlightly scraped^x"     , "is ^wlightly scraped.^x"         },
        { "^wperfect^x"             , "is in ^wperfect health.^x"       },
        //With No Color
        { "dead"                    , "is DEAD!"                        },
        { "dying"                   , "is dying!"                       },              
        { "leaking guts"            , "is leaking guts."                },              
        { "covered in blood"        , "is covered in blood."            },
        { "bleeding freely"         , "is bleeding freely."             },              
        { "nastily wounded"         , "is very nastily wounded."        },              
        { "hurt and bleeding"       , "is hurt and bleeding."           },
        { "minor cuts"              , "has some cuts and bruises."      },      
        { "lightly scraped"         , "is lightly scraped."             },
        { "perfect"                 , "is in perfect health."           }
    };

    
    float   nHpPerc     = 100*(mHits/((double)(mHitsMax ? mHitsMax : 1)));
    SHORT   nIndex;
    
         if(nHpPerc <= 0 && mPosition == kPosDead)
        nIndex = 0;         //** Dead **//
    else if(nHpPerc <= 0)
        nIndex = 1;         //** Dying **//
    else if(nHpPerc <= 15)
        nIndex = 2;         //** Leaking Guts **//
    else if(nHpPerc <= 30)
        nIndex = 3;         //** Covered in Blood **//
    else if(nHpPerc <= 45)
        nIndex = 4;         //** Bleeding Freeling **//
    else if(nHpPerc <= 60)
        nIndex = 5;         //** Nastily Wounded **//
    else if(nHpPerc <= 75)
        nIndex = 6;         //** Hurt & Bleeding **//
    else if(nHpPerc <= 90)
        nIndex = 7;         //** Minor Cuts **//
    else if(nHpPerc < 100)
        nIndex = 8;         //** Lightly Scraped **//
    else
        nIndex = 9;         //** Perfect **//
        
    if(!hInColor)
        nIndex += kNumConds;
        
    return sConds[nIndex][hShort ? 0 : 1];
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetParsedPrompt
//
//   Desc:: Gets a string that is the prompt of this Person. The prompt is
//          parsed so all the %h's are replaced with current hitpoints and 
//          such. The results are stored in a static string so they are only
//          valid until the next call to this function.
//
// Output:: This Person's parsed prompt.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW& Person::GetParsedPrompt()
{
    static STRINGCW nNoPrompt("[Promptless]>");
    static STRINGCW nBuf;
    static CHAR     nQuickBuf[40];
    
    STRINGCW& nFormat = nNoPrompt;
    
    if(isMobile)
    {
        static STRINGCW nMobPrompt("[MOB][%h/%Hhp %e/%Een %m/%Mmv]%x>");
        nFormat = nMobPrompt;
    }else{
        nFormat = isPlayer->mPrompt;
    }
    
    
    
    //** Do The Parsing **//
    nBuf.Clear();
    for(LONG i = 0; i < nFormat.Length(); ++i)
    {
        if(nFormat[i] != '%' || i+1 == nFormat.Length())
        {
            nBuf += nFormat[i];
            continue;
        }
        
        i++;
        switch(nFormat[i])
        {
            case 'h': sprintf(nQuickBuf, "%ld", mHits);     nBuf += nQuickBuf; break;
            case 'H': sprintf(nQuickBuf, "%ld", mHitsMax);  nBuf += nQuickBuf; break;
            case 'e': sprintf(nQuickBuf, "%ld", mEnergy);   nBuf += nQuickBuf; break;
            case 'E': sprintf(nQuickBuf, "%ld", mEnergyMax);nBuf += nQuickBuf; break;
            case 'm': sprintf(nQuickBuf, "%ld", mMoves);    nBuf += nQuickBuf; break;
            case 'M': sprintf(nQuickBuf, "%ld", mMovesMax); nBuf += nQuickBuf; break;
            case 'R':
                if(IsImm() && mInRoom)
                    nBuf += mInRoom->GetIdentTo(this);
                break;
            case 'x':
            case 'X':
                if(mInRoom)
                    nBuf += mInRoom->GetExitsCharsTo(this);
                break;
            default: 
                nBuf += '%';
                nBuf += nFormat[i];
                break;
        }
    }
    
    return nBuf;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetRaceAbrvTo
//
//   Desc:: Gets a string corresponding to what this Person's 3-letter race
//          abbreviation appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's race abbreviation relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetRaceAbrvTo(Person* hTo)
{
    return isMobile ? STRINGCW("MOB") : gRaces[mRace]->GetAbrv();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetClassAbrvTo
//
//   Desc:: Gets a string corresponding to what this Person's 3-letter class
//          abbreviation appears to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's class abbreviation relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetClassAbrvTo(Person* hTo)
{
    return isMobile ? STRINGCW("MOB") : gClasses[mClass]->GetAbrv();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetRaceNameTo
//
//   Desc:: Gets a string corresponding to what this Person's race name appears
//          to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's race name relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


STRINGCW Person::GetRaceNameTo(Person* hTo)
{
    return isMobile ? STRINGCW("Mobile") : gRaces[mRace]->GetName();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetRaceNameTo
//
//   Desc:: Gets a string corresponding to what this Person's class name appears
//          to be relative to another Person (hTo).
//
//  Input:: As seen by this Person.
//
// Output:: This Person's class name relative to hTo.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


STRINGCW Person::GetClassNameTo(Person* hTo)    
{
    if(isMobile)
    {
        return "Mobile";
    }

    if(mClass < 0 || mClass >= gClasses.size())
        mClass = 0;
        
    return gClasses[mClass]->GetName();
}
    



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetTextForCodeTo
//
//   Desc:: An ActThing function overriden so that whenever a $n, $m, etc is
//          encountered in relation to this class that it returns a string you
//          decide upon. The code is the 'n' part of a $n. Every ActThing
//          object is required to respond to at least the '?' code. 
//
//  Input:: hTo: Get the text for hCode relative to this person.
//          hCode: The code.
//
// Output:: The string results.
//
//   Note:: The codes n,e,m,s,v,c are reserved for Person/Player/Mobile classes
//          ONLY so you cannot use those as codes for any other classes to
//          respond to, but all other characters are usable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRINGCW Person::GetTextForCodeTo(Person* hTo, CHAR hCode)
{
    STRINGCW nText;
    
    
    if(hCode == 'n' || hCode == '?')
    {
        nText = hTo->CanSee(this) ? *GetNameTo(hTo) : "someone";
    }else
    if(hCode == 'e')
    {
        nText = gSexTable[mSex].mHeShe;
    }else
    if(hCode == 'm')
    {
        nText = gSexTable[mSex].mHimHer;
    }else
    if(hCode == 's')
    {
        nText = gSexTable[mSex].mHisHer;
    }else
    if(hCode == 'v')
    {
        nText = isMobile ? isMobile->mIndex->mMovement : STRINGCW("walks");
    }else
    if(hCode == 'c')
    {
        nText = GetConditionTo(hTo, false, false);
    }else{
        nText.SPrintF("{Unrecognized_Code(%c)_On_Person}", hCode);
    }
    
    
    return nText;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CheckNumber
//
//   Desc:: Checks if the hArg inputed by this player is a number. If it's not
//          it tells them so and returns false. It is a number it tells them
//          nothing and returns true. A great time saver for checking input.
//
//  Input:: Some text argument that is expected to be a number.
//
// Output:: True if hArg is a number.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CheckNumber(STRINGCW& hArg)
{
    if(!hArg.IsNumber())
    {
        SendF("Your input \"%s\" is not a number.\n", *hArg);
        return false;
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CountUsers
//
//   Desc:: Returns the number of people currently riding upon this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT Person::CountUsers()
{
    SHORT nUsers = 0;
    
    if(mInRoom)
    {
        for(PersonIter i = mInRoom->PersonHolder::Begin(); i != mInRoom->PersonHolder::End(); ++i)
        if((*i)->mRiding == this)
            ++nUsers;
    }

    return nUsers;
}




bool Person::DoEditor(CHAR* hArg, STRINGCW& hText, CHAR* hClearMsg)
{
    TEditor     nEditText(hText);
    STRINGCW    nArg = hArg;
    
    if(nEditText.Handle(this, nArg, hClearMsg))
    {
        hText = nEditText.mAfter.c_str();
        return true;
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ShowTo
//
//   Desc:: Show this Person to Person hTo. Shows description and any other
//          important things.
//
//  Input:: The Person to show this Person to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::ShowTo(Person* hTo)
{
    hTo->SendF("%s\n", *GetDescriptionTo(hTo));
    hTo->SendF("%s\n", kGfxSep);
    Act("$N $t\n", hTo, ActStr(GetConditionTo(hTo, true, false)), this, ToChar, kPosDead);


    //Check that we see any equipment before showing
    ObjectHolder nEquipment;
    if(mWearing.RetrieveObjectsAs(hTo, "all", nEquipment))
    {
        hTo->Send("\n");
        ShowEquipTo(hTo);
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsImm
//
//   Desc:: Checks if this Person is an immortal. It is the standard check but
//          can return false if the immortal turns off their kOptImmPowers in
//          an attempt to be treated as a mortal.
//
// Output:: True if this Person is an immortal.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsImm()
{
    return GetTrust() >= kLevelImmortal && IsOpt(kOptImmPowers); 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsTrueImm
//
//   Desc:: Checks if this Person is an immortal. It is the standard check but
//          can return false if the immortal turns off their kOptImm in an
//          attempt to be treated as a mortal.
//
// Output:: True if this Person is an immortal.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsTrueImm()
{
    return GetTrust() >= kLevelImmortal; 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsOpt
//
//   Desc:: Find out if this person has a specific option preference turned on
//          or off. Uses kOpt constants like kOptIncognito.
//
//  Input:: The kOpt option constant.
//
// Output:: True if the person has it on, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsOpt(LONG hOptBit)
{
    if(isMobile)
        return false;
    
    return IsSet(isPlayer->mFlagsOptions, hOptBit);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsInfo
//
//   Desc:: Find out if this person has a specific info preference turned on
//          or off. Uses kOpt constants like kOptIncognito.
//
//  Input:: The kOpt option constant.
//
// Output:: True if the person has it on, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsInfo(LONG hInfoBit)
{
    if(isMobile)
        return false;
    
    return IsSet(isPlayer->mFlagsInfo, hInfoBit);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsStatus
//
//   Desc:: Find out if this person has a specific info status bit turned on
//          or off. Uses kStatus constants like kStatusAFK.
//
//  Input:: The kStatus constant.
//
// Output:: True if the Person has it on, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsStatus(LONG hStatBit)
{
    if(isMobile)
        return false;
    
    return IsSet(isPlayer->mFlagsStatus, hStatBit);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::IsSameGroupAs
//
//   Desc:: Find out if we're in the same group as another person.
//
//  Input:: The person who's group we want to know if we're in.
//
// Output:: True if we're in the same group, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::IsSameGroupAs(Person* hTo)
{
    ASSERT(hTo != NULL);
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetUID
//
//   Desc:: Get the Unique ID number corresponding to this Player/Mobile.
//          Mobiles always return 0. Non-zero IDs are unique across the game
//          and should be used to reference players instead of names.
//
// Output:: The unique id.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

UID Person::GetUID()
{
    return isPlayer ? isPlayer->mUID : 0;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetTrust
//
//   Desc:: Finds out to what level this Person is trusted. Trust is usually
//          equal to the person's level. Immortals may assume many different
//          player levels for testing but their trust will always remain the
//          same.
//
// Output:: The trust of this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

SHORT Person::GetTrust()
{
    return isPlayer ? UMAX(isPlayer->mTrust, mLevel) : mLevel;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::RetrieveObjectHere
//
//   Desc:: Retrieve an object from the Person's current area. You pick the
//          level of depth searched. Objects are found in a specific order that
//          must be strictly followed. Always the inventory is searched first,
//          then the things being worn, then finally the contents of the room
//          contents. You find the object by virtual reference.
//
//  Input:: hVRef: The VRef of an object to find.
//          hMode: A kRetrieve constant specifying the search depth.
//
// Output:: Non-NULL ptr to the found object, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Object* Person::RetrieveObjectHere(VREF hVRef, BYTE hMode)
{
    Object* nObjectWanted;
    
    //Retrieve From Inventory
    if((nObjectWanted = RetrieveObjectAs(this, hVRef)) != NULL)
        return nObjectWanted;
        
    if(hMode != kFromInvWear && hMode != kFromInvWearRoom)
        return NULL;
    
    
    //Retrieve From Wearing
    if((nObjectWanted = mWearing.RetrieveObjectAs(this, hVRef)) != NULL)
        return nObjectWanted;
        
    if(hMode != kFromInvWearRoom)
        return NULL;
    
    
    //Retrieve From Room
    return mInRoom->RetrieveObjectAs(this, hVRef);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::RetrieveObjectsHere
//
//   Desc:: Retrieve a set of objects from the Person's current area. You pick
//          the level of depth searched. Objects are found in a specific order
//          that must be strictly followed. Always the inventory is searched
//          first, then the things being worn, then finally the contents of the
//          room contents.
//
//  Input:: hArg: The keywords for finding the objects.
//          hHolder: A place to store the objects found.
//          hMode: A kRetrieve constant specifying the search depth.
//
// Output:: True if some objects were found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::RetrieveObjectsHere(const CHAR* hArg, ObjectHolder& hHolder, BYTE hMode)
{
    LONG        nFound = 0, nCollected = 0;
    LONG        nStartWhere, nWantAmount;
    STRINGCW    nMatchArg;
    bool        nWantsAll;
    
    
    ParseMatching(hArg, nStartWhere, nWantAmount, nMatchArg, nWantsAll);
    
    //Retrieve From Inventory
    if(hMode == kFromInv || hMode == kFromInvWear || hMode == kFromInvWearRoom)
    {
        RetrieveObjectsAs(this, hArg, hHolder, nFound, nCollected);
        if(nCollected == nWantAmount)
            return true;
    }
        
    //Retrieve From Wearing
    if(hMode == kFromInvWear || hMode == kFromInvWearRoom || hMode == kFromWear)
    {
        mWearing.RetrieveObjectsAs(this, hArg, hHolder, nFound, nCollected);
        if(nCollected == nWantAmount)
            return true;
    }
    
    //Retrieve From Room
    if(hMode == kFromInvWearRoom || hMode == kFromRoom)
    {
        mInRoom->RetrieveObjectsAs(this, hArg, hHolder, nFound, nCollected);
    }
    
    return nCollected ? true : false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ResolveObject
//
//   Desc:: Retrieve a single object from any area around or on the person.
//          Like other Resolve-named methods it will alert the Person if the
//          resolution fails. This keeps down on repeative "You don't see xx"
//          coding.
//
//  Input:: hArg: The keywords for finding the object.
//          hMode: A kRetrieve constant specifying the search depth.
//
// Output:: Pointer to the object on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Object* Person::ResolveObject(const CHAR* hArg, BYTE hMode)
{
    ObjectHolder nGotObjs;
    
    if(!ResolveObjects(hArg, hMode, nGotObjs))
        return NULL;
    
    if(nGotObjs.Size() != 1)
    {
        Send("You may only do this with ONE object at a time.\n");
        return NULL;    
    }
    
    return nGotObjs.Front();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ResolveObjects
//
//   Desc:: Retrieve any number of objects from any area around or on the this
//          Person. Like other Resolve-named methods it will alert the Person
//          if the resolution fails. This keeps down on repeative "You don't
//          see xx" coding.
//
//  Input:: hArg: The keywords for finding the object.
//          hMode: A kRetrieve constant specifying the search depth.
//          hHolder: Holds found objects.
//
// Output:: True if any were found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::ResolveObjects(const CHAR* hArg, BYTE hMode, ObjectHolder& hHolder)
{
    switch(hMode)
    {
        case kFromInv:
            if(!RetrieveObjectsHere(hArg, hHolder, hMode))
            {
                SendF("You don't see any ^H%s^x in your inventory.\n", hArg);
                return false;
            }
            break;
        case kFromInvWear:
            if(!RetrieveObjectsHere(hArg, hHolder, hMode))
            {
                SendF("You don't see any ^H%s^x on your person.\n", hArg);
                return false;
            }
            break;
        case kFromInvWearRoom:
            if(!RetrieveObjectsHere(hArg, hHolder, hMode))
            {
                SendF("You don't see any ^H%s^x around anywhere.\n", hArg);
                return false;
            }
            break;
        case kFromWear:
            if(!mWearing.RetrieveObjectsAs(this, hArg, hHolder))
            {
                SendF("You aren't wearing any ^H%s^x.\n", hArg);
                return false;
            }
            break;
        case kFromRoom:
            if(!mInRoom->RetrieveObjectsAs(this, hArg, hHolder))
            {
                SendF("You don't see any ^H%s^x in the room.\n", hArg);
                return false;
            }
            break;
        default:
            ASSERT(!"Unhandled ResolveObjects mode.");
            break;
    }
    
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ResolvePerson
//
//   Desc:: Retrieve a single person from the room this Person is in. Like
//          other Resolve-named methods it will alert the Person if the
//          resolution fails. This keeps down on repeative "You don't see xx"
//          coding.
//
//  Input:: hArg: The keywords for finding the person.
//
// Output:: Pointer to the person on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Person* Person::ResolvePerson(const CHAR* hArg)
{
    PersonHolder nGotPeople;
    if(!ResolvePeople(hArg, nGotPeople))
        return NULL;
    
    if(nGotPeople.Size() != 1)
    {
        Send("You may only do this with ONE person at a time.\n");
        return NULL;    
    }
    
    return nGotPeople.Front();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ResolvePeople
//
//   Desc:: Retrieve any number of people from the room this Person is in. Like
//          other Resolve-named methods it will alert the Person if the
//          resolution fails. This keeps down on repeative "You don't see xx"
//          coding.
//
//  Input:: hArg: The keywords for finding the people.
//          hHolder: Holds found People.
//
// Output:: True if any were found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::ResolvePeople(const CHAR* hArg, PersonHolder& hHolder)
{
    ASSERT(mInRoom);
    if(!mInRoom->RetrievePeopleAs(this, (CHAR*) hArg, hHolder))
    {
        SendF("You don't see ^H%s^x around anywhere.\n", hArg);
        return false;
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::FindLocation
//
//   Desc:: Finds a RoomIndex in the world determined by a textual argument. By
//          default a list of words or keywords will be assumed to point to a
//          Player, then it tries Mobiles, then Objects. If a virtual reference
//          is passed (1/bedroom) then it assumes a room is being requested
//          and searches for it accordingly.
//
//  Input:: The textual argument describing a location in the game.
//
// Output:: Non-NULL if a location was found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RoomIndex* Person::FindLocation(const STRINGCW& hArg)
{
    STRINGCW        nAreaStr, nThingStr;
    PersonHolder    nFindPerson;
    ObjectHolder    nFindObject;
    
    
    //** Find Room Location **//
    if(hArg.IsVRef())
        return MUD::Get()->ResolveRoomVRef(NULL, hArg);

    
    //** Goto Player **//
    if(MUD::Get()->mPlayers.RetrievePeopleAs(this, hArg, nFindPerson))
    {
        Person* nWantPlayer = *nFindPerson.Begin();
        ASSERT(nWantPlayer->mInRoom);
        return nWantPlayer->mInRoom;
    }
    
    
    //** Goto Mobile **//
    if(MUD::Get()->mMobiles.RetrievePeopleAs(this, hArg, nFindPerson))
    {
        Person* nWantMobile = *nFindPerson.Begin();
        ASSERT(nWantMobile->mInRoom);
        return nWantMobile->mInRoom;
    }
    
    
    //** Goto Object **//
    STRINGCW nObjArg = "all*"; nObjArg += hArg;
    if(MUD::Get()->mObjects.RetrieveObjectsAs(this, nObjArg, nFindObject))
    {
        Object* nWantObject;
        RoomIndex* nInRoom;
        for(ObjectIter i = nFindObject.Begin(); i != nFindObject.End(); ++i)
        {
            nWantObject = *i;
            nInRoom = dynamic_cast<RoomIndex*>(nWantObject->mInsideThis);
            if(nInRoom)
                return nInRoom;
        }
    }   

    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::FindExit
//
//   Desc:: Finds an exit in the room this Person is in. Searches directions
//          first then checks for exit keywords.
//
//  Input:: The textual argument describing the exit.
//
// Output:: Non-NULL if an exit was found, NULL otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Exit* Person::FindExit(const STRINGCW& hArg)
{
    ASSERT(mInRoom != NULL);
    RoomIndex*  nRoom = mInRoom;
    
    //Check North, East, South, West, Up, Down
    for(ULONG i = 0; i < kDirMax; ++i)
    if(gDirName[i].IsNamed(hArg))
    if(nRoom->mExits[i] && CanSee(nRoom->mExits[i]))
        return nRoom->mExits[i];
    
    //Check Exit Keywords
    for(ULONG i = 0; i < kDirMax; ++i)
    if(nRoom->mExits[i] && CanSee(nRoom->mExits[i]))
    if(nRoom->mExits[i]->mKeywords.IsNamed(hArg))
        return nRoom->mExits[i];
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ChangePosition
//
//   Desc:: Changes this Person's position to hToPos. It shows the correct
//          transition message for the change from the current position to the
//          new one. It doesn't do anything things like kPosStanding to
//          kPosDead, justs sets mPosition to kPosDead.
//
//  Input:: hToPos: A kPos position constant to change position to.
//          hUse: Optional. Object to use.
//          hHow: Optional. How to the the hUse object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::ChangePosition(BYTE hToPos, Object* hUse, BYTE hHow)
{
    //////////////////////////////////////////////////////////////////////////
    static const CHAR* sFromStanding[(kPosMax-kPosStunned)*2] =
    {
        /* Stunned  */ NULL                                         ,
        /* Stunned  */ NULL                                         ,
        /* Sleeping */ "You lay down and go to sleep.\n"            ,
        /* Sleeping */ "$n lays down and goes to sleep.\n"          ,
        /* Resting  */ "You lay down and rest.\n"                   ,
        /* Resting  */ "$n lays down and rests.\n"                  ,
        /* Sitting  */ "You sit down.\n"                            ,
        /* Sitting  */ "$n sits down.\n"                            ,
        /* Hunted   */ NULL                                         ,
        /* Hunted   */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Standing */ NULL                                         ,
        /* Standing */ NULL
    };
    //////////////////////////////////////////////////////////////////////////
    static const CHAR* sFromSitting[(kPosMax-kPosStunned)*2] =
    {
        /* Stunned  */ NULL                                         ,
        /* Stunned  */ NULL                                         ,
        /* Sleeping */ "You lay down and go to sleep.\n"            ,
        /* Sleeping */ "$n lays down and goes to sleep.\n"          ,
        /* Resting  */ "You lay down and rest.\n"                   ,
        /* Resting  */ "$n lays down and rests.\n"                  ,
        /* Sitting  */ NULL                                         ,
        /* Sitting  */ NULL                                         ,
        /* Hunted   */ NULL                                         ,
        /* Hunted   */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Standing */ "You stop sitting and stand up.\n"           ,
        /* Standing */ "$n stops sitting and stands up.\n"
    };
    //////////////////////////////////////////////////////////////////////////
    static const CHAR* sFromResting[(kPosMax-kPosStunned)*2] =
    {
        /* Stunned  */ NULL                                         ,
        /* Stunned  */ NULL                                         ,
        /* Sleeping */ "You get comfortable and fall sleep.\n"      ,
        /* Sleeping */ "$n gets comfortable and falls asleep.\n"    ,
        /* Resting  */ NULL                                         ,
        /* Resting  */ NULL                                         ,
        /* Sitting  */ "You sit up.\n"                              ,
        /* Sitting  */ "$n sits up.\n"                              ,
        /* Hunted   */ NULL                                         ,
        /* Hunted   */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Standing */ "You stop resting and stand up.\n"           ,
        /* Standing */ "$n stops resting and stands up.\n"
    };
    //////////////////////////////////////////////////////////////////////////
    static const CHAR* sFromSleeping[(kPosMax-kPosStunned)*2] =
    {
        /* Stunned  */ NULL                                         ,
        /* Stunned  */ NULL                                         ,
        /* Sleeping */ NULL                                         ,
        /* Sleeping */ NULL                                         ,
        /* Resting  */ "You wake up and lay on your back.\n"        ,
        /* Resting  */ "$n wakes up and lays on $s back.\n"         ,
        /* Sitting  */ "You wake and sit up.\n"                     ,
        /* Sitting  */ "$n wakes and sits up.\n"                    ,
        /* Hunted   */ NULL                                         ,
        /* Hunted   */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Fighting */ NULL                                         ,
        /* Standing */ "You wake and stand up drowsily.\n"          ,
        /* Standing */ "$n wakes and stands up drowsily.\n"
    };
    //////////////////////////////////////////////////////////////////////////
    ASSERT(hToPos >= kPosDead && hToPos < kPosMax);
    SHORT   nArrayPos   = (hToPos - kPosStunned)*2;
    bool    nShowable   = nArrayPos >= 0;
    const CHAR** sMessageTable = NULL;
    
    if(!nShowable)
    {
        mPosition = hToPos;
        return;
    }

    //Show
    switch(mPosition)
    {
        case kPosStanding: sMessageTable = sFromStanding; break;
        case kPosSitting : sMessageTable = sFromSitting ; break;
        case kPosResting : sMessageTable = sFromResting ; break;
        case kPosSleeping: sMessageTable = sFromSleeping; break;
    }
    if(sMessageTable && sMessageTable[nArrayPos])
    {
        if(!hUse)
        {
            Act(sMessageTable[nArrayPos  ], this, NULL, NULL, ToChar, kPosDead);
            Act(sMessageTable[nArrayPos+1], this, NULL, NULL, ToRoom);
        }else{
            STRINGCW nToCharStr = sMessageTable[nArrayPos  ];
            STRINGCW nToRestStr = sMessageTable[nArrayPos+1];
            const CHAR* nOnHowStrs[] = { "", "at", "on", "in" };
            CHAR nPunct;
            nPunct = nToCharStr[nToCharStr.Length()-2];
            nToCharStr.Erase(nToCharStr.Length()-2, 2);
            nToCharStr.SPrintF("%s %s $p%c\n", *nToCharStr, nOnHowStrs[hHow], nPunct);
            nPunct = nToRestStr[nToRestStr.Length()-2];
            nToRestStr.Erase(nToRestStr.Length()-2, 2);
            nToRestStr.SPrintF("%s %s $p%c\n", *nToRestStr, nOnHowStrs[hHow], nPunct);
            Act(nToCharStr, this, hUse, NULL, ToChar, kPosDead);
            Act(nToRestStr, this, hUse, NULL, ToRoom);
        }
    }

    mOnHow = hHow;
    mOnObject = hUse;
    mPosition = hToPos; 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::EquipMessages
//
//   Desc:: Show a change in equipment location. hToLoc can be any valid wear
//          location > 0, or kLocNone to show to everyone that this Person has
//          just removed the item.
//
//  Input:: hItem: The item to show Equip messages for.
//          hToLoc: The wear location the Equip messages are for.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::EquipMessages(Object* hItem, CHAR hToLoc)
{
    ASSERT(hItem != NULL);
    
    if(hToLoc > kLocNone)
        goto Equip_Object;
    if(hToLoc == kLocNone)
        goto Remove_Object;
    ASSERT(!"Unhandled item location.");    
    
    
    //** Show Messages For Equip **//
    Equip_Object:;
    if(hItem->IsLighting())
    {
        Act("You ignite your $o.\n", this, hItem, NULL, ToChar);
        Act("$n ignites $s $o.\n"  , this, hItem, NULL, ToRoom);
    }
    Act(gWearMessageTable[hToLoc].mWearPerson, this, hItem, NULL, ToChar);
    Act(gWearMessageTable[hToLoc].mWearOthers, this, hItem, NULL, ToRoom);
    return;
    
    
    //** Show Messages For Removal **//
    Remove_Object:;
    if(hItem->IsLighting())
    {
        Act("You deignite your $o.\n", this, hItem, NULL, ToChar);
        Act("$n deignites $s $o.\n"  , this, hItem, NULL, ToRoom);
    }
    Act("You remove your $o.\n", this, hItem, NULL, ToChar);
    Act("$n removes $s $o.\n"  , this, hItem, NULL, ToRoom);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::EquipObject
//
//   Desc:: Lowest level method for equiping an item on a Person. No checking
//          whatsoever is done. The item is just removed from whatever it is in
//          right now and added to the Person's mWearing ObjectHolder. Anything
//          magical like affects, etc, are applied. And it is moved into its
//          correct wearing location. kLocNone is valid to specify that this
//          Person has removed the item, but you are responsible for for the
//          item after that.
//
//  Input:: hItem: The item to Equip.
//          hToLoc: The wear location to Equip it.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::EquipObject(Object* hItem, CHAR hToLoc)
{
    ASSERT(hItem != NULL);
    
    if(hToLoc > kLocNone)
        goto Equip_Object;
    if(hToLoc == kLocNone)
        goto Remove_Object;
    ASSERT(!"Unhandled item location.");    
    
    
    
    //** Equip An Object **//
    Equip_Object:;
    ASSERT(GetEquipAt(hToLoc) == NULL);
    ASSERT(hItem->mInsideThis != &mWearing);
    
    hItem->RemoveFrom();
    mWearing.AddObject(hItem, true);
    hItem->mWearLoc = hToLoc;
    if(mInRoom && hItem->IsLighting())
        ++mInRoom->mLights;
    return;
    
    
    
    //** Remove An Object **//
    Remove_Object:;
    ASSERT(hItem->mInsideThis == &mWearing);

    if(mInRoom && hItem->IsLighting())
        --mInRoom->mLights;

    hItem->RemoveFrom();
    hItem->mWearLoc = kLocNone;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::GetEquipAt
//
//   Desc:: Searches the equipment this Person is wearing and returns the item
//          at the specified wear location. If there is no item at that wear
//          location it returns NULL.
//
//  Input:: The wear location of the object you want.
//
// Output:: A Non-NULL ptr to the object if found, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

Object* Person::GetEquipAt(CHAR hAtLoc)
{
    if(mWearing.Empty())
        return NULL;
        
    for(ObjectIter nScan = mWearing.Begin(); nScan != mWearing.End(); ++nScan)
    if((*nScan)->mWearLoc == hAtLoc)
        return *nScan;
    
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::ShowEquipTo
//
//   Desc:: Displays the equipment this Person is wearing to the hTo Person.
//          If they cannot see an item it will not be shown. You could rework
//          the code to show items you can't see and just show them as 
//          "something" easily.
//
//  Input:: Show equipment to this Person.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Person::ShowEquipTo(Person* hTo)
{
    ObjectHolder nEquipment;


    if(!mWearing.RetrieveObjectsAs(hTo, "all", nEquipment))
    {
        hTo->Send("Nothing.\n");
        return;
    }
    
    
    STRINGCW    nEquipNameAt[kLocMax];
    ObjectIter  nEqScan;
    Object*     nEquip;
    STRINGCW    nOut;
    
    
    for(nEqScan = mWearing.Begin(); nEqScan != mWearing.End(); ++nEqScan)
    {
        nEquip = *nEqScan;
        ASSERT(nEquip->mWearLoc > kLocNone);
        
        nEquipNameAt[nEquip->mWearLoc] = nEquip->GetNameTo(hTo);
    }
    
    
    for(ULONG i = 0; i < gWearFormatTable.size(); ++i)
    {
        bool        nShownSlot = false;
        CHAR        nWearLoc;
        
        for(LONG l = 0; l < kMaxWearFormatTypeLocs; ++l)
        {
            if((nWearLoc = gWearFormatTable[i].mLocations[l]) == kLocNone)
                continue;
            if(nEquipNameAt[nWearLoc].Empty())
                continue;
            
            const CHAR* nSpace = "                    ";
            const CHAR* nPart1 = nShownSlot ? nSpace : gWearFormatTable[i].mName;
            const CHAR* nPart2 = nEquipNameAt[nWearLoc];
            const CHAR* nPart3 = "";
             
            nOut += QCBuf("%s %-45s %s\n", nPart1, nPart2, nPart3);
            nShownSlot = true;
        }
    }
    
    hTo->Send(nOut);
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanHandle
//
//   Desc:: Can this Person handle picking up and taking this Object.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanHandle(Object* hObject)
{
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSee
//
//   Desc:: Can this Person see this other Person?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSee(Person* hCh)
{
    if(hCh != this)
    if(hCh->isPlayer && hCh->isPlayer->isImm && hCh->isPlayer->isImm->mInvisLevel)
    if(hCh->isPlayer->isImm->mInvisLevel >= GetTrust())
        return false;
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSee
//
//   Desc:: Can this Person see this Object?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSee(Object* hCh)
{
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSee
//
//   Desc:: Can this Person see this Room?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSee(RoomIndex* hRoom)
{
    if(IsImm())
        return true;
        
    return !hRoom->IsDark();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSee
//
//   Desc:: Can this Person see this Exit?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSee(Exit* hExit)
{
    return hExit->IsHidden() == false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanEnter
//
//   Desc:: Can this Person enter this room?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanEnter(RoomIndex* hRoom)
{
    if(hRoom->IsPrivate())
    {
        Send("That room is currently private.\n");
        return false;
    }


    SHORT nRoomUsers = hRoom->PersonHolder::Size();
    SHORT nWouldMove = 1 + CountUsers();
    if(mRiding)
        nWouldMove += mRiding->CountUsers();


    if(hRoom->mCapacity > 0 && nRoomUsers+nWouldMove > hRoom->mCapacity)
    {
        Send("It's too full in there for you.\n");
        return false;
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanMove
//
//   Desc:: Can this Person move this direction from their current location.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanMove(SHORT hInDir, bool hAsPassenger)
{
    ASSERT(hInDir >= 0 && hInDir < kDirMax);

    Exit* nOutExit = mInRoom->mExits[hInDir];


    //** No Exit There? **//
    if(!nOutExit || !CanSee(nOutExit))
    {
        SendF("Alas, going %s doesn't seem to be possible.\n", *gDirName[hInDir]);
        return false;       
    }


    if(!nOutExit->mToRoom)
    {
        SendF("Unlinked room exit, report this.\n");
        return false;
    }


    //** Cant Move If You're Riding Something You Don't Control **//
    if(!hAsPassenger && mRiding && mRiding->mFollowing != this)
    {
        Act("You are but a passenger on $N.", this, NULL, mRiding, ToChar);
        return false;
    }
    

    //** Closed Door? **//
    if(nOutExit->IsClosed())
    {
        Act("$x $i closed.\n", this, nOutExit, NULL, ToChar);
        return false;
    }
    

    //** In A Hiding Place? **//
    if(mInSpot)
    {
        SendF("You should probably get out from behind %s first.\n", *mInSpot->mName);
        return false;
    }
    
    
    return CanEnter(nOutExit->mToRoom); 
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::TryMove
//
//   Desc:: Try to move this Person this direction from their current room.
//
//  Input:: hInDir: The direction (like kDirEast).
//          hFollow: If any followers should follow this Person.
//
// Output:: True if this Person moved successfully in the direction.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::TryMove(SHORT hInDir, bool hFollow)
{
    ASSERT(hInDir >= 0 && hInDir < kDirMax);
        
    RoomIndex* nToRoom = mInRoom->mExits[hInDir]->mToRoom;
    RoomIndex* nOrigRoom = mInRoom;
    LONG nMoveLoss = 1; //MoveLoss(nToRoom);


    if(mRiding && mRiding->mFollowing == this)
    {
        TrigMove(this, kScriptTrig_Leave, gDirName[hInDir]);
        TrigMove(mRiding, kScriptTrig_Leave, gDirName[hInDir]);
        Act("You ride $N $t.\n" , this, ActStr(gDirName[hInDir]), mRiding, ToChar);
        Act("$n rides you $t.\n", this, ActStr(gDirName[hInDir]), mRiding, ToVict);
        Act("$n rides $N $t.\n" , this, ActStr(gDirName[hInDir]), mRiding, ToNotVict);
        mWasInRoom = mInRoom;
        RemoveFrom();
        PlaceIn(nToRoom);
        mRiding->mWasInRoom = mInRoom;
        mRiding->RemoveFrom();
        mRiding->PlaceIn(nToRoom);
        Act("$n rides $N in from $t.\n", this, ActStr(gRevDirMoveStrs[hInDir]), mRiding, ToNotVict);
        mInRoom->ShowTo(this, IsOpt(kOptAutoScan));
        mInRoom->ShowTo(mRiding, mRiding->IsOpt(kOptAutoScan));
        TrigMove(this, kScriptTrig_Enter, gRevDirName[hInDir]);
        TrigMove(mRiding, kScriptTrig_Enter, gRevDirName[hInDir]);
        mRiding->mMoves -= nMoveLoss;
    }
    else if(mRiding)
    {
        TrigMove(this, kScriptTrig_Leave, gDirName[hInDir]);
        Act("You ride $t upon $N.\n" , this, ActStr(gDirName[hInDir]), mRiding, ToChar);
        Act("$n rides $t upon $N.\n" , this, ActStr(gDirName[hInDir]), mRiding, ToNotVict);
        mWasInRoom = mInRoom;
        RemoveFrom();
        PlaceIn(nToRoom);
        Act("$n rides in from $t upon $N.\n", this, ActStr(gRevDirMoveStrs[hInDir]), mRiding, ToNotVict);
        mInRoom->ShowTo(this, IsOpt(kOptAutoScan));
        TrigMove(this, kScriptTrig_Enter, gRevDirName[hInDir]);
    }
    else
    {
        TrigMove(this, kScriptTrig_Leave, gDirName[hInDir]);
        Act("You head $t.\n", this, ActStr(gDirName[hInDir]), NULL, ToChar);
        Act("$n $v $t.\n", this, ActStr(gDirName[hInDir]), NULL, ToRoom);
        mWasInRoom = mInRoom;
        RemoveFrom();
        PlaceIn(nToRoom);
        Act("$n $v in from $t.\n", this, ActStr(gRevDirMoveStrs[hInDir]), NULL, ToRoom);
        mInRoom->ShowTo(this, IsOpt(kOptAutoScan));
        TrigMove(this, kScriptTrig_Enter, gRevDirName[hInDir]);
        mMoves -= nMoveLoss;
    }


    //** Gather Followers **//
    if(hFollow && nToRoom != nOrigRoom)
    {
        PersonIter nFolScanNext, nFolScan = nOrigRoom->PersonHolder::Begin();
        Person* nFollower;
        
        for(; nFolScan != nOrigRoom->PersonHolder::End(); nFolScan = nFolScanNext)
        {
            ++(nFolScanNext = nFolScan);
            nFollower = *nFolScan;
            
            if(nFollower == this || nFollower == mRiding)
                continue;
            if(nFollower->mFollowing != this && nFollower->mRiding != this)
                continue;   
            if(nFollower->mPosition < kPosStanding)
                continue;
            if(!nFollower->CanMove(hInDir, nFollower->mRiding == mRiding))
            {
                if(nFollower->mRiding == mRiding)
                    nFollower->mRiding = NULL;
                continue;
            }
            
            nFollower->TryMove(hInDir, true);
        }
    }

    
//  if(mRiding)
//      UsedAbil(ABIL_RIDE);
        
//  if(nToRoom->IsWater())
//      UsedAbil(ABIL_SWIM);


    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanStand
//
//   Desc:: Can this Person stand. Optionally on an object.
//
//  Input:: hUse: NULL or stand on an Object.
//          hHow: If standing on an Object, kStandIn, kStandOn, kStandAt.
//
// Output:: True if the Person can, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanStand(Object* hUse, BYTE hHow)
{
    if(hUse && !hUse->IsUsableHow(this, kPosStanding, hHow))
        return false;

    if(mPosition == kPosStanding)
    {
        Send("You're already standing genius.\n");
        return false;
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::TryStand
//
//   Desc:: Try to stand this Person, optionally on an Object.
//
//  Input:: hUse: NULL or stand on an Object.
//          hHow: If standing on an Object, kStandIn, kStandOn, kStandAt.
//
// Output:: True if this Person was able to stand.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::TryStand(Object* hUse, BYTE hHow)
{
    ChangePosition(kPosStanding, hUse, hHow);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSit
//
//   Desc:: Can this Person sit. Optionally on an object.
//
//  Input:: hUse: NULL or sit on an Object.
//          hHow: If sitting on an Object, kSitIn, kSitOn, kSitAt.
//
// Output:: True if the Person can, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSit(Object* hUse, BYTE hHow)
{
    if(hUse && !hUse->IsUsableHow(this, kPosSitting, hHow))
        return false;

    if(mPosition == kPosSitting)
    {
        Send("It seems you're already sitting.\n");
        return false;
    }

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::TrySit
//
//   Desc:: Try to sit this Person, optionally on an Object.
//
//  Input:: hUse: NULL or sit on an Object.
//          hHow: If sitting on an Object, kSitIn, kSitOn, kSitAt.
//
// Output:: True if this Person was able to sit.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::TrySit(Object* hUse, BYTE hHow)
{
    ChangePosition(kPosSitting, hUse, hHow);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanRest
//
//   Desc:: Can this Person rest. Optionally on an object.
//
//  Input:: hUse: NULL or rest on an Object.
//          hHow: If resting on an Object, kRestIn, kRestOn, kRestAt.
//
// Output:: True if the Person can, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanRest(Object* hUse, BYTE hHow)
{
    if(hUse && !hUse->IsUsableHow(this, kPosResting, hHow))
        return false;

    if(mPosition == kPosResting)
    {
        Send("It doesn't get much more resting than this.\n");
        return false;
    }

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::TryRest
//
//   Desc:: Try to rest this Person, optionally on an Object.
//
//  Input:: hUse: NULL or rest on an Object.
//          hHow: If resting on an Object, kRestIn, kRestOn, kRestAt.
//
// Output:: True if this Person was able to rest.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::TryRest(Object* hUse, BYTE hHow)
{
    ChangePosition(kPosResting, hUse, hHow);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::CanSleep
//
//   Desc:: Can this Person sleep. Optionally on an object.
//
//  Input:: hUse: NULL or sleep on an Object.
//          hHow: If sleeping on an Object, kSleepIn, kSleepOn, kSleepAt.
//
// Output:: True if the Person can, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::CanSleep(Object* hUse, BYTE hHow)
{
    if(hUse && !hUse->IsUsableHow(this, kPosSleeping, hHow))
        return false;

    if(mPosition == kPosSleeping)
    {
        Send("You're already sleeping.\n");
        return false;
    }

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Person::TrySleep
//
//   Desc:: Try to sleep this Person, optionally on an Object.
//
//  Input:: hUse: NULL or sleep on an Object.
//          hHow: If sleeping on an Object, kSleepIn, kSleepOn, kSleepAt.
//
// Output:: True if this Person was able to sleep.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool Person::TrySleep(Object* hUse, BYTE hHow)
{
    ChangePosition(kPosSleeping, hUse, hHow);
    return true;
}



bool Person::CanWear(Object* hItem, CHAR hInPos)
{
    ASSERT(hItem != NULL);
    ASSERT(hInPos > kLocNone);  


    ULONG nSlot = 0;

    for(ULONG i = 1; i < gWearTable.size() && !nSlot; ++i)
    for(ULONG w = 0; w < kMaxWearTypeLocs; ++w)
    if(gWearTable[i].mWearLocs[w] == hInPos)
    {
        nSlot = i;
        break;
    }
    
    ASSERT(nSlot != 0); //Wear Location hInPos not found in table?
    if(!IsSet(hItem->WearFlags(), gWearTable[nSlot].mWearBit))
    {
        Act("$P cannot be worn in the '$t' slot.\n", this, ActStr(gWearTable[nSlot].mName), hItem, ToChar, kPosDead);
        return false;
    }
    
    
    return true;
}




bool Person::TryWear(Object* hItem, CHAR hInPos)
{
    ASSERT(hItem != NULL);
    ASSERT(hItem->mInsideThis != &mWearing);

    EquipMessages(hItem, hInPos);
    EquipObject(hItem, hInPos);
    return true;
}



bool Person::CanRemove(Object* hItem)
{
    ASSERT(hItem != NULL);
    ASSERT(hItem->mInsideThis == &mWearing);
    
    
    return true;
}




bool Person::TryRemove(Object* hItem)
{
    ASSERT(hItem != NULL);
    ASSERT(hItem->mInsideThis == &mWearing);


    EquipMessages(hItem, kLocNone);
    EquipObject(hItem, kLocNone);
    AddObject(hItem, true); //Removed, Add To Inventory
    return true;
}






