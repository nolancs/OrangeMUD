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
#include "ScriptedThing.h"
#include "OrangeScript.h"
#include "OrangeScriptCompiler.h"
#include "Person.h"
#include "StringUtils.h"
#include "Tables.h"


//## Globals ##//
OrangeScriptCompiler gCompiler;
bool gCompilerInited = false;




ScriptedThing::ScriptedThing()
{
}



ScriptedThing::~ScriptedThing()
{
    for(OScriptIter i = mScriptMap.begin(); i != mScriptMap.end(); ++i)
        delete i->second;
}



#if 0
//***************************************************************************//
///////////////////////////////////////////////////////////////////////////////
//***************************************************************************//
#pragma mark -
#endif



OrangeScript* ScriptedThing::GetScript(VNUM hScriptID) const
{
    OScriptCIter nFind = mScriptMap.find(hScriptID);
    return nFind == mScriptMap.end() ? NULL : nFind->second;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptedThing::ResolveScript
//
//   Desc:: Resolves a script on this ScriptedThing by a string containing the
//          ID of the script. It will handle all failure cases for you and
//          return false, otherwise it returns true and set hScript.
//
//  Input:: hTo: The Person to resolve the script to.
//          hArg: A textual argument like "10" which is the script id.
//          hScript: Will point to the script if resolution succeeds.
//
// Output:: True of success, false otherwise.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptedThing::ResolveScript(Person* hTo, STRINGCW& hArg, OrangeScript*& hScript)
{
    VNUM nScriptNum;
    if(!hArg.ResolveShort(hTo, nScriptNum))
        return false;
    
    hScript = GetScript(nScriptNum);
    if(!hScript)
    {
        hTo->Send("No such script.\n");
        return false;
    }
    
    return true;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptedThing::StatListTo
//
//   Desc:: Shows a Person a listing of the Scripts, their types, and other
//          information about them. Primarily used with stat functions.
//
//  Input:: The Person to send the lines of info to.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ScriptedThing::StatListTo(Person* hTo)
{
    if(!mScriptMap.empty())
    {
        OrangeScript* nScript;
        STRINGCW cmdBuf;
        
        hTo->Send("\n^UScript^x   ^UTrigger  ^x   ^UCommands & Keywords               ^x   ^UCompiled^x   ^ULines      ^x\n");

        for(VNUM i = 1; (nScript = GetScript(i)) != NULL; ++i)
        {
            cmdBuf.SPrintF("\"%s\", \"%s\"", *nScript->mCommands, *nScript->mKeywords);
            hTo->SendF("  #%-3d   %-9s   %-34s     %-6s   %d\n", i, gTriggerTable[nScript->mTrigger].mName, *cmdBuf, 
            nScript->mBinary==NULL?"NO":"YES", CountLines(nScript->mSource.c_str()));
        }
        
    }
}



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//   Name:: ScriptedThing::CompileTo
//
//   Desc:: Attempts to compile a script and optionally send failure info to
//          a Person about why the script could not be compiled. This is used
//          to compile all scripts using the global script compiler.
//
//  Input:: hScript: the script.
//          hTo: the Person, may be NULL.
//
// Output:: True on successful compilation.
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

bool ScriptedThing::CompileTo(OrangeScript* hScript, Person* hTo)
{
    if(!gCompilerInited)
    {
        gCompiler.InitFunctionality();
        gCompilerInited = true;
    }
    
    if(gCompiler.Compile(hScript))
        return true;

    if(hTo)
    {
        hTo->Send("Failure:\n");
        hTo->Send(hScript->mErrorBuffer.c_str());
        hTo->Send("----------\n");
        hTo->Send(hScript->mSource.c_str());
    }
    
    return false;
}

