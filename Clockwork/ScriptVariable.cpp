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
#include "ScriptVariable.h"
#include "ScriptFunction.h"
#include "ScriptDataType.h"
#include "ScriptLibrary.h"
#include "Script.h"


ScriptVariable::ScriptVariable(ScriptDataTypeDef*& hDef, void* hValue, bool hConst)
    : mCurrent(NULL)
    , mScope(0)
    , mConstant(hConst)
{
    ASSERT(hDef != NULL);
    mDef    = hDef;
    mType   = mDef->mType;
    
    if(mDef->mCreateMethod)
    {
        mValue = (*mDef->mCreateMethod)();
        mValue->OperatorEqual(hValue);
        //mValue->mvaluePtr = hValue;
    }
}


ScriptVariable::~ScriptVariable()
{
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptVariable::Execute
//
//   Desc:: Called on every ScriptVariable just before they are used. It calls
//          the variable function if there is one. It sets our mValue using its
//          OperatorEqual(void*) call to the value that is returned from the
//          function execution.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptVariable::Execute()
{
    if(mFunction != NULL && mValue != NULL) 
        mValue->OperatorEqual(mFunction->Execute());
}



