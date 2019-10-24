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
#ifndef SCRIPTCOMPILER_H
#define SCRIPTCOMPILER_H 1
#include "ScriptLibrary.h"

class ScriptCompiler
{
    //** De/constructors **//
    public: ScriptCompiler();
    public: virtual ~ScriptCompiler();

    //** Public Virtual Functions **//
    public:
    virtual bool Compile(Script* hScript);
    virtual void InitFunctionality();
    virtual ScriptBinary*   NewBinary();
    virtual ScriptVariable* NewVariable(ScriptDataTypeDef* hDef, void* hValue = NULL, bool hConst = false);
    virtual bool            FindVariable(ScriptVariable*& hVar, STRING& hText);
    virtual bool            FindConstant(ScriptVariable*& hVar, STRING& hText);
    virtual bool            FindRawValue(ScriptVariable*& hVar, STRING& hText);
    virtual bool            FindFunction(ScriptFuncDef*& hFunc, STRING& hFuncName);
    virtual bool            FindDataType(ScriptDataTypeDef*& hDT, STRING& hDTName);

    //** Language Control Related Functions **//
    public:
    void    AddFunction     (ScriptFuncDef*     hFunction, bool hWeOwn = false);
    void    AddOperator     (ScriptFuncDef*     hOperator, bool hWeOwn = false);
    void    AddFlowControl  (ScriptFuncDef*     hFlowCtrl, bool hWeOwn = false);
    void    AddDataType     (ScriptDataTypeDef* hDataType, bool hWeOwn = false);
    void    AddConstant     (ScriptConstDef*    hConstant, bool hWeOwn = false);
    bool    RemoveFunction      (ScriptFuncDef*     hFunction);
    bool    RemoveOperator      (ScriptFuncDef*     hOperator);
    bool    RemoveFlowControl   (ScriptFuncDef*     hFlowCtrl);
    bool    RemoveDataType      (ScriptDataTypeDef* hDataType);
    bool    RemoveConstant      (ScriptConstDef*    hConstant);
    void    ClearFunctions();
    void    ClearOperators();
    void    ClearFlowControls();
    void    ClearDataTypes();
    void    ClearConstants();

    //** Protected Member Functions **//
    protected:
    void    CompileStep(bool& hDoneCompiling);
    void    CompileIf();
    void    CompileElse();
    void    CompileElseIf();
    void    CompileWhile();
    void    CompileArgument(ScriptVariable*& hVar, STRING& hText);
    bool    TryCompileFControl(STRING& hText);
    bool    TryCompileVariables(STRING& hText);
    bool    TryCompileFunction(APtr<ScriptFunction>& hFunc, STRING& hText);
    bool    TryCompileOperator(APtr<ScriptFunction>& hFunc, STRING& hText);
    void    AddCodeLine(APtr<ScriptCodeLine>& hNextLine);
    void    UpdateOnLine(const CHAR* hAtSpotInCode);
    void    ValidateInsertion
                (ScriptCodeLine*        hNextLine, 
                 ScriptCodeLineList*    hCurrentList,
                 ScriptCodeLine*        hCurrentLine, 
                 ScriptCodeLineIter     hCurrentLineIter,
                 ScriptCodeLineList*    hContainingList,
                 ScriptCodeLine*        hContainingLine, 
                 ScriptCodeLineIter     hContainingLineIter);
    void    FinishConditional();
    bool    ParseArgTill(const CHAR*& hSource, Vector<STRING>& hDelims, STRING& hOut, bool hOpCareful = false);
    bool    ParseArgTillOperator(const CHAR*& hSource, SHORT hOpLevel, ScriptFuncDef*& hOpFunc, STRING& hOut);
    ScriptVariable*     CreateVariable(SHORT hType);
    ScriptDataTypeDef*  FindDTDef(SHORT hType);
    ScriptCodeLineList* GetCurrentCodeList();
    ScriptCodeLine*     GetCurrentCodeLine();
    ScriptCodeLineIter  GetCurrentCodeLineIter();
    ScriptCodeLineList* GetContainingCodeList();
    ScriptCodeLine*     GetContainingCodeLine();
    ScriptCodeLineIter  GetContainingCodeLineIter();
    

    //** Public Data Members **//
    public:
    Script*                     mScript;        //Script we're compiling
    const CHAR*                 mSourceCode;    //Compilation ptr to the source
    SHORT                       mLineNumber;    //Line we're on while compiling
    SHORT                       mCurScope;
    stack<ScriptCodeLineList*>  mCompileStack;

    //** Protected Data Members **//
    protected:
    typedef pair<ScriptFuncDef*,bool>       FuncDefBool;
    typedef pair<ScriptConstDef*,bool>      ConstDefBool;
    typedef pair<ScriptDataTypeDef*,bool>   DataTypeDefBool;
    List<FuncDefBool>       mFunctionList;
    List<FuncDefBool>       mOperatorList;
    List<FuncDefBool>       mFlowControlList;
    List<DataTypeDefBool>   mDataTypeList;
    List<ConstDefBool>      mConstantList;
};


#endif //ifndef SCRIPTCOMPILER_H


