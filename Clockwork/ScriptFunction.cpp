/******************************************************************************
  Author: Matthew Nolan                       Clockwork Crossplatform Framework
    Date: April, 2001                                           [Crossplatform]
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
#include "CommonTypesCW.h"
#include "ScriptFunction.h"
#include "ScriptVariable.h"
#include "ScriptLibrary.h"



ScriptFunction::ScriptFunction()
    : mFuncDef(NULL)
    , mReturnType(kScriptDT_None)
{
    for(USHORT i = 0; i < kMaxScriptArg; i++)
        mArgs[i] = NULL;
}


ScriptFunction::~ScriptFunction()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptFunction::Execute
//
//   Desc:: This takes a function data and sets up the function call then runs
//          it. It will return the value the function returned. It does special
//          checks for 'current' variable which are used for dynamic master
//          copies and also makes a special check for the truth && and truth ||
//          functions.
//
// Output:: ScriptFunction result as void*.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void* ScriptFunction::Execute()
{
    void*       nArgumentValues[kMaxScriptArg];
    void*       nReturnValue;
    SHORT       i, nNumArgs;
    ScriptVariable* nArg;

    
    for(i = 0; i < kMaxScriptArg; ++i)
        nArgumentValues[i] = NULL;
    
    nNumArgs = abs(mFuncDef->mNumArgs);
    
    for(i = 0; i < nNumArgs; ++i)
    {
        nArg = mArgs[i]->mCurrent ? mArgs[i]->mCurrent : mArgs[i];
        
        //** Special Case **//
        //The || and && truth functions exec the variables on their own
        //to make sure they dont continue if the conditional statement
        //has already been answered, so we leave them alone here.
        if((mFuncDef->mFuncCall != TruthOr_)
        && (mFuncDef->mFuncCall != TruthAnd_))
            nArg->Execute();
        
        //Operators take ScriptVariable*, Functions take their void* mValuePtr
        nArgumentValues[i] = mFuncDef->mOp ? nArg : (nArg->mValue == NULL ? NULL : nArg->mValue->mValuePtr);
    }
    
    //if(mFunction->mName)
    //cout << "Script call: " << mFunction->mName << endl;
    
    //Call the function
    nReturnValue = (mFuncDef->mFuncCall)(nArgumentValues);
    return nReturnValue;
}
