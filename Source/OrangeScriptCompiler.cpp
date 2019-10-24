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
#include "OrangeScriptCompiler.h"
#include "OrangeScriptBinary.h"
#include "OrangeScriptVariable.h"
#include "OrangeScriptLibrary.h"
#include "OrangeScript.h"


OrangeScriptCompiler::OrangeScriptCompiler()
{
}


OrangeScriptCompiler::~OrangeScriptCompiler()
{
}




#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScriptCompiler::InitFunctionality
//
//   Desc:: Goes in and adds on the bare-bones functions, operators, flow
//          controls, data types, and constants to this ScriptCompiler. Any
//          Subclass would override this method in to add their own specific
//          functions, data types, and constants. Subclasses can still call
//          this function if they wish to use all the base functionality, or
//          they can choose which they want and add them on their own.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void OrangeScriptCompiler::InitFunctionality()
{
    ScriptCompiler::InitFunctionality();
    
    //Add our OrangeScriptLibrary stuff here
    ULONG i, j;
    
    for(i = 0; gOrangeFunctionTable[i].mName; ++i)
        AddFunction(&gOrangeFunctionTable[i], false);
    
    for(i = 0; gOrangeDataTypeTable[i].mName; ++i)
        AddDataType(&gOrangeDataTypeTable[i], false);
    
    for(i = 0; gOrangeConstantTables[i].mName; ++i)
    for(j = 0; gOrangeConstantTables[i].mTable[j].mName; ++j)
        AddConstant(&gOrangeConstantTables[i].mTable[j], false);

}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScriptCompiler::Compile
//
//   Desc:: Compiles a Script object. It fills the Script's mBinary data member
//          with the resulting ScriptBinary on success, otherwise a textual
//          description of what went wrong and on what line is placed into the
//          Script's mErrorBuffer data member.
//
// Output:: True on successful compilation. False otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool OrangeScriptCompiler::Compile(Script* hScript)
{
    return ScriptCompiler::Compile(hScript);
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScriptCompiler::NewBinary
//
//   Desc:: Creates a new ScriptBinary for the compiler to use. Overide this
//          if you need it to create a custom binary object that you've derived
//          from ScriptBinary.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ScriptBinary* OrangeScriptCompiler::NewBinary()
{
    
    
    return new OrangeScriptBinary();
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: OrangeScriptCompiler::FindVariable
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

bool OrangeScriptCompiler::FindVariable(ScriptVariable*& hVar, STRING& hText)
{
    OrangeScript*           nOScript    = static_cast<OrangeScript*>(mScript);
    OrangeScriptBinary*     nOBinary    = static_cast<OrangeScriptBinary*>(mScript->mBinary.Ptr());
    OScriptVariableList&    nList       = nOScript->mTextVariables;
    OScriptVariableIter i;
    
    
    //Check or default variables in every script
    if(nOBinary->mRoom->mName == hText)
    {   
        hVar = nOBinary->mRoom;
        return true;
    }
    if(nOBinary->mCh->mName == hText)
    {   
        hVar = nOBinary->mCh;
        return true;
    }
    if(nOBinary->mMob->mName == hText)
    {   
        hVar = nOBinary->mMob;
        return true;
    }
    if(nOBinary->mObj->mName == hText)
    {   
        hVar = nOBinary->mObj;
        return true;
    }
    if(nOBinary->mVictim->mName == hText)
    {   
        hVar = nOBinary->mVictim;
        return true;
    }
    
    
    //Check our human-edited constant text variables
    for(i = nList.begin(); i != nList.end(); ++i)
    {
        OrangeScriptVariable* nVariable = *i;

        if(hText == nVariable->mName)
        {
            hVar = nVariable;
            return true;
        }
    }
    
    
    //Check normal script variables
    return ScriptCompiler::FindVariable(hVar, hText);
}
