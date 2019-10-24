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
#ifndef SCRIPTLIBRARY_H
#define SCRIPTLIBRARY_H 1
#include "Script.h"
#include "ScriptDataType.h"

///////////////////////////////////////////////////////////////////////////////
// Structures For The Scripting Library
///////////////////////////////////////////////////////////////////////////////

struct ScriptFuncDef
{
    CHAR*           mName;
    ScriptingFunc*  mFuncCall;
    SHORT           mOp;
    SHORT           mReturns;
    SHORT           mNumArgs;
    SHORT           mArgTypes[kMaxScriptArg];
    CHAR*           mArgNames[kMaxScriptArg];
    CHAR*           mHelp;
};

struct ScriptConstDef
{
    CHAR*               mName;
    SHORT               mType;
    ScriptVariable*     mVariable;
};

struct ScriptDataTypeDef
{
    CHAR*           mName;
    SHORT           mType;
    CreateDTFunc*   mCreateMethod;
    CHAR*           mHelp;
};



///////////////////////////////////////////////////////////////////////////////
// Standard Scripting Library Functions & Variables
///////////////////////////////////////////////////////////////////////////////
extern ScriptFuncDef        gBaseFunctionTable[];
extern ScriptFuncDef        gBaseOperatorTable[];
extern ScriptFuncDef        gBaseFlowControlTable[];
extern ScriptDataTypeDef    gBaseDataTypeTable[];
extern ScriptConstDef       gBaseConstantTable[];

ScriptingFunc       MathAdd_;
ScriptingFunc       MathSubtract_;
ScriptingFunc       MathMultiply_;
ScriptingFunc       MathDivide_;
ScriptingFunc       MathOr_;
ScriptingFunc       MathAnd_;
ScriptingFunc       EqEqual_;
ScriptingFunc       EqAdd_;
ScriptingFunc       EqSubtract_;
ScriptingFunc       EqMultiply_;
ScriptingFunc       EqDivide_;
ScriptingFunc       TruthEqual_;
ScriptingFunc       TruthNotEqual_;
ScriptingFunc       TruthGThenEq_;
ScriptingFunc       TruthLThenEq_;
ScriptingFunc       TruthGThen_;
ScriptingFunc       TruthLThen_;
ScriptingFunc       TruthOr_;
ScriptingFunc       TruthAnd_;
ScriptingFunc       TruthNot_;

ScriptingFunc       FlowBreak_;
ScriptingFunc       FlowContinue_;
ScriptingFunc       FlowEnd_;


#endif //ifndef SCRIPTLIBRARY_H


