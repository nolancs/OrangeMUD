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
#include "Person.h"
#include "Social.h"




Social::Social()
{
}


Social::~Social()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: Social::Do
//
//   Desc:: Runs the social for a Person.
//
//  Input:: The Person/argument.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Social::Do(Person* hCh, STRINGCW& hArg)
{
    Person* nVictim = NULL;
    STRINGCW nTargetArg;
    
    hArg.OneArgument(nTargetArg);
    if(!nTargetArg.Empty())
    if(!(nVictim = hCh->ResolvePerson(nTargetArg)))
        return;
    
    //No Target
    if(nTargetArg.Empty())
    {
        if(!mCharNoArg.Empty())
            Act(mCharNoArg+"\n", hCh, NULL, nVictim, ToChar);
        if(!mOthersNoArg.Empty())
            Act(mOthersNoArg+"\n", hCh, NULL, nVictim, ToRoom);
    }
    //Target Is Self
    else if(nVictim == hCh)
    {
        if(!mCharAuto.Empty())
            Act(mCharAuto+"\n", hCh, NULL, nVictim, ToChar);
        if(!mOthersAuto.Empty())
            Act(mOthersAuto+"\n", hCh, NULL, nVictim, ToRoom);
    }
    //Target Is Another Person
    else
    {
        if(!mCharFound.Empty())
            Act(mCharFound+"\n", hCh, NULL, nVictim, ToChar);
        if(!mVictFound.Empty())
            Act(mVictFound+"\n", hCh, NULL, nVictim, ToVict);
        if(!mOthersFound.Empty())
            Act(mOthersFound+"\n", hCh, NULL, nVictim, ToNotVict);
    }
}

