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
#ifndef ORANGESCRIPTBINARY_H
#define ORANGESCRIPTBINARY_H 1
#include "ScriptBinary.h"

class OrangeScriptBinary
    : public ScriptBinary
{
    //** De/constructors **//
    public: OrangeScriptBinary();
    public: ~OrangeScriptBinary();


    //** Runtime Data Members **//
    public:
    APtr<ScriptVariable>    mRoom;      //Room if this is a room script
    APtr<ScriptVariable>    mCh;        //Character who activated the script
    APtr<ScriptVariable>    mMob;       //Mob if this is a mob script
    APtr<ScriptVariable>    mObj;       //Obj if this is an obj script
    APtr<ScriptVariable>    mVictim;    //Victim if this is targeting another person
};


#endif //ifndef ORANGESCRIPTBINARY_H


