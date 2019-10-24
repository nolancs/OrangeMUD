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
#include "MUD.h"
#include "Person.h"
#include "StringUtils.h"
#include "UIDHolder.h"




UIDHolder::UIDHolder()
{
}

UIDHolder::~UIDHolder()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



void UIDHolder::Add(UID hID)
{
    mUIDs.AddBack(hID);
}

void UIDHolder::Remove(UID hID)
{
    mUIDs.Remove(hID);
}

bool UIDHolder::Exists(UID hID)
{
    return mUIDs.Exists(hID);
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: UIDHolder::Edit
//
//   Desc:: Edits the list of UIDs as the given Person. Editing uses a list of
//          player names with plus or minus in front of them to signify adding
//          or removing. If trust is used then hCh can't remove anyone >= their
//          trust level.
//
//  Input:: hCh: Edit the list of UIDs as this Person.
//          hArg: Textual argument of +player and/or -minus player.
//          hUseTrust: Take into account the trust level of the players?
//
// Output:: True if the list was changed, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool UIDHolder::Edit(Person* hCh, STRINGCW& hArg,  bool hUseTrust)
{
    typedef pair<UIDIter,STRINGCW>  IDNamePair;
    typedef Vector<IDNamePair>      IDNamePairArray;
    ASSERT(hCh != NULL);
    IDNamePairArray nUIDAndNames;

    for(UIDIter i = mUIDs.begin(); i != mUIDs.end(); ++i)
    {
        const CHAR* nName = MUD::Get()->ResolveUIDToName(*i);
        if(!nName)
        {
            mUIDs.erase(i);
            continue;
        }
        nUIDAndNames.Add(IDNamePair(i, STRINGCW(nName)));
    }

    STRINGCW nEditArg;
    bool nEdited = false;
    for(hArg = hArg.OneArgument(nEditArg); !nEditArg.Empty(); hArg = hArg.OneArgument(nEditArg))
    {
        //** Remove A Player From The List **//
        if(nEditArg[0] == '-')
        {
            nEditArg.Erase(0,1);
            bool nRemovedAPlayer = false;
            for(ULONG i = 0; i < nUIDAndNames.size(); ++i)
            if(!StrCmp(nEditArg, nUIDAndNames[i].second))
            {
                hCh->SendF("Removed player '%s' from the list.\n", *nEditArg);
                mUIDs.erase(nUIDAndNames[i].first);
                nUIDAndNames.RemoveAt(i);
                nRemovedAPlayer = true;
                nEdited = true;
                break;
            }
            
            if(!nRemovedAPlayer)
                hCh->SendF("No such player %s.\n", *nEditArg);
        }
        //** Add A Player To The List **//
        else if(nEditArg[0] == '+')
        {
            nEditArg.Erase(0,1);
            UID nAddUID = MUD::Get()->ResolveNameToUID(nEditArg);
            if(nAddUID == 0)
            {
                hCh->SendF("No such player as \"%s\"\n", *nEditArg);
                continue;
            }
            UIDIter nAlready = std::find(mUIDs.begin(), mUIDs.end(), nAddUID);
            if(nAlready != mUIDs.end())
            {
                hCh->SendF("%s already exists.\n", *nEditArg);
                continue;
            }
            mUIDs.AddBack(nAddUID);
            UIDIter nAddIter = mUIDs.end();
            nUIDAndNames.Add(IDNamePair(--nAddIter,nEditArg));
            nEdited = true;
            hCh->SendF("Added player '%s' to the list.\n", *nEditArg);
        }
        else
            hCh->SendF("Invalid \"%s\" argument. Must be +player or -player.\n", *nEditArg);
    }

    return nEdited;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: UIDHolder::ResolveNames
//
//   Desc:: Resolves every UID in the list into the name of the player it
//          represents and puts that name into a vector of strings.
//
//  Input:: The vector of strings to hold the names.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void UIDHolder::ResolveNames(Vector<StringCW>& hNames)
{
    for(UIDCIter i = mUIDs.begin(); i != mUIDs.end(); ++i)
    {
        const CHAR* nName = MUD::Get()->ResolveUIDToName(*i);
        hNames.AddBack(STRINGCW(nName ? nName : "<Unresolvable-UID>"));
    }
}

