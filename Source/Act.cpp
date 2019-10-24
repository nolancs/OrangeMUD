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
#include "Person.h"
#include "Act.h"
#include "PersonHolder.h"
#include "RoomIndex.h"
#include "MUD.h"
#include "OrangeSession.h"
#include "Object.h"




void Act(const CHAR* hFormat, Person* hCh, ActThing* hAct1, ActThing* hAct2, LONG hTargets, 
        SHORT hMinPos, PersonHolder* hSentTo)
{
    ASSERT(hFormat != NULL);
    
    Person*     hVCh    = dynamic_cast<Person*>(hAct2);
    Person*     hTo     = NULL;
    PersonIter  nPScan, nPEnd;
    PersonList  nWorldPeople;
    
    if(!IsSet(hTargets, kActWorld))
    {
        ASSERT(hCh->mInRoom);
        
        nPScan  = hCh->mInRoom->PersonHolder::Begin();
        nPEnd   = hCh->mInRoom->PersonHolder::End();
    }
    else
    {
        SessionIter nSScan  = MUD::Get()->mSessions.begin();
        SessionIter nSEnd   = MUD::Get()->mSessions.end();
        
        for(; nSScan != nSEnd; ++nSScan)
        if((*nSScan)->mPerson)
            nWorldPeople.Add((*nSScan)->mPerson);
            
        nPScan  = nWorldPeople.begin();
        nPEnd   = nWorldPeople.end();
    }   
    
    for(; nPScan != nPEnd; ++nPScan)
    {
        hTo = *nPScan;
        
        if(!hTo->mSession || hTo->mPosition < hMinPos)
            continue;
        
        if(IsSet(hTargets, kActOnlySeeChar) && !hTo->CanSee(hCh))
            continue;
        if(IsSet(hTargets, kActOnlySeeVict) && !hTo->CanSee(hVCh))
            continue;
        if(IsSet(hTargets, kActChar) && hTo != hCh)
            continue;
        if(IsSet(hTargets, kActVict) && (hTo != hVCh || hTo == hCh))
            continue;
        if(IsSet(hTargets, kActNoChar) && hTo == hCh)
            continue;
        if(IsSet(hTargets, kActNoVict) && hTo == hVCh)
            continue;
        if(IsSet(hTargets, kActGChar) && !hTo->IsSameGroupAs(hCh))
            continue;
        if(IsSet(hTargets, kActGVict) && !hTo->IsSameGroupAs(hVCh))
            continue;
        if(IsSet(hTargets, kActNoGChar) && hTo->IsSameGroupAs(hCh))
            continue;
        if(IsSet(hTargets, kActNoGVict) && hTo->IsSameGroupAs(hVCh))
            continue;
        if(IsSet(hTargets, kActImm) && !hTo->IsImm())
            continue;
        if(IsSet(hTargets, kActMort) && hTo->IsImm())
            continue;
    
        hTo->Send(ActParseTo(hTo, hFormat, hCh, hAct1, hAct2));
        if(hSentTo)
            hSentTo->AddPerson(hTo);
    }
}




void Act(const CHAR* hFormat, RoomIndex* hRoom, ActThing* hAct1, ActThing* hAct2, LONG hTargets, 
        SHORT hMinPos, PersonHolder* hSentTo)
{
    if(!hRoom)
        return;
    
    if(hRoom->PersonHolder::Empty())
        return;
        
    Person* nFirstPerson = *hRoom->PersonHolder::Begin();
    ASSERT(nFirstPerson);
    
    Act(hFormat, nFirstPerson, hAct1, hAct2, hTargets, hMinPos, hSentTo);
}





STRINGCW ActParseTo(Person* hTo, const CHAR* hFormat, Person* hCh, ActThing* hAct1, ActThing* hAct2)
{
    ObjectHolder*   nAct1IsObjectHolder = dynamic_cast<ObjectHolder*>(hAct1);
    ObjectHolder*   nAct2IsObjectHolder = dynamic_cast<ObjectHolder*>(hAct2);
    PersonHolder*   nAct2IsPersonHolder = dynamic_cast<PersonHolder*>(hAct2);
    ULONG           nCombinedSize;
    STRINGCW        nText(kMaxStringLen);

    
    //** Mutiple People Specified In Act2 **//
    if(nAct2IsPersonHolder)
    {
        nAct2IsPersonHolder->CombineTo(hTo, PersonLookShortSame);
        nCombinedSize = nAct2IsPersonHolder->CombinedSize();
        
        for(ULONG i = 0; i < nCombinedSize; ++i)
        {
            nText += ActParseTo(hTo, hFormat, hCh, hAct1, nAct2IsPersonHolder->CombinedAt(i));
//          if(i+1 != nCombinedSize)
//              nText += "\n";
        }
        
        return nText;
    }
    
    
    //** Mutiple Objects Specified In Act1 **//
    if(nAct1IsObjectHolder && !dynamic_cast<Object*>(hAct1))
    {
        nAct1IsObjectHolder->CombineTo(hTo, ObjectLookShortSame);
        nCombinedSize = nAct1IsObjectHolder->CombinedSize();
        
        for(ULONG i = 0; i < nCombinedSize; ++i)
        {
            nText += ActParseTo(hTo, hFormat, hCh, nAct1IsObjectHolder->CombinedAt(i), hAct2);
//          if(i+1 != nCombinedSize)
//              nText += "\n";
        }
        
        return nText;
    }
    
    
    //** Mutiple Objects Specified In Act2 **//
    if(nAct2IsObjectHolder && !dynamic_cast<Object*>(hAct2) && !dynamic_cast<Person*>(hAct2))
    {
        nAct2IsObjectHolder->CombineTo(hTo, ObjectLookShortSame);
        nCombinedSize = nAct2IsObjectHolder->CombinedSize();
        
        for(ULONG i = 0; i < nCombinedSize; ++i)
        {
            nText += ActParseTo(hTo, hFormat, hCh, hAct1, nAct2IsObjectHolder->CombinedAt(i));
            if(i+1 != nCombinedSize)
                nText += "\n";
        }
        
        return nText;
    }
    
    
    
    
    //** No Multiple Anything, Simply Parse The Line **//
    LONG    i, nFormatLen = strlen(hFormat);
    CHAR    c;


    for(i = 0; i < nFormatLen; ++i)
    {
        c = hFormat[i];
        
        //Not Special, Add & Continue
        if(c != '$' || i+1 == nFormatLen)
        {
            nText += c;
            continue;
        }
    
        //Special, Grab Related Text
        c = hFormat[++i];
        
        if(islower(c))
        {
            switch(c)
            {
                //Person-hCh Only Codes
                case 'n':
                case 'e':
                case 'm':
                case 's':
                case 'v':
                case 'c':
                    if(hCh == NULL)
                        nText += STRINGCW().SPrintF("{NULL_Person1_On_%c_Code}", c);
                    else
                        nText += hCh->GetTextForCodeTo(hTo, c);
                    break;
                
                //Otherwise References Act1 Thing
                default:
                    if(hAct1 == NULL)
                        nText += STRINGCW().SPrintF("{NULL_Act1_On_%c_Code}", c);
                    else
                        nText += hAct1->GetTextForCodeTo(hTo, c);
                    break;
            }
        }
        else
        {
            //Uppercase Always References Act2 Thing
            if(hAct2 == NULL)
                nText += STRINGCW().SPrintF("{NULL_Act2_On_%c_Code}", c);
            else
                nText += hAct2->GetTextForCodeTo(hTo, LOWER(c));
        }
    }
    
    
    return nText.Capitalize();
}



