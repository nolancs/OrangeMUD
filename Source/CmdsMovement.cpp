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
#include "StringUtils.h"
#include "Object.h"
#include "Act.h"
#include "Exit.h"
#include "ObjectIndex.h"



void Do_North(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirNorth, false))
        hCh->TryMove(kDirNorth, true);
}



void Do_East(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirEast, false))
        hCh->TryMove(kDirEast, true);
}



void Do_South(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirSouth, false))
        hCh->TryMove(kDirSouth, true);
}



void Do_West(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirWest, false))
        hCh->TryMove(kDirWest, true);
}



void Do_Up(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirUp, false))
        hCh->TryMove(kDirUp, true);
}



void Do_Down(Person* hCh, STRINGCW& hArg)
{
    if(hCh->CanMove(kDirDown, false))
        hCh->TryMove(kDirDown, true);
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_Stand(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        if(!hCh->CanStand())
            return;
        
        hCh->TryStand();
        return;
    }

    hCh->UseFurniture(kPosStanding, hArg);      
}



void Do_Sit(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        if(!hCh->CanSit())
            return;
        
        hCh->TrySit();
        return;
    }
    
    hCh->UseFurniture(kPosSitting, hArg);       
}




void Do_Rest(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        if(!hCh->CanRest())
            return;
        
        hCh->TryRest();
        return;
    }

    hCh->UseFurniture(kPosResting, hArg);       
}




void Do_Sleep(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        if(!hCh->CanSleep())
            return;
        
        hCh->TrySleep();
        return;
    }
    
    hCh->UseFurniture(kPosSleeping, hArg);      
}



void Do_Wake(Person* hCh, STRINGCW& hArg)
{
    if(hCh->mPosition > kPosSleeping)
    {
        hCh->Send("You're not sleeping\n");
        return;
    }
    
    if(hCh->CanSit() && hCh->TrySit())
    {
        STRINGCW noArg;
        Do_Look(hCh, noArg);
    }   
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




void Do_Open(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Open what?\n");
        return;
    }

    ObjectHolder    nContainers;
    STRINGCW        nOpenArg;
    
    hArg.OneArgument(nOpenArg);
    
    //** Open Object **//
    if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
    && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
    if(hCh->RetrieveObjectsHere(nOpenArg, nContainers, kFromInvWearRoom))
    {
        if(nContainers.Size() > 1)
        {
            hCh->Send("You can only open one thing at a time.\n");
            return;
        }
        
        Object* nContainer = nContainers.Front();
        
        if(!nContainer->IsContainer())
        {
            Act("$p isn't a container.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        if(!nContainer->IsClosed())
        {
            Act("$p is already open.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        if(nContainer->IsLocked())
        {
            Act("$p is locked.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        
        RemoveBit(nContainer->mContFlags, kContClosed);
        if(ObjInPerson(nContainer, hCh))
        {
            Act("You open your $o.\n", hCh, nContainer, NULL, ToChar);
            Act("$n opens $s $o.\n"  , hCh, nContainer, NULL, ToRoom);
        }
        else
        {
            Act("You open $p.\n", hCh, nContainer, NULL, ToChar);
            Act("$n opens $p.\n", hCh, nContainer, NULL, ToRoom);
        }
        return;
    }
    
    
    
    //** Open Door **//
    Exit* nExit;
    Exit* nOtherSide;
    
    if(!(nExit = hCh->FindExit(nOpenArg)))
    {
        if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
        && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
            hCh->SendF("There's no object or exit \"%s\" to open.\n", *nOpenArg);
        else
            hCh->Send("There's no exit in that direction.\n");
        return;
    }

    if(!nExit->IsDoor())
    {
        Act("The $d exit is not a door.\n", hCh, nExit, NULL, ToChar);
        return;
    }
    
    if(!nExit->IsClosed())
    {
        Act("$x $w $i already open.\n", hCh, nExit, NULL, ToChar);
        return;
    }
    
    if(nExit->IsLocked())
    {
        Act("$x $w $i locked.\n", hCh, nExit, NULL, ToChar);
        return;
    }


    //Open It
    nExit->Open(false);
    if((nOtherSide = nExit->GetOtherSideExit()) != NULL)
    {
        nOtherSide->Open(false);
        if(IsSet(nOtherSide->mFlags, kExPlural))
            Act("$x $w open.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
        else
            Act("$x $w opens.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
    }
    
    Act("You open $x $w.\n", hCh, nExit, NULL, ToChar);
    Act("$n opens $x $w.\n", hCh, nExit, NULL, ToRoom);
}



void Do_Close(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Close what?\n");
        return;
    }


    ObjectHolder    nContainers;
    STRINGCW        nOpenArg;
    
    hArg.OneArgument(nOpenArg);
    
    
    //** Close Object **//
    if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
    && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
    if(hCh->RetrieveObjectsHere(nOpenArg, nContainers, kFromInvWearRoom))
    {
        if(nContainers.Size() > 1)
        {
            hCh->Send("You can only close one thing at a time.\n");
            return;
        }
        
        Object* nContainer = nContainers.Front();
        
        if(!nContainer->IsContainer())
        {
            Act("$p isn't a container.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        if(nContainer->IsClosed())
        {
            Act("$p is already closed.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        
        //Close It
        SetBit(nContainer->mContFlags, kContClosed);
        if(ObjInPerson(nContainer, hCh))
        {
            Act("You close your $o.\n", hCh, nContainer, NULL, ToChar);
            Act("$n closes $s $o.\n"  , hCh, nContainer, NULL, ToRoom);
        }
        else
        {
            Act("You close $p.\n", hCh, nContainer, NULL, ToChar);
            Act("$n closes $p.\n", hCh, nContainer, NULL, ToRoom);
        }
        return;
    }
    
    
    
    //** Close Door **//
    Exit* nExit;
    Exit* nOtherSide;
    
    if(!(nExit = hCh->FindExit(nOpenArg)))
    {
        if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
        && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
            hCh->SendF("There's no object or exit \"%s\" to close.\n", *nOpenArg);
        else
            hCh->Send("There's no exit in that direction.\n");
        return;
    }

    if(!nExit->IsDoor())
    {
        Act("The $d exit is not a door.\n", hCh, nExit, NULL, ToChar);
        return;
    }
    
    if(nExit->IsClosed())
    {
        Act("$x $w $i already closed.\n", hCh, nExit, NULL, ToChar);
        return;
    }
    
    //Close It
    nExit->Close(false);
    if((nOtherSide = nExit->GetOtherSideExit()) != NULL)
    {
        nOtherSide->Close(false);
        if(IsSet(nOtherSide->mFlags, kExPlural))
            Act("$x $w close.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
        else
            Act("$x $w closes.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
    }

    Act("You close $x $w.\n", hCh, nExit, NULL, ToChar);
    Act("$n closes $x $w.\n", hCh, nExit, NULL, ToRoom);
}



void Do_Lock(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Lock what?\n");
        return;
    }


    ObjectHolder    nContainers;
    STRINGCW        nOpenArg;
    
    hArg.OneArgument(nOpenArg);
    
    //** Lock Object **//
    if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
    && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
    if(hCh->RetrieveObjectsHere(nOpenArg, nContainers, kFromInvWearRoom))
    {
        if(nContainers.Size() > 1)
        {
            hCh->Send("You can only lock one thing at a time.\n");
            return;
        }
        
        Object* nContainer  = nContainers.Front();
        VREF    nKeyVRef    = nContainer->ContKey();
        Object* nKey        = NULL;
        

        //** Is A Container **//
        if(!nContainer->IsContainer())
        {
            Act("$p isn't a container.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Container Is Lockable **//
        if(!IsSet(nContainer->mContFlags, kContLockable))
        {
            Act("$p cannot be locked.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Not Even Closed **//
        if(!nContainer->IsClosed())
        {
            Act("$p isn't closed.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Locked Already **//
        if(nContainer->IsLocked())
        {
            Act("$p is already locked.\n", hCh, nContainer, NULL, ToChar);
            return;
        }
        
        //** Check For Key **//
        if(nKeyVRef == kNoKey)
        {
            Act("$p doesn't have a keyhole.\n", hCh, nContainer, NULL, ToChar);
            return;
        }else
        if(!(nKey = hCh->RetrieveObjectHere(nKeyVRef, kFromInvWear)))
        {
            Act("You don't have the key to lock $p.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        
        //** DONE! Lock Container **//  
        SetBit(nContainer->mContFlags, kContLocked);
        if(ObjInPerson(nContainer, hCh))
        {
            Act("You lock your $o with $P.\n", hCh, nContainer, nKey, ToChar);
            Act("$n locks $s $o with $P.\n"  , hCh, nContainer, nKey, ToRoom);
        }
        else
        {
            Act("You lock $p with $P.\n", hCh, nContainer, nKey, ToChar);
            Act("$n locks $p with $P.\n", hCh, nContainer, nKey, ToRoom);
        }
        return;
    }
    
    
    
    //** Lock Door **//
    VREF    nKeyVRef    = kNoKey;
    Object* nKey        = NULL;
    Exit*   nOtherSide;
    Exit*   nExit;
    

    if(!(nExit = hCh->FindExit(nOpenArg)))
    {
        if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
        && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
            hCh->SendF("There's no object or exit \"%s\" to open.\n", *nOpenArg);
        else
            hCh->Send("There's no exit in that direction.\n");
        return;
    }

    //** Is It A Door **//
    if(!nExit->IsDoor())
    {
        Act("The $d exit is not a door.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Is The Door Closed **//
    if(!nExit->IsClosed())
    {
        Act("$x $w $i not closed.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Already Locked **//
    if(nExit->IsLocked())
    {
        Act("$x $w $i already locked.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Lockable Door Or If So, Have The Key **//
    nKeyVRef = nExit->mKey;
    if(nKeyVRef == kNoKey)
    {
        Act("There's no keyhole in $x.\n", hCh, nExit, NULL, ToChar);
        return;
    }else
    if(!(nKey = hCh->RetrieveObjectHere(nKeyVRef, kFromInvWear)))
    {
        Act("You don't have the key to lock $x.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Is The Lock Broken **//
    if(IsSet(nExit->mFlags, kExLockBroken))
    {
        Act("$x $w won't lock, even with the $O.\n", hCh, nExit, nKey, ToChar);
        Act("$n can't lock $x $w, even with the $O.\n", hCh, nExit, nKey, ToRoom);
        return;
    }   


    //** DONE! Lock Both Sides **//
    SetBit(nExit->mFlags, kExLocked);
    if((nOtherSide = nExit->GetOtherSideExit()) != NULL)
    {
        SetBit(nOtherSide->mFlags, kExLocked);
        if(IsSet(nOtherSide->mFlags, kExPlural))
            Act("$x $w lock from the other side.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
        else
            Act("$x $w locks from the other side.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
    }
    
    Act("You lock $x $w with $P.\n", hCh, nExit, nKey, ToChar);
    Act("$n locks $x $w with $P.\n", hCh, nExit, nKey, ToRoom);
}




void Do_Unlock(Person* hCh, STRINGCW& hArg)
{
    if(hArg.Empty())
    {
        hCh->Send("Unlock what?\n");
        return;
    }


    ObjectHolder    nContainers;
    STRINGCW        nOpenArg;
    
    hArg.OneArgument(nOpenArg);
    
    //** Unlock Object **//
    if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
    && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
    if(hCh->RetrieveObjectsHere(nOpenArg, nContainers, kFromInvWearRoom))
    {
        if(nContainers.Size() > 1)
        {
            hCh->Send("You can only unlock one thing at a time.\n");
            return;
        }
        
        Object* nContainer  = *nContainers.Begin();
        VREF    nKeyVRef    = nContainer->ContKey();
        Object* nKey        = NULL;
        

        //** Is A Container **//
        if(!nContainer->IsContainer())
        {
            Act("$p isn't a container.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Container Is Lockable **//
        if(!IsSet(nContainer->mContFlags, kContLockable))
        {
            Act("$p cannot be locked.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Unlocked Already **//
        if(!nContainer->IsLocked())
        {
            Act("$p isn't locked.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        //** Check For Key **//
        if(nKeyVRef == kNoKey)
        {
            Act("$p doesn't have a keyhole.\n", hCh, nContainer, NULL, ToChar);
            return;
        }else
        if(!(nKey = hCh->RetrieveObjectHere(nKeyVRef, kFromInvWear)))
        {
            Act("You don't have the key to unlock $p.\n", hCh, nContainer, NULL, ToChar);
            return;
        }

        
        //** DONE! Unlock Container **//    
        RemoveBit(nContainer->mContFlags, kContLocked);
        if(ObjInPerson(nContainer, hCh))
        {
            Act("You unlock your $o with $P.\n", hCh, nContainer, nKey, ToChar);
            Act("$n unlocks $s $o with $P.\n"  , hCh, nContainer, nKey, ToRoom);
        }
        else
        {
            Act("You unlock $p with $P.\n", hCh, nContainer, nKey, ToChar);
            Act("$n unlocks $p with $P.\n", hCh, nContainer, nKey, ToRoom);
        }
        return;
    }
    
    
    
    //** Unlock Door **//
    VREF    nKeyVRef    = kNoKey;
    Object* nKey        = NULL;
    Exit*   nOtherSide;
    Exit*   nExit;
    

    if(!(nExit = hCh->FindExit(nOpenArg)))
    {
        if(StrCmp(nOpenArg, "n") && StrCmp(nOpenArg, "e") && StrCmp(nOpenArg, "s") 
        && StrCmp(nOpenArg, "w") && StrCmp(nOpenArg, "u") && StrCmp(nOpenArg, "d"))
            hCh->SendF("There's no object or exit \"%s\" to open.\n", *nOpenArg);
        else
            hCh->Send("There's no exit in that direction.\n");
        return;
    }

    //** Is It A Door **//
    if(!nExit->IsDoor())
    {
        Act("The $d exit is not a door.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Already Unlocked **//
    if(!nExit->IsLocked())
    {
        Act("$x $w $i not locked.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Lockable Door Or If So, Have The Key **//
    nKeyVRef = nExit->mKey;
    if(nKeyVRef == kNoKey)
    {
        Act("$x doesn't have a keyhole.\n", hCh, nExit, NULL, ToChar);
        return;
    }else
    if(!(nKey = hCh->RetrieveObjectHere(nKeyVRef, kFromInvWear)))
    {
        Act("You don't have the key to unlock $x.\n", hCh, nExit, NULL, ToChar);
        return;
    }

    //** Is The Lock Broken **//
    if(IsSet(nExit->mFlags, kExLockBroken))
    {
        Act("$x $w won't unlock, even with the $O.\n", hCh, nExit, nKey, ToChar);
        Act("$n can't unlock $x $w, even with the $O.\n", hCh, nExit, nKey, ToRoom);
        return;
    }   


    //** DONE! Lock Both Sides **//
    RemoveBit(nExit->mFlags, kExLocked);
    if((nOtherSide = nExit->GetOtherSideExit()) != NULL)
    {
        RemoveBit(nOtherSide->mFlags, kExLocked);
        if(IsSet(nOtherSide->mFlags, kExPlural))
            Act("$x $w unlock from the other side.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
        else
            Act("$x $w unlocks from the other side.\n", nOtherSide->mInRoom, nOtherSide, NULL, ToAll);
    }
    
    Act("You unlock $x $w with $P.\n", hCh, nExit, nKey, ToChar);
    Act("$n unlocks $x $w with $P.\n", hCh, nExit, nKey, ToRoom);
}



