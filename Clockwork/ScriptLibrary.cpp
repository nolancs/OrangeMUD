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
#include "Script.h"
#include "ScriptLibrary.h"
#include "ScriptVariable.h"

#define SCRIPTING_ASSERT(x)     do{if(!(x)){printf("Scripting Assertion ("#x") failed\n");return NULL;}}while(0)

//## Macros For Easy Function Setting ##//
///////////////////////////////////////////////////////////////////////////////
#define Arg0                                            0,{xx,xx,xx,xx,xx,xx,xx},{NULL,NULL,NULL,NULL,NULL,NULL,NULL}
#define Neg1(t1,n1)                                    -1,{t1,xx,xx,xx,xx,xx,xx},{n1,NULL,NULL,NULL,NULL,NULL,NULL}
#define Arg1(t1,n1)                                     1,{t1,xx,xx,xx,xx,xx,xx},{n1,NULL,NULL,NULL,NULL,NULL,NULL}
#define Arg2(t1,n1,t2,n2)                               2,{t1,t2,xx,xx,xx,xx,xx},{n1,n2,NULL,NULL,NULL,NULL,NULL}
#define Arg3(t1,n1,t2,n2,t3,n3)                         3,{t1,t2,t3,xx,xx,xx,xx},{n1,n2,n3,NULL,NULL,NULL,NULL}
#define Arg4(t1,n1,t2,n2,t3,n3,t4,n4)                   4,{t1,t2,t3,t4,xx,xx,xx},{n1,n2,n3,n4,NULL,NULL,NULL}
#define Arg5(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5)             5,{t1,t2,t3,t4,t5,xx,xx},{n1,n2,n3,n4,n5,NULL,NULL}
#define Arg6(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6)       6,{t1,t2,t3,t4,t5,t6,xx},{n1,n2,n3,n4,n5,n6,NULL}
#define Arg7(t1,n1,t2,n2,t3,n3,t4,n4,t5,n5,t6,n6,t7,n7) 7,{t1,t2,t3,t4,t5,t6,t7},{n1,n2,n3,n4,n5,n6,n7}

#define OF(xName,xFunc,xOpLev,xRet,xARGSx)  {xName,xFunc,xOpLev,xRet,xARGSx,NULL}
#define SF(xName,xFunc,xRet,xARGSx,xHelp)   {xName,xFunc,0     ,xRet,xARGSx,xHelp}
#define DT(xName,xType,xCreate)             {xName,xType,xCreate
#define CT(xName,xType,xDef,xValue)         {xName,xType, new ScriptVariable(xDef,(void*)xValue, true)}
#define NoHelp                              ,NULL}
#define WithHelp(xHelp)                     ,xHelp}
#define EmptyOperatorDecl                   OF(NULL, NULL, 0, 0, Arg0)
#define EmptyFunctionDecl                   SF(NULL, NULL, 0, Arg0, NULL)
#define EmptyDataTypeDecl                   DT(NULL, 0, NULL) NoHelp
#define EmptyConstantDecl                   CT(NULL,0,gDTDef_None,NULL)


#define xx kScriptDT_None
#define fu kScriptDT_None
#define st kScriptDT_STRING
#define in kScriptDT_INT

#define ch kScriptDT_PERSON
#define ob kScriptDT_OBJECT
#define ro kScriptDT_ROOM
#define di kScriptDT_DIR
#define bo kScriptDT_BOOL
#define af kScriptDT_AFFECT
///////////////////////////////////////////////////////////////////////////////





//** Base Function Table **//
ScriptFuncDef gBaseFunctionTable[] = 
{
    EmptyFunctionDecl
};




//** Base Operator Table **// 
// Format of: x <op> y
//  * means you can pass a variable or a mValue
//  x means they must pass a variable (not mValue)
//  > means it references something inside the previous structure
// Negative num args means the operator works before the hArgList, not after/between.
ScriptFuncDef gBaseOperatorTable[] = 
{
    //Math               OpLevel  NumArgs
    OF("+"  , MathAdd_          , 2, in, Arg2(in,"*", in,"*")),
    OF("-"  , MathSubtract_     , 2, in, Arg2(in,"*", in,"*")),
    OF("*"  , MathMultiply_     , 2, in, Arg2(in,"*", in,"*")),
    OF("/"  , MathDivide_       , 2, in, Arg2(in,"*", in,"*")),
    OF("|"  , MathOr_           , 2, in, Arg2(in,"*", in,"*")),
    OF("&"  , MathAnd_          , 2, in, Arg2(in,"*", in,"*")),         /*
    OF(">>" ,&MathShiftBitsR    , 2, in, Arg2(in,"*", in,"*")),
    OF("<<" ,&MathShiftBitsL    , 2, in, Arg2(in,"*", in,"*")),         */
    //Assignment
    OF("="  , EqEqual_          , 3, xx, Arg2(xx,"x", xx,"*")),
    OF("+=" , EqAdd_            , 3, in, Arg2(in,"x", in,"*")),
    OF("-=" , EqSubtract_       , 3, in, Arg2(in,"x", in,"*")),
    OF("*=" , EqMultiply_       , 3, in, Arg2(in,"x", in,"*")),
    OF("/=" , EqDivide_         , 3, in, Arg2(in,"x", in,"*")),
    //Truth
    OF("==" , TruthEqual_       , 4, bo, Arg2(xx,"*", xx,"*")),
    OF("!=" , TruthNotEqual_    , 4, bo, Arg2(xx,"*", xx,"*")),
    OF(">=" , TruthGThenEq_     , 4, bo, Arg2(in,"*", in,"*")),
    OF("<=" , TruthLThenEq_     , 4, bo, Arg2(in,"*", in,"*")),
    OF(">"  , TruthGThen_       , 4, bo, Arg2(in,"*", in,"*")),
    OF("<"  , TruthLThen_       , 4, bo, Arg2(in,"*", in,"*")),
    OF("||" , TruthOr_          , 5, bo, Arg2(xx,"*", xx,"*")),
    OF("&&" , TruthAnd_         , 5, bo, Arg2(xx,"*", xx,"*")),
    OF("!"  , TruthNot_         , 2, bo, Neg1(xx,"*")        ),
    EmptyOperatorDecl
};


//** Base Flow Control Table **//
ScriptFuncDef gBaseFlowControlTable[] =
{
    //Name                           OpLevel
    OF("break"      , FlowBreak_    , 1, xx, Arg0),
    OF("continue"   , FlowContinue_ , 1, xx, Arg0),
    OF("end"        , FlowEnd_      , 1, xx, Arg0),
    EmptyOperatorDecl
};



//** Base DataType Table **//
ScriptDataTypeDef gBaseDataTypeTable[] = 
{
    DT("void", kScriptDT_None, NULL)
    WithHelp("Used when no variable is needed, ignore these."),

    DT("BOOL", kScriptDT_BOOL, CreateDT_BOOL)
    WithHelp("A truth variable, values of TRUE or FALSE."),

    DT("INT", kScriptDT_INT, CreateDT_INT)
    WithHelp("A signed number value like 10 or 20."),

    DT("STRING", kScriptDT_STRING, CreateDT_STRING)
    WithHelp("Text, a string of letters. Use \"quotes\" around the text in code."),

    EmptyDataTypeDecl
};
ScriptDataTypeDef* gDTDef_None      = &gBaseDataTypeTable[0];
ScriptDataTypeDef* gDTDef_BOOL      = &gBaseDataTypeTable[1];
ScriptDataTypeDef* gDTDef_INT       = &gBaseDataTypeTable[2];
ScriptDataTypeDef* gDTDef_STRING    = &gBaseDataTypeTable[3];


//** Base Constant Table **//
// The constant "NULL" is inherent to the language
ScriptConstDef gBaseConstantTable[] = 
{
    CT("TRUE"   , kScriptDT_BOOL    , gDTDef_BOOL   , true),
    CT("FALSE"  , kScriptDT_BOOL    , gDTDef_BOOL   , false),
    EmptyConstantDecl
};



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif

///////////////////////////////////////////////////////////////////////////////
// Some Macros For Working With ScriptVariables of X DataType
///////////////////////////////////////////////////////////////////////////////
#define AsDT_INT(xVar)      ((LONG) (xVar)->mValue->mValuePtr)



void* MathAdd_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) + AsDT_INT(y));
}

void* MathSubtract_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) - AsDT_INT(y));
}


void* MathMultiply_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) * AsDT_INT(y));
}

void* MathDivide_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    SCRIPTING_ASSERT(AsDT_INT(y) != 0);

    return (void*) (AsDT_INT(x) / AsDT_INT(y));
}


void* MathOr_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) | AsDT_INT(y));
}


void* MathAnd_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) & AsDT_INT(y));
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


void* EqEqual_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    x->mValue->OperatorEqual(y);
    return x->mValue->mValuePtr;
}

void* EqAdd_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    (LONG&) x->mValue->mValuePtr += AsDT_INT(y);
    
    return x->mValue->mValuePtr;
}

void* EqSubtract_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    (LONG&) x->mValue->mValuePtr -= AsDT_INT(y);
    
    return x->mValue->mValuePtr;
}

void* EqMultiply_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    (LONG&) x->mValue->mValuePtr *= AsDT_INT(y);
    
    return x->mValue->mValuePtr;
}

void* EqDivide_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    SCRIPTING_ASSERT(AsDT_INT(y) != 0);
    (LONG&) x->mValue->mValuePtr /= AsDT_INT(y);
    
    return x->mValue->mValuePtr;
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


void* TruthEqual_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) x->mValue->OperatorEqualEqual(y);
}


void* TruthNotEqual_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) !x->mValue->OperatorEqualEqual(y);
}

void* TruthGThen_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) > AsDT_INT(y));
}

void* TruthLThen_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) < AsDT_INT(y));
}

void* TruthGThenEq_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) >= AsDT_INT(y));
}

void* TruthLThenEq_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    return (void*) (AsDT_INT(x) <= AsDT_INT(y));
}

void* TruthOr_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    
    //If the first mValue is true we dont need to execute the
    //other variable, this is a special case
    x->Execute();
    if((LONG)x->mValue->mValuePtr)
        return (void*)true;
        
    y->Execute();   
    
    return (void*) ((LONG)x->mValue->mValuePtr || (LONG)y->mValue->mValuePtr);
}

void* TruthAnd_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    ScriptVariable* y = (ScriptVariable*) hArgList[1];
    
    SCRIPTING_ASSERT(x && y);
    
    //If the first mValue is true we dont need to execute the
    //other variable, this is a special case
    x->Execute();
    if((LONG)x->mValue->mValuePtr == false)
        return (void*)false;
        
    y->Execute();

    return (void*) ((LONG)x->mValue->mValuePtr && (LONG)y->mValue->mValuePtr);
}

void* TruthNot_(void* hArgList[])
{
    ScriptVariable* x = (ScriptVariable*) hArgList[0];
    
    SCRIPTING_ASSERT(x);
    if(x->mValue == NULL)
        return (void*) true;
    return (void*) (x->mValue->mValuePtr ? false : true);
}


#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif


void* FlowBreak_(void* hArgList[])
{
    //Handled Inside The Executor
    return NULL;    
}

void* FlowContinue_(void* hArgList[])
{
    //Handled Inside The Executor
    return NULL;    
}

void* FlowEnd_(void* hArgList[])
{
    //Handled Inside The Executor
    return NULL;    
}



