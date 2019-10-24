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
#include "ScriptExecutor.h"
#include "ScriptVariable.h"
#include "ScriptDataType.h"
#include "ScriptCodeLine.h"
#include "ScriptFunction.h"
#include "ScriptConditional.h"
#include "ScriptBinary.h"
#include "ScriptLibrary.h"
#include "Script.h"

#define STEP_BY_STEP_MSG(x)  
//#define   STEP_BY_STEP_MSG(x)  printf(x);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::ScriptExecutor
//
//   Desc:: Construction and initialization of an executor to run hThisScript.
//          The script has to be compiled and have a working binary. A copy of
//          of the the binary's mVariables will be made (the only variales that
//          can change during execution) so that version of the variables local
//          to this executor can be held, allowing multiple simultaneous
//          executors to execute over the same binary.
//
//   Note:: You would want to derive from this class if your own types of
//          scripts require special initialization and runtime setup.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptExecutor::ScriptExecutor(Script* hThisScript)
    : mEnded(false)
    , mWait(0)
    , mScript(hThisScript)
{
    ASSERT(mScript != NULL);
    ASSERT(mScript->mBinary != NULL);
    //We need to make a duplicate of every variable in the binary's mVariable
    //list because multiple executors can exist on a single binary (to save
    //space/time) so each executor stores a copy of what it knows each variable
    //to be and they can vary from executor to executor. Then before we execute
    //we set the binary's variables' mCurrent pts to point to our mRTVariables.
    ScriptVariableList&     nList = mScript->mBinary->mVariables;
    ScriptVariableIter      i;
    ScriptVariable*         nOrig, *nCopy;
    
    for(i = nList.begin(); i != nList.end(); ++i)
    {
        nOrig = *i;
        nCopy = new ScriptVariable(nOrig->mDef);
        nCopy->mValue->OperatorEqual(nOrig);
        mRTVariables.AddBack(nCopy);
    }
    
    mScopeStack.push(&mScript->mBinary->mCode); 
    mStack.push(mScopeStack.top()->begin());
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::~ScriptExecutor
//
//   Desc:: Deconstructor.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptExecutor::~ScriptExecutor()
{
    for(ScriptVariableIter i = mRTVariables.begin(); i != mRTVariables.end(); ++i)
        delete *i;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::Done
//
//   Desc:: Returns true if the execution is done.
//
// Output:: True if done, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptExecutor::Done()
{
    return mEnded;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::Execute
//
//   Desc:: Execute is called by your ScriptExecutor handler. It will execute
//          the script until it comes to a point where mWait > 0 or the flow
//          control 'end' is used or the script is out of code. Swaps the
//          runtime variables in and out during execution.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptExecutor::Execute()
{
    SwapRuntimeIn();
    
    while(mWait == 0 && !Done())
        ExecuteStep();
    
    SwapRuntimeOut();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::ExecuteStep
//
//   Desc:: A single step in execution. It runs the function associated with
//          the line and takes care of advancing it to the next codeline.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptExecutor::ExecuteStep()
{
    ScriptCodeLineList* nList       = mScopeStack.top();
    ScriptCodeLineIter  nLineIter   = mStack.top();
    ScriptCodeLine*     nLine       = nLineIter == nList->end() ? NULL : *nLineIter;
    ASSERT(nLine != NULL);

    if(nLine->mFunction)
    {
        ScriptingFunc* nTheCall = nLine->mFunction->mFuncDef->mFuncCall;
        if(FlowBreak_ == nTheCall)
        {
        }
        if(FlowContinue_ == nTheCall)
        {
        }
        if(FlowEnd_ == nTheCall)
        {
            mEnded = true;
            return;
        }
        
        STEP_BY_STEP_MSG("|...| Executing Line\n");
        nLine->mFunction->Execute();
    }
    

    bool nIsTrue = false;
    if(nLine->IsConditional() && !nLine->IsConditional(kScriptCond_Else))
    {
        ASSERT(nLine->mConditional->mEval != NULL);         
        nLine->mConditional->mEval->Execute();
        nIsTrue = (nLine->mConditional->mEval->mValue->mValuePtr != 0);
        if(nIsTrue)
        {
            STEP_BY_STEP_MSG("|>>>| Conditional Succeded, Entering\n");
            mStack.push(nLine->mConditional->mCode.begin()); //Enter conditional's code
            mScopeStack.push(&nLine->mConditional->mCode);
            return;
        }
    }
    
    
    if(nIsTrue == false) //Check for an }else{ conditional following if's
    if(nLine->IsConditional(kScriptCond_If) || nLine->IsConditional(kScriptCond_ElseIf))
    {
        if(++nLineIter != nList->end() && (*nLineIter)->IsConditional(kScriptCond_Else))
        {
            STEP_BY_STEP_MSG("|>>>| Conditional Failed, Else\n");
            mStack.push((*nLineIter)->mConditional->mCode.begin()); //Enter else's code
            mScopeStack.push(&(*nLineIter)->mConditional->mCode);
            return;
        }
        else
            --nLineIter;
    }
    
    
    //Advance to the next line of code
    Advance:;
    if(++nLineIter != nList->end())
    {
        //STEP_BY_STEP_MSG("|...| Advancing...\n");
        mStack.top() = nLineIter;
        return;
    }
    
    STEP_BY_STEP_MSG("|<<<| Exiting Conditional\n");
    mStack.pop();
    mScopeStack.pop();
    if(mStack.empty())
    {
        mEnded = true;
        return;
    }
    nList = mScopeStack.top();
    nLineIter = mStack.top();
    nLine = *nLineIter;

    //If we just left an If/ElseIf conditional that means we entered it for
    //evaluating to true. Now that it's contents have been executed we have 
    //to skip past any IfElse/Else following it because they don't need to 
    //be executed.
    if(nLine->IsConditional(kScriptCond_If) 
    || nLine->IsConditional(kScriptCond_ElseIf))
    {
        while(true)
        {
            nLine = ++nLineIter == nList->end() ? NULL : *nLineIter;
            if(nLine == NULL)
                break;
            if(!nLine->IsConditional(kScriptCond_Else) 
            && !nLine->IsConditional(kScriptCond_ElseIf))
                break;
        }
        --nLineIter;
    }
    else
    //If we just left a While/For loop, stay on that conditional's line, it 
    //will advance over it and continue on when the While/For conditional 
    //finally evaluates to false.
    if(nLine->IsConditional(kScriptCond_While))
    {
        STEP_BY_STEP_MSG("|...| Continuing While Loop\n");
        return;
    }
    else
    if(nLine->IsConditional(kScriptCond_For))
    {
        STEP_BY_STEP_MSG("|...| Continuing For Loop\n");
        //WRITE ME//
        //Execute the x++ part of the "for(x = 0; x != 10; x++)" conditional
        return;
    }


    //Finish advancing
    mStack.top() = nLineIter;
    goto Advance;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::SwapRuntimeIn
//
//   Desc:: Called before the executor begins executing over the binary to make
//          sure our local runtime variables are in the mCurrent ptr of the
//          binary's master variables.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptExecutor::SwapRuntimeIn()
{
    ASSERT(mScript->mBinary != NULL);
    ScriptVariableList&     nBinList = mScript->mBinary->mVariables;
    ScriptVariableList&     nRunList = mRTVariables;
    ScriptVariableIter      nBinIter, nRunIter;

    ASSERT(nBinList.size() == nRunList.size());
    for(nBinIter = nBinList.begin(), nRunIter = nRunList.begin(); nBinIter != nBinList.end();)
    {
        (*nBinIter)->mCurrent = (*nRunIter);
        ++nBinIter;
        ++nRunIter;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::SwapRuntimeOut
//
//   Desc:: Called after the executor finishes executing over the binary, the
//          execution may be over or it may have just hit a wait state. This
//          method is required to reset all the mCurrent ptrs we set with the
//          SwapRuntimeIn back to NULL.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptExecutor::SwapRuntimeOut()
{
    ASSERT(mScript->mBinary != NULL);
    ScriptVariableList&     nBinList = mScript->mBinary->mVariables;
    ScriptVariableIter      nBinIter;

    ASSERT(nBinList.size() == mRTVariables.size());
    for(nBinIter = nBinList.begin(); nBinIter != nBinList.end(); ++nBinIter)
        (*nBinIter)->mCurrent = NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptExecutor::DecrementWait
//
//   Desc:: Called to decrement the wait state by one. Your own update function
//          would use this call on ScriptExecutors that are waiting until zero
//          before they can continue execution.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptExecutor::DecrementWait()
{
    if(mWait > 0)
        mWait--;
}




