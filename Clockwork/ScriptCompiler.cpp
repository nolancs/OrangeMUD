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
#include "ScriptCompiler.h"
#include "ScriptLibrary.h"
#include "ScriptBinary.h"
#include "ScriptVariable.h"
#include "ScriptFunction.h"
#include "ScriptCodeLine.h"
#include "ScriptConditional.h"
#include "StringUtils.h"


//## Globals ##//
ScriptVariable gNullValue(gDTDef_None, NULL, true);

//## Local Function Prototypes ##//
void    ScanTillNextChar(const CHAR*& hSource, bool hAdvance = true);
bool    IsValidVariableName(STRING& hText);
STRING  StripParenthesis(STRING hLine);




ScriptCompiler::ScriptCompiler()
    : mScript(NULL)
    , mSourceCode(NULL)
    , mLineNumber(0)
    , mCurScope(0)
{
}


ScriptCompiler::~ScriptCompiler()
{
    ClearFunctions();
    ClearOperators();
    ClearFlowControls();
    ClearDataTypes();
    ClearConstants();
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddFunction
//
//   Desc:: Adds a ScriptFuncDef function definition into this
//          ScriptCompiler's library of available functions. hWeOwn is true if
//          you want the ScriptCompiler to take care of the memory associated
//          with hFunction.
//
//  Input:: hFunction: The function to add.
//          hWeOwn: Default false. Do we own the function?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddFunction(ScriptFuncDef* hFunction, bool hWeOwn)
{
    mFunctionList.AddBack(FuncDefBool(hFunction, hWeOwn));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddOperator
//
//   Desc:: Adds a ScriptFuncDef operator definition into this
//          ScriptCompiler's library of available operators. hWeOwn is true if
//          you want the ScriptCompiler to take care of the memory associated
//          with hOperator.
//
//  Input:: hOperator: The operator to add.
//          hWeOwn: Default false. Do we own the operator?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddOperator(ScriptFuncDef* hOperator, bool hWeOwn)
{
    mOperatorList.AddBack(FuncDefBool(hOperator, hWeOwn));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddFlowControl
//
//   Desc:: Adds a ScriptFuncDef flow control definition into this
//          ScriptCompiler's library of available flow controls. hWeOwn is true
//          if you want the ScriptCompiler to take care of the memory
//          associated with hFlowCtrl. Flow controls are things like continue;
//          and return; that control the "flow" of execution.
//
//  Input:: hFlowCtrl: The flow control to add.
//          hWeOwn: Default false. Do we own the flow control?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddFlowControl(ScriptFuncDef* hFlowCtrl, bool hWeOwn)
{
    mFlowControlList.AddBack(FuncDefBool(hFlowCtrl, hWeOwn));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddDataType
//
//   Desc:: Adds a ScriptDataTypeDef data type definition into this
//          ScriptCompiler's library of available data types. hWeOwn is true if
//          you want the ScriptCompiler to take care of the memory associated
//          with hDataType. Data types are things like INT and BOOL that define
//          what a ScriptVariable is.
//
//  Input:: hDataType: The data type to add.
//          hWeOwn: Default false. Do we own the data type?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddDataType(ScriptDataTypeDef* hDataType, bool hWeOwn)
{
    mDataTypeList.AddBack(DataTypeDefBool(hDataType, hWeOwn));
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddConstant
//
//   Desc:: Adds a ScriptConstDef constant definition into this
//          ScriptCompiler's library of available constants. hWeOwn is true if
//          you want the ScriptCompiler to take care of the memory associated
//          with hConstant. Constants are things like true, false, and NULL.
//
//  Input:: hConstant: The constant to add.
//          hWeOwn: Default false. Do we own the constant?
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddConstant(ScriptConstDef* hConstant, bool hWeOwn)
{
    mConstantList.AddBack(ConstDefBool(hConstant, hWeOwn));
}


bool ScriptCompiler::RemoveFunction(ScriptFuncDef* hFunction)
{
    return false;
}


bool ScriptCompiler::RemoveOperator(ScriptFuncDef* hOperator)
{
    return false;
}


bool ScriptCompiler::RemoveFlowControl(ScriptFuncDef* hFlowCtrl)
{
    return false;
}


bool ScriptCompiler::RemoveDataType(ScriptDataTypeDef* hDataType)
{
    return false;
}


bool ScriptCompiler::RemoveConstant(ScriptConstDef* hConstant)
{
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ClearFunctions
//
//   Desc:: Clears the entire function list for this ScriptCompiler object. It
//          will delete any of the function's ScriptFuncDef pointers it owns.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ClearFunctions()
{
    List<FuncDefBool>::iterator i;
    
    for(i = mFunctionList.begin(); i != mFunctionList.end(); ++i)
    if((*i).second == true)
        delete (*i).first;
    
    mFunctionList.clear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ClearOperators
//
//   Desc:: Clears the entire operator list for this ScriptCompiler object. It
//          willdelete any of the operator's ScriptFuncDef pointers it owns.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ClearOperators()
{
    List<FuncDefBool>::iterator i;
    
    for(i = mOperatorList.begin(); i != mOperatorList.end(); ++i)
    if((*i).second == true)
        delete (*i).first;
    
    mOperatorList.clear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ClearFlowControls
//
//   Desc:: Clears the entire flow control list for this ScriptCompiler object.
//          It will delete any of the flow control's ScriptFuncDef pointers it
//          owns.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ClearFlowControls()
{
    List<FuncDefBool>::iterator i;
    
    for(i = mFlowControlList.begin(); i != mFlowControlList.end(); ++i)
    if((*i).second == true)
        delete (*i).first;
    
    mFlowControlList.clear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ClearDataTypes
//
//   Desc:: Clears the entire data type list for this ScriptCompiler object. It
//          will delete any of the data type's ScriptDataTypeDef pointers it owns.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ClearDataTypes()
{
    List<DataTypeDefBool>::iterator i;
    
    for(i = mDataTypeList.begin(); i != mDataTypeList.end(); ++i)
    if((*i).second == true)
        delete (*i).first;
    
    mDataTypeList.clear();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ClearConstants
//
//   Desc:: Clears the entire constant list for this ScriptCompiler object. It
//          will delete any of the constant's ScriptConstDef pointers it owns.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ClearConstants()
{
    List<ConstDefBool>::iterator i;
    
    for(i = mConstantList.begin(); i != mConstantList.end(); ++i)
    if((*i).second == true)
        delete (*i).first;
    
    mConstantList.clear();
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::InitFunctionality
//
//   Desc:: Goes in and adds on the bare-bones functions, operators, flow
//          controls, data types, and constants to this ScriptCompiler. Any
//          Subclass would override this method in to add their own specific
//          functions, data types, and constants. Subclasses can still call
//          this function if they wish to use all the base functionality, or
//          they can choose which they want and add them on their own.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::InitFunctionality()
{
    ULONG i;
    
    for(i = 0; gBaseFunctionTable[i].mName; i++)
        AddFunction(&gBaseFunctionTable[i], false);
    
    for(i = 0; gBaseOperatorTable[i].mName; i++)
        AddOperator(&gBaseOperatorTable[i], false);

    for(i = 0; gBaseFlowControlTable[i].mName; i++)
        AddFlowControl(&gBaseFlowControlTable[i], false);
        
    for(i = 0; gBaseDataTypeTable[i].mName; i++)
        AddDataType(&gBaseDataTypeTable[i], false);
    
    for(i = 0; gBaseConstantTable[i].mName; i++)
        AddConstant(&gBaseConstantTable[i], false);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::Compile
//
//   Desc:: Compiles a Script object. It fills the Script's mBinary data member
//          with the resulting ScriptBinary on success, otherwise a textual
//          description of what went wrong and on what line is placed into the
//          Script's mErrorBuffer data member.
//
// Output:: True on successful compilation. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::Compile(Script* hScript)
{
    bool nDoneCompiling = false;
    
    mScript                 = hScript;
    mScript->mBinary        = NewBinary();
    mSourceCode             = mScript->mSource.c_str();
    mScript->mErrorBuffer   = "";
    mLineNumber             = 1;
    mCurScope               = 0;

    
    try
    {
        while(!nDoneCompiling)
            CompileStep(nDoneCompiling);    
    }
    catch(...)
    {
        mScript->mBinary = NULL;
        while(!mCompileStack.empty())
            mCompileStack.pop();
        return false;
    }
    
    while(!mCompileStack.empty())
        mCompileStack.pop();
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::NewBinary
//
//   Desc:: Creates a new ScriptBinary for the compiler to use. Overide this
//          if you need it to create a custom binary object that you've derived
//          from ScriptBinary.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptBinary* ScriptCompiler::NewBinary()
{
    return new ScriptBinary();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::NewVariable
//
//   Desc:: Creates a new ScriptVariable for the compiler to use. Overide this
//          if you need it to create a custom variable object that you've
//          derived from ScriptVariable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptVariable* ScriptCompiler::NewVariable(ScriptDataTypeDef* hDef, void* hValue, bool hConst)
{
    ASSERT(hDef != NULL);
    return new ScriptVariable(hDef, hValue, hConst);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FindVariable
//
//   Desc:: Searches the binary for a variable declared with a name matching
//          hText. It only checks the current compilation scope. If a matching
//          variable is found it sets hVar and returns true.
//
//  Input:: hVar: Pointer to the resulting ScriptVariable on success.
//          hText: Search for a variable matching this.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::FindVariable(ScriptVariable*& hVar, STRING& hText)
{
    ScriptVariableList& nList = mScript->mBinary->mVariables;
    ScriptVariableIter  i;

    for(i = nList.begin(); i != nList.end(); ++i)
    {
        ScriptVariable* nVariable = *i;
        if(nVariable->mScope > mCurScope)
            continue;
        
        if(hText == nVariable->mName)
        {
            hVar = nVariable;
            return true;
        }
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FindConstant
//
//   Desc:: Searches the compiler for a constant declared with a name matching
//          hText. If a matching constant is found it sets hVar and returns
//          true.
//
//  Input:: hVar: Pointer to the resulting ScriptVariable on success.
//          hText: Search for a constant matching this.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::FindConstant(ScriptVariable*& hVar, STRING& hText)
{
    List<ConstDefBool>::iterator i;
    
    if(hText == "NULL") //Inherent to the language
    {
        hVar = &gNullValue;
        return true;
    }
    
    for(i = mConstantList.begin(); i != mConstantList.end(); ++i)
    {
        ScriptConstDef* nConstant = (*i).first;

        if(hText == nConstant->mName)
        {
            hVar = nConstant->mVariable;
            return true;
        }
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FindRawValue
//
//   Desc:: Determines if hText is a declaration of a "raw value". That could
//          be a number like 1024 or "a string like this". If it turns out to
//          be a raw value it will create the appropriate ScriptVariable in the
//          ScriptBinary and set hVar pointing to it.
//
//  Input:: hVar: Pointer to the resulting ScriptVariable on success.
//          hText: Determine if this is a raw value.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::FindRawValue(ScriptVariable*& hVar, STRING& hText)
{
    //** Create INT Variable **//
    if(IsNumber(hText))
    {
        LONG nNumberValue = atol(hText.c_str());
        ScriptVariableList& nScanList = mScript->mBinary->mValueVars;
        ScriptVariableIter  i;
        
        //A constant variable of value "0" might already be around
        for(i = nScanList.begin(); i != nScanList.end(); ++i)
        {
            ScriptVariable* nVarScan = *i;
            if(nVarScan->mType != kScriptDT_INT)
                continue;
            if(((LONG)nVarScan->mValue->mValuePtr) != nNumberValue)
                continue;
            
            hVar = nVarScan;
            return true;
        }
        
        //Make a new constant value variable
        APtr<ScriptVariable> nNewINT = NewVariable(gDTDef_INT, (void*)nNumberValue);
        
        hVar = nNewINT;
        mScript->mBinary->mValueVars.Add(nNewINT.Detach());
        return true;
    }
    else
    
    
    //** Create STRING Variable **//
    if(hText[0] == '"' && hText[hText.length()-1] == '"')
    {
        APtr<ScriptVariable> nNewSTRING = NewVariable(gDTDef_STRING);
        DataTypeSTRING* nStrDT = static_cast<DataTypeSTRING*>(nNewSTRING->mValue.Ptr());
        
        nStrDT->mString.append(&hText[1], hText.length()-2);
        hVar = nNewSTRING;
        mScript->mBinary->mValueVars.Add(nNewSTRING.Detach());
        return true;
    }
    
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FindFunction
//
//   Desc:: Searches the compiler for a function declared with a name matching
//          hFuncName. If a matching function is found in this compiler's
//          library of functions it will set hFunc to it and return true.
//
//  Input:: hFunc: Pointer to the resulting ScriptFuncDef on success.
//          hFuncName: Search for a function matching this.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::FindFunction(ScriptFuncDef*& hFunc, STRING& hFuncName)
{
    List<FuncDefBool>::iterator i;
    ScriptFuncDef* nFuncScan;
    
    for(i = mFunctionList.begin(); i != mFunctionList.end(); ++i)
    {
        nFuncScan = (*i).first;
        if(hFuncName != nFuncScan->mName)
            continue;   
        
        hFunc = nFuncScan;
        return true;
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FindDataType
//
//   Desc:: Searches the compiler for a data type with a name matching hDTName.
//          If a matching data type is found in this compiler's library of data
//          types it will set hDT to it and return true.
//
//  Input:: hDT: Pointer to the resulting ScriptDataTypeDef on success.
//          hDTName: Search for a function matching this.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::FindDataType(ScriptDataTypeDef*& hDT, STRING& hDTName)
{
    List<DataTypeDefBool>::iterator i;
    ScriptDataTypeDef* nDT;
    
    for(i = mDataTypeList.begin(); i != mDataTypeList.end(); ++i)
    {
        nDT = (*i).first;
        if(hDTName == nDT->mName)
        {
            hDT = nDT;
            return true;
        }
    }
    
    return false;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileStep
//
//   Desc:: A single iteration through the compiling loop.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileStep(bool& hDoneCompiling)
{   
    ScanTillNextChar(mSourceCode, false);
    while(isspace(*mSourceCode) || *mSourceCode == '\n')
        ScanTillNextChar(mSourceCode);

    UpdateOnLine(mSourceCode);
    if(*mSourceCode == '\0')    
    {
        hDoneCompiling = true;
        return;
    }

    
    //** Check Exiting Conditonal **//
    if(!strncmp("}", mSourceCode, 1))
    {
        mSourceCode += 1;
        FinishConditional();
        return;
    }
    
    //** Check 'if(' Conds **//
    if(!strncmp("if(", mSourceCode, 3))
    {
        mSourceCode += 3;
        CompileIf();
        return;
    }

    //** Check 'else' Conds **//
    if(!strncmp("else", mSourceCode, 4))
    {
        mSourceCode += 4;
        CompileElse();
        return;
    }
    
    //** Check 'while(' Conds **//
    if(!strncmp("while(", mSourceCode, 6))
    {   
        mSourceCode += 6;
        CompileWhile();
        return;
    }



    //** Parse To Semicolin, Compile Chunk **//
    Vector<STRING>  nDelims;
    STRING          nText;
    
    nDelims.Add(";");   
    if(!ParseArgTill(mSourceCode, nDelims, nText))
    {
        UpdateOnLine(mSourceCode);
        mScript->AddError(QBuf("Parse error at \"%s\".", mSourceCode), mLineNumber);
        throw -1;
    }
    UpdateOnLine(mSourceCode);
    nText = StripWS(nText);
    

    //** Compile Variable Declarations **//
    if(TryCompileVariables(nText))
        return;     
    

    //** Compile Flow Control Calls **//
    if(TryCompileFControl(nText))
        return;
    

    //** Compile Function Call **//
    APtr<ScriptFunction>    nFunction;
    APtr<ScriptCodeLine>    nCodeLine;
    if(TryCompileFunction(nFunction, nText))
    {
        nCodeLine = new ScriptCodeLine();
        nCodeLine->mFunction = nFunction.Detach();
        AddCodeLine(nCodeLine);
        return;
    }


    //** Compile Operator Call **//
    if(TryCompileOperator(nFunction, nText))
    {
        nCodeLine = new ScriptCodeLine();
        nCodeLine->mFunction = nFunction.Detach();
        AddCodeLine(nCodeLine);
        return;
    }
    
    
    //No dice - garbage we can't compile
    mScript->AddError(QBuf("Parse error, cannot make sense of \"%s\".", nText.c_str()), mLineNumber);
    throw -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileIf
//
//   Desc:: This is called if the text starts out with 'if(' so it is assumed
//          to be an if statement. It throws an exception if it could not
//          correctly read the statement, otherwise it reads the if statement
//          and adds it to the code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileIf()
{
    Vector<STRING>  nParathesisDelim;
    STRING          nChunkArgText;
    
    nParathesisDelim.Add(")");
    if(!ParseArgTill(mSourceCode, nParathesisDelim, nChunkArgText))
    {
        UpdateOnLine(mSourceCode);
        mScript->AddError("Bad 'if()' statement", mLineNumber);
        throw -1;
    }
    
    //Search For Brackets
    bool nFoundBracket = false;
    while(isspace(*mSourceCode) || *mSourceCode == '\n')
        ScanTillNextChar(mSourceCode);
    if(*mSourceCode == '{')
    {
        nFoundBracket = true;
        ++mSourceCode;
    }
    
    //Create 'if()' ScriptConditional
    ScriptVariable* nVariable;
    CompileArgument(nVariable, nChunkArgText);

    APtr<ScriptCodeLine>    nIfCodeLine     = new ScriptCodeLine();
    APtr<ScriptConditional> nIfConditional  = new ScriptConditional();
    
    nIfConditional->mType       = kScriptCond_If;
    nIfConditional->mEval       = nVariable;
    nIfConditional->mBrackets   = nFoundBracket;
    nIfConditional->mFromCode   = nIfCodeLine;
    
    nIfCodeLine->mConditional   = nIfConditional.Detach();
    AddCodeLine(nIfCodeLine);
}





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileElse
//
//   Desc:: This is called if the text starts out with 'else' so it is assumed
//          to be an else statement. It throws an exception if it could not
//          correctly read the statement, otherwise it reads the else statement
//          and adds it to the mElseCode member of the just previously compiled
//          If conditional.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileElse()
{
    //Search For Brackets
    bool nFoundBracket = false;
    while(isspace(*mSourceCode))
        ScanTillNextChar(mSourceCode);
    if(*mSourceCode == '{')
    {
        nFoundBracket = true;
        mSourceCode++;
    }else
    if(!strncmp("if(", mSourceCode, 3))
    {
        mSourceCode += 3;
        CompileElseIf();
        return;
    }
    
    //Create 'else' ScriptConditional
    APtr<ScriptCodeLine>    nElseCodeLine   = new ScriptCodeLine();
    APtr<ScriptConditional> nElseConditional= new ScriptConditional();
    
    nElseConditional->mType     = kScriptCond_Else;
    nElseConditional->mEval     = NULL;
    nElseConditional->mBrackets = nFoundBracket;
    nElseConditional->mFromCode = nElseCodeLine;
    
    nElseCodeLine->mConditional = nElseConditional.Detach();
    AddCodeLine(nElseCodeLine);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileElseIf
//
//   Desc:: This is called if the text starts out with 'else' then followed by
//          an 'if(' so it is assumed to be an else if statement. It throws an
//          exception if it could not correctly read the statement, otherwise
//          it reads the else if statement and adds it to the code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileElseIf()
{
    Vector<STRING>  nParathesisDelim;
    STRING          nChunkArgText;
    
    nParathesisDelim.Add(")");
    if(!ParseArgTill(mSourceCode, nParathesisDelim, nChunkArgText))
    {
        UpdateOnLine(mSourceCode);
        mScript->AddError("Bad 'else if()' statement", mLineNumber);
        throw -1;
    }
    
    //Search For Brackets
    bool nFoundBracket = false;
    while(isspace(*mSourceCode) || *mSourceCode == '\n')
        ScanTillNextChar(mSourceCode);
    if(*mSourceCode == '{')
    {
        nFoundBracket = true;
        ++mSourceCode;
    }
    
    //Create 'else if()' ScriptConditional
    ScriptVariable* nVariable;
    CompileArgument(nVariable, nChunkArgText);

    APtr<ScriptCodeLine>    nIfCodeLine     = new ScriptCodeLine();
    APtr<ScriptConditional> nIfConditional  = new ScriptConditional();
    
    nIfConditional->mType       = kScriptCond_ElseIf;
    nIfConditional->mEval       = nVariable;
    nIfConditional->mBrackets   = nFoundBracket;
    nIfConditional->mFromCode   = nIfCodeLine;
    
    nIfCodeLine->mConditional   = nIfConditional.Detach();
    AddCodeLine(nIfCodeLine);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileWhile
//
//   Desc:: This is called if the text starts out with 'while' so it is assumed
//          to be a while statement. It throws an exception if it could not
//          correctly read the statement, otherwise it reads the while
//          statement and adds it to the code.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileWhile()
{
    Vector<STRING>  nParathesisDelim;
    STRING          nChunkArgText;
    
    nParathesisDelim.Add(")");
    if(!ParseArgTill(mSourceCode, nParathesisDelim, nChunkArgText))
    {
        UpdateOnLine(mSourceCode);
        mScript->AddError("Bad 'while()' statement", mLineNumber);
        throw -1;
    }
    
    //Search For Brackets
    bool nFoundBracket = false;
    while(isspace(*mSourceCode) || *mSourceCode == '\n')
        ScanTillNextChar(mSourceCode);
    if(*mSourceCode == '{')
    {
        nFoundBracket = true;
        ++mSourceCode;
    }
    
    //Create 'while()' ScriptConditional
    ScriptVariable* nVariable;
    CompileArgument(nVariable, nChunkArgText);

    APtr<ScriptCodeLine>    nWhileCodeLine      = new ScriptCodeLine();
    APtr<ScriptConditional> nWhileConditional   = new ScriptConditional();
    
    nWhileConditional->mType        = kScriptCond_While;
    nWhileConditional->mEval        = nVariable;
    nWhileConditional->mBrackets    = nFoundBracket;
    nWhileConditional->mFromCode    = nWhileCodeLine;
    
    nWhileCodeLine->mConditional    = nWhileConditional.Detach();
    AddCodeLine(nWhileCodeLine);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CompileArgument
//
//   Desc:: This is the main recursive cruncher of the ScriptCompiler. Given a 
//          ParseArgTill chunk of text at any operator level, it will take the
//          text and transform it into ScriptVariable. It sets the error and
//          throws an exception if it cannot make sense of hText.
//
//  Input:: hVar: The compiled ScriptVariable will be placed here.
//          hText: Compile the argument from this text.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::CompileArgument(ScriptVariable*& hVar, STRING& hText)
{
//  ASSERT(hVar == NULL);
    STRING  nText           = StripWS(hText);
    bool    nValidVarName   = IsValidVariableName(hText);
    
    
    //** Find Raw Value **//
    if(FindRawValue(hVar, nText))
        return;
    
    
    //** Find Constant Or Declared Variable **//
    if(nValidVarName)
    {
        if(FindConstant(hVar, nText))
            return;
            
        if(FindVariable(hVar, nText))
            return;
        
        mScript->AddError(QBuf("Variable '%s' is undeclared.", nText.c_str()), mLineNumber);
        throw -1;
    }
    
    
    //** Operator Call **//
    APtr<ScriptFunction> nFunction;
    if(TryCompileOperator(nFunction, hText))
    {
        APtr<ScriptVariable> nOpCall;
        
        nOpCall = CreateVariable(nFunction->mReturnType);
        nOpCall->mFunction = nFunction.Detach();
        hVar = nOpCall;
        mScript->mBinary->mFuncVars.Add(nOpCall.Detach());
        return;
    }
    
    
    //** Function Call **//
    if(TryCompileFunction(nFunction, hText))
    {
        APtr<ScriptVariable> nFuncCall = CreateVariable(nFunction->mFuncDef->mReturns);
        
        if(nFunction->mFuncDef->mReturns == kScriptDT_None)
        {
            mScript->AddError(QBuf("The '%s' function returns nothing so it may not "
            "be used as an argument.", nFunction->mFuncDef->mName), mLineNumber);
            throw -1;
        }
        
        nFuncCall->mFunction = nFunction.Detach();
        hVar = nFuncCall;
        mScript->mBinary->mFuncVars.Add(nFuncCall.Detach());
        return;
    }

    
    //Couldn't compile the argument
    mScript->AddError(QBuf("Cannot make sense of argument \"%s\".", hText.c_str()), mLineNumber);
    throw -1;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::TryCompileFControl                       [Fix This]
//
//   Desc:: Takes a semicolin parsed line of text and attempts to compile it
//          into a flow control call. Flow control calls look like "end;" or
//          "continue;" in the code. The flow control ScriptCodeLine is added
//          automatically to the ScriptBinary on success.
//
//  Input:: Try to compile this text.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::TryCompileFControl(STRING& hText)
{
    List<FuncDefBool>::iterator i;
    ScriptFuncDef* nFControl;
    
    for(i = mFlowControlList.begin(); i != mFlowControlList.end(); ++i)
    {
        nFControl = (*i).first;
        if(hText == nFControl->mName)
            break;  
    }
    if(i == mFlowControlList.end())
        return false;


    //Check Flow Control
    if(nFControl->mFuncCall == FlowBreak_ || nFControl->mFuncCall == FlowContinue_)
    {
        //## FIX THIS ##//
        //Needs to be able to check if it's inside a conditional maybe three levels out, using
        //a recurvsive check and not just check the parent for being a looping conditional.

        //Gather Info
        ///////////////////////////////////////////////////////////////////////
        ScriptCodeLineList* nOnList = mCompileStack.top(); 
        //ScriptCodeLine*   nOnLine = nOnList->empty() ? NULL : nOnList->back();
    
        mCompileStack.pop();
        ScriptCodeLineList* nInList = mCompileStack.empty() ? NULL : mCompileStack.top();
        mCompileStack.push(nOnList); //Add this back
    
        ScriptCodeLine* nInLine = nInList ? nInList->back() : NULL;
        ///////////////////////////////////////////////////////////////////////
        if(!nInLine->IsConditional(kScriptCond_For)
        && !nInLine->IsConditional(kScriptCond_While))
        {
            mScript->AddError(QBuf("The \"%s\" control not used in a loop.", 
            nFControl->mName), mLineNumber);
            throw -1;
        }
    }
    

    //Make/Add Flow Control
    APtr<ScriptCodeLine>    nFlowLine = new ScriptCodeLine();
    APtr<ScriptFunction>    nFlowFunc = new ScriptFunction();
    
    nFlowFunc->mFuncDef = nFControl;
    nFlowLine->mFunction = nFlowFunc.Detach();
    AddCodeLine(nFlowLine);
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::TryCompileVariables
//
//   Desc:: Takes a semicolin parsed line of text and attempts to compile it
//          into variable declarations. This would be a valid line of variable
//          declarations "INT nInt1, nInt2, nInt3;" You cannot as of yet assign
//          to a variable while you declare it. The variables are added into
//          the ScriptBinary automatically on success.
//
//  Input:: Try to compile this text.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::TryCompileVariables(STRING& hText)
{
    const CHAR*     nSource = hText.c_str();
    STRING          nDTNameStr, nVarNameStr;
    Vector<STRING>  nDelims;
    
    nDelims.Add(" ");   //Space
    nDelims.Add("\t");  //Tab
    if(!ParseArgTill(nSource, nDelims, nDTNameStr))
        return false;

    while(isspace(*nSource))
        ScanTillNextChar(nSource);

    if(*nSource == '\0')
        return false;


    //Search For nDTNameStr
    ScriptDataTypeDef* nDT;
    if(!FindDataType(nDT, nDTNameStr))
        return false;

    
    //OK, Is A Variable Declaration
    SHORT nDeclaredVariables = 0;
    nDelims.clear();
    nDelims.Add(",");
    nDelims.Add(""); //break on '\0'
    
    while(true)
    {
        nVarNameStr.erase();
        if(!ParseArgTill(nSource, nDelims, nVarNameStr))
        {
            ASSERT(!"Shoudn't happen");
            //nVarNameStr = nSource;
            //nSource += strlen(nSource);
        }
        
            
        nVarNameStr = StripWS(nVarNameStr);
        if(nVarNameStr.empty())
            break;
        
        
        if(!IsValidVariableName(nVarNameStr))
        {
            mScript->AddError(QBuf("\"%s\" is not a valid variable name.", 
            nVarNameStr.c_str()), mLineNumber);
            throw -1;
        }
        
        ScriptDataTypeDef* nTempDT;
        if(FindDataType(nTempDT, nVarNameStr))
        {
            mScript->AddError(QBuf("\"%s\" is a data type and cannot be used as a"
            " variable name.", nVarNameStr.c_str()), mLineNumber);
            throw -1;
        }
        
        ScriptVariable* nTempVar;
        if(FindConstant(nTempVar, nVarNameStr) || FindVariable(nTempVar, nVarNameStr))
        {
            mScript->AddError(QBuf("The variable name \"%s\" is already in use.",
            nVarNameStr.c_str()), mLineNumber);
            throw -1;
        }
        
        
        //Create ScriptVariable in the ScriptBinary
        APtr<ScriptVariable> nVariable = NewVariable(nDT);
        
        nVariable->mScope   = mCurScope;
        nVariable->mName    = nVarNameStr;
        mScript->mBinary->mVariables.Add(nVariable.Detach());
        nDeclaredVariables++;
    }
    
    
    if(nDeclaredVariables == 0)
    {
        mScript->AddError("No variables declared after variable name.", mLineNumber);
        throw -1;
    }

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::TryCompileFunction
//
//   Desc:: Takes a chunk of text and attempts to compile it into a valid
//          ScriptFunction function call. It returns false if the text is not
//          a function call, otherwise it throws an exception on any other
//          failures compiling the function and sets an error in the mScript.
//          On Success the hFunc auto-pointer will contain the ScriptFunction.
//
//  Input:: hFunc: Hold the ScriptFunction on success.
//          hText: Try to compile this text as a function call.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::TryCompileFunction(APtr<ScriptFunction>& hFunc, STRING& hText)
{
    const CHAR* nScanText = hText.c_str();
    STRING      nFunctionName;


    //** Scan Through And Find Function Name **//
    while(true)
    {
        if(isalnum(*nScanText) || *nScanText == '_')
        {
            nFunctionName += *nScanText;
            ScanTillNextChar(nScanText);
            continue;
        }
        if(*nScanText == '(')
        {
            nScanText++;
            break;
        }
        return false; //'\0' will return false
    }

    if(nFunctionName.empty() || !IsValidVariableName(nFunctionName))
        return false;

    
    //** It's A Function Call, Find Function **//
    ScriptFuncDef* nFuncDef;
    if(!FindFunction(nFuncDef, nFunctionName))
    {
        mScript->AddError(QBuf("Unknown function \"%s\".", nFunctionName.c_str()), mLineNumber);
        throw -1;
    }
    
    
    
    //** Massive Argument Reading For The Function **// 
    SHORT           nAtArg, nNeedArgs = nFuncDef->mNumArgs;
    ScriptVariable* nArgs[kMaxScriptArg];
    Vector<STRING>  nBreakOnDelims;
    STRING          nArgBuf;

    nBreakOnDelims.Add(","); //could be tailored to care more if it gets
    nBreakOnDelims.Add(")"); //comma or a parenthesis but i dont care.
    for(nAtArg = 0; nAtArg < kMaxScriptArg; nAtArg++)
        nArgs[nAtArg] = NULL;
    

    for(nAtArg = 0; nNeedArgs;)
    {
        if(!ParseArgTill(nScanText, nBreakOnDelims, nArgBuf))
        {
            mScript->AddError(QBuf("Correct number of arguments not found for the %s "
            "function. There should be %d.",  nFuncDef->mName, nNeedArgs), mLineNumber);
            throw -1;
        }       
        
        //Compile Argument
        nArgBuf = StripParenthesis(nArgBuf);
        CompileArgument(nArgs[nAtArg], nArgBuf);

        if(nFuncDef->mArgTypes[nAtArg] != kScriptDT_None
        && nFuncDef->mArgTypes[nAtArg] != nArgs[nAtArg]->mType
        && nArgs[nAtArg] != &gNullValue)
        {
            mScript->AddError(QBuf("Argument %d of the %s function requires a %s "
            "not a %s.", nAtArg+1, nFuncDef->mName, 
            FindDTDef(nFuncDef->mArgTypes[nAtArg])->mName, 
            FindDTDef(nArgs[nAtArg]->mType)->mName), mLineNumber);
            throw -1;
        }
        
        //Good Argument
        nAtArg++;
        if(nAtArg == nNeedArgs)
        {
            STRING nTextEnd = nScanText;
            if(!StripWS(nTextEnd).empty())
            {
                mScript->AddError(QBuf("The %s function takes %d arguments, you have specified "
                "more than that with \"%s\".", nFuncDef->mName, nScanText), mLineNumber);
                throw -1;
            }
            break;
        }
    }
    

    ASSERT(nAtArg == nNeedArgs);
    hFunc               = new ScriptFunction();
    hFunc->mFuncDef     = nFuncDef;
    hFunc->mReturnType  = nFuncDef->mReturns;
    for(nAtArg = 0; nAtArg < kMaxScriptArg; nAtArg++)
        hFunc->mArgs[nAtArg] = nArgs[nAtArg];

    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::TryCompileOperator
//
//   Desc:: Takes a chunk of text and attempts to compile it into a valid
//          ScriptFunction operator call. It returns false if the text is not
//          an operator call, otherwise it throws an exception on any other
//          failures compiling the operator and sets an error in the mScript.
//          On Success the hFunc auto-pointer will contain the ScriptFunction
//          representing the operator call.
//
//  Input:: hFunc: Hold the ScriptFunction on success.
//          hText: Try to compile this text as an operator call.
//
// Output:: True on success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::TryCompileOperator(APtr<ScriptFunction>& hFunc, STRING& hText)
{
    const CHAR*     nScanText = hText.c_str();
    SHORT           nOperatorLevel;
    ScriptFuncDef*  nOperatorDef;
    STRING          nBuf;
    
    for(nOperatorLevel = kMaxScriptOp; nOperatorLevel > 0; nOperatorLevel--)
    if(ParseArgTillOperator(nScanText, nOperatorLevel, nOperatorDef, nBuf))
        break;
    
    if(nOperatorLevel == 0)
        return false;


    //** No Preceding Argument Found And One Is Needed? **//
    if(nOperatorDef->mNumArgs > 0 && StripWS(nBuf).empty())
    {
        mScript->AddError(QBuf("No argument found before the %s operator.", 
        nOperatorDef->mName), mLineNumber);
        throw -1;
    }
    
    
    //** Argument Found Before An Operator That Cant Use It? **//
    if(nOperatorDef->mNumArgs < 0 && !StripWS(nBuf).empty())
    {
        mScript->AddError(QBuf("Unusable argument \"%s\" found before the %s operator.", 
        nBuf.c_str(), nOperatorDef->mName), mLineNumber);
        throw -1;
    }
    
    
    //** Massive Argument Reading For The Operator **// 
    ScriptVariable* nArgs[kMaxScriptArg];
    SHORT           nAtArg, nNeedArgs, nType = kScriptDT_None;
    Vector<STRING>  nBreakOnDelims;
    

    nNeedArgs = abs(nOperatorDef->mNumArgs);
//  nBreakOnDelims.Add(")");    //could be tailored to know if we're working
//  nBreakOnDelims.Add(";");    //on a codeline or inside a if() or whatnot
    nBreakOnDelims.Add("");     //Break on end of string
    for(nAtArg = 0; nAtArg < kMaxScriptArg; nAtArg++)
        nArgs[nAtArg] = NULL;
    
    for(nAtArg = 0; nAtArg < nNeedArgs; nAtArg++)
    {
        //Empty Buf? Read an Argument
        if(nBuf.empty())
        if(!ParseArgTill(nScanText, nBreakOnDelims, nBuf))
        {
            mScript->AddError(QBuf("Correct number of arguments not found for the %s "
            "operator. There should be %d.",  nOperatorDef->mName, nNeedArgs), mLineNumber);
            throw -1;
        }
        
        //Compile Argument
        nBuf = StripParenthesis(nBuf);
        CompileArgument(nArgs[nAtArg], nBuf);
        
        //Return Type Undetermined As Of Yet?
        if(nType == kScriptDT_None)
        {
            nType = nOperatorDef->mArgTypes[nAtArg] == kScriptDT_None ? 
                    nArgs[nAtArg]->mType : nOperatorDef->mArgTypes[nAtArg];
            ASSERT(nType != kScriptDT_None);
        }           
        
        //Uses Correct Variable Type?
        if(nArgs[nAtArg]->mType != nType && nArgs[nAtArg] != &gNullValue)
        {
            mScript->AddError(QBuf("The %s operator needs a data type %s for it to "
            "work here, not %s.", nOperatorDef->mName, FindDTDef(nType)->mName, 
            FindDTDef(nArgs[nAtArg]->mType)->mName), mLineNumber);
            throw -1;   
        }
        
        //Must Be Variable Reference?
        if(nOperatorDef->mArgNames[nAtArg]
        && nOperatorDef->mArgNames[nAtArg][0] == 'x')
        if(nArgs[nAtArg]->mConstant || nArgs[nAtArg]->mName.empty())
        {
            mScript->AddError(QBuf("The \"%s\" argument cannot be used with the %s "
            "operator, it needs to a previously declared variable, not a value or "
            "constant.", nBuf.c_str(), nOperatorDef->mName), mLineNumber);
            throw -1;
        }
        
        //WooHoo It's All Good
        nBuf = "";
    }
    
    
    //** Check For Correct Number Of Arguments **//
    if(nAtArg != nNeedArgs)
    {
        mScript->AddError(QBuf("Correct number of arguments not found for the %s " 
        "operator. There should be %d.", nOperatorDef->mName, nNeedArgs), mLineNumber);
        throw -1;
    }
    

    //** Finally, Build The Operator Function **//
    hFunc               = new ScriptFunction();
    hFunc->mFuncDef     = nOperatorDef;
    hFunc->mReturnType  = nOperatorDef->mReturns == kScriptDT_None ? nType : nOperatorDef->mReturns;
    for(nAtArg = 0; nAtArg < kMaxScriptArg; nAtArg++)
        hFunc->mArgs[nAtArg] = nArgs[nAtArg];
    
    return true;
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif





//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::AddCodeLine
//
//   Desc:: Adds a compiled ScriptCodeLine into the mBinary object at the
//          correct spot. Takes care of keeping track of bracketed conditionals
//          and such. An errors and exception will be thrown if the user had
//          set up a bad script with misplaced brackets or conditionals.
//
//  Input:: The code line to add.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::AddCodeLine(APtr<ScriptCodeLine>& hNextLine)
{
    ScriptCodeLine* nNextLine = hNextLine; //So you can get at it after .Detach()
    ScriptCodeLineList* nCurrentList        = GetCurrentCodeList();
    ScriptCodeLine*     nCurrentLine        = GetCurrentCodeLine();
    ScriptCodeLineIter  nCurrentLineIter    = GetCurrentCodeLineIter();
    ScriptCodeLineList* nContainingList     = GetContainingCodeList();
    ScriptCodeLine*     nContainingLine     = GetContainingCodeLine();
    ScriptCodeLineIter  nContainingLineIter = GetContainingCodeLineIter();
    
    
    //Special Case: First Line Of The Script
    if(!nCurrentList)
    {
        ASSERT(mScript->mBinary->mCode.empty());
        ValidateInsertion(hNextLine, NULL, NULL, nCurrentLineIter, NULL, NULL, nContainingLineIter);
        mScript->mBinary->mCode.AddBack(hNextLine.Detach());
        mCompileStack.push(&mScript->mBinary->mCode);
        if(nNextLine->IsConditional())
        {
            mCompileStack.push(&nNextLine->mConditional->mCode);
            mCurScope++;
        }
        return;
    }
    
        
    ///////////////////////////////////////////////////////////////////////////
    ASSERT(nCurrentList != NULL);
    ASSERT(nCurrentLine || (nContainingLine && nContainingLine->IsConditional()));
    //
    //
    // State Summary:
    // nCurrentList may NEVER be NULL.
    // nCurrentLine may be NULL ONLY if we're inside the start of a conditional
    // nContainingList may be NULL, must be Non-NULL if nOnLine is NULL (Conditional's list)
    // nContainingLine may be NULL, must be Non-NULL if nOnLine is NULL (Conditional's line)
    //
    // When add a Conditional we push it's mCode ScriptCodeLineList onto mCompileStack
    // and it remains empty until further calls to add.
    ///////////////////////////////////////////////////////////////////////////
    
    
    //Just Entered A Conditional
    if(nCurrentLine == NULL)
    {
        ValidateInsertion(hNextLine, nCurrentList, nCurrentLine, nCurrentLineIter, nContainingList, nContainingLine, nContainingLineIter);
        hNextLine->mInCodeLine = new ScriptCodeLineIter(nContainingLineIter);
        nCurrentList->AddBack(hNextLine);
        hNextLine.Detach();
        
        if(nNextLine->IsConditional())
        {
            mCompileStack.push(&nNextLine->mConditional->mCode);
            mCurScope++;
        }else
        //Only FinishConditional we're in a conditional that didn't have any brackets and
        //they added a non-conditional/normal ScriptCodeLine inside that.
        if(nContainingLine->IsConditional() && !nContainingLine->IsConditionalBracketed())
        {
            FinishConditional();
        }
        return;
    }
    
    
    //Normal Insert
    ValidateInsertion(hNextLine, nCurrentList, nCurrentLine, nCurrentLineIter, nContainingList, nContainingLine, nContainingLineIter);
    if(nContainingLine)
        hNextLine->mInCodeLine = new ScriptCodeLineIter(nContainingLineIter);
    nCurrentList->AddBack(hNextLine);
    hNextLine.Detach();
    if(nNextLine->IsConditional())
    {
        mCompileStack.push(&nNextLine->mConditional->mCode);
        mCurScope++;
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ValidateInsertion
//
//   Desc:: Called before every insertion in AddCodeLine to make sure that the
//          ScriptCodeLine can be inserted where the user wants it. Right now
//          it just checks for a previous if statement when you try to insert
//          an else.
//
//  Input:: A plethora of goodness
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::ValidateInsertion
    (ScriptCodeLine*        hNextLine, 
     ScriptCodeLineList*    hCurrentList,
     ScriptCodeLine*        hCurrentLine, 
     ScriptCodeLineIter     hCurrentLineIter,
     ScriptCodeLineList*    hContainingList,
     ScriptCodeLine*        hContainingLine, 
     ScriptCodeLineIter     hContainingLineIter)
{
    ASSERT(hNextLine);
    if(hCurrentLine) ASSERT(hCurrentList);
    if(hContainingLine) ASSERT(hContainingList);
    if(hContainingList) ASSERT(hContainingLine);

    
    //if() else if() else if() else looks like...
    //[If_Statment] { Code... } 
    //[Else_Statement] { [If_Statement]{ Code... } }
    //[Else_Statement] { [If_Statement]{ Code... } }
    //[Else_Statement] { Code... }
    
    if(hNextLine->IsConditional(kScriptCond_Else)
    || hNextLine->IsConditional(kScriptCond_ElseIf))
    {
        if(hCurrentLine == NULL
        || !(hCurrentLine->IsConditional(kScriptCond_If) || hCurrentLine->IsConditional(kScriptCond_ElseIf)))
        {
            mScript->AddError("No preceeding if() statement for 'else'.", mLineNumber);
            throw -1;
        }
    }
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::FinishConditional
//
//   Desc:: Called from AddCodeLine when it adds a ScriptCodeLine into a 
//          non-bracketed ScriptCondition's ScriptCodeLineList. It basically
//          pops off the top item from mCompileStack. It is also called from
//          CompileStep when a closing } is found. It does some checking to
//          make sure that we are in fact inside a conditional before doing the
//          pop, it will set an error in mScript and throw an exception if it's
//          improperly called - which can happen if the user misplaces a
//          closing } bracket in their script.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::FinishConditional()
{
    bool nBadlyCalled = true;
    
    
    if(!mCompileStack.empty())
    {
        //Gather Info
        ///////////////////////////////////////////////////////////////////////
        ScriptCodeLineList* nCurrentList    = GetCurrentCodeList();
        ScriptCodeLine*     nCurrentLine    = GetCurrentCodeLine(); 
        ScriptCodeLineList* nContainingList = GetContainingCodeList();
        ScriptCodeLine*     nContainingLine = GetContainingCodeLine();  
        ///////////////////////////////////////////////////////////////////////
        
        if(nContainingLine && nContainingLine->IsConditional())
        {
            if(!nContainingLine->IsConditionalBracketed())
                ASSERT(nContainingLine->mConditional->mCode.size() == 1);
            
            
            nBadlyCalled = false;
            nContainingLine->SetConditionalFinished();
            mCompileStack.pop();
            mCurScope--;

            //Gather Info
            ///////////////////////////////////////////////////////////////////////
            ASSERT(!mCompileStack.empty());
            nCurrentList    = GetCurrentCodeList();
            nCurrentLine    = GetCurrentCodeLine(); 
            nContainingList = GetContainingCodeList();
            nContainingLine = GetContainingCodeLine();  
            ///////////////////////////////////////////////////////////////////////

            //Recursively Exit Non-Bracketed Conditionals
            if(nContainingLine && nContainingLine->IsConditional() 
            && !nContainingLine->IsConditionalBracketed())
            {
                //printf("AutoFinishing\n");
                FinishConditional();
            }
        }
    }
    
    
    
    if(nBadlyCalled)
    {
        mScript->AddError("Misplaced closing } bracket.", mLineNumber);
        throw -1;
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::UpdateOnLine
//
//   Desc:: Give it a ptr to a spot somewhere in the original source code that
//          has been parsed to and it will set the mLineNumber data member to the
//          correct number of lines at that spot.
//
//  Input:: Ptr to a spot somewhere inside mScript->mSource.c_str()
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptCompiler::UpdateOnLine(const CHAR* hAtSpotInCode)
{
    STRING nParsedText;
    
    //Add all the code we've parsed
    nParsedText.append(mScript->mSource.c_str(), hAtSpotInCode-mScript->mSource.c_str());
    mLineNumber = CountLines(nParsedText.c_str());
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::ParseArgTill
//
//   Desc:: Given a list of delimiters it will scan through text, correctly
//          looking through "strings" and (parathesised arguments) and any
//          comments till it hits one of the delimiters. The text from start
//          to finish (but not including the delimiter) is put into hOut.
//
//  Input:: hSource: Ptr to a spot of text to start parsing from.
//          hDelims: List of delimiters to look for.
//          hOut: Buffer to hold the results.
//
// Output:: True on success.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptCompiler::ParseArgTill(const CHAR*& hSource, Vector<STRING>& hDelims, STRING& hOut, bool hOpCareful)
{
    hOut.erase();
    ScanTillNextChar(hSource, false);
    while(isspace(*hSource))
        ScanTillNextChar(hSource);
    
    
    bool nBreakOnSlashZero = false;
    for(USHORT i = 0; i < hDelims.size(); i++)
    if(hDelims[i].length() == 0)
        nBreakOnSlashZero = true;
    
    const CHAR* nOriginalSource = hSource;
    SHORT       nParathesisDepth = 0;
    bool        nInQuote = false;
    
    while(*hSource != '\0')
    {
        if(*hSource == '"')
        {
            nInQuote = !nInQuote;
            hOut += *hSource;
            ScanTillNextChar(hSource);
            continue;
        }


        //Check for delims
        if(nParathesisDepth == 0 && !nInQuote)
        for(USHORT i = 0; i < hDelims.size(); i++)
        if(hDelims[i].length())
        if(!strncmp(hSource, hDelims[i].c_str(), hDelims[i].length()))
        {
            CHAR nCheckChar = hSource == nOriginalSource ? ' ' : *(hSource-1);
            bool nPassedSafetyTest = true;
            
            //This check will make sure we didn't just match the first/last part of
            //a "!=" when we're looking for a "!" only. Makes sure the character
            //following the operator isn't another operator-ish character itself.
            if(hOpCareful)
            if(!(isalnum(nCheckChar) || isspace(nCheckChar)
              || nCheckChar == ')'   || nCheckChar == '(' 
              || nCheckChar == '}'   || nCheckChar == '{'))
                nPassedSafetyTest = false;
    
            hSource += hDelims[i].length();
            
            if(hOpCareful)
            if(!(isalnum(nCheckChar) || isspace(nCheckChar)
              || nCheckChar == ')'   || nCheckChar == '(' 
              || nCheckChar == '}'   || nCheckChar == '{'))
                nPassedSafetyTest = false;

            if(!hOpCareful || nPassedSafetyTest)
                return true;

            //No dice
            hSource -= hDelims[i].length();
        }


        if(*hSource == '(' && !nInQuote)
            nParathesisDepth++;

        if(*hSource == ')' && !nInQuote)
            nParathesisDepth--;
            
        hOut += *hSource;
        ScanTillNextChar(hSource);
    }

    if(nBreakOnSlashZero)
        return true;

    hOut.erase();
    hSource = nOriginalSource;
    return false;
}




bool ScriptCompiler::ParseArgTillOperator(const CHAR*& hSource, SHORT hOpLevel, ScriptFuncDef*& hOpFunc, STRING& hOut)
{
    List<ScriptFuncDef*>    nOperators;
    ScriptFuncDef*          nOpDefScan;
    
    //Gather Operators Of hOpLevel Level
    List<FuncDefBool>::iterator i;
    for(i = mOperatorList.begin(); i != mOperatorList.end(); ++i)
    {
        nOpDefScan = (*i).first;
        if(nOpDefScan->mOp == hOpLevel)
            nOperators.AddBack(nOpDefScan);
    }
    

    if(nOperators.empty())
        return false;
    
    List<ScriptFuncDef*>::iterator ii;
    for(ii = nOperators.begin(); ii != nOperators.end(); ++ii)
    {
        Vector<STRING> nDelims;
        
        nOpDefScan = *ii;
        nDelims.Add(nOpDefScan->mName);
        
        const CHAR* nTempSource = hSource;
        if(ParseArgTill(nTempSource, nDelims, hOut, true))
        {
            hSource = nTempSource;
            hOpFunc = nOpDefScan;
            return true;
        }
    }
    
    return false;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CreateVariable
//
//   Desc:: Given a kScriptDT_XX type it will create a ScriptVariable of that 
//          type. It searches the internal library of data types to find the
//          creation method. 
//
//  Input:: The kScriptDT_XX type to create. 
//
// Output:: Non-NULL pointer to the created ScriptVariable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptVariable* ScriptCompiler::CreateVariable(SHORT hType)
{
    if(hType == kScriptDT_None)
        return NewVariable(gDTDef_None);


    //Search our library of ScriptDataTypes
    List<DataTypeDefBool>::iterator i;
    for(i = mDataTypeList.begin(); i != mDataTypeList.end(); ++i)
    {
        ScriptDataTypeDef* nDTDef = (*i).first;
        
        if(nDTDef->mType == hType)
        {
            ASSERT(nDTDef->mCreateMethod != NULL);
            return NewVariable(nDTDef);
        }
    }
    

    //No dice
    ASSERT(!"Bad hType passed into ScriptCompiler::CreateVariable. Not in library.");
    return NULL;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::CreateVariable
//
//   Desc:: Given a kScriptDT_XX type it will create a ScriptVariable of that 
//          type. It searches the internal library of data types to find the
//          creation method. 
//
//  Input:: The kScriptDT_XX type to create. 
//
// Output:: Non-NULL pointer to the created ScriptVariable.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptDataTypeDef* ScriptCompiler::FindDTDef(SHORT hType)
{
    //Search our library of ScriptDataTypes
    List<DataTypeDefBool>::iterator i;
    for(i = mDataTypeList.begin(); i != mDataTypeList.end(); ++i)
    {
        ScriptDataTypeDef* nDTDef = (*i).first;
        
        if(nDTDef->mType == hType)
            return nDTDef;
    }
    

    //No dice
    ASSERT(!"Bad hType passed into ScriptCompiler::FindDTDef. Not in library.");
    return NULL;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetCurrentCodeList
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLineList* ScriptCompiler::GetCurrentCodeList()
{
    return mCompileStack.empty() ? NULL : mCompileStack.top(); 
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetCurrentCodeLine
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLine* ScriptCompiler::GetCurrentCodeLine()
{
    ScriptCodeLineList* nCurList = GetCurrentCodeList();
    if(!nCurList)
        return NULL;    
    return nCurList->empty() ? NULL : nCurList->back();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetCurrentCodeLineIter
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLineIter ScriptCompiler::GetCurrentCodeLineIter()
{
    ScriptCodeLineList* nCurList = GetCurrentCodeList();
    if(!nCurList)
        return ScriptCodeLineIter();
    ScriptCodeLineIter nIter = nCurList->end();
    return nCurList->empty() ? nIter : --nIter;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetContainingCodeList
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLineList* ScriptCompiler::GetContainingCodeList()
{
    if(mCompileStack.empty())
        return NULL;
    ScriptCodeLineList* nCurList = mCompileStack.top();
    mCompileStack.pop();
    ScriptCodeLineList* nContList = mCompileStack.empty() ? NULL : mCompileStack.top();
    mCompileStack.push(nCurList);
    return nContList;
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetContainingCodeLine
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLine* ScriptCompiler::GetContainingCodeLine()
{
    ScriptCodeLineList* nContList = GetContainingCodeList();
    if(!nContList)
        return NULL;
    return nContList->empty() ? NULL : nContList->back();
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptCompiler::GetContainingCodeLineIter
//
//   Desc:: (Write me)
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptCodeLineIter ScriptCompiler::GetContainingCodeLineIter()
{
    ScriptCodeLineList* nContList = GetContainingCodeList();
    if(!nContList)
        return ScriptCodeLineIter();
    ScriptCodeLineIter nIter = nContList->end();
    return nContList->empty() ? nIter : --nIter;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScanTillNextChar
//
//   Desc:: Helper function. Scans through a chunk of text until it finds the
//          next ACTUAL character that isn't a comment. Used in all functions
//          that step through text so they correctly ignore comments. Once it
//          returns *hSource will be the next actual character or '\0'.
//
//  Input:: Reference to a CHAR* pointer to some text.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScanTillNextChar(const CHAR*& hSource, bool hAdvance)
{
    const CHAR* nCameIn = hSource;
    CHAR        nCur, nNext = ' ';
    bool        nInSlashStarComment = false;

    do
    {
        nCur    = *hSource;
        nNext   = nCur == '\0' ? '\0' : *(hSource+1);
    
        //Ending Of A Slash Star Comment
        if(nCur == '*' && nNext == '/' && nInSlashStarComment)
        {
            nInSlashStarComment = false;
            hSource += 2;
            continue;
        }
        
        //Continue Till End Of Comment
        if(nInSlashStarComment)
        {
            hSource++;
            continue;
        }

        //Skip till End Of Line Comment
        if(nCur == '/' && nNext == '/')
        {
            while(*hSource != '\n')
            {
                if(*hSource == '\0')
                    return;
                hSource++;
            }
            hSource++;
            continue;
        }

        //Begining Of A Slash Star Comment
        if(nCur == '/' && nNext == '*')
        {
            nInSlashStarComment = true;
            hSource += 2;
            continue;
        }
        
        
        //Valid Next Character
        if(hAdvance && nCameIn == hSource && *hSource != '\0')
        {
            hSource++;
            continue;
        }
        break;  
    }
    while(true);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: IsValidVariableName
//
//   Desc:: Helper function. Checks to make sure that hText is a valid name for
//          for a variable. Names may be any combination of letters, numbers,
//          and underscores given that the name contains at least one letter
//          or underscore somewhere.
//
//  Input:: The STRING to check.
//
// Output:: True if it's a valid variable name, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool IsValidVariableName(STRING& hText)
{
    bool nIsValid = true, nFoundNonDigit = false;
    
    //** Determine If It's A Variable Name **//
    for(ULONG i = 0; nIsValid && i < hText.length(); i++)
    {
        CHAR c = hText[i];
        if(isalpha(c) || c == '_')
        {
            nFoundNonDigit = true;
            continue;
        }
        
        if(isdigit(c))
            continue;
            
        nIsValid = false;
    }
    
    if(nIsValid && !nFoundNonDigit)
        nIsValid = false;
    
    return nIsValid;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: StripParenthesis
//
//   Desc:: Intelligently strips the paratheses off of line, taking care to
//          only remove uneeded ones that do not affect the overall statement
//          of line. Examples: It would turn "((variable))" into "variable".
//          It would turn "((variable/2)+3)" into "(variable/2)+3". It would
//          turn "((1)+(2))" into "(1)+(2)"
//
//  Input:: The string.
//
// Output:: Parenthesis stripped new string.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

STRING StripParenthesis(STRING hLine)
{
    bool    nInQuote    = false;
    ULONG   nParaDepth  = 0;
    CHAR*   nParaStart  = NULL;
    CHAR*   nParaEnd    = NULL;
    
    
    hLine = StripWS(hLine);
    if(hLine.length() < 2 || hLine[0] != '(' || hLine[hLine.length()-1] != ')')
        return hLine;

    for(ULONG i = 0; i < hLine.length(); i++)
    {
        if(hLine[i] == '(' && !nInQuote)
        {
            if(nParaDepth == 0)
                nParaStart = &hLine[i];
            nParaDepth++;
        }
        else if(hLine[i] == ')' && !nInQuote)
        {
            nParaDepth--;
            if(nParaDepth == 0)
                nParaEnd = &hLine[i];
        }
        else if(hLine[i] == '"')
            nInQuote = !nInQuote;
    }
    
    //No balanced parenthesis with ( at start and ) at end
    if(nParaDepth != 0 || &hLine[0] != nParaStart || &hLine[hLine.length()-1] != nParaEnd)
        return hLine;

    hLine.erase(hLine.begin()); //Strip off first (
    hLine.erase(hLine.begin()+(hLine.length()-1));  //Strip off last  )
    
    //Recursion is your very best friend
    return StripParenthesis(hLine);
}


