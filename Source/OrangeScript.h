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
#ifndef ORANGESCRIPT_H
#define ORANGESCRIPT_H 1
#include "Script.h"
#include "LoadOld.h"
#include "SaveLoadable.h"

class OrangeScript
    : public Script
    , public LoadOld
    , public SaveLoadable
{
    public: OrangeScript();
    public: virtual ~OrangeScript();
    

    //** SaveLoadable Functions **//
    virtual void    SaveTo(XMLWriter& hFile, const CHAR* hTagName);
    virtual void    SaveValuesTo(XMLWriter& hFile);
    virtual bool    MatchValues(XMLReader& hXML, XMLTag& hTag); 


    //** LoadOld Abstract Functions **//
    virtual bool    MatchValues(MUDFile& hFp, CHAR* hName);
    virtual bool    MatchLists(MUDFile& hFp, CHAR* hName);
    static void     LoadListInto(MUDFile& hFp, ScriptedThing* hInto);


    //** Public Member Functions **//
    public:
    bool        CheckTrigger(STRINGCW& hCmd, STRINGCW& hArg, Person* theCh, Mobile* theMob, Object* theObj, Person* theVict);
    void        Convert();
    bool        IsRunning();
    STRINGCW    OnName();
    void        StatTo(Person* hTo);

    //** Public Data Members **//
    public:
    ScriptedThing*      mOnThis;
    STRINGCW            mCommands;          //Command followed by one of the keywords
    STRINGCW            mKeywords;          //will activate the script.
    SHORT               mType;              //Object/Room/Mobile script.
    SHORT               mTrigger;           //How is it set off
    LONG                mFlags;             //Script flags
    VNUM                mID;                //Script ID number
    OScriptVariableList mTextVariables;     //Specially edited text variables
};



///////////////////////////////////////////////////////////////////////////////
// OrangeMUD Additions
///////////////////////////////////////////////////////////////////////////////
enum
{
    kScriptTrig_None = 0    ,
    kScriptTrig_Phrase      ,
    kScriptTrig_Wear        ,
    kScriptTrig_Remove      ,
    kScriptTrig_Attack      ,
    kScriptTrig_Ask         ,
    kScriptTrig_Random      ,
    kScriptTrig_Enter       ,
    kScriptTrig_Leave       ,
    kScriptTrig_Death       ,
    kScriptTrig_Timer       ,
    kScriptTrig_Eat         ,
    kScriptTrig_Light       ,
    kScriptTrig_Identify    ,
    kScriptTrig_Assist
};



enum
{
    kRoomScript = 0 ,
    kObjectScript   ,
    kMobileScript           
};



bool TrigPhrase(Person* hCh, STRINGCW& hCmd, STRINGCW& hArg);
bool TrigMove(Person* hCh, SHORT hType, const STRINGCW& hDirName);



#endif //ifndef ORANGESCRIPT_H
