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
#ifndef SCRIPTCONDITIONAL_H
#define SCRIPTCONDITIONAL_H 1

class ScriptConditional
{
    //** De/constructors **//
    public: ScriptConditional();
    public: ~ScriptConditional();


    //** Runtime Data Members **//
    public:
    SHORT                   mType;      //If, While, For?
    ScriptVariable*         mEval;      //Truth conditional
    bool                    mBrackets;  
    ScriptCodeLineList      mCode;      //Code lines
    ScriptCodeLine*         mFromCode;  //Line of code that called this conditional
    bool                    mFinished;  //Has all the code it needs inside
};

//Values for the mType field
enum
{
    kScriptCond_If = 0      ,
    kScriptCond_Else        ,
    kScriptCond_ElseIf      ,
    kScriptCond_While       ,
    kScriptCond_For
};

#endif //ifndef SCRIPTCONDITIONAL_H


